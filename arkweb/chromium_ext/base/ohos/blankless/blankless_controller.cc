/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "blankless_controller.h"

#include <algorithm>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/files/file.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/values.h"

using namespace OHOS::NWeb;

static constexpr int32_t MAX_ENABLED_URL_COUNT = 100;

namespace base {
namespace ohos {
void BlanklessController::BlankOptWhiteList::LoadWhiteList()
{
  if (m_is_loaded_) {
    return;
  }
  m_is_loaded_ = true;

  base::FilePath data_path = base::FilePath("/etc/web/blank_opt_white_list.json");
  base::File tfile(data_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!tfile.IsValid()) {
    LOG(WARNING) << "BlankOptWhiteList file is invalid or not exist.";
    return;
  }

  std::vector<char> buffer(tfile.GetLength());
  int bytes_read = tfile.Read(0, buffer.data(), buffer.size());
  if (bytes_read == -1) {
    LOG(WARNING) << "BlankOptWhiteList read file failed.";
    return;
  }

  auto buffer_str = std::string_view(buffer.data(), buffer.size());
  std::optional<base::Value> json = base::JSONReader::Read(buffer_str, base::JSON_ALLOW_TRAILING_COMMAS);
  if (!json.has_value() || !json->is_dict()) {
    LOG(WARNING) << "BlankOptWhiteList parse file as invalid json format failed.";
    return;
  }

  base::Value::Dict& dict = json->GetDict();
  base::Value* exactMatch = dict.Find("exact-match");
  base::Value* fuzzyMatch = dict.Find("fuzzy-match");
  if (!exactMatch || !fuzzyMatch || !exactMatch->is_list() || !fuzzyMatch->is_list()) {
    LOG(WARNING) << "BlankOptWhiteList read white list failed.";
    return;
  }
  base::Value::List& exactMatchList = exactMatch->GetList();
  for (const auto& item : exactMatchList) {
    m_exact_match_set_.insert(item.GetString());
  }
  base::Value::List& fuzzyMatchList = fuzzyMatch->GetList();
  for (const auto& item : fuzzyMatchList) {
    m_fuzzy_match_set_.insert(item.GetString());
  }
  LOG(DEBUG) << "BlankOptWhiteList read white list success.";
}

bool BlanklessController::BlankOptWhiteList::CheckWhiteList(const std::string& url)
{
  LoadWhiteList();
  return ExactMatch(url) || FuzzyMatch(url);
}

bool BlanklessController::BlankOptWhiteList::ExactMatch(const std::string& url)
{
  return m_exact_match_set_.find(url) != m_exact_match_set_.end();
}

bool BlanklessController::BlankOptWhiteList::FuzzyMatch(const std::string& url)
{
  for (const std::string& str : m_fuzzy_match_set_) {
    if (url.compare(0, str.size(), str) == 0) {
      return true;
    }
  }
  return false;
}

uint64_t BlanklessController::ConvertToBlanklessKey(const std::string& value)
{
  return std::hash<std::string>{}(value);
}

BlanklessController& BlanklessController::GetInstance()
{
  static BlanklessController instance;
  return instance;
}

uint64_t BlanklessController::GetBlanklessLoadingKey(const std::string& url, int32_t nweb_id)
{
  if (!CheckGlobalProperty() || !CheckEnableForDeviceType() || GetPrivacyStatus(nweb_id) || m_capacity_ == 0) {
    return INVALID_BLANKLESS_KEY;
  }
  if (uint64_t key = GetKeyAndResetLoadingStatus(nweb_id); key != INVALID_BLANKLESS_KEY) {
    return key;
  }
  if (CheckEnableForUrl(url)) {
    return std::hash<std::string>{}(url);
  }
  return INVALID_BLANKLESS_KEY;
}

void BlanklessController::SetPrivacyStatus(int32_t nweb_id, bool is_private)
{
  std::lock_guard<std::mutex> lck(m_privacy_mtx_);
  m_nweb_privacy_map_[nweb_id] = is_private;
}

bool BlanklessController::GetPrivacyStatus(int32_t nweb_id)
{
  std::lock_guard<std::mutex> lck(m_privacy_mtx_);
  auto it = m_nweb_privacy_map_.find(nweb_id);
  if (it != m_nweb_privacy_map_.end()) {
    return it->second;
  }
  return false;
}

void BlanklessController::SetCapacity(int32_t capacity)
{
  m_capacity_.store(capacity);
}

int32_t BlanklessController::GetCapacity() const
{
  return m_capacity_.load();
}

void BlanklessController::RecordBlanklessKey(int32_t nweb_id, uint64_t blankless_key)
{
  if (m_capacity_ == 0) {
    return;
  }
  std::lock_guard<std::mutex> lck(m_nweb_info_map_mtx_);
  m_nweb_info_map_[nweb_id] = {blankless_key, LoadingStatus::LOADING_CAN_SET};
}

bool BlanklessController::SetLoadingEnabled(int32_t nweb_id, uint64_t blankless_key, bool enabled)
{
  if (m_capacity_ == 0) {
    return true;
  }
  std::lock_guard<std::mutex> lck(m_nweb_info_map_mtx_);
  auto it = m_nweb_info_map_.find(nweb_id);
  if (it == m_nweb_info_map_.end()) {
    return false;
  }
  if (it->second.blankless_key != blankless_key || it->second.status != LoadingStatus::LOADING_CAN_SET) {
    return false;
  }
  it->second.status = enabled ? LoadingStatus::LOADING_ENABLE : LoadingStatus::LOADING_DISABLE;
  return true;
}

uint32_t BlanklessController::AddEnabledUrlList(const std::vector<std::string>& url_list)
{
  std::lock_guard<std::mutex> lck(m_enabled_url_set_mtx_);
  std::unordered_set<std::string> enabled_url_set;
  for (const std::string& url : url_list) {
    if (m_enabled_url_set_.find(url) != m_enabled_url_set_.end()) {
      enabled_url_set.insert(url);
      continue;
    }
    if (m_enabled_url_set_.size() >= MAX_ENABLED_URL_COUNT) {
      continue;
    }
    m_enabled_url_set_.insert(url);
    enabled_url_set.insert(url);
  }
  return static_cast<uint32_t>(enabled_url_set.size());
}

void BlanklessController::RemoveEnabledUrlList(const std::vector<std::string>& url_list)
{
  std::lock_guard<std::mutex> lck(m_enabled_url_set_mtx_);
  for (const std::string& url : url_list) {
    m_enabled_url_set_.erase(url);
  }
}

void BlanklessController::ClearEnabledUrlList()
{
  std::lock_guard<std::mutex> lck(m_enabled_url_set_mtx_);
  m_enabled_url_set_.clear();
}

bool BlanklessController::CheckEnableForUrl(const std::string& url)
{
  if (m_white_list_.CheckWhiteList(url)) {
    return true;
  }
  std::lock_guard<std::mutex> lck(m_enabled_url_set_mtx_);
  return (m_enabled_url_set_.find(url) != m_enabled_url_set_.end());
}

bool BlanklessController::CheckEnableForDeviceType()
{
  static auto type = OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetProductDeviceType();
  return type == ProductDeviceType::DEVICE_TYPE_MOBILE;
}

bool BlanklessController::CheckGlobalProperty()
{
  static bool BlankOptEnableFlag =
      OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBoolParameter("web.blank.opt.enable", true);
  return BlankOptEnableFlag;
}

bool BlanklessController::SimpleCheck()
{
  return CheckGlobalProperty() && CheckEnableForDeviceType();
}

uint64_t BlanklessController::GetKeyAndResetLoadingStatus(int32_t nweb_id)
{
  std::lock_guard<std::mutex> lck(m_nweb_info_map_mtx_);
  auto it = m_nweb_info_map_.find(nweb_id);
  if (it == m_nweb_info_map_.end()) {
    return INVALID_BLANKLESS_KEY;
  }
  uint64_t ret = it->second.status == LoadingStatus::LOADING_ENABLE ? it->second.blankless_key : INVALID_BLANKLESS_KEY;
  it->second.status = LoadingStatus::LOADING_UNSET;
  return ret;
}
}  // namespace ohos
}  // namespace base
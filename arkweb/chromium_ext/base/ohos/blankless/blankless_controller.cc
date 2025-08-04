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
    LOG(WARNING) << "blankless BlankOptWhiteList file is invalid or not exist.";
    return;
  }

  std::vector<char> buffer(tfile.GetLength());
  int bytes_read = tfile.Read(0, buffer.data(), buffer.size());
  if (bytes_read == -1) {
    LOG(WARNING) << "blankless BlankOptWhiteList read file failed.";
    return;
  }

  auto buffer_str = std::string_view(buffer.data(), buffer.size());
  std::optional<base::Value> json = base::JSONReader::Read(buffer_str, base::JSON_ALLOW_TRAILING_COMMAS);
  if (!json.has_value() || !json->is_dict()) {
    LOG(WARNING) << "blankless BlankOptWhiteList parse file as invalid json format failed.";
    return;
  }

  base::Value::Dict& dict = json->GetDict();
  base::Value* exactMatch = dict.Find("exact-match");
  base::Value* fuzzyMatch = dict.Find("fuzzy-match");
  if (!exactMatch || !fuzzyMatch || !exactMatch->is_list() || !fuzzyMatch->is_list()) {
    LOG(WARNING) << "blankless BlankOptWhiteList read white list failed.";
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
  LOG(DEBUG) << "blankless BlankOptWhiteList read white list success.";
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

void BlanklessController::RegisterFrameRemoveCallback(uint64_t blankless_key, Callback&& callback)
{
  std::lock_guard<std::mutex> lck(m_frame_remove_callback_map_mtx_);
  m_frame_remove_callback_map_[blankless_key] = callback;
}

void BlanklessController::FireFrameRemoveCallback(uint64_t blankless_key)
{
  std::optional<Callback> callback;
  {
    std::lock_guard<std::mutex> lck(m_frame_remove_callback_map_mtx_);
    if (auto it = m_frame_remove_callback_map_.find(blankless_key); it != m_frame_remove_callback_map_.end()) {
      callback = std::move(it->second);
      m_frame_remove_callback_map_.erase(it);
    }
  }
  if (callback.has_value()) {
    callback.value()();
  }
}

void BlanklessController::RegisterFrameInsertCallback(uint64_t blankless_key, Callback&& callback, int32_t lcp_time)
{
  std::lock_guard<std::mutex> lck(m_frame_insert_callback_map_mtx_);
  m_frame_insert_callback_map_[blankless_key] = { callback, lcp_time };
}

int32_t BlanklessController::FireFrameInsertCallback(uint64_t blankless_key)
{
  std::optional<Callback> callback;
  int32_t ret = INT32_MAX;
  {
    std::lock_guard<std::mutex> lck(m_frame_insert_callback_map_mtx_);
    if (auto it = m_frame_insert_callback_map_.find(blankless_key); it != m_frame_insert_callback_map_.end()) {
      callback = std::move(it->second.first);
      ret = it->second.second;
      m_frame_insert_callback_map_.erase(it);
    }
  }
  if (callback.has_value()) {
    callback.value()();
  }
  return ret;
}

void BlanklessController::CancelFrameInsertCallback(uint64_t blankless_key)
{
  std::lock_guard<std::mutex> lck(m_frame_insert_callback_map_mtx_);
  m_frame_insert_callback_map_.erase(blankless_key);
}

BlanklessController::StatusCode BlanklessController::ResetStatus(int32_t nweb_id, bool is_main_frame, bool is_redirect)
{
  bool allowed = (is_main_frame && !is_redirect);
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto& info = m_nweb_status_map_[nweb_id];
  info.blankless_key = INVALID_BLANKLESS_KEY;
  info.allowed = allowed;
  info.status_code = allowed ? StatusCode::ALLOWED : StatusCode::NOT_ALLOWED;
  return info.status_code;
}

void BlanklessController::RemoveStatus(int32_t nweb_id)
{
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  m_nweb_status_map_.erase(nweb_id);
}

BlanklessController::StatusCode BlanklessController::RecordKey(int32_t nweb_id, uint64_t blankless_key)
{
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto& info = m_nweb_status_map_[nweb_id];
  info.blankless_key = blankless_key;
  if (!info.allowed) {
    return info.status_code = StatusCode::NOT_ALLOWED;
  }
  if (info.blankless_key_dumped_history.find(blankless_key) != info.blankless_key_dumped_history.end()) {
    return info.status_code = StatusCode::CALL_MULTIPLED_TIMES;
  }
  info.blankless_key_dumped_history.insert(blankless_key);
  return info.status_code = StatusCode::DUMPED;
}

BlanklessController::StatusCode BlanklessController::MatchKey(int32_t nweb_id, uint64_t blankless_key)
{
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto it = m_nweb_status_map_.find(nweb_id);
  if (it == m_nweb_status_map_.end()) {
    return StatusCode::KEY_NOT_MATCH;
  }
  auto& info = it->second;
  if (info.blankless_key != blankless_key) {
    return info.status_code = StatusCode::KEY_NOT_MATCH;
  }
  if (!info.allowed) {
    return info.status_code = StatusCode::NOT_ALLOWED;
  }
  info.blankless_key = INVALID_BLANKLESS_KEY;
  if (info.blankless_key_inserted_history.find(blankless_key) != info.blankless_key_inserted_history.end()) {
    return info.status_code = StatusCode::CALL_MULTIPLED_TIMES;
  }
  info.blankless_key_inserted_history.insert(blankless_key);
  return info.status_code = StatusCode::INSERTED;
}

bool BlanklessController::CheckEnableForUrl(const std::string& url)
{
  return m_white_list_.CheckWhiteList(url);
}

bool BlanklessController::CheckGlobalProperty()
{
  static bool BlankOptEnableFlag =
      OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBoolParameter("web.blankless.enabled", false);
  return BlankOptEnableFlag;
}

void BlanklessController::ResetForTest()
{
  {
    std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
    m_nweb_status_map_.clear();
  }
  {
    std::lock_guard<std::mutex> lck(m_frame_insert_callback_map_mtx_);
    m_frame_insert_callback_map_.clear();
  }
  {
    std::lock_guard<std::mutex> lck(m_frame_remove_callback_map_mtx_);
    m_frame_remove_callback_map_.clear();
  }
}

bool BlanklessController::CheckStatusForTest(
  int32_t nweb_id, const BlanklessController::StatusInfo& expected_status, bool expected_found)
{
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto it = m_nweb_status_map_.find(nweb_id);
  if (it == m_nweb_status_map_.end()) {
    return !expected_found;
  }
  auto& info = it->second;
  bool ret = expected_status.allowed == info.allowed &&
             expected_status.blankless_key == info.blankless_key &&
             expected_status.status_code == info.status_code &&
             expected_status.blankless_key_dumped_history == info.blankless_key_dumped_history &&
             expected_status.blankless_key_inserted_history == info.blankless_key_inserted_history;
  return ret;
}
}  // namespace ohos
}  // namespace base
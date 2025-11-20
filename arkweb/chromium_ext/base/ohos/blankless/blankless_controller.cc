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
#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
#include "net/base/url_util.h"
#include "url/gurl.h"
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)

using namespace OHOS::NWeb;

static constexpr int32_t MAX_ENABLED_URL_COUNT = 100;

namespace base {
namespace ohos {
void BlanklessController::BlankOptWhiteList::LoadSysWhiteList()
{
  if (m_is_sys_loaded_) {
    return;
  }
  m_is_sys_loaded_ = true;

#if BUILDFLAG(ARKWEB_TEST)
  base::FilePath data_path = base::FilePath("/data/ut/blank_opt_white_list.json");
#else
  base::FilePath data_path = base::FilePath("/etc/web/blank_opt_white_list.json");
#endif
  base::File tfile(data_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!tfile.IsValid() || tfile.GetLength() <= 0) {
    LOG(WARNING) << "blankless BlankOptWhiteList sys file is invalid or not exist.";
    return;
  }

  std::vector<char> buffer(static_cast<size_t>(tfile.GetLength()));
  int bytes_read = tfile.Read(0, buffer.data(), static_cast<int>(buffer.size()));
  if (bytes_read == -1) {
    LOG(WARNING) << "blankless BlankOptWhiteList read sys file failed.";
    return;
  }

  auto buffer_str = std::string_view(buffer.data(), buffer.size());
  std::optional<base::Value> json = base::JSONReader::Read(buffer_str, base::JSON_ALLOW_TRAILING_COMMAS);
  if (!json.has_value() || !json->is_dict()) {
    LOG(WARNING) << "blankless BlankOptWhiteList parse sys file as invalid json format failed.";
    return;
  }

  base::Value::Dict& dict = json->GetDict();
  base::Value* exactMatch = dict.Find("exact-match");
  base::Value* fuzzyMatch = dict.Find("fuzzy-match");
  if (!exactMatch || !fuzzyMatch || !exactMatch->is_list() || !fuzzyMatch->is_list()) {
    LOG(WARNING) << "blankless BlankOptWhiteList read sys white list failed.";
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
  LOG(DEBUG) << "blankless BlankOptWhiteList read sys white list success.";
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

bool BlanklessController::BlankOptWhiteList::CheckSysWhiteList(const std::string& url, uint64_t& blankless_key)
{
  LoadSysWhiteList();
  if (ExactMatch(url) || FuzzyMatch(url)) {
    blankless_key = std::hash<std::string>{}(url);
    return true;
  }
  return false;
}

#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
void BlanklessController::BlankOptWhiteList::LoadAppWhiteList()
{
  if (m_is_app_loaded_) {
    return;
  }
  m_is_app_loaded_ = true;

  const std::string bundleName =
      OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBundleName();
  if (bundleName.empty()) {
    LOG(WARNING) << "blankless BlankOptWhiteList get app bundle name failed.";
    return;
  }
  base::FilePath data_path = base::FilePath("/sys_prod/etc/web/" + bundleName + ".json");
  base::File tfile(data_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!tfile.IsValid()) {
    LOG(WARNING) << "blankless BlankOptWhiteList app file is invalid or not exist.";
    return;
  }

  std::vector<char> buffer(static_cast<size_t>(tfile.GetLength()));
  int bytes_read = tfile.Read(0, buffer.data(), static_cast<int>(buffer.size()));
  if (bytes_read == -1) {
    LOG(WARNING) << "blankless BlankOptWhiteList read app white list failed.";
    return;
  }

  ParseAppWhiteList(buffer);
}

void BlanklessController::BlankOptWhiteList::ParseAppWhiteList(std::vector<char>& buffer)
{
  auto buffer_str = std::string_view(buffer.data(), buffer.size());
  std::optional<base::Value> json = base::JSONReader::Read(buffer_str, base::JSON_ALLOW_TRAILING_COMMAS);
  if (!json.has_value() || !json->is_dict()) {
    LOG(WARNING) << "blankless BlankOptWhiteList parse app file as invalid json format failed.";
    return;
  }

  base::Value::Dict& dict = json->GetDict();
  const std::optional<bool> enable = dict.FindBool("enable");
  if (enable.has_value() && !enable.value()) {
    LOG(WARNING) << "blankless BlankOptWhiteList app file not enable.";
    return;
  }

  base::Value* queryUrls = dict.Find("query-match");
  if (queryUrls && queryUrls->is_list()) {
    base::Value::List& queryUrlsList = queryUrls->GetList();
    for (const auto& item : queryUrlsList) {
      const base::Value::Dict* dict_val = item.GetIfDict();
      if (!dict_val) {
        LOG(WARNING) << "blankless BlankOptWhiteList read app query match item failed.";
        continue;
      }
      const std::string* url_value = dict_val->FindString("url");
      const base::Value::List* query_keys = dict_val->FindList("query_keys");
      if (!url_value || !query_keys) {
        LOG(WARNING) << "blankless BlankOptWhiteList read app url or query keys failed.";
        continue;
      }
      std::unordered_set<std::string> query_keys_set;
      for (const auto& key : *query_keys) {
        const std::string key_str = key.GetString();
        query_keys_set.insert(key_str);
      }
      m_query_match_map_.emplace(*url_value, query_keys_set);
    }
  }

  LOG(DEBUG) << "blankless BlankOptWhiteList read app white list success."; 
}

std::string BlanklessController::BlankOptWhiteList::GetBaseUrl(const std::string& url)
{
  size_t pos = url.find('?');
  return (pos == std::string::npos) ? url : url.substr(0, pos);
}

std::string BlanklessController::BlankOptWhiteList::GetQueryUrl(const std::string& url)
{
  // split hostname and query string
  std::string baseUrl = GetBaseUrl(url);
  auto it = m_query_match_map_.find(baseUrl);
  if (it == m_query_match_map_.end()) {
    return url;
  }

  // build new url by querykeys
  const std::unordered_set<std::string>& keySet = it->second;
  if (keySet.size() == 0) {
    return baseUrl;
  }
  std::ostringstream result;
  GURL gurl = GURL(url);
  result << baseUrl << "?";
  for (const std::string& key : keySet) {
    std::string value;
    bool isExist = net::GetValueForKeyInQuery(gurl, key, &value);
    if (!isExist) {
      continue;
    }
    result << key << "=" << value << "&";
  }
  return result.str();
}

bool BlanklessController::BlankOptWhiteList::QueryMatch(const std::string& url)
{
  std::string baseUrl = GetBaseUrl(url);
  return m_query_match_map_.find(baseUrl) != m_query_match_map_.end();
}

bool BlanklessController::BlankOptWhiteList::CheckAppWhiteList(const std::string& url, uint64_t& blankless_key)
{
  LoadAppWhiteList();
  if (QueryMatch(url)) {
    const std::string queryUrl = GetQueryUrl(url);
    blankless_key = std::hash<std::string>{}(queryUrl);
    return true;
  }
  return false;
}
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)

uint64_t BlanklessController::ConvertToBlanklessKey(const std::string& value)
{
  return std::hash<std::string>{}(value);
}

BlanklessController& BlanklessController::GetInstance()
{
  static BlanklessController instance;
  return instance;
}

void BlanklessController::RegisterFrameRemoveCallback(uint32_t nweb_id, uint64_t blankless_key, Callback&& callback)
{
  m_frame_remove_callback_map_.Insert(nweb_id, blankless_key, std::move(callback));
}

void BlanklessController::FireFrameRemoveCallback(uint32_t nweb_id, uint64_t blankless_key)
{
  std::optional<Callback> callback = m_frame_remove_callback_map_.Get(nweb_id, blankless_key, /*move*/true);
  if (callback.has_value()) {
    callback.value()();
    LOG(DEBUG) << "blankless FireFrameRemoveCallback nweb_id: " << nweb_id << ", blankless_key: " << blankless_key;
  }
}

void BlanklessController::RegisterFrameInsertCallback(
  uint32_t nweb_id, uint64_t blankless_key, Callback&& callback, int32_t lcp_time)
{
  m_frame_insert_callback_map_.Insert(nweb_id, blankless_key,  { std::move(callback), lcp_time });
}

int32_t BlanklessController::FireFrameInsertCallback(uint32_t nweb_id, uint64_t blankless_key)
{
  auto info = m_frame_insert_callback_map_.Get(nweb_id, blankless_key, /*move*/true);
  if (!info.has_value()) {
    return 0;
  }
  std::pair<Callback, int32_t> info_ = info.value();
  info_.first();
  return info_.second;
}

void BlanklessController::CancelFrameInsertCallback(uint32_t nweb_id, uint64_t blankless_key)
{
  m_frame_insert_callback_map_.Erase(nweb_id, blankless_key);
}

BlanklessController::StatusCode BlanklessController::ResetStatus(uint32_t nweb_id, bool is_main_frame, bool is_redirect)
{
  bool allowed = (is_main_frame && !is_redirect);
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto& info = m_nweb_status_map_[nweb_id];
  info.allowed = allowed;
  info.status_code = allowed ? StatusCode::ALLOWED : StatusCode::NOT_ALLOWED;
  return info.status_code;
}

BlanklessController::StatusCode BlanklessController::RecordKey(uint32_t nweb_id, uint64_t blankless_key)
{
  std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
  auto& info = m_nweb_status_map_[nweb_id];
  info.blankless_key = blankless_key;
  if (!info.allowed) {
    return info.status_code = StatusCode::NOT_ALLOWED;
  }
  return info.status_code = StatusCode::DUMPED;
}

BlanklessController::StatusCode BlanklessController::MatchKey(uint32_t nweb_id, uint64_t blankless_key)
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
  return info.status_code = StatusCode::INSERTED;
}

void BlanklessController::RecordSystemTime(uint32_t nweb_id, uint64_t blankless_key, uint64_t system_time)
{
  m_system_time_map_.Insert(nweb_id, blankless_key, system_time);
}

uint64_t BlanklessController::GetSystemTime(uint32_t nweb_id, uint64_t blankless_key)
{
  auto system_time = m_system_time_map_.Get(nweb_id, blankless_key, /*move*/false);
  if (system_time.has_value()) {
    return system_time.value();
  }
  return INVALID_TIMESTAMP;
}

void BlanklessController::RecordDumpTime(uint32_t nweb_id, uint64_t blankless_key, uint64_t dump_time)
{
  m_dump_time_map_.Insert(nweb_id, blankless_key, dump_time);
}

uint64_t BlanklessController::GetDumpTime(uint32_t nweb_id, uint64_t blankless_key)
{
  auto dump_time = m_dump_time_map_.Get(nweb_id, blankless_key, /*move*/false);
  if (dump_time.has_value()) {
    return dump_time.value();
  }
  return INVALID_TIMESTAMP;
}

void BlanklessController::Clear(uint32_t nweb_id)
{
  if (nweb_id == 0) {
    m_frame_insert_callback_map_.Clear();
    m_frame_remove_callback_map_.Clear();
    m_system_time_map_.Clear();
    m_dump_time_map_.Clear();
    {
      std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
      m_nweb_status_map_.clear();
    }
    std::lock_guard<std::mutex> window_id_map_lck(m_window_id_map_mtx_);
    m_window_id_map_.clear();
    return;
  }
  m_frame_insert_callback_map_.Erase(nweb_id, INVALID_BLANKLESS_KEY);
  m_frame_remove_callback_map_.Erase(nweb_id, INVALID_BLANKLESS_KEY);
  m_system_time_map_.Erase(nweb_id, INVALID_BLANKLESS_KEY);
  m_dump_time_map_.Erase(nweb_id, INVALID_BLANKLESS_KEY);
  {
    std::lock_guard<std::mutex> lck(m_nweb_status_map_mtx_);
    m_nweb_status_map_.erase(nweb_id);
  }
  std::lock_guard<std::mutex> window_id_map_lck(m_window_id_map_mtx_);
  m_window_id_map_.erase(nweb_id);
}

bool BlanklessController::CheckEnableForSysUrl(const std::string& url, uint64_t& blankless_key)
{
  return m_white_list_.CheckSysWhiteList(url, blankless_key);
}

#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
bool BlanklessController::CheckEnableForAppUrl(const std::string& url, uint64_t& blankless_key)
{
  return m_white_list_.CheckAppWhiteList(url, blankless_key);
}
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)

bool BlanklessController::CheckGlobalProperty()
{
  static bool BlankOptEnableFlag =
      OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBoolParameter("web.blankless.enabled", false);
  return BlankOptEnableFlag;
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
             expected_status.status_code == info.status_code;
  return ret;
}

void BlanklessController::RecordWindowId(uint32_t nweb_id, uint32_t window_id) {
  std::lock_guard<std::mutex> lck(m_window_id_map_mtx_);
  m_window_id_map_.emplace(nweb_id, window_id);
}

uint32_t BlanklessController::GetWindowIdByNWebId(uint32_t nweb_id) {
  std::lock_guard<std::mutex> lck(m_window_id_map_mtx_);
  auto it = m_window_id_map_.find(nweb_id);
  if (it == m_window_id_map_.end()) {
    return 0;
  }
  return it->second;
}
}  // namespace ohos
}  // namespace base
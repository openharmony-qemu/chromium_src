// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_request_info_cache_mgr.h"

#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "base/logging.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"

namespace {
constexpr base::TimeDelta CHECK_FLUSH_TO_DISK_TIMEOUT = base::Milliseconds(200);
const std::string PARAM_REQUEST_INFO_TYPE = "request_info_type";
const std::string PARAM_PAGE_ORIGIN = "page_origin";
const std::string PARAM_PAGE_INDEX = "page_index";
const std::string REQUEST_HEADERS_COOKIE = "Cookie";
const std::string PRIVACY_TAG = "ac/";
constexpr int32_t MIN_INDEX_TO_DELETE_DYNAMIC_REQUEST = 3;
}  // namespace

namespace ohos_prp_preload {
bool DoNotNeedToStore(const std::shared_ptr<PRRequestInfo>& info, int64_t page_index)
{
  if (page_index < MIN_INDEX_TO_DELETE_DYNAMIC_REQUEST) {
    return false;
  }
  std::string preload_seq_num = info->preload_seq_num();
  return !info->preload_seq_num().ends_with(std::to_string(page_index - 1) + ", ");
}

bool ParseResReqInfoList(LinkedHashMap& info_list,
    base::Value::List& list, PRPPreloadMode mode, int64_t page_index, bool is_preconnect_limit_list)
{
  bool need_store = false;
  if (is_preconnect_limit_list) {
    for (auto precommect_limit_info : info_list) {
      if (precommect_limit_info.second->limit_num() <= 0) {
        continue;
      }
      base::Value::Dict dict;
      list.Append(std::move(dict));
      DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnectLimit(precommect_limit_info.second,
          list.back().GetDict());
      need_store = true;
    }
    return need_store;
  }
  for (auto info : info_list) {
    if (mode == PRPPreloadMode::PRELOAD &&
        (((info.second->preload_flag() & PRPP_FLAGS_VISIBLE) != PRPP_FLAGS_VISIBLE) ||
        (DoNotNeedToStore(info.second, page_index)))) {
      continue;
    }
    base::Value::Dict dict;
    list.Append(std::move(dict));
    DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnect(info.second, list.back().GetDict());
    if (mode == PRPPreloadMode::PRELOAD) {
      DiskCacheInfoParser::ParseResReqPreloadInfoForPreload(info.second, list.back().GetDict());
    }
    need_store = true;
  }
  return need_store;
}

bool ResReqPreloadInfoListToJson(const std::string& page_origin,
    LinkedHashMap& preload_info_list,
    LinkedHashMap& load_prelaod_info_list,
    LinkedHashMap& precommect_limit_list,
    int64_t page_index,
    std::string& entry_content) {
  bool need_store = false;
  base::Value::List list;
  PRPPreloadMode mode =
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode();
  if (mode == PRPPreloadMode::PRELOAD && page_origin.empty()) {
    LOG(DEBUG) << "PRPPreload.ResReqPreloadInfoListToJson, page_origin empty";
    return false;
  }
  base::Value::Dict dict;
  dict.Set(PARAM_REQUEST_INFO_TYPE, static_cast<int>(PRRequestInfoType::TYPE_PAGE_ORIGIN));
  dict.Set(PARAM_PAGE_INDEX, std::to_string(page_index));
  if (mode == PRPPreloadMode::PRELOAD) {
    dict.Set(PARAM_PAGE_ORIGIN, page_origin);
  }
  list.Append(std::move(dict));
  need_store |= ParseResReqInfoList(preload_info_list, list, mode, 0, false);
  need_store |= ParseResReqInfoList(load_prelaod_info_list, list, mode, page_index, false);
  need_store |= ParseResReqInfoList(precommect_limit_list, list, mode, 0, true);

  bool write_success = base::JSONWriter::Write(list, &entry_content);
  if (!write_success) {
    LOG(WARNING) << "PRPPreload.ResReqPreloadInfoListToJson failed";
    need_store = false;
  }
  LOG(DEBUG) << "PRPPreload.ResReqPreloadInfoListToJson size:" << list.size() <<
    ", need_store:" << need_store;
  return need_store;
}

void ParseJson(base::Value& json_value,
    LinkedHashMap& prelaod_info_list,
    LinkedHashMap& preconnect_limit_info_list,
    int64_t& page_index)
{
  base::Value::List::iterator it = json_value.GetList().begin();
  if (it == json_value.GetList().end()) {
    return;
  }
  PRPPreloadMode mode =
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode();
  std::shared_ptr<PRRequestInfo> origin_info = std::make_shared<PRRequestInfo>();
  if (!DiskCacheInfoParser::GetRequestInfoTypeFromJson(*it, PARAM_REQUEST_INFO_TYPE, origin_info) ||
      (origin_info->type() != PRRequestInfoType::TYPE_PAGE_ORIGIN)) {
    LOG(DEBUG) << "PRPPreload.JsonToResReqPreloadInfoList json parse request_info_type failed";
    return;
  }
  if (!DiskCacheInfoParser::GetPageIndexFromJson(*it, PARAM_PAGE_INDEX, origin_info)) {
    LOG(DEBUG) << "PRPPreload.JsonToResReqPreloadInfoList json parse page_index failed";
    return;
  }
  page_index = origin_info->page_index() + 1;
  if (mode == PRPPreloadMode::PRELOAD &&
      !DiskCacheInfoParser::GetPageOriginFromJson(*it, PARAM_PAGE_ORIGIN, origin_info)) {
    LOG(DEBUG) << "PRPPreload.JsonToResReqPreloadInfoList json parse page_origin failed";
    mode = PRPPreloadMode::PRECONNECT;
  }
  prelaod_info_list.insert({origin_info->url().spec(), origin_info});
  for (base::Value::List::iterator json_item = it++; json_item != json_value.GetList().end(); ++json_item) {
    std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
    if (!DiskCacheInfoParser::GetRequestInfoTypeFromJson(*json_item, PARAM_REQUEST_INFO_TYPE, info)) {
      continue;
    }
    if (info->type() == PRRequestInfoType::TYPE_PAGE_PRECONNECT_LIMIT) {
      if (DiskCacheInfoParser::ParseJsonForPreconnectLimit(*json_item, info)) {
        std::string key;
        ResReqInfoCacheMgr::GetPreconnectLimitListKey(info->url(), info->allow_credentials(), key);
        preconnect_limit_info_list.insert({key, info});
      }
      continue;
    }
    if (!DiskCacheInfoParser::ParseJsonForPreconnect(*json_item, info)) {
      continue;
    }
    if (mode == PRPPreloadMode::PRELOAD && !DiskCacheInfoParser::ParseJsonForPreload(*json_item, info)) {
      continue;
    }
    prelaod_info_list.insert({info->url().spec(), info});
  }
}

void JsonToResReqPreloadInfoList(const std::string& content,
    LinkedHashMap& prelaod_info_list,
    LinkedHashMap& preconnect_limit_info_list,
    int64_t& page_index)
{
  page_index += 1;
  if (content.empty()) {
    prelaod_info_list.clear();
    preconnect_limit_info_list.clear();
    return;
  }
  absl::optional<base::Value> json_value = base::JSONReader::Read(content);
  if (!json_value || !json_value->is_list()) {
    LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList failed";
    return;
  }
  ParseJson(json_value.value(), prelaod_info_list, preconnect_limit_info_list, page_index);
  LOG(DEBUG) << "PRPPreload.JsonToResReqPreloadInfoList size:" << prelaod_info_list.size();
}

ResReqInfoCacheMgr::ResReqInfoCacheMgr(const std::string& url,
    const net::NetworkAnonymizationKey& networkAnonymizationKey,
    const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
    const ResRequestInfoCacheLoadedCB& info_cache_cb) :
  networkAnonymizationKey_(networkAnonymizationKey), info_cache_loaded_cb_(info_cache_cb)
{
  disk_cache_ = base::WrapRefCounted(new (std::nothrow) DiskCacheFile(disk_cache_backend_factory, url,
    base::BindRepeating(&ResReqInfoCacheMgr::OnEntryLoadedCallback, weak_factory_.GetWeakPtr())));
  if (disk_cache_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResReqInfoCacheMgr::ResReqInfoCacheMgr new DiskCacheFile failed";
  }
}

void ResReqInfoCacheMgr::Start() {
  if ((disk_cache_ == nullptr) || (load_preload_info_list_.size() > 0) || is_start_) {
    return;
  }
  is_start_ = true;
  disk_cache_->LoadInfoAsync();
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
    base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), false),
    CHECK_FLUSH_TO_DISK_TIMEOUT);
}

void ResReqInfoCacheMgr::UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (disk_cache_ == nullptr || !is_start_ || IsRequestWithCookie(info)) {
    return;
  }
  bool checkflush_immediately = false;
  if (info->url().has_username() || info->url().has_password() || info->url().has_ref()) {
    GURL origin_url;
    GetPreconnectOriginUrl(info, origin_url);
    info->set_url(origin_url);
    info->or_preload_flag(PRPP_FLAGS_UNSUPPORT);
  }
  std::string key = info->url().spec();
  if ((info->type() == PRRequestInfoType::TYPE_PAGE_PREFLIGHT) &&
       !key.starts_with(PRPP_PREFLIGHT_PREFIX)) {
    key = std::string(PRPP_PREFLIGHT_PREFIX) + key;
  }
  auto it = new_preload_info_list_.find(info->url().spec());
  if (it != new_preload_info_list_.end()) {
    checkflush_immediately = true;
  } else {
    UpdatePreloadInfo(key, info);
  }
  checkflush_immediately |= UpdatePreconnectInfo(info);
  if (checkflush_immediately) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE,
      base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), checkflush_immediately));
  }
}

bool ResReqInfoCacheMgr::UpdatePreconnectInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (!info->is_preconnect() && !info->is_reused_sokcet()) {
    return false;
  }
  GURL origin_url;
  std::string key;
  ResReqInfoCacheMgr::GetPreconnectOriginUrl(info, origin_url);
  ResReqInfoCacheMgr::GetPreconnectLimitListKey(origin_url, info->allow_credentials(), key);
  auto it_new = new_preconnect_limit_list_.find(key);
  if (it_new != new_preconnect_limit_list_.end()) {
    it_new->second->add_preconnect_num(info->is_preconnect());
    it_new->second->add_reused_preconnect_num(info->is_preconnect() && info->is_reused_sokcet());
    it_new->second->compute_limit_num();
    return true;
  }
  std::shared_ptr<ohos_prp_preload::PRRequestInfo> preconnect_limit_info =
    std::make_shared<ohos_prp_preload::PRRequestInfo>(origin_url, info->allow_credentials());
  preconnect_limit_info->set_type(PRRequestInfoType::TYPE_PAGE_PRECONNECT_LIMIT);
  preconnect_limit_info->add_preconnect_num(info->is_preconnect());
  preconnect_limit_info->add_reused_preconnect_num(info->is_preconnect() && info->is_reused_sokcet());
  preconnect_limit_info->compute_limit_num();
  (void)new_preconnect_limit_list_.emplace(key, preconnect_limit_info);
  return false;
}

void ResReqInfoCacheMgr::UpdatePreloadInfo(const std::string& key, const std::shared_ptr<PRRequestInfo>& info)
{
  auto it = load_preload_info_list_.find(key);
  if (it != load_preload_info_list_.end()) {
    std::string preload_seq_num = it->second->preload_seq_num();
    size_t last_comma_pos = preload_seq_num.rfind(',');
    if (last_comma_pos != std::string::npos) {
      size_t second_last_comma_pos = preload_seq_num.rfind(',', last_comma_pos - 1);
      if (second_last_comma_pos != std::string::npos) {
        preload_seq_num = preload_seq_num.substr(second_last_comma_pos + 1, last_comma_pos - second_last_comma_pos)
          + " ";
      }
    }
    info->set_preload_seq_num(preload_seq_num + std::to_string(page_index_) + ", ");
    (void)load_preload_info_list_.erase(it);
  } else {
    info->set_preload_seq_num(std::to_string(page_index_) + ", ");
  }
  (void)new_preload_info_list_.insert({key, info});
}

void ResReqInfoCacheMgr::GetPreconnectLimitListKey(const GURL& origin_url, bool allow_credentials, std::string& key)
{
  key = allow_credentials ? PRIVACY_TAG + origin_url.spec() : origin_url.spec();
}

void ResReqInfoCacheMgr::GetPreconnectOriginUrl(const std::shared_ptr<PRRequestInfo>& info, GURL& origin_url)
{
  if (info->type() == PRRequestInfoType::TYPE_PAGE_PREFLIGHT && info->url().spec().starts_with(PRPP_PREFLIGHT_PREFIX)) {
    origin_url = url::Origin::Create(GURL(info->url().spec().substr(strlen(PRPP_PREFLIGHT_PREFIX)))).GetURL();
  } else {
    origin_url = url::Origin::Create(info->url()).GetURL();
  }
}

void ResReqInfoCacheMgr::SetPageOrigin(const std::string& page_origin) {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  page_origin_ = page_origin;
}

void ResReqInfoCacheMgr::Stop() {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  is_start_ = false;
  std::string entry_content;
  if ((last_flush_len_ < new_preload_info_list_.size()) &&
      ResReqPreloadInfoListToJson(page_origin_, new_preload_info_list_, load_preload_info_list_,
          new_preconnect_limit_list_, page_index_, entry_content)) {
    disk_cache_->StoreInfoAsync(entry_content);
  }
  load_preload_info_list_.clear();
  new_preload_info_list_.clear();
  new_preconnect_limit_list_.clear();
  load_preconnect_limit_list_.clear();
  last_flush_len_ = 0;
}

void ResReqInfoCacheMgr::OnEntryLoadedCallback(const std::string& entry_content) {
  if (load_preload_info_list_.size() > 0 || !is_start_) {
    return;
  }
  JsonToResReqPreloadInfoList(entry_content, load_preload_info_list_, load_preconnect_limit_list_, page_index_);
  int64_t load_page_index = page_index_ - 1;
  std::string page_seq_num = (load_page_index == 1) ? (std::to_string(load_page_index) + ", ") :
    (std::to_string(load_page_index - 1) + ", " + std::to_string(load_page_index) + ", ");
  if (!info_cache_loaded_cb_.is_null()) {
    info_cache_loaded_cb_.Run(load_preload_info_list_, load_preconnect_limit_list_,
        networkAnonymizationKey_, page_seq_num);
  }
}

void ResReqInfoCacheMgr::CheckFlush(bool checkflush_immediately) {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  std::string entry_content;
  if (((last_flush_len_ < new_preload_info_list_.size()) || checkflush_immediately) &&
      ResReqPreloadInfoListToJson(page_origin_, new_preload_info_list_, load_preload_info_list_,
          new_preconnect_limit_list_, page_index_, entry_content)) {
    disk_cache_->StoreInfoAsync(entry_content);
    last_flush_len_ = new_preload_info_list_.size();
  }
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
    base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), false),
    CHECK_FLUSH_TO_DISK_TIMEOUT);
}

bool ResReqInfoCacheMgr::IsRequestWithCookie(const std::shared_ptr<PRRequestInfo>& info)
{
  return info->extra_request_headers().HasHeader(REQUEST_HEADERS_COOKIE);
}
}  // namespace ohos_prp_preload

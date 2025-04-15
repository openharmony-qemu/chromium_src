// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/res_request_info_cache_mgr.h"

#include "base/logging.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "services/network/prp_preload/include/page_res_parallel_preload_mgr.h"

namespace {
constexpr base::TimeDelta CHECK_FLUSH_TO_DISK_TIMEOUT = base::Milliseconds(200);
const std::string PARAM_REQUEST_INFO_TYPE = "request_info_type";
const std::string PARAM_PAGE_ORIGIN = "page_origin";
const std::string REQUEST_HEADERS_COOKIE = "Cookie";
}  // namespace

namespace ohos_prp_preload {
bool ResReqPreloadInfoListToJson(const std::string& page_origin,
    const std::list<std::shared_ptr<PRRequestInfo>>& info_list,
    std::string& entry_content) {
  bool need_store = false;
  entry_content = "";
  base::Value::List list;
  PRPPreloadMode mode =
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode();
  if (mode == PRPPreloadMode::PRELOAD) {
    if (!page_origin.empty()) {
      base::Value::Dict dict;
      dict.Set(PARAM_PAGE_ORIGIN, page_origin);
      dict.Set(PARAM_REQUEST_INFO_TYPE, static_cast<int>(PRRequestInfoType::TYPE_PAGE_ORIGIN));
      list.Append(std::move(dict));
    } else {
      LOG(WARNING) << "PRPPreload.ResReqPreloadInfoListToJson, page_origin empty";
      return false;
    }
  }

  for (std::shared_ptr<PRRequestInfo> info : info_list) {
    if (mode == PRPPreloadMode::PRELOAD && ((info->preload_flag() & PRPP_FLAGS_VISIBLE) != PRPP_FLAGS_VISIBLE)) {
      continue;
    }
    base::Value::Dict dict;
    list.Append(std::move(dict));
    DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnect(info, list.back().GetDict());
    if (mode == PRPPreloadMode::PRELOAD) {
      DiskCacheInfoParser::ParseResReqPreloadInfoForPreload(info, list.back().GetDict());
    }
    need_store = true;
  }

  bool write_success = base::JSONWriter::Write(list, &entry_content);
  if (!write_success) {
    LOG(WARNING) << "PRPPreload.ResReqPreloadInfoListToJson failed";
    need_store = false;
  }
  LOG(DEBUG) << "PRPPreload.ResReqPreloadInfoListToJson size:" << list.size() <<
    ", need_store:" << need_store;
  return need_store;
}

void JsonToResReqPreloadInfoList(const std::string& content,
    std::list<std::shared_ptr<PRRequestInfo>>& info_list) {
  if (content.empty()) {
    info_list.clear();
    return;
  }
  absl::optional<base::Value> json_value = base::JSONReader::Read(content);
  if (!json_value || !json_value->is_list()) {
    LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList failed";
    return;
  }
  base::Value::List::iterator it = json_value->GetList().begin();
  if (it == json_value->GetList().end()) {
    return;
  }
  PRPPreloadMode mode =
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode();
  std::shared_ptr<PRRequestInfo> origin_info = std::make_shared<PRRequestInfo>();
  if (!DiskCacheInfoParser::GetRequestInfoTypeFromJson(*it, PARAM_REQUEST_INFO_TYPE, origin_info) &&
      mode == PRPPreloadMode::PRELOAD) {
    LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList failed to parse incorrect format json";
    mode = PRPPreloadMode::PRECONNECT;
  }

  if (origin_info->type() == PRRequestInfoType::TYPE_PAGE_ORIGIN && mode == PRPPreloadMode::PRELOAD) {
    const std::string* page_origin = (*it).GetDict().FindString(PARAM_PAGE_ORIGIN);
    if (!page_origin) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList json parse page_origin failed";
      mode = PRPPreloadMode::PRECONNECT;
    }
    origin_info->set_page_origin(*page_origin);
    origin_info->set_only_send_reuse_request(true);
    info_list.push_back(origin_info);
    it++;
  }

  for (base::Value::List::iterator json_item = it;
       json_item != json_value->GetList().end(); ++json_item) {
    std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
    if (!DiskCacheInfoParser::ParseJsonForPreconnect(*json_item, info)) {
      continue;
    }
    if (mode == PRPPreloadMode::PRELOAD && !DiskCacheInfoParser::ParseJsonForPreload(*json_item, info)) {
      continue;
    }
    info_list.push_back(info);
  }
  LOG(DEBUG) << "PRPPreload.JsonToResReqPreloadInfoList size:" << info_list.size();
}

ResReqInfoCacheMgr::ResReqInfoCacheMgr(const std::string& url,
    const net::NetworkAnonymizationKey& networkAnonymizationKey,
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
    const ResRequestInfoCacheLoadedCB& info_cache_cb) :
    networkAnonymizationKey_(networkAnonymizationKey),
    sth_task_runner_(sth_task_runner), info_cache_loaded_cb_(info_cache_cb) {
  disk_cache_ = base::WrapRefCounted(new (std::nothrow) DiskCacheFile(disk_cache_backend_factory, url,
    base::BindRepeating(&ResReqInfoCacheMgr::OnEntryLoadedCallback, weak_factory_.GetWeakPtr())));
  if (disk_cache_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResReqInfoCacheMgr::ResReqInfoCacheMgr new DiskCacheFile failed";
  }
}

void ResReqInfoCacheMgr::Start() {
  if ((disk_cache_ == nullptr) || (load_info_list_.size() > 0) || is_start_) {
    return;
  }
  is_start_ = true;
  disk_cache_->LoadInfoAsync();
  if (sth_task_runner_ != nullptr) {
    sth_task_runner_->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), false),
      CHECK_FLUSH_TO_DISK_TIMEOUT);
  }
}

void ResReqInfoCacheMgr::UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (disk_cache_ == nullptr || !is_start_ || IsRequestWithCookie(info)) {
    return;
  }
  std::string url_without_params = info->url().spec();
  bool is_url_with_params = false;
  // find the begin index of params in request url
  size_t index = url_without_params.find('?');
  if (index != url_without_params.npos) {
    url_without_params = url_without_params.substr(0, index);
    info->set_url(GURL(url_without_params));
    is_url_with_params = true;
  }
  if (((info->preload_flag() & PRPP_FLAGS_UNSUPPORT) == PRPP_FLAGS_UNSUPPORT)) {
    std::list<std::shared_ptr<PRRequestInfo>>::iterator info_it =
      std::find(new_info_list_.begin(), new_info_list_.end(), info);
    if (info_it != new_info_list_.end() && sth_task_runner_ != nullptr) {
      (*info_it)->or_preload_flag(ohos_prp_preload::PRPP_FLAGS_UNSUPPORT);
      sth_task_runner_->PostTask(FROM_HERE,
        base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), true));
      return;
    }
  }

  if (is_url_with_params) {
    info->or_preload_flag(PRPP_FLAGS_UNSUPPORT);
  }
  new_info_list_.push_back(info);
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
  if ((last_flush_len_ < new_info_list_.size()) &&
      ResReqPreloadInfoListToJson(page_origin_, new_info_list_, entry_content)) {
    disk_cache_->StoreInfoAsync(entry_content);
  }
  load_info_list_.clear();
  new_info_list_.clear();
  last_flush_len_ = 0;
}

void ResReqInfoCacheMgr::OnEntryLoadedCallback(const std::string& entry_content) {
  if (load_info_list_.size() > 0 || !is_start_) {
    return;
  }
  JsonToResReqPreloadInfoList(entry_content, load_info_list_);
  if (!info_cache_loaded_cb_.is_null()) {
    info_cache_loaded_cb_.Run(load_info_list_, networkAnonymizationKey_);
  }
}

void ResReqInfoCacheMgr::CheckFlush(bool checkflush_immediately) {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  std::string entry_content;
  if (((last_flush_len_ < new_info_list_.size()) || checkflush_immediately) &&
      ResReqPreloadInfoListToJson(page_origin_, new_info_list_, entry_content)) {
    disk_cache_->StoreInfoAsync(entry_content);
    last_flush_len_ = new_info_list_.size();
  }
  if (sth_task_runner_ != nullptr) {
    sth_task_runner_->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr(), false),
      CHECK_FLUSH_TO_DISK_TIMEOUT);
  }
}

bool ResReqInfoCacheMgr::IsRequestWithCookie(const std::shared_ptr<PRRequestInfo>& info) {
  std::string cookie_value;
  return info->extra_request_headers().GetHeader(REQUEST_HEADERS_COOKIE, &cookie_value);
}
}  // namespace ohos_prp_preload

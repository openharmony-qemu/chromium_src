// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "res_request_info_cache_mgr.h"

#include "base/logging.h"
#include "url/origin.h"

namespace {
static constexpr base::TimeDelta CHECK_FLUSH_TO_DISK_TIMEOUT = base::Milliseconds(200);
}  // namespace

namespace ohos_prp_preload {
void ResReqPreloadInfoListToJson(const std::list<std::shared_ptr<PRRequestInfo>>& info_list,
                                 std::string& entry_content) {
  entry_content = "";
  base::Value::List list;
  for (auto info : info_list) {
    base::Value::Dict dict;
    dict.Set("url", url::Origin::Create(info->url()).GetURL().spec());
    dict.Set("allow_credentials", info->allow_credentials());
    auto cache_info = info->cache_info();
    dict.Set("cache_type", static_cast<int>(cache_info.cache_type));
    dict.Set("freshness_life_times", std::to_string(cache_info.freshness_life_times));
    dict.Set("e_tag", cache_info.e_tag);
    dict.Set("last_modified", cache_info.last_modified);
    list.Append(std::move(dict));
  }
  bool write_success = base::JSONWriter::Write(list, &entry_content);
  if (!write_success) {
    LOG(ERROR) << "PRPPreload.ResReqPreloadInfoListToJson failed";
  }
}

void JsonToResReqPreloadInfoList(const std::string& json,
                                 std::list<std::shared_ptr<PRRequestInfo>>& info_list) {
  absl::optional<base::Value> json_value = base::JSONReader::Read(json);
  if (!json_value || !json_value->is_list()) {
    LOG(ERROR) << "PRPPreload.JsonToResReqPreloadInfoList failed";
    return;
  }

  for (const auto& json_item : json_value->GetList()) {
    auto info = std::make_shared<PRRequestInfo>();
    info->set_url(GURL(*json_item.GetDict().FindString("url")));
    info->set_allow_credentials(*json_item.GetDict().FindBool("allow_credentials"));
    info->set_cache_type(static_cast<PRRequestCacheType>(*json_item.GetDict().FindInt("cache_type")));
    info->set_freshness_life_times(std::stoll(*json_item.GetDict().FindString("freshness_life_times")));
    info->set_e_tag(*json_item.GetDict().FindString("e_tag"));
    info->set_last_modified(*json_item.GetDict().FindString("last_modified"));
    info_list.push_back(info);
  }
}
ResReqInfoCacheMgr::ResReqInfoCacheMgr(const std::string& url,
  const net::NetworkAnonymizationKey& networkAnonymizationKey,
  const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
  const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
  const ResRequestInfoCacheLoadedCB& info_cache_cb) :
    url_(url), networkAnonymizationKey_(networkAnonymizationKey),
    sth_task_runner_(sth_task_runner), info_cache_loaded_cb_(info_cache_cb) {
      disk_cache_ = base::WrapRefCounted(new (std::nothrow) DiskCacheFile(disk_cache_backend_factory, url,
        base::BindRepeating(&ResReqInfoCacheMgr::OnEntryLoadedCallback, weak_factory_.GetWeakPtr())));
      if (disk_cache_ == nullptr) {
        LOG(ERROR) << "PRPPreload.ResReqInfoCacheMgr::ResReqInfoCacheMgr new DiskCacheFile failed";
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
      base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr()),
      CHECK_FLUSH_TO_DISK_TIMEOUT);
  }
}

void ResReqInfoCacheMgr::UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  new_info_list_.push_back(info);
}

void ResReqInfoCacheMgr::Stop() {
  if (disk_cache_ == nullptr || !is_start_) {
    return;
  }
  is_start_ = false;
  if (last_flush_len_ < new_info_list_.size()) {
    std::string entry_content;
    ResReqPreloadInfoListToJson(new_info_list_, entry_content);
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

void ResReqInfoCacheMgr::CheckFlush() {
  if (!is_start_) {
    LOG(DEBUG) << "PRPPreload.ResReqInfoCacheMgr::CheckFlush no need to flush" << url_;
    return;
  }
  if (last_flush_len_ < new_info_list_.size()) {
    std::string entry_content;
    ResReqPreloadInfoListToJson(new_info_list_, entry_content);
    disk_cache_->StoreInfoAsync(entry_content);
    last_flush_len_ = new_info_list_.size();
  }
  if (sth_task_runner_ != nullptr) {
    sth_task_runner_->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResReqInfoCacheMgr::CheckFlush, weak_factory_.GetWeakPtr()),
      CHECK_FLUSH_TO_DISK_TIMEOUT);
  }
}
}  // namespace ohos_prp_preload

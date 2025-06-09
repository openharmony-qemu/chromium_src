/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
  std::optional<base::Value> json_value = base::JSONReader::Read(json);
  if (!json_value || !json_value->is_list()) {
    LOG(ERROR) << "PRPPreload.JsonToResReqPreloadInfoList failed";
    return;
  }

  for (const auto& json_item : json_value->GetList()) {
    auto info = std::make_shared<PRRequestInfo>();
    const std::string* url = json_item.GetDict().FindString("url");
    if (!url || url->empty()) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList url is invalid";
      continue;
    }
    info->set_url(GURL(*url));
    const std::optional<bool> allow_credentials = json_item.GetDict().FindBool("allow_credentials");
    if (!allow_credentials.has_value()) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList allow_credentials is invalid";
      continue;
    }
    info->set_allow_credentials(allow_credentials.value());
    const std::optional<int> cache_type = json_item.GetDict().FindInt("cache_type");
    if (!cache_type.has_value() ||
        cache_type.value() < PRRequestCacheType::NEGOTIATION_CACHE ||
        cache_type.value() > PRRequestCacheType::DISABLE_CACHE) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList cache_type is invalid";
      continue;
    }
    info->set_cache_type(static_cast<PRRequestCacheType>(cache_type.value()));
    const std::string* freshness_life_times_str = json_item.GetDict().FindString("freshness_life_times");
    if (!freshness_life_times_str || freshness_life_times_str->empty()) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList freshness_life_times is none";
      continue;
    }
    char* end = nullptr;
    errno = 0;
    int64_t freshness_life_times = std::strtoll(freshness_life_times_str->c_str(), &end, 10);
    if (errno != 0 || !end || *end) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList freshness_life_times is invalid";
      continue;
    }
    info->set_freshness_life_times(freshness_life_times);
    const std::string* e_tag = json_item.GetDict().FindString("e_tag");
    if (!e_tag) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList e_tag is invalid";
      continue;
    }
    info->set_e_tag(*e_tag);
    const std::string* last_modified = json_item.GetDict().FindString("last_modified");
    if (!last_modified) {
      LOG(WARNING) << "PRPPreload.JsonToResReqPreloadInfoList last_modified is invalid";
      continue;
    }
    info->set_last_modified(*last_modified);
    info_list.push_back(info);
  }
}

ResReqInfoCacheMgr::ResReqInfoCacheMgr(
    const std::string& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
    const ResRequestInfoCacheLoadedCB& info_cache_cb) :
    url_(url), network_anonymization_key_(network_anonymization_key),
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
    info_cache_loaded_cb_.Run(load_info_list_, network_anonymization_key_);
  }
}

void ResReqInfoCacheMgr::CheckFlush() {
  if (!is_start_) {
    LOG(INFO) << "PRPPreload.ResReqInfoCacheMgr::CheckFlush no need to flush" << url_;
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

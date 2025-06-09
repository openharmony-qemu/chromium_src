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

#ifndef NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H
#define NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H

#include <mutex>
#include "url/gurl.h"

namespace ohos_prp_preload {
enum PRRequestCacheType {
  NEGOTIATION_CACHE,
  FORCE_CACHE,
  DISABLE_CACHE,
};

struct CacheInfo {
  PRRequestCacheType cache_type = PRRequestCacheType::DISABLE_CACHE;
  int64_t freshness_life_times = 0;
  std::string e_tag;
  std::string last_modified;
};

class PRRequestInfo {
 public:
  PRRequestInfo() = default;
  PRRequestInfo(const GURL& url, bool allow_credentials) :
      url_(url), allow_credentials_(allow_credentials) {}
  ~PRRequestInfo() = default;

  const GURL& url() const { return url_; }
  void set_url(const GURL& url) { url_ = url; }
  bool allow_credentials() const { return allow_credentials_; }
  void set_allow_credentials(bool allow_credentials) {
    allow_credentials_ = allow_credentials;
  }
  PRRequestCacheType cache_type() const { return cache_type_; }
  void set_cache_type(PRRequestCacheType cache_type) { cache_type_ = cache_type; }
  int64_t freshness_life_times() const { return freshness_life_times_; }
  void set_freshness_life_times(int64_t freshness_life_times) {
    freshness_life_times_ = freshness_life_times;
  }
  const std::string& e_tag() const { return e_tag_; }
  void set_e_tag(const std::string& e_tag) { e_tag_ = e_tag; }
  const std::string& last_modified() const { return last_modified_; }
  void set_last_modified(const std::string& last_modified) {
    last_modified_ = last_modified;
  }

  // for multi-thread read/write scenarios
  CacheInfo cache_info() {
    std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
    return CacheInfo {cache_type_, freshness_life_times_, e_tag_, last_modified_};
  }
  void set_cache_info(PRRequestCacheType cache_type,
                      int64_t freshness_life_times,
                      const std::string& e_tag,
                      const std::string& last_modified) {
    std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
    cache_type_ = cache_type;
    freshness_life_times_ = freshness_life_times;
    e_tag_ = e_tag;
    last_modified_ = last_modified;
  }

 private:
 std::mutex cache_info_mutex_;
  GURL url_ = GURL::EmptyGURL();
  bool allow_credentials_ = false;
  PRRequestCacheType cache_type_ = PRRequestCacheType::DISABLE_CACHE;
  int64_t freshness_life_times_ = 0;
  std::string e_tag_;
  std::string last_modified_;
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H

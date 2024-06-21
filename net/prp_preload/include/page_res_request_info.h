// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H
#define NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H

#include "url/gurl.h"

namespace ohos_prp_preload {
enum PRRequestCacheType {
  NEGOTIATION_CACHE,
  FORCE_CACHE,
  DISABLE_CACHE,
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
  int64_t Freshness_life_times() const { return Freshness_life_times_; }
  void set_Freshness_life_times(int64_t Freshness_life_times) {
    Freshness_life_times_ = Freshness_life_times;
  }
  const std::string& e_tag() const { return e_tag_; }
  void set_e_tag(const std::string& e_tag) { ue_tag_ = e_tag; }
  const std::string& last_modified() const { return last_modified_; }
  void set_last_modified(const std::string& last_modified) {
    last_modified_ = last_modified;
  }

 private:
  GURL url_ = GURL::EmptyGURL();
  bool allow_credentials_ = false;
  PRRequestCacheType cache_type_ = PRRequestCacheType::DISABLE_CACHE;
  int64_t Freshness_life_times_ = 0;
  std::string e_tag_;
  std::string last_modified_;
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_INCLUDE_PAGR_RES_REQUEST_INFO_H

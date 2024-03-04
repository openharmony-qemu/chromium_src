// Copyright (c) 2024 Huawei Device Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cookie_shared_memory_holder.h"

#include "base/functional/bind.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/cookies/cookie_util.h"
#include "services/network/restricted_cookie_manager.h"

namespace network {

CookieSharedMemoryHolder::CookieSharedMemoryHolder(
    net::CookieStore* cookie_store,
    const RestrictedCookieManager* restricted_cookie_manager,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    bool has_storage_access,
    const absl::optional<net::CookiePartitionKey>& cookie_partition_key,
    net::CookieOptions options,
    mojo::ScopedSharedBufferMapping mapping,
    bool same_party_attribute_enabled)
    : cookie_store_(cookie_store),
      restricted_cookie_manager_(restricted_cookie_manager),
      url_(url),
      site_for_cookies_(site_for_cookies),
      top_frame_origin_(top_frame_origin),
      has_storage_access_(has_storage_access),
      options_(options),
      mapping_(std::move(mapping)),
      same_party_attribute_enabled_(same_party_attribute_enabled) {
  cookie_store_subscription_ =
      cookie_store->GetChangeDispatcher().AddCallbackForUrl(
          url, cookie_partition_key,
          base::BindRepeating(&CookieSharedMemoryHolder::OnCookieChange,
                              base::Unretained(this)));
  }

CookieSharedMemoryHolder::~CookieSharedMemoryHolder() {
}

const GURL& CookieSharedMemoryHolder::url() {
  return url_;
}

const mojo::ScopedSharedBufferMapping& CookieSharedMemoryHolder::mapping() {
  return mapping_;
}

void CookieSharedMemoryHolder::OnCookieChange(
    const net::CookieChangeInfo& change) {
  bool delegate_treats_url_as_trustworthy =
      cookie_store_->cookie_access_delegate() &&
      cookie_store_->cookie_access_delegate()->ShouldTreatUrlAsTrustworthy(
          url_);

  net::CookieSamePartyStatus same_party_status =
      net::cookie_util::GetSamePartyStatus(change.cookie, options_,
                                            same_party_attribute_enabled_);
  if (!change.cookie
            .IncludeForRequestURL(
                url_, options_,
                net::CookieAccessParams{change.access_result.access_semantics,
                                        delegate_treats_url_as_trustworthy,
                                        same_party_status})
            .status.IsInclude()) {
    return;
  }

  if (!restricted_cookie_manager_->cookie_settings().IsCookieAccessible(
          change.cookie, url_, site_for_cookies_, top_frame_origin_,
          restricted_cookie_manager_->GetCookieSettingOverrides(
              has_storage_access_))) {
    return;
  }
  bool* cookie_changed(static_cast<bool*>(mapping_.get()));
  *cookie_changed = true;
}
} // namespace network
// Copyright (c) 2024 Huawei Device Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_COOKIE_SHARED_MEMORY_HOLDER_H
#define SERVICES_NETWORK_COOKIE_SHARED_MEMORY_HOLDER_H

#include "base/containers/linked_list.h"
#include "mojo/public/cpp/system/buffer.h"
#include "net/cookies/cookie_change_dispatcher.h"
#include "net/cookies/cookie_store.h"
#include "net/cookies/site_for_cookies.h"
#include "services/network/cookie_settings.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom.h"

namespace network {
class RestrictedCookieManager;
class CookieSharedMemoryHolder
    : public base::LinkNode<CookieSharedMemoryHolder> {
 public:
  CookieSharedMemoryHolder(
      net::CookieStore* cookie_store,
      const RestrictedCookieManager* restricted_cookie_manager,
      const GURL& url,
      const net::SiteForCookies& site_for_cookies,
      const url::Origin& top_frame_origin,
      bool has_storage_access,
      const absl::optional<net::CookiePartitionKey>& cookie_partition_key,
      net::CookieOptions options,
      mojo::ScopedSharedBufferMapping mapping,
      bool same_party_attribute_enabled);
  CookieSharedMemoryHolder(const CookieSharedMemoryHolder&) = delete;
  CookieSharedMemoryHolder& operator=(const CookieSharedMemoryHolder&) = delete;

  ~CookieSharedMemoryHolder();

  const GURL& url();

  const mojo::ScopedSharedBufferMapping& mapping();

 private:
  void OnCookieChange(const net::CookieChangeInfo& change);
  raw_ptr<net::CookieStore> cookie_store_;

  // Raw pointer usage is safe because RestrictedCookieManager owns this
  // instance and is guaranteed to outlive it.
  const raw_ptr<const RestrictedCookieManager> restricted_cookie_manager_;

  // The URL whose cookies this listener is interested in.
  const GURL url_;

  // Site context in which we're used; used to determine if a cookie is accessed
  // in a third-party context.
  const net::SiteForCookies site_for_cookies_;

  // Site context in which we're used; used to check content settings.
  const url::Origin top_frame_origin_;

  // Whether the Listener has storage access. Note that if a listener is created
  // from a document that has not called `document.requestStorageAccess()`, and
  // the script later calls `document.requestStorageAccess()` to obtain storage
  // access, this listener's state will not be updated.
  const bool has_storage_access_;

  // CanonicalCookie::IncludeForRequestURL options for this listener's interest.
  const net::CookieOptions options_;

  // The CookieChangeDispatcher sunscription used by this listener.
  std::unique_ptr<net::CookieChangeSubscription> cookie_store_subscription_;

  mojo::ScopedSharedBufferMapping mapping_;

  const bool same_party_attribute_enabled_;
};
} // namespace network
#endif // SERVICES_NETWORK_COOKIE_SHARED_MEMORY_HOLDER_H
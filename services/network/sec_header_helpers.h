// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_SEC_HEADER_HELPERS_H_
#define SERVICES_NETWORK_SEC_HEADER_HELPERS_H_

#include <map>

#include "base/component_export.h"
#include "services/network/public/mojom/fetch_api.mojom-forward.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace net {
class URLRequest;
}  // namespace net

namespace network {

namespace cors {
class OriginAccessList;
}  // namespace cors

namespace mojom {
class URLLoaderFactoryParams;
}  // namespace mojom

// Sets the right Sec-Fetch-Site request header on |request|, comparing the
// origins of |request.url_chain()| and |pending_redirect_url| against
// |request.initiator()|.
//
// Note that |pending_redirect_url| is optional - it should be set only when
// calling this method from net::URLRequest::Delegate::OnReceivedRedirect (in
// this case |request.url_chain()| won't yet contain the URL being redirected
// to).
//
// Spec: https://w3c.github.io/webappsec-fetch-metadata/
COMPONENT_EXPORT(NETWORK_SERVICE)
void SetFetchMetadataHeaders(
    net::URLRequest* request,
    network::mojom::RequestMode mode,
    bool has_user_activation,
    network::mojom::RequestDestination dest,
    const GURL* pending_redirect_url,
    const mojom::URLLoaderFactoryParams& factory_params,
    const cors::OriginAccessList& origin_access_list);

// Removes any sec-ch- or sec-fetch- prefixed request headers on the |request|
// if the |pending_redirect_url| is not trustworthy and the current url is.
COMPONENT_EXPORT(NETWORK_SERVICE)
void MaybeRemoveSecHeaders(net::URLRequest* request,
                           const GURL& pending_redirect_url);

#ifdef OHOS_NETWORK_LOAD
COMPONENT_EXPORT(NETWORK_SERVICE)
std::map<std::string, std::string> GetFetchMetadataHeaders(
     const GURL& target_url,
     network::mojom::RequestMode mode,
     bool has_user_activation,
     network::mojom::RequestDestination dest,
     const absl::optional<url::Origin>& initiator);
#endif

}  // namespace network

#endif  // SERVICES_NETWORK_SEC_HEADER_HELPERS_H_

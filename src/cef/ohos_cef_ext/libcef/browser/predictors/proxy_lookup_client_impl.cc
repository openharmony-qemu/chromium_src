// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/proxy_lookup_client_impl.h"

#include <utility>

#include "base/functional/bind.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "net/proxy_resolution/proxy_info.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "url/gurl.h"

namespace ohos_predictors {

ProxyLookupClientImpl::ProxyLookupClientImpl(
    const GURL& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    ProxyLookupCallback callback,
    network::mojom::NetworkContext* network_context)
    : callback_(std::move(callback)) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  network_context->LookUpProxyForURL(url, network_anonymization_key,
                                     receiver_.BindNewPipeAndPassRemote());
  receiver_.set_disconnect_handler(
      base::BindOnce(&ProxyLookupClientImpl::OnProxyLookupComplete,
                     base::Unretained(this), net::ERR_ABORTED, absl::nullopt));
}

ProxyLookupClientImpl::~ProxyLookupClientImpl() = default;

void ProxyLookupClientImpl::OnProxyLookupComplete(
    int32_t net_error,
    const absl::optional<net::ProxyInfo>& proxy_info) {
  bool success = proxy_info.has_value() && !proxy_info->is_direct();
  std::move(callback_).Run(success);
}

}  // namespace ohos_predictors

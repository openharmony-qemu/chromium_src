// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/resolve_host_client_impl.h"

#include <utility>

#include "base/functional/bind.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/host_port_pair.h"
#include "net/base/net_errors.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/request_priority.h"
#include "net/dns/public/resolve_error_info.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "url/gurl.h"

namespace ohos_predictors {

ResolveHostClientImpl::ResolveHostClientImpl(
    const GURL& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    ResolveHostCallback callback,
    network::mojom::NetworkContext* network_context)
    : callback_(std::move(callback)) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  network::mojom::ResolveHostParametersPtr parameters =
      network::mojom::ResolveHostParameters::New();
  parameters->initial_priority = net::RequestPriority::IDLE;
  parameters->is_speculative = true;
  network_context->ResolveHost(
      network::mojom::HostResolverHost::NewSchemeHostPort(
          url::SchemeHostPort(url)),
      network_anonymization_key, std::move(parameters),
      receiver_.BindNewPipeAndPassRemote());
  receiver_.set_disconnect_handler(base::BindOnce(
      &ResolveHostClientImpl::OnConnectionError, base::Unretained(this)));
}

ResolveHostClientImpl::~ResolveHostClientImpl() = default;

void ResolveHostClientImpl::OnComplete(
    int32_t result,
    const net::ResolveErrorInfo& resolve_error_info,
    const net::AddressList& resolved_addresses,
    const std::vector<net::HostResolverEndpointResult>&
        endpoint_results_with_metadata) {
  std::move(callback_).Run(result == net::OK);
}

void ResolveHostClientImpl::OnConnectionError() {
  std::move(callback_).Run(false);
}

}  // namespace ohos_predictors

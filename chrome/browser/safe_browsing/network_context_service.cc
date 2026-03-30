// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/safe_browsing/network_context_service.h"

#include "base/feature_list.h"
#include "chrome/browser/browser_features.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/profiles/profile.h"

namespace safe_browsing {

NetworkContextService::NetworkContextService(Profile* profile) {
  network_context_ = std::make_unique<SafeBrowsingNetworkContext>(
      profile->GetPath(),
      base::FeatureList::IsEnabled(features::kTriggerNetworkDataMigration),
      base::BindRepeating(&NetworkContextService::CreateNetworkContextParams,
                          // This is safe because `this` owns
                          // `network_context_`.
                          base::Unretained(this)));
#if !BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  proxy_config_monitor_ = std::make_unique<ProxyConfigMonitor>(profile);
#endif // ARKWEB_ARKWEB_EXTENSIONS
}

NetworkContextService::~NetworkContextService() = default;

void NetworkContextService::Shutdown() {
  network_context_->ServiceShuttingDown();
}

network::mojom::NetworkContext* NetworkContextService::GetNetworkContext() {
  // The SafeBrowsingNetworkContext cannot operate correctly without a
  // SystemNetworkContextManager instance, so return early if it does not exist.
  if (!SystemNetworkContextManager::GetInstance())
    return nullptr;
  return network_context_->GetNetworkContext();
}

scoped_refptr<network::SharedURLLoaderFactory>
NetworkContextService::GetURLLoaderFactory() {
  return network_context_->GetURLLoaderFactory();
}

void NetworkContextService::FlushNetworkInterfaceForTesting() {
  network_context_->FlushForTesting();
}

network::mojom::NetworkContextParamsPtr
NetworkContextService::CreateNetworkContextParams() {
  auto params = SystemNetworkContextManager::GetInstance()
                    ->CreateDefaultNetworkContextParams();
#if !BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  proxy_config_monitor_->AddToNetworkContextParams(params.get());
#endif // ARKWEB_ARKWEB_EXTENSIONS
  return params;
}

}  // namespace safe_browsing

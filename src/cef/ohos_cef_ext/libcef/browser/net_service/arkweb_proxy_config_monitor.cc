/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ohos_cef_ext/libcef/browser/net_service/arkweb_proxy_config_monitor.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/barrier_closure.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/no_destructor.h"
#include "base/trace_event/trace_event.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/mojom/network_context.mojom.h"

namespace NWEB {

namespace {

const char kProxyServerSwitch[] = "proxy-server";
const char kProxyBypassListSwitch[] = "proxy-bypass-list";

constexpr net::NetworkTrafficAnnotationTag kProxyConfigTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("webview_proxy_config", R"(
      semantics {
        sender: "Proxy configuration via a command line flag"
        description:
          "Used to fetch HTTP/HTTPS/SOCKS5/PAC proxy configuration when "
          "proxy is configured by the --proxy-server command line flag. "
          "When proxy implies automatic configuration, it can send network "
          "requests in the scope of this annotation."
        trigger:
          "Whenever a network request is made when the system proxy settings "
          "are used, and they indicate to use a proxy server."
        data:
          "Proxy configuration."
        destination: OTHER
        destination_other: "The proxy server specified in the configuration."
      }
      policy {
        cookies_allowed: NO
        setting:
          "This request cannot be disabled in settings. However it will never "
          "be made if user does not run with the '--proxy-server' switch."
        policy_exception_justification:
          "Not implemented, behaviour only available behind a switch."
      })");

}  // namespace

ProxyConfigMonitor::ProxyConfigMonitor() {
  TRACE_EVENT0("startup", "ProxyConfigMonitor");
  proxy_config_service_ = std::make_unique<net::ProxyConfigServiceOHOS>(
      base::SingleThreadTaskRunner::GetCurrentDefault());
  proxy_config_service_->set_exclude_pac_url(true);
  proxy_config_service_->AddObserver(this);
}

ProxyConfigMonitor::~ProxyConfigMonitor() {
  proxy_config_service_->RemoveObserver(this);
}

ProxyConfigMonitor* ProxyConfigMonitor::GetInstance() {
  static base::NoDestructor<ProxyConfigMonitor> instance;
  return instance.get();
}

void ProxyConfigMonitor::AddProxyToNetworkContextParams(
    network::mojom::NetworkContextParams* network_context_params) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(kProxyServerSwitch)) {
    std::string proxy = command_line.GetSwitchValueASCII(kProxyServerSwitch);
    net::ProxyConfig proxy_config;
    proxy_config.proxy_rules().ParseFromString(proxy);
    if (command_line.HasSwitch(kProxyBypassListSwitch)) {
      std::string bypass_list =
          command_line.GetSwitchValueASCII(kProxyBypassListSwitch);
      proxy_config.proxy_rules().bypass_rules.ParseFromString(bypass_list);
    }

    network_context_params->initial_proxy_config =
        net::ProxyConfigWithAnnotation(proxy_config,
                                       kProxyConfigTrafficAnnotation);
  } else {
    mojo::PendingRemote<network::mojom::ProxyConfigClient> proxy_config_client;
    network_context_params->proxy_config_client_receiver =
        proxy_config_client.InitWithNewPipeAndPassReceiver();
    proxy_config_client_set_.Add(std::move(proxy_config_client));

    net::ProxyConfigWithAnnotation proxy_config;
    net::ProxyConfigService::ConfigAvailability availability =
        proxy_config_service_->GetLatestProxyConfig(&proxy_config);
    if (availability == net::ProxyConfigService::CONFIG_VALID) {
      network_context_params->initial_proxy_config = proxy_config;
    }
  }
}

void ProxyConfigMonitor::OnProxyConfigChanged(
    const net::ProxyConfigWithAnnotation& config,
    net::ProxyConfigService::ConfigAvailability availability) {
  for (const auto& proxy_config_client : proxy_config_client_set_) {
    switch (availability) {
      case net::ProxyConfigService::CONFIG_VALID:
        proxy_config_client->OnProxyConfigUpdated(config);
        break;
      case net::ProxyConfigService::CONFIG_UNSET:
        proxy_config_client->OnProxyConfigUpdated(
            net::ProxyConfigWithAnnotation::CreateDirect());
        break;
      case net::ProxyConfigService::CONFIG_PENDING:
        NOTREACHED();
    }
  }
}

std::string ProxyConfigMonitor::SetProxyOverride(
    const std::vector<net::ProxyConfigServiceOHOS::ProxyOverrideRule>&
        proxy_rules,
    const std::vector<std::string>& bypass_rules,
    const bool reverse_bypass,
    base::OnceClosure callback) {
  return proxy_config_service_->SetProxyOverride(
      proxy_rules, bypass_rules, reverse_bypass,
      base::BindOnce(&ProxyConfigMonitor::FlushProxyConfig,
                     base::Unretained(this), std::move(callback)));
}

void ProxyConfigMonitor::ClearProxyOverride(base::OnceClosure callback) {
  proxy_config_service_->ClearProxyOverride(
      base::BindOnce(&ProxyConfigMonitor::FlushProxyConfig,
                     base::Unretained(this), std::move(callback)));
}

void ProxyConfigMonitor::FlushProxyConfig(base::OnceClosure callback) {
  int count = proxy_config_client_set_.size();
  base::RepeatingClosure closure =
      base::BarrierClosure(count, std::move(callback));
  for (auto& proxy_config_client : proxy_config_client_set_) {
    proxy_config_client->FlushProxyConfig(closure);
  }
}

}  // namespace NWEB

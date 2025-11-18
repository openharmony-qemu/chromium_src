/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <iostream>
#include "extensions/browser/extension_pref_value_map.h"
#include "extensions/browser/extension_pref_value_map_factory.h"
#include "net/base/proxy_chain.h"
#include "net/base/scheme_host_port_matcher.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_proxy_types.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_proxy_cef_delegate.h"

namespace {

NWebExtensionScheme ConvertProxyServerScheme(net::ProxyServer::Scheme scheme) {
  switch (scheme) {
  case net::ProxyServer::Scheme::SCHEME_HTTP:
    return HTTP;
  case net::ProxyServer::Scheme::SCHEME_SOCKS4:
    return SOCKS4;
  case net::ProxyServer::Scheme::SCHEME_SOCKS5:
    return SOCKS5;
  case net::ProxyServer::Scheme::SCHEME_HTTPS:
    return HTTPS;
  case net::ProxyServer::Scheme::SCHEME_QUIC:
    return QUIC;
  default:
    return INVALID;
  }
}

std::optional<NWebExtensionProxyServer> ProxyServerToNWebExtensionProxyServer(
    const std::vector<net::ProxyChain>& chains) {
  if (!chains.empty()) {
    net::ProxyChain chain = chains.front();
    const net::ProxyServer& server = chain.First();
    NWebExtensionProxyServer proxy_server;
    proxy_server.scheme = ConvertProxyServerScheme(server.scheme());
    proxy_server.host = server.GetHost();
    proxy_server.port = server.GetPort();
    return proxy_server;
  }
  return std::nullopt;
}

void ProxyConfigToProxyInfo(
    const std::string& extension_id,
    int mode,
    const net::ProxyConfig& config,
    NWebExtensionProxyInfo& info) {
  if (extension_id == std::string()) {
    info.enable = false;
    info.effective = std::nullopt;
    return;
  }

  NWebExtensionEffectiveProxyInfo effective;
  effective.extensionId = extension_id;
  effective.mode = (NWebExtensionProxyMode)mode;
  if (effective.mode == PAC_SCRIPT) {
    NWebExtensionPacScript pac;
    pac.mandatory = config.pac_mandatory();
    pac.url = config.pac_url().possibly_invalid_spec();
    effective.pacScript = pac;
  }

  if (effective.mode == FIXED_SERVERS) {
    NWebExtensionProxyRules rules;
    const net::SchemeHostPortMatcher::RuleList& list = config.proxy_rules().bypass_rules.rules();
    for (const auto& rule : list) {
      rules.bypassList.push_back(rule->ToString());
    }

    if (config.proxy_rules().type == net::ProxyConfig::ProxyRules::Type::PROXY_LIST) {
      rules.singleProxy =
          ProxyServerToNWebExtensionProxyServer(config.proxy_rules().single_proxies.AllChains());
    } else if (config.proxy_rules().type == net::ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME) {
      rules.proxyForHttp =
          ProxyServerToNWebExtensionProxyServer(config.proxy_rules().proxies_for_http.AllChains());
      rules.proxyForHttps =
          ProxyServerToNWebExtensionProxyServer(config.proxy_rules().proxies_for_https.AllChains());
      rules.proxyForFtp =
          ProxyServerToNWebExtensionProxyServer(config.proxy_rules().proxies_for_ftp.AllChains());
      rules.fallbackProxy =
          ProxyServerToNWebExtensionProxyServer(config.proxy_rules().fallback_proxies.AllChains());
    }
    effective.rules = rules;
  }

  info.enable = true;
  info.effective = effective;
}

void NotifyProxyInfo(Profile* profile, const net::ProxyConfigWithAnnotation& pref_config) {
  if (profile && !profile->IsOffTheRecord()) {
    ExtensionPrefValueMap* pref_value_map =
        ExtensionPrefValueMapFactory::GetForBrowserContext(profile);
    if (pref_value_map) {
      std::string extension_id = pref_value_map->GetExtensionControllingPref(proxy_config::prefs::kProxy);
      NWebExtensionProxyInfo info;
      if (pref_config.pref_proxy_mode() >= (int)DIRECT &&
          pref_config.pref_proxy_mode() <= (int)SYSTEM) {
        ProxyConfigToProxyInfo(extension_id, pref_config.pref_proxy_mode(), pref_config.value(), info);
        OHOS::NWeb::NWebExtensionProxyCefDelegate::NotifyProxyInfo(info);
      } else {
        LOG(ERROR) << "invalid proxy mode:" << pref_config.pref_proxy_mode();
      }
    }
  }
}

} // namespace

ProxyConfigServiceImpl::ProxyConfigServiceImpl(
    std::unique_ptr<net::ProxyConfigService> base_service,
    ProxyPrefs::ConfigState initial_config_state,
    const net::ProxyConfigWithAnnotation& initial_config,
    Profile* profile)
    : base_service_(std::move(base_service)),
      pref_config_state_(initial_config_state),
      pref_config_(initial_config),
      profile_(profile),
      registered_observer_(false) {
  NotifyProxyInfo(profile_, pref_config_);
  // ProxyConfigServiceImpl is created on the UI thread, but used on the network
  // thread.
  thread_checker_.DetachFromThread();
}

void ProxyConfigServiceImpl::UpdateProxyConfig(
    ProxyPrefs::ConfigState config_state,
    const net::ProxyConfigWithAnnotation& config) {
  DCHECK(thread_checker_.CalledOnValidThread());
  pref_config_state_ = config_state;
  pref_config_ = config;

  if (observers_.empty()) {
    return;
  }

  NotifyProxyInfo(profile_, pref_config_);

  // Evaluate the proxy configuration. If GetLatestProxyConfig returns
  // CONFIG_PENDING, we are using the system proxy service, but it doesn't have
  // a valid configuration yet. Once it is ready, OnProxyConfigChanged() will be
  // called and broadcast the proxy configuration.
  // Note: If a switch between a preference proxy configuration and the system
  // proxy configuration occurs an unnecessary notification might get send if
  // the two configurations agree. This case should be rare however, so we don't
  // handle that case specially.
  net::ProxyConfigWithAnnotation new_config;
  ConfigAvailability availability = GetLatestProxyConfig(&new_config);
  if (availability != CONFIG_PENDING) {
    for (net::ProxyConfigService::Observer& observer : observers_) {
      observer.OnProxyConfigChanged(new_config, availability);
    }
  }
}

std::unique_ptr<net::ProxyConfigService>
PrefProxyConfigTrackerImpl::CreateTrackingProxyConfigService(
    std::unique_ptr<net::ProxyConfigService> base_service,
    Profile* profile) {
  DCHECK(!proxy_config_service_impl_);
  proxy_config_service_impl_ = new ProxyConfigServiceImpl(
      std::move(base_service), active_config_state_,
      active_config_, profile);

  return std::unique_ptr<net::ProxyConfigService>(proxy_config_service_impl_);
}

bool PrefProxyConfigTrackerImpl::PrefConfigToNetConfig(
    const ProxyConfigDictionary& proxy_dict,
    net::ProxyConfigWithAnnotation* config) {
  ProxyPrefs::ProxyMode mode;
  if (!proxy_dict.GetMode(&mode)) {
    // Fall back to system settings if the mode preference is invalid.
    config->set_pref_proxy_mode(ProxyPrefs::MODE_SYSTEM);
    return false;
  }
  net::ProxyConfig proxy_config = config->value();
  switch (mode) {
    case ProxyPrefs::MODE_SYSTEM:
      // Use system settings.
      config->set_pref_proxy_mode(mode);
      return false;
    case ProxyPrefs::MODE_DIRECT:
      // Ignore all the other proxy config preferences if the use of a proxy
      // has been explicitly disabled.
      config->set_pref_proxy_mode(mode);
      return true;
    case ProxyPrefs::MODE_AUTO_DETECT:
      proxy_config.set_auto_detect(true);
      *config = net::ProxyConfigWithAnnotation(
          proxy_config, kSettingsProxyConfigTrafficAnnotation);
      config->set_pref_proxy_mode(mode);
      return true;
    case ProxyPrefs::MODE_PAC_SCRIPT: {
      std::string proxy_pac;
      if (!proxy_dict.GetPacUrl(&proxy_pac)) {
        LOG(ERROR) << "Proxy settings request PAC script but do not specify "
                   << "its URL. Falling back to direct connection.";
        config->set_pref_proxy_mode(ProxyPrefs::MODE_DIRECT);
        return true;
      }
      GURL proxy_pac_url(proxy_pac);
      if (!proxy_pac_url.is_valid()) {
        LOG(ERROR) << "Invalid proxy PAC url";
        config->set_pref_proxy_mode(ProxyPrefs::MODE_DIRECT);
        return true;
      }
      proxy_config.set_pac_url(proxy_pac_url);
      bool pac_mandatory = false;
      proxy_dict.GetPacMandatory(&pac_mandatory);
      proxy_config.set_pac_mandatory(pac_mandatory);
      *config = net::ProxyConfigWithAnnotation(
          proxy_config, kSettingsProxyConfigTrafficAnnotation);
      config->set_pref_proxy_mode(mode);
      return true;
    }
    case ProxyPrefs::MODE_FIXED_SERVERS: {
      std::string proxy_server;
      if (!proxy_dict.GetProxyServer(&proxy_server)) {
        LOG(ERROR) << "Proxy settings request fixed proxy servers but do not "
                   << "specify their URLs. Falling back to direct connection.";
        config->set_pref_proxy_mode(ProxyPrefs::MODE_DIRECT);
        return true;
      }

      bool allow_bracketed_proxy_chains = false;
      bool allow_quic_proxy_support = false;

#if BUILDFLAG(ENABLE_BRACKETED_PROXY_URIS)
      allow_bracketed_proxy_chains = true;
#endif  // BUILDFLAG(ENABLE_BRACKETED_PROXY_URIS)
#if BUILDFLAG(ENABLE_QUIC_PROXY_SUPPORT)
      allow_quic_proxy_support = true;
#endif  // BUILDFLAG(ENABLE_QUIC_PROXY_SUPPORT)

      proxy_config.proxy_rules().ParseFromString(
          proxy_server, allow_bracketed_proxy_chains, allow_quic_proxy_support);

      std::string proxy_bypass;
      if (proxy_dict.GetBypassList(&proxy_bypass)) {
        proxy_config.proxy_rules().bypass_rules.ParseFromString(proxy_bypass);
      }
      *config = net::ProxyConfigWithAnnotation(
          proxy_config, kSettingsProxyConfigTrafficAnnotation);
      config->set_pref_proxy_mode(mode);
      return true;
    }
    case ProxyPrefs::kModeCount: {
      // Fall through to NOTREACHED().
    }
  }
  NOTREACHED_IN_MIGRATION()
      << "Unknown proxy mode, falling back to system settings.";
  config->set_pref_proxy_mode(ProxyPrefs::MODE_SYSTEM);
  return false;
}

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
 *
 * Based on proxy_config_service_android.cc originally written by
 * Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "net/proxy_resolution/proxy_config_service_ohos.h"

#include "base/check_op.h"
#include "base/compiler_specific.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/task/sequenced_task_runner.h"
#include "net/base/host_port_pair.h"
#include "net/base/proxy_server.h"
#include "net/base/proxy_string_util.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "url/third_party/mozilla/url_parse.h"

namespace net {

namespace {

typedef ProxyConfigServiceOHOS::GetPropertyCallback GetPropertyCallback;

// Returns whether the provided string was successfully converted to a port.
bool ConvertStringToPort(const std::string& port, int* output) {
  url::Component component(0, port.size());
  int result = url::ParsePort(port.c_str(), component);
  if (result == url::PORT_INVALID || result == url::PORT_UNSPECIFIED) {
    return false;
  }
  *output = result;
  return true;
}

ProxyServer ConstructProxyServer(ProxyServer::Scheme scheme,
                                 const std::string& proxy_host,
                                 const std::string& proxy_port) {
  DCHECK(!proxy_host.empty());
  int port_as_int = 0;
  if (proxy_port.empty()) {
    port_as_int = ProxyServer::GetDefaultPortForScheme(scheme);
  } else if (!ConvertStringToPort(proxy_port, &port_as_int)) {
    return ProxyServer();
  }
  DCHECK(port_as_int > 0);
  return ProxyServer(
      scheme, HostPortPair(proxy_host, static_cast<uint16_t>(port_as_int)));
}

ProxyServer LookupProxy(const std::string& prefix,
                        const GetPropertyCallback& get_property,
                        ProxyServer::Scheme scheme) {
  DCHECK(!prefix.empty());
  std::string proxy_host = get_property.Run(prefix + ".proxyHost");
  if (!proxy_host.empty()) {
    std::string proxy_port = get_property.Run(prefix + ".proxyPort");
    return ConstructProxyServer(scheme, proxy_host, proxy_port);
  }

  // Fall back to default proxy, if any.
  proxy_host = get_property.Run("proxyHost");
  if (!proxy_host.empty()) {
    std::string proxy_port = get_property.Run("proxyPort");
    return ConstructProxyServer(scheme, proxy_host, proxy_port);
  }
  return ProxyServer();
}

ProxyServer LookupSocksProxy(const GetPropertyCallback& get_property) {
  std::string proxy_host = get_property.Run("socksProxyHost");
  if (!proxy_host.empty()) {
    std::string proxy_port = get_property.Run("socksProxyPort");
    return ConstructProxyServer(ProxyServer::SCHEME_SOCKS5, proxy_host,
                                proxy_port);
  }
  return ProxyServer();
}

void AddBypassRules(const std::string& scheme,
                    const GetPropertyCallback& get_property,
                    ProxyBypassRules* bypass_rules) {
  // The format of a hostname pattern is a list of hostnames that are separated
  // by | and that use * as a wildcard. For example, setting the
  // http.nonProxyHosts property to *.huawei.com|*.kernel.org will cause
  // requests to http://developer.huawei.com to be made without a proxy.
  std::string non_proxy_hosts = get_property.Run(scheme + ".nonProxyHosts");
  if (non_proxy_hosts.empty()) {
    return;
  }
  base::StringTokenizer tokenizer(non_proxy_hosts, ",");
  while (tokenizer.GetNext()) {
    std::string token = tokenizer.token();
    std::string pattern;
    base::TrimWhitespaceASCII(token, base::TRIM_ALL, &pattern);
    if (pattern.empty()) {
      continue;
    }
    // '?' is not one of the specified pattern characters above.
    DCHECK_EQ(std::string::npos, pattern.find('?'));
    bypass_rules->AddRuleFromString(scheme + "://" + pattern);
  }
}

// Returns true if a valid proxy was found.
bool GetProxyRules(const GetPropertyCallback& get_property,
                   ProxyConfig::ProxyRules* rules) {
  // There is one intentional  difference: by default Chromium uses the HTTP
  // port (80) for HTTPS connections via proxy. This default is identical on
  // other platforms. On the opposite, Java spec suggests to use HTTPS port
  // (443) by default (the default value of https.proxyPort).
  rules->type = ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME;
  rules->proxies_for_http.SetSingleProxyServer(
      LookupProxy("http", get_property, ProxyServer::SCHEME_HTTP));
  rules->proxies_for_https.SetSingleProxyServer(
      LookupProxy("https", get_property, ProxyServer::SCHEME_HTTP));
  rules->proxies_for_ftp.SetSingleProxyServer(
      LookupProxy("ftp", get_property, ProxyServer::SCHEME_HTTP));
  rules->fallback_proxies.SetSingleProxyServer(LookupSocksProxy(get_property));
  rules->bypass_rules.Clear();
  AddBypassRules("ftp", get_property, &rules->bypass_rules);
  AddBypassRules("http", get_property, &rules->bypass_rules);
  AddBypassRules("https", get_property, &rules->bypass_rules);
  AddBypassRules("ws", get_property, &rules->bypass_rules);
  AddBypassRules("wss", get_property, &rules->bypass_rules);
  // We know a proxy was found if not all of the proxy lists are empty.
  return !(
      rules->proxies_for_http.IsEmpty() && rules->proxies_for_https.IsEmpty() &&
      rules->proxies_for_ftp.IsEmpty() && rules->fallback_proxies.IsEmpty());
}

void GetLatestProxyConfigInternal(const GetPropertyCallback& get_property,
                                  ProxyConfigWithAnnotation* config) {
  ProxyConfig proxy_config;
  proxy_config.set_from_system(true);
  if (GetProxyRules(get_property, &proxy_config.proxy_rules())) {
    *config =
        ProxyConfigWithAnnotation(proxy_config, MISSING_TRAFFIC_ANNOTATION);
  } else {
    *config = ProxyConfigWithAnnotation::CreateDirect();
  }
}

std::string FixupProxyHostScheme(std::string host) {
  // Strip the scheme if any.
  std::string::size_type colon = host.find("://");
  if (colon != std::string::npos) {
    host = host.substr(colon + 3);
  }
  // If a username and perhaps password are specified, give a warning.
  std::string::size_type at_sign = host.find("@");
  // Should this be supported?
  if (at_sign != std::string::npos) {
    // ProxyConfig does not support authentication parameters, but Chrome
    // will prompt for the password later. Disregard the
    // authentication parameters and continue with this hostname.
    LOG(WARNING) << "Proxy authentication parameters ignored, see bug 16709";
    host = host.substr(at_sign + 1);
  }
  return host;
}

std::string GetProperty(const std::string& property) {
  // Use OH network to get configuration information.
  std::string host;
  uint16_t port;
  std::string exclusion;
  std::string pac_url;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .GetProperty(host, port, pac_url, exclusion);

  if (property == "http.proxyHost" || property == "https.proxyHost") {
    return FixupProxyHostScheme(host);
  } else if (property == "http.proxyPort" || property == "https.proxyPort") {
    return std::to_string(port);
  } else if (property == "http.nonProxyHosts" ||
             property == "https.nonProxyHosts" ||
             property == "ws.nonProxyHosts" ||
             property == "wss.nonProxyHosts") {
    return exclusion;
  }

  return std::string();
}

void CreateStaticProxyConfig(const std::string& host,
                             int port,
                             const std::string& pac_url,
                             const std::vector<std::string>& exclusion_list,
                             ProxyConfigWithAnnotation* config) {
  ProxyConfig proxy_config;
  if (!pac_url.empty()) {
    proxy_config.set_pac_url(GURL(pac_url));
    proxy_config.set_pac_mandatory(false);
    *config =
        ProxyConfigWithAnnotation(proxy_config, MISSING_TRAFFIC_ANNOTATION);
  } else if (port != 0 && !host.empty()) {
    std::string rules = base::StringPrintf("%s:%d", host.c_str(), port);
    proxy_config.proxy_rules().ParseFromString(rules);
    proxy_config.proxy_rules().bypass_rules.Clear();

    std::vector<std::string>::const_iterator it;
    for (it = exclusion_list.begin(); it != exclusion_list.end(); ++it) {
      std::string pattern;
      base::TrimWhitespaceASCII(*it, base::TRIM_ALL, &pattern);
      if (pattern.empty()) {
        continue;
      }
      proxy_config.proxy_rules().bypass_rules.AddRuleFromString(pattern);
    }
    *config =
        ProxyConfigWithAnnotation(proxy_config, MISSING_TRAFFIC_ANNOTATION);
  } else {
    *config = ProxyConfigWithAnnotation::CreateDirect();
  }
}

std::string ParseOverrideRules(
    const std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule>&
        override_rules,
    ProxyConfig::ProxyRules* proxy_rules) {
  // If no rules were specified, use DIRECT for everything.
  if (override_rules.empty()) {
    DCHECK(proxy_rules->empty());
    return "";
  }

  // Otherwise use a proxy list per URL scheme.
  proxy_rules->type = ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME;

  for (const auto& rule : override_rules) {
    // Parse the proxy URL.
    ProxyServer proxy_server =
        ProxyUriToProxyServer(rule.proxy_url, ProxyServer::Scheme::SCHEME_HTTP);
    if (!proxy_server.is_valid()) {
      return "Invalid Proxy URL: " + rule.proxy_url;
    } else if (proxy_server.is_quic()) {
      return "Unsupported proxy scheme: " + rule.proxy_url;
    }

    // Parse the URL scheme.
    if (base::EqualsCaseInsensitiveASCII(rule.url_scheme, "http")) {
      proxy_rules->proxies_for_http.AddProxyServer(proxy_server);
    } else if (base::EqualsCaseInsensitiveASCII(rule.url_scheme, "https")) {
      proxy_rules->proxies_for_https.AddProxyServer(proxy_server);
    } else if (rule.url_scheme == "*") {
      proxy_rules->fallback_proxies.AddProxyServer(proxy_server);
    } else {
      return "Unsupported URL scheme: " + rule.url_scheme;
    }
  }

  // If there is no per-URL scheme distinction simplify the ProxyRules.
  if (proxy_rules->proxies_for_http.IsEmpty() &&
      proxy_rules->proxies_for_https.IsEmpty() &&
      !proxy_rules->fallback_proxies.IsEmpty()) {
    proxy_rules->type = ProxyConfig::ProxyRules::Type::PROXY_LIST;
    std::swap(proxy_rules->single_proxies, proxy_rules->fallback_proxies);
  }

  return "";
}

std::string CreateOverrideProxyConfig(
    const std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule>& proxy_rules,
    const std::vector<std::string>& bypass_rules,
    const bool reverse_bypass,
    ProxyConfigWithAnnotation* config) {
  ProxyConfig proxy_config;
  auto result = ParseOverrideRules(proxy_rules, &proxy_config.proxy_rules());
  if (!result.empty()) {
    return result;
  }

  proxy_config.proxy_rules().reverse_bypass = reverse_bypass;

  for (const auto& bypass_rule : bypass_rules) {
    if (!proxy_config.proxy_rules().bypass_rules.AddRuleFromString(
            bypass_rule)) {
      return "Invalid bypass rule " + bypass_rule;
    }
  }
  *config = ProxyConfigWithAnnotation(proxy_config, MISSING_TRAFFIC_ANNOTATION);
  return "";
}

}  // namespace

class ProxyConfigServiceOHOS::Delegate
    : public base::RefCountedThreadSafe<Delegate> {
 public:
  Delegate(const scoped_refptr<base::SequencedTaskRunner>& main_task_runner,
           const GetPropertyCallback& get_property_callback)
      : main_task_runner_(main_task_runner),
        get_property_callback_(get_property_callback),
        exclude_pac_url_(false),
        has_proxy_override_(false) {}

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  void FetchInitialConfig() {
    ProxyConfigWithAnnotation proxy_config;
    GetLatestProxyConfigInternal(get_property_callback_, &proxy_config);
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&Delegate::SetNewConfigInMainSequence, this,
                                  proxy_config));
  }

  // Called only in the network sequence.
  void AddObserver(Observer* observer) {
    DCHECK(InMainSequence());
    observers_.AddObserver(observer);
  }

  void RemoveObserver(Observer* observer) {
    DCHECK(InMainSequence());
    observers_.RemoveObserver(observer);
  }

  ConfigAvailability GetLatestProxyConfig(ProxyConfigWithAnnotation* config) {
    DCHECK(InMainSequence());
    if (!config) {
      return ProxyConfigService::CONFIG_UNSET;
    }
    *config = proxy_config_;
    return ProxyConfigService::CONFIG_VALID;
  }

  // Called by NWEB adapter.
  void ProxySettingsChanged() {
    if (has_proxy_override_) {
      return;
    }

    ProxyConfigWithAnnotation proxy_config;
    GetLatestProxyConfigInternal(get_property_callback_, &proxy_config);
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&Delegate::SetNewConfigInMainSequence, this,
                                  proxy_config));
  }

  // Called by NWEB adapter.
  void ProxySettingsChangedTo(const std::string& host,
                              int port,
                              const std::string& pac_url,
                              const std::vector<std::string>& exclusion_list) {
    if (has_proxy_override_) {
      return;
    }

    ProxyConfigWithAnnotation proxy_config;
    if (exclude_pac_url_) {
      CreateStaticProxyConfig(host, port, "", exclusion_list, &proxy_config);
    } else {
      CreateStaticProxyConfig(host, port, pac_url, exclusion_list,
                              &proxy_config);
    }
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&Delegate::SetNewConfigInMainSequence, this,
                                  proxy_config));
  }

  void set_exclude_pac_url(bool enabled) { exclude_pac_url_ = enabled; }

  // Called by NWEB adapter.
  std::string SetProxyOverride(
      const std::vector<ProxyOverrideRule>& proxy_rules,
      const std::vector<std::string>& bypass_rules,
      const bool reverse_bypass,
      base::OnceClosure callback) {
    has_proxy_override_ = true;

    // Creates a new proxy config
    ProxyConfigWithAnnotation proxy_config;
    std::string result = CreateOverrideProxyConfig(
        proxy_rules, bypass_rules, reverse_bypass, &proxy_config);
    if (!result.empty()) {
      return result;
    }

    main_task_runner_->PostTaskAndReply(
        FROM_HERE,
        base::BindOnce(&Delegate::SetNewConfigInMainSequence, this,
                       proxy_config),
        std::move(callback));

    return "";
  }

  // Called by NWEB adapter.
  void ClearProxyOverride(base::OnceClosure callback) {
    if (!has_proxy_override_) {
      std::move(callback).Run();
      return;
    }

    ProxyConfigWithAnnotation proxy_config;
    GetLatestProxyConfigInternal(get_property_callback_, &proxy_config);
    main_task_runner_->PostTaskAndReply(
        FROM_HERE,
        base::BindOnce(&Delegate::SetNewConfigInMainSequence, this,
                       proxy_config),
        std::move(callback));
    has_proxy_override_ = false;
  }

 private:
  friend class base::RefCountedThreadSafe<Delegate>;

  virtual ~Delegate() {}

  // Called on the network sequence.
  void SetNewConfigInMainSequence(
      const ProxyConfigWithAnnotation& proxy_config) {
    DCHECK(InMainSequence());
    proxy_config_ = proxy_config;
    for (auto& observer : observers_) {
      observer.OnProxyConfigChanged(proxy_config,
                                    ProxyConfigService::CONFIG_VALID);
    }
  }

  bool InMainSequence() const {
    return main_task_runner_->RunsTasksInCurrentSequence();
  }

  base::ObserverList<Observer>::Unchecked observers_;
  scoped_refptr<base::SequencedTaskRunner> main_task_runner_;
  GetPropertyCallback get_property_callback_;
  ProxyConfigWithAnnotation proxy_config_;
  bool exclude_pac_url_;
  // This may only be accessed or modified on Nweb adapter
  bool has_proxy_override_;
};

void NetProxyEventCallback::Changed(
    const std::string& host,
    const uint16_t& port,
    const std::string& pacUrl,
    const std::vector<std::string>& exclusionList) {
  if (service_) {
    service_->ProxySettingsChangedTo(host, port, pacUrl, exclusionList);
  }
}

ProxyConfigServiceOHOS::ProxyConfigServiceOHOS(
    const scoped_refptr<base::SequencedTaskRunner>& main_task_runner)
    : delegate_(
          new Delegate(main_task_runner, base::BindRepeating(&GetProperty))) {
  delegate_->FetchInitialConfig();
  event_callback_ = std::make_shared<NetProxyEventCallback>(this);

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .RegNetProxyEvent(event_callback_);
}

ProxyConfigServiceOHOS::~ProxyConfigServiceOHOS() {}

void ProxyConfigServiceOHOS::set_exclude_pac_url(bool enabled) {
  delegate_->set_exclude_pac_url(enabled);
}

void ProxyConfigServiceOHOS::AddObserver(Observer* observer) {
  delegate_->AddObserver(observer);
}

void ProxyConfigServiceOHOS::RemoveObserver(Observer* observer) {
  delegate_->RemoveObserver(observer);
}

ProxyConfigService::ConfigAvailability
ProxyConfigServiceOHOS::GetLatestProxyConfig(
    ProxyConfigWithAnnotation* config) {
  return delegate_->GetLatestProxyConfig(config);
}

ProxyConfigServiceOHOS::ProxyConfigServiceOHOS(
    const scoped_refptr<base::SequencedTaskRunner>& main_task_runner,
    GetPropertyCallback get_property_callback)
    : delegate_(new Delegate(main_task_runner, get_property_callback)) {
  delegate_->FetchInitialConfig();
}

void ProxyConfigServiceOHOS::ProxySettingsChangedTo(
    const std::string& host,
    int port,
    const std::string& pac_url,
    const std::vector<std::string>& exclusion_list) {
  delegate_->ProxySettingsChangedTo(host, port, pac_url, exclusion_list);
}

void ProxyConfigServiceOHOS::ProxySettingsChanged() {
  delegate_->ProxySettingsChanged();
}

std::string ProxyConfigServiceOHOS::SetProxyOverride(
    const std::vector<ProxyOverrideRule>& proxy_rules,
    const std::vector<std::string>& bypass_rules,
    const bool reverse_bypass,
    base::OnceClosure callback) {
  return delegate_->SetProxyOverride(proxy_rules, bypass_rules, reverse_bypass,
                                     std::move(callback));
}

void ProxyConfigServiceOHOS::ClearProxyOverride(base::OnceClosure callback) {
  delegate_->ClearProxyOverride(std::move(callback));
}

}  // namespace net

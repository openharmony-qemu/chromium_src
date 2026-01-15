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
 * Based on proxy_config_service_android.h originally written by
 * Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H_
#define NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H_

#include <string>

#include "base/compiler_specific.h"
#include "base/functional/callback_forward.h"
#include "base/memory/ref_counted.h"
#include "base/memory/raw_ptr.h"
#include "base/synchronization/lock.h"
#include "net/base/net_export.h"
#include "net/proxy_resolution/proxy_config_service.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace base {
class SequencedTaskRunner;
}

namespace net {

class ProxyConfigWithAnnotation;
class NetProxyEventCallback;

class NET_EXPORT ProxyConfigServiceOHOS : public ProxyConfigService {
 public:
  // Callback that returns the value of the property identified by the provided
  // key. If it was not found, an empty string is returned. Note that this
  // interface does not let you distinguish an empty property from a
  // non-existing property.
  typedef base::RepeatingCallback<std::string(const std::string& property)>
      GetPropertyCallback;

  ProxyConfigServiceOHOS(
      const scoped_refptr<base::SequencedTaskRunner>& main_task_runner);

  ProxyConfigServiceOHOS(const ProxyConfigServiceOHOS&) = delete;
  ProxyConfigServiceOHOS& operator=(const ProxyConfigServiceOHOS&) = delete;

  ~ProxyConfigServiceOHOS() override;

  // OHOS provides a local HTTP proxy that does PAC resolution. When this
  // setting is enabled, the proxy config service ignores the PAC URL and uses
  // the local proxy for all proxy resolution.
  void set_exclude_pac_url(bool enabled);

  // ProxyConfigService:
  // Called only on the network thread.
  void AddObserver(Observer* observer) override;
  void RemoveObserver(Observer* observer) override;
  ConfigAvailability GetLatestProxyConfig(
      ProxyConfigWithAnnotation* config) override;

  // Holds a single proxy_url and the scheme for which it should be used.
  // If url_scheme is "*", this proxy will be used for all schemes.
  // The proxy_url is a string in the format: [scheme://] host [:port] for
  // a proxy or "direct://" for direct. Scheme is optional and will default to
  // HTTP; port is optional and defaults to 80 for HTTP, 443 for HTTPS and QUIC,
  // and 1080 for SOCKS. Host must be one of:
  // - IPv6 literal with brackets
  // - IPv4 literal
  // - A label or labels separated by periods
  struct ProxyOverrideRule {
    ProxyOverrideRule(const std::string& url_scheme,
                      const std::string& proxy_url)
        : url_scheme(url_scheme), proxy_url(proxy_url) {}

    std::string url_scheme;
    std::string proxy_url;
  };

  // Receives a list of ProxyOverrideRule and a list of strings for bypass
  // rules. Wildcards are accepted. URLs that match any pattern in any bypass
  // rule will go to DIRECT. "<local>" and "<-loopback>" are also accepted.
  // If no errors were found, returns an empty string, otherwise an UTF-8 string
  // with a description of the error that was found.
  // Callback is called (asynchronously) if input was valid.
  std::string SetProxyOverride(
      const std::vector<ProxyOverrideRule>& proxy_rules,
      const std::vector<std::string>& bypass_rules,
      const bool reverse_bypass,
      base::OnceClosure callback);
  void ClearProxyOverride(base::OnceClosure callback);

 private:
  // friend class ProxyConfigServiceOHOSTestBase
  friend class TestDelegate;
  friend class ProxyConfigServiceOHOSTest;
  class Delegate;
  friend class NetProxyEventCallback;

  // For tests.
  ProxyConfigServiceOHOS(
      const scoped_refptr<base::SequencedTaskRunner>& main_task_runner,
      GetPropertyCallback get_property_callback);

  // For tests.
  void ProxySettingsChanged();

  void ProxySettingsChangedTo(const std::string& host,
                              int port,
                              const std::string& pac_url,
                              const std::vector<std::string>& exclusion_list);

  scoped_refptr<Delegate> delegate_;
};

class NetProxyEventCallback : public OHOS::NWeb::NetProxyEventCallbackAdapter {
 public:
  static std::shared_ptr<NetProxyEventCallback> GetInstance();
  ~NetProxyEventCallback();

  void Changed(const std::string& host,
               const uint16_t& port,
               const std::string& pacUrl,
               const std::vector<std::string>& exclusionList) override;

  void AddObserver(ProxyConfigServiceOHOS*);

 private:
  NetProxyEventCallback() = default;
  base::Lock services_lock_;
  std::list<raw_ptr<ProxyConfigServiceOHOS>> services_;
};

}  // namespace net

#endif  // NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H_

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

#ifndef CEF_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_UA_CONFIG_H_
#define CEF_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_UA_CONFIG_H_
#pragma once
#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "base/base_export.h"
#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"
#include "base/memory/raw_ptr.h"
#include "base/no_destructor.h"

using UserAgentForHostsMap =
    base::flat_map<std::string, base::flat_set<std::string>>;
using HostForUserAgentMap = base::flat_map<std::string, std::string>;

enum UserAgentOverridePolicy {
  CUSTOM,  // Object-level UA set by developers via setCustomUserAgent, with the
           // highest priority
  APP_HOST,   // Domain-level UA set by developers (private interface
              // configuration has higher priority than public interface), with
              // the second-highest priority
  APP_CLOUD,  // Browser-side cloud-controlled UA (dynamically configured via
              // backend)
  APP_DEFAULT,  // Application-level UA set by developers (private interface
                // configuration has higher priority than public interface for
                // application-level UA)
  ARKWEB_DEFAULT,  // Default UA of ArkWeb framework, with the lowest priority
};

class AlloyBrowserUAConfig {
 public:
  static AlloyBrowserUAConfig* GetInstance();
  void SetAppCustomUserAgent(const std::string& user_agent);
  void SetUserAgentForHosts(const std::string& user_agent,
                            const std::vector<std::string>& hosts);
  UserAgentOverridePolicy MatchUserAgent(const std::string& host,
                                         std::string& user_agent);

 private:
  friend class base::NoDestructor<AlloyBrowserUAConfig>;
  AlloyBrowserUAConfig();
  ~AlloyBrowserUAConfig();
  std::string GetUAInHostList(const std::string& host);
  std::string app_custom_user_agent_;
  UserAgentForHostsMap user_agent_for_hosts_map_;
  HostForUserAgentMap host_for_user_agent_map_;
};
#endif  // ARKWEB_USERAGENT
#endif  // CEF_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_UA_CONFIG_H_

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

#include "libcef/browser/alloy/alloy_browser_ua_config.h"

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/logging.h"
#include "base/values.h"
#include "components/embedder_support/user_agent_utils.h"

namespace {
const size_t kMaxHostsPerUserAgent = 20000;
}

// static
AlloyBrowserUAConfig* AlloyBrowserUAConfig::GetInstance() {
  static base::NoDestructor<AlloyBrowserUAConfig> instance;
  return instance.get();
}

AlloyBrowserUAConfig::AlloyBrowserUAConfig() {}

AlloyBrowserUAConfig::~AlloyBrowserUAConfig() = default;

void AlloyBrowserUAConfig::SetAppCustomUserAgent(
    const std::string& user_agent) {
  if (app_custom_user_agent_ == user_agent) {
    return;
  }
  app_custom_user_agent_ = user_agent;
  LOG(DEBUG) << __func__ << " user_agent " << app_custom_user_agent_;
}

void AlloyBrowserUAConfig::SetUserAgentForHosts(
    const std::string& user_agent,
    const std::vector<std::string>& hosts) {
  std::vector<std::string> config_hosts_list(hosts);
  if (config_hosts_list.size() > kMaxHostsPerUserAgent) {
    config_hosts_list.resize(kMaxHostsPerUserAgent);
  }

  auto it = user_agent_for_hosts_map_.find(user_agent);
  if (it != user_agent_for_hosts_map_.end()) {
    auto previous_set = it->second;
    for (const auto& host : previous_set) {
      host_for_user_agent_map_.erase(host);
    }
  }

  base::flat_set<std::string> config_hosts(config_hosts_list.begin(),
                                           config_hosts_list.end());
  LOG(DEBUG) << __func__ << " user_agent " << user_agent
             << ", config_hosts.size " << config_hosts.size();
  for (const auto& host : config_hosts) {
    host_for_user_agent_map_[host] = user_agent;
  }
  user_agent_for_hosts_map_[user_agent] = config_hosts;
}

std::string AlloyBrowserUAConfig::GetUAInHostList(const std::string& host) {
  auto it = host_for_user_agent_map_.find(host);
  if (it != host_for_user_agent_map_.end()) {
    return it->second;
  }
  return std::string();
}

UserAgentOverridePolicy AlloyBrowserUAConfig::MatchUserAgent(
    const std::string& host,
    std::string& user_agent) {
  // 1. Matching the host list has the highest priority.
  user_agent = GetUAInHostList(host);
  if (!user_agent.empty()) {
    LOG(DEBUG) << __func__ << " in host list, host:"
               << url::LogUtils::ConvertUrlWithMask(host)
               << ", user_agent:" << user_agent;
    return UserAgentOverridePolicy::APP_HOST;
  }

  // 2. The default UA priority set by App custom.
  user_agent = app_custom_user_agent_;
  if (!user_agent.empty()) {
    LOG(DEBUG) << __func__ << " browser's default ua, host:"
               << url::LogUtils::ConvertUrlWithMask(host)
               << ", user_agent:" << user_agent;
    return UserAgentOverridePolicy::APP_DEFAULT;
  }

  user_agent = embedder_support::GetUserAgent();
  LOG(DEBUG) << __func__ << " arkweb's default ua, host:"
             << url::LogUtils::ConvertUrlWithMask(host)
             << ", user_agent:" << user_agent;
  return UserAgentOverridePolicy::ARKWEB_DEFAULT;
}
#endif  // ARKWEB_USERAGENT

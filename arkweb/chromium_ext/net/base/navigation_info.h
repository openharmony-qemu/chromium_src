/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_CHROMIUM_EXT_NET_BASE_NAVIGATION_INFO_H_
#define ARKWEB_CHROMIUM_EXT_NET_BASE_NAVIGATION_INFO_H_

#include <stdint.h>

#include <vector>

#include "arkweb/chromium_ext/net/base/request_attempt.h"
#include "arkweb/chromium_ext/net/dns/public/resolve_info.h"
#include "base/json/json_writer.h"
#include "net/base/net_errors.h"
#include "net/base/net_export.h"

namespace net {

// Navigation info collection fields for ArkWeb extension.
// This is really a struct.  All members are public.
struct NET_EXPORT NavigationInfo {
 public:
  NavigationInfo();
  NavigationInfo(const NavigationInfo& info);
  ~NavigationInfo();
  NavigationInfo& operator=(const NavigationInfo& info);

  // String members
  std::string request_url = "";
  std::string request_uuid = "";
  std::string time_stamp = "";
  std::string dns_name_servers = "";
  std::string local_address = "";

  // Integer members
  int32_t original_error_code = 0;
  int32_t error_code = 0;

  // Boolean members
  bool can_use_secure_dns = false;
  bool is_fallback_proxy_enabled = false;

  // Complex type members
  ResolveInfo resolve_info;
  std::vector<RequestAttempt> request_attempts = {};
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_BASE_NAVIGATION_INFO_H_

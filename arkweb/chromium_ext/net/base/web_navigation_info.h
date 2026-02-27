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

#ifndef ARKWEB_CHROMIUM_EXT_NET_BASE_WEB_NAVIGATION_INFO_H_
#define ARKWEB_CHROMIUM_EXT_NET_BASE_WEB_NAVIGATION_INFO_H_

#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/public/browser/error_page_reload_reason.h"
#include "arkweb/chromium_ext/net/base/navigation_info.h"
#include "net/base/net_errors.h"

namespace net {

class NET_EXPORT WebNavigationInfo {
 public:
  WebNavigationInfo();
  WebNavigationInfo(const WebNavigationInfo& info);
  ~WebNavigationInfo();
  WebNavigationInfo& operator=(const WebNavigationInfo& info);

  std::string page_trace_id;
  std::string original_url;

  int connection_type = 0;
  int threat_type = 0;
  int hw_code = 0;

  bool did_use_http_dns = false;
  bool did_use_fallback_proxy = false;
  bool is_captive_portal = false;
  bool is_auto_reload = false;
  bool has_ignore_certificate_error = false;

  content::ErrorPageReloadReason auto_reload_reason =
      content::ErrorPageReloadReason::INVALID;
  std::vector<std::string> insecure_dns_records;
  std::vector<std::string> secure_dns_records;

  net::NavigationInfo navigation_info;
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_BASE_WEB_NAVIGATION_INFO_H_

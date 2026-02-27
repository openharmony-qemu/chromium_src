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

#ifndef CONTENT_BROWSER_RENDER_HOST_NAIGATION_REQUEST_UTILS_H_
#define CONTENT_BROWSER_RENDER_HOST_NAIGATION_REQUEST_UTILS_H_

#include "arkweb/build/features/features.h"
#include "content/browser/renderer_host/navigation_request.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "arkweb/chromium_ext/net/base/web_navigation_info.h"
#endif

namespace {
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
constexpr char ORIGIN[] = "origin.DEFAULT";
#endif
}  // namespace

namespace content {

class NavigationRequest;

class NavigationRequestUtils {
public:
  raw_ptr<NavigationRequest> nav_request_;
  NavigationRequestUtils(NavigationRequest* nav_request);
  base::WeakPtr<NavigationRequestUtils> GetWeakPtr();

#if BUILDFLAG(ARKWEB_EXT_UA)
  void RemoveUserAgentHeaderForDevTools(bool devtools_useragent_override);
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  uint64_t addr_web_handle_;
  uint64_t GetAddrWebHandle() { return addr_web_handle_; }
  network::mojom::NetworkContext* GetNetworkContext() const;
  void StartPage(const net::NetworkAnonymizationKey networkAnonymizationKey, uint64_t addr_web_handle);
  void OnGetIsolation(const std::string& origin);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void SetAdblockEnabledStatus(blink::mojom::CommitNavigationParams* commit_params);
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool GetCustomScheme(const std::pair<url::Origin, std::string>& origin_and_debug_info);
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  void BeginNavigationImpl(bool& should_override_url_loading);
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  net::WebNavigationInfo& GetWebNavigationInfo();
  const net::WebNavigationInfo& GetWebNavigationInfo() const;
  void PopulateNavigationInfo(
      const std::optional<network::URLLoaderCompletionStatus>& status);
  bool IsAutoReload();
  void OnReportNewNavigationInfo(const std::string& page_trace_id);
  bool enable_nweb_ex_{false};
#endif
  base::WeakPtrFactory<NavigationRequestUtils> weak_factory_{this};
};
}
#endif

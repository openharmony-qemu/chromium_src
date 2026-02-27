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

#ifndef SERVICES_NETWORK_URL_LOADER_UTILS_H_
#define SERVICES_NETWORK_URL_LOADER_UTILS_H_

#include "base/memory/raw_ptr.h"
#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/time/time_utils.h"
#include "net/url_request/url_request.h"
#include "services/network/url_loader.h"
#include "services/network/public/cpp/net_adapters.h"

namespace network {
class URLLoader;

class URLLoaderUtils {
 public:
  raw_ptr<URLLoader> url_loader_;
  URLLoaderUtils(URLLoader* url_loader, const bool corb_detachable);
  const bool corb_detachable_;

#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
  std::string InMilliseconds(base::TimeTicks time);
  void PrintNetworkTimingInfo();
  void PrintNetworkCacheInfo();
  void PrintNetworkInfo();
  std::string GetRequestHttpVersion();
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void UpdateResRequestInfo(
      const std::string& key,
      const std::shared_ptr<ohos_prp_preload::PRRequestInfo>& info);

  // Configures `url_request_rollback_`, including registering callbacks.
  void ConfigureRequestForRollback(
      const GURL& url,
      std::string_view method,
      const net::SiteForCookies& site_for_cookies,
      bool force_ignore_site_for_cookies,
      const std::vector<GURL>& url_chain,
      const GURL& referrer,
      net::ReferrerPolicy referrer_policy,
      bool upgrade_if_insecure,
      bool is_ad_tagged,
      std::optional<net::IsolationInfo> isolation_info,
      bool force_main_frame_for_same_site_cookies,
      net::SecureDnsPolicy secure_dns_policy,
      net::HttpRequestHeaders extra_request_headers,
      const std::optional<std::vector<net::SourceStream::SourceType>>&
          accepted_stream_types,
      const std::optional<url::Origin>& initiator,
      net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy,
      int request_load_flags,
      bool priority_incremental,
      net::CookieSettingOverrides cookie_setting_overrides,
      std::optional<net::SharedDictionaryGetter> shared_dictionary_getter);

  void InitUrlRequestForRollback(
      URLLoaderContext& context,
      const ResourceRequest& request,
      const net::NetworkTrafficAnnotationTag& traffic_annotation,
      SharedDictionaryManager* shared_dictionary_manager,
      const std::string& org_main_url,
      std::shared_ptr<ohos_prp_preload::PRRequestInfo> preload_info);

  void SetUrlRequestForPRPP(
      const ResourceRequest& request,
      const std::shared_ptr<net::URLRequest>& url_request,
      const std::string& org_main_url,
      std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info);

  void RollbackFromPPRP();
  void ResetUrlRequest(const std::shared_ptr<net::URLRequest>& url_request);
  void SetPrppLoaderCallbacks(std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader);
  void ConfigureAndCreateURLRequest(
      const ResourceRequest& request,
      const net::NetworkTrafficAnnotationTag& traffic_annotation,
      const bool shared_storage_writable_eligible,
      std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info);
  void UpdatePreconnectInfo();
  void SetPreloadFlagUnSupport();
  void SetPreloadFlagVisible(const net::RequestPriority priority);
  void UpdateResponseTimes(
      mojom::URLResponseHeadPtr& response,
      net::LoadTimingInfo& load_timing_info
  );

  void GetResponseLoadingTime(mojom::URLResponseHeadPtr& response);
  void SetRequestStartTime(
      mojom::URLResponseHeadPtr& response,
      net::LoadTimingInfo& load_timing_info
  );

  void HandleRedirectUrl(const net::RedirectInfo& redirect_info);
  bool HandlePrppLoaderStartReplay(
    std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader,
    URLLoaderContext& context,
    const ResourceRequest& request,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    SharedDictionaryManager* shared_dictionary_manager,
    const std::string& org_main_url,
    std::shared_ptr<ohos_prp_preload::PRRequestInfo> preload_info,
    const bool shared_storage_writable_eligible);

  void CleanupAndRollback();
  void GetLoadTimeInfo(net::LoadTimingInfo* load_timing_info);
  int ReadDataFromLoaderOrRequest(scoped_refptr<NetToMojoIOBuffer> buf);

  std::shared_ptr<net::URLRequest> url_request_rollback_;
  std::shared_ptr<ohos_prp_preload::PRPPRequestLoader> prpp_loader_ { nullptr };
  bool redirect_updated_ { false };
  bool already_update_info_ { false };
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  std::optional<network::URLLoaderCompletionStatus>
  CreateURLLoaderCompletionStatus();
  void PopulateURLLoaderCompletionStatus(URLLoaderCompletionStatus& status);
#endif

  base::WeakPtrFactory<URLLoaderUtils> weak_ptr_factory_{this};
};
}  // namespace network
#endif

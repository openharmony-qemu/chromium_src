/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_ENGINE_IMPL_H
#define NWEB_ENGINE_IMPL_H

#include "build/build_config.h"
#include "build/buildflag.h"
#include "arkweb/build/features/features.h"
#include "nweb_engine.h"

namespace OHOS::NWeb {

class NWebEngineImpl : public NWebEngine {
 public:
  NWebEngineImpl();
  ~NWebEngineImpl() = default;

  std::shared_ptr<NWeb> CreateNWeb(
      std::shared_ptr<NWebCreateInfo> create_info) override;

  std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id) override;

  std::shared_ptr<NWebDataBase> GetDataBase() override;

  std::shared_ptr<NWebWebStorage> GetWebStorage() override;

  std::shared_ptr<NWebCookieManager> GetCookieManager() override;

  std::shared_ptr<NWebDownloadManager> GetDownloadManager() override;

  void SetWebTag(int32_t nweb_id, const char* web_tag) override;

  void InitializeWebEngine(
      std::shared_ptr<NWebEngineInitArgs> init_args) override;

  void PrepareForPageLoad(const std::string& url,
                          bool preconnectable,
                          int32_t num_sockets) override;

  void SetWebDebuggingAccess(bool isEnableDebug) override;

  void AddIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts) override;

  void RemoveIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts) override;

  void ClearIntelligentTrackingPreventionBypassingList() override;

  std::string GetDefaultUserAgent() override;

  void PauseAllTimers() override;

  void ResumeAllTimers() override;

  void PrefetchResource(
      const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
      const std::map<std::string, std::string>& additional_http_headers,
      const std::string& cache_key,
      const uint32_t& cache_valid_time) override;

  void SetRenderProcessMode(RenderProcessMode mode) override;

  RenderProcessMode GetRenderProcessMode() override;

  void ClearPrefetchedResource(
      const std::vector<std::string>& cache_key_list) override;

  void WarmupServiceWorker(const std::string& url) override;

  void SetHostIP(const std::string& hostName,
                 const std::string& address,
                 int32_t aliveTime) override;

  void ClearHostIP(const std::string& hostName) override;

  void SetAppCustomUserAgent(const std::string& user_agent) override;

  void SetUserAgentForHosts(const std::string& user_agent,
                            const std::vector<std::string>& hosts) override;

  std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager() override;

// todo: check webview
#if BUILDFLAG(IS_ARKWEB_EXT)
  void SetWholeWebDrawing() override;
#endif

  void TrimMemoryByPressureLevel(int32_t memoryLevel) override;

  void RemoveAllCache(bool include_disk_files) override;

  void SetProxyOverride(
      const std::vector<std::string>& proxyUrls,
      const std::vector<std::string>& proxySchemeFilters,
      const std::vector<std::string>& bypassRules,
      const bool& reverseBypass,
      std::shared_ptr<NWebProxyChangedCallback> callback) override;

  void RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback) override;
  void SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port) override;

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  uint32_t AddBlanklessLoadingUrls(const std::vector<std::string>& urls) override;
  void RemoveBlanklessLoadingUrls(const std::vector<std::string>& urls) override;
  void ClearBlanklessLoadingCache(const std::vector<std::string>& urls) override;
  std::string CheckBlankOptEnable(const std::string& key, int32_t nweb_id) override;
  void SetBlanklessLoadingCacheCapacity(int32_t capacity) override;
#endif

 private:
  std::shared_ptr<NWebDataBase> nweb_data_base_ = nullptr;
  std::shared_ptr<NWebWebStorage> nweb_web_storage_ = nullptr;
  std::shared_ptr<NWebDownloadManager> nweb_download_manager_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_ENGINE_IMPL_H

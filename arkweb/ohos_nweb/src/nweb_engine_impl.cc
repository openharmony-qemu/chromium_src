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

#include "nweb_engine_impl.h"

#include "arkweb/build/features/features.h"
#include "cef/ohos_cef_ext/libcef/browser/devtools/devtools_manager_delegate.h"
#include "nweb_adsblock_manager_impl.h"
#include "nweb_cookie_manager_impl.h"
#include "nweb_data_base_impl.h"
#include "nweb_download_manager_impl.h"
#include "nweb_hilog.h"
#include "nweb_impl.h"
#include "nweb_key_event_impl.h"
#include "nweb_web_storage_impl.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/chromium_ext/components/viz/host/blankless_data_controller.h"
#endif

namespace OHOS::NWeb {

namespace {
  bool g_web_debugging_enabled = false;
  int32_t g_web_debugging_port = 0;
} // namespace

static std::shared_ptr<NWebEngineImpl> g_nweb_engine_impl =
    std::make_shared<NWebEngineImpl>();

NWebEngineImpl::NWebEngineImpl() {
  nweb_data_base_ = std::make_shared<NWebDataBaseImpl>();
  nweb_web_storage_ = std::make_shared<NWebWebStorageImpl>();
  nweb_download_manager_ = std::make_shared<NWebDownloadManagerImpl>();
}

// static
std::shared_ptr<NWebEngine> NWebEngine::GetInstance() {
  return g_nweb_engine_impl;
}

std::shared_ptr<NWeb> NWebEngineImpl::CreateNWeb(
    std::shared_ptr<NWebCreateInfo> create_info) {
  return NWebImpl::CreateNWeb(create_info);
}

std::shared_ptr<NWeb> NWebEngineImpl::GetNWeb(int32_t nweb_id) {
  return NWebImpl::GetNWeb(nweb_id);
}

std::shared_ptr<NWebDataBase> NWebEngineImpl::GetDataBase() {
  return nweb_data_base_;
}

std::shared_ptr<NWebWebStorage> NWebEngineImpl::GetWebStorage() {
  return nweb_web_storage_;
}

std::shared_ptr<NWebCookieManager> NWebEngineImpl::GetCookieManager() {
  return NWebCookieManagerImpl::GetInstance();
}

std::shared_ptr<NWebDownloadManager> NWebEngineImpl::GetDownloadManager() {
  return nweb_download_manager_;
}

void NWebEngineImpl::SetWebTag(int32_t nweb_id, const char* web_tag) {
  NWebImpl::SetWebTag(nweb_id, web_tag);
}

void NWebEngineImpl::InitializeWebEngine(
    std::shared_ptr<NWebEngineInitArgs> init_args) {
#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
  NWebImpl::InitializeWebEngine(init_args);
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
}

void NWebEngineImpl::PrepareForPageLoad(const std::string& url,
                                        bool preconnectable,
                                        int32_t num_sockets) {
  return NWebImpl::PrepareForPageLoad(url, preconnectable, num_sockets);
}

void NWebEngineImpl::RemoveAllCache(bool include_disk_files) {
  return NWebImpl::RemoveAllCache(include_disk_files);
}

void NWebEngineImpl::SetWebDebuggingAccess(bool isEnableDebug) {
  LOG(INFO) << "SetWebDebuggingAccess(" << isEnableDebug
            << "), g_web_debugging_enabled[" << g_web_debugging_enabled
            << "], g_web_debugging_port[" << g_web_debugging_port
            << "]";
  if (isEnableDebug == g_web_debugging_enabled && g_web_debugging_port == 0) {
    return;
  }
  g_web_debugging_enabled = isEnableDebug;
  g_web_debugging_port = 0;
  if (g_web_debugging_enabled) {
    CefDevToolsManagerDelegate::StartHttpHandler(nullptr);
  } else if (!g_web_debugging_enabled) {
    CefDevToolsManagerDelegate::StopHttpHandler();
  }
}

void NWebEngineImpl::SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port)
{
  LOG(INFO) << "SetWebDebuggingAccess(" << isEnableDebug << ", " << port
            << "), g_web_debugging_enabled[" << g_web_debugging_enabled
            << "], g_web_debugging_port[" << g_web_debugging_port
            << "]";
  if (isEnableDebug == g_web_debugging_enabled && port == g_web_debugging_port) {
    return;
  }
  g_web_debugging_enabled = isEnableDebug;
  g_web_debugging_port = port;
  if (g_web_debugging_enabled) {
    CefDevToolsManagerDelegate::StartHttpHandlerWithPort(nullptr, port);
  } else if (!g_web_debugging_enabled) {
    CefDevToolsManagerDelegate::StopHttpHandler();
  }
}

void NWebEngineImpl::AddIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
#if BUILDFLAG(ARKWEB_ITP)
  NWebImpl::AddIntelligentTrackingPreventionBypassingList(hosts);
#endif
}

void NWebEngineImpl::RemoveIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
#if BUILDFLAG(ARKWEB_ITP)
  NWebImpl::RemoveIntelligentTrackingPreventionBypassingList(hosts);
#endif
}

void NWebEngineImpl::ClearIntelligentTrackingPreventionBypassingList() {
#if BUILDFLAG(ARKWEB_ITP)
  NWebImpl::ClearIntelligentTrackingPreventionBypassingList();
#endif
}

std::string NWebEngineImpl::GetDefaultUserAgent() {
#if BUILDFLAG(ARKWEB_USERAGENT)
  return NWebImpl::GetDefaultUserAgent();
#else
  return "";
#endif
}

void NWebEngineImpl::PauseAllTimers() {
  NWebImpl::PauseAllTimers();
}

void NWebEngineImpl::ResumeAllTimers() {
  NWebImpl::ResumeAllTimers();
}

void NWebEngineImpl::PrefetchResource(
    const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
    const std::map<std::string, std::string>& additional_http_headers,
    const std::string& cache_key,
    const uint32_t& cache_valid_time) {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  return NWebImpl::PrefetchResource(pre_args, additional_http_headers,
                                    cache_key, cache_valid_time);
#endif
}

void NWebEngineImpl::SetRenderProcessMode(RenderProcessMode mode) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  NWebImpl::SetRenderProcessMode(mode);
#endif
}

RenderProcessMode NWebEngineImpl::GetRenderProcessMode() {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  return NWebImpl::GetRenderProcessMode();
#else
  return RenderProcessMode::SINGLE_MODE;
#endif
}

void NWebEngineImpl::ClearPrefetchedResource(
    const std::vector<std::string>& cache_key_list) {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  return NWebImpl::ClearPrefetchedResource(cache_key_list);
#endif
}

void NWebEngineImpl::WarmupServiceWorker(const std::string& url) {
  NWebImpl::WarmupServiceWorker(url);
}

void NWebEngineImpl::SetHostIP(const std::string& hostName,
                               const std::string& address,
                               int32_t aliveTime) {
  NWebImpl::SetHostIP(hostName, address, aliveTime);
}

void NWebEngineImpl::ClearHostIP(const std::string& hostName) {
  NWebImpl::ClearHostIP(hostName);
}

void NWebEngineImpl::SetAppCustomUserAgent(const std::string& user_agent) {
#if BUILDFLAG(ARKWEB_USERAGENT)
  NWebImpl::SetAppCustomUserAgent(user_agent);
#endif
}

void NWebEngineImpl::SetUserAgentForHosts(
    const std::string& user_agent,
    const std::vector<std::string>& hosts) {
#if BUILDFLAG(ARKWEB_USERAGENT)
  NWebImpl::SetUserAgentForHosts(user_agent, hosts);
#endif
}

// todo: check webview
#if BUILDFLAG(IS_ARKWEB_EXT)
void NWebEngineImpl::SetWholeWebDrawing() {
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  NWebImpl::SetWholeWebDrawing();
#endif
}
#endif

std::shared_ptr<NWebAdsBlockManager> NWebEngineImpl::GetAdsBlockManager() {
  return NWebAdsBlockManagerImpl::GetInstance();
}

void NWebEngineImpl::TrimMemoryByPressureLevel(int32_t memoryLevel) {
  NWebImpl::TrimMemoryByPressureLevel(memoryLevel);
}

void NWebEngineImpl::SetProxyOverride(
    const std::vector<std::string>& proxyUrls,
    const std::vector<std::string>& proxySchemeFilters,
    const std::vector<std::string>& bypassRules,
    const bool& reverseBypass,
    std::shared_ptr<NWebProxyChangedCallback> callback) {
  NWebImpl::SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, callback);
}

void NWebEngineImpl::RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback) {
  NWebImpl::RemoveProxyOverride(callback);
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
uint32_t NWebEngineImpl::AddBlanklessLoadingUrls(const std::vector<std::string>& urls) {
  if (base::ohos::BlanklessController::SimpleCheck()) {
    return base::ohos::BlanklessController::GetInstance().AddEnabledUrlList(urls);
  }
  return 0;
}

void NWebEngineImpl::RemoveBlanklessLoadingUrls(const std::vector<std::string>& urls) {
  if (base::ohos::BlanklessController::SimpleCheck()) {
    base::ohos::BlanklessController::GetInstance().RemoveEnabledUrlList(urls);
  }
}

void NWebEngineImpl::ClearBlanklessLoadingCache(const std::vector<std::string>& urls) {
  if (base::ohos::BlanklessController::SimpleCheck()) {
    std::vector<int64_t> keys;
    for (const auto& url : urls) {
      keys.push_back(static_cast<int64_t>(std::hash<std::string>{}(url)));
    }
    base::ohos::BlanklessDataController::GetInstance().ClearSnapshotDataItem(keys);
  }
}

std::string NWebEngineImpl::CheckBlankOptEnable(const std::string& key, int32_t nweb_id) {
  if (base::ohos::BlanklessController::SimpleCheck()) {
    uint64_t blankless_key = base::ohos::BlanklessController::GetInstance().GetBlanklessLoadingKey(key, nweb_id);
    if (base::ohos::BlanklessController::INVALID_BLANKLESS_KEY != blankless_key) {
      const auto& data = base::ohos::BlanklessDataController::GetInstance().GetSnapshotDataItem(blankless_key,
        NWebImpl::GetPreferenceHashByNwebId(nweb_id));
      return data.staticPath;
    }
  }
  return "";
}

void NWebEngineImpl::SetBlanklessLoadingCacheCapacity(int32_t capacity) {
  if (base::ohos::BlanklessController::SimpleCheck()) {
    base::ohos::BlanklessController::GetInstance().SetCapacity(capacity);
    base::ohos::BlanklessDataController::GetInstance().SetBlanklessLoadingCacheCapacity(capacity);
  }
}
#endif
}  // namespace OHOS::NWeb

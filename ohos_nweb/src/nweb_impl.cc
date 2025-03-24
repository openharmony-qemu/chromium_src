/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "nweb_impl.h"

#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/report_loss_frame.h"
#include "components/web_cache/browser/web_cache_manager.h"

#ifdef OHOS_PERFORMANCE_MEMORY_THRESHOLD
#include "base/memory/memory_pressure_listener.h"
#include "base/time/time.h"
#endif  // OHOS_PERFORMANCE_MEMORY_THRESHOLD

#include "base/ohos/sys_info_utils.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"
#include "cef_delegate/nweb_download_handler_delegate.h"
#include "content/renderer/host_proxy.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "ndk/arkweb_native_object.h"
#include "nweb_delegate_adapter.h"
#include "nweb_export.h"
#include "nweb_handler.h"
#include "nweb_handler_delegate.h"
#include "nweb_hilog.h"
#include "nweb_hit_test_result_impl.h"
#include "ohos_adapter_helper.h"
#include "res_sched_client_adapter.h"
#include "ui/base/clipboard/ohos/clipboard_ohos.h"
#include "nweb_resize_helper.h"

#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif
#if BUILDFLAG(IS_OHOS) && defined(OHOS_PERFORMANCE_INC_FREQ)
#include "soc_perf_client_adapter.h"
#endif

#if defined(OHOS_API_INIT_WEB_ENGINE)
#include "cef_delegate/nweb_application.h"
#include "content/public/browser/network_service_instance.h"
#include "services/network/network_service.h"
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)

#ifdef OHOS_SECURITY_STATE
#include "components/security_state/core/security_state.h"
#endif // define(OHOS_SECURITY_STATE)

#if defined(OHOS_HTTP_DNS)
#include "cef/include/cef_app.h"
#include "cef/libcef/browser/net_service/net_helpers.h"
#endif  // defined(OHOS_HTTP_DNS)

#if defined(OHOS_NO_STATE_PREFETCH)
#include "libcef/browser/browser_context.h"
#include "libcef/browser/predictors/loading_predictor.h"
#include "libcef/browser/predictors/loading_predictor_config.h"
#include "libcef/browser/predictors/loading_predictor_factory.h"
#include "libcef/browser/predictors/predictor_database.h"
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_COOKIE)
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "content/public/common/content_paths.h"
#endif // defined(OHOS_COOKIE)

#ifdef OHOS_EX_NETWORK_CONNECTION
#include "content/public/browser/network_service_instance.h"
#include "services/network/network_service.h"
#endif

#ifdef OHOS_EX_UA
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_ua_config.h"
#endif

#if defined(OHOS_VIDEO_ASSISTANT)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_engine_cloud_config.h"
#endif

#ifdef OHOS_EX_GET_ZOOM_LEVEL
#include "third_party/blink/public/common/page/page_zoom.h"
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "cef/libcef/browser/alloy/alloy_browser_context.h"
#endif

#if defined(OHOS_EX_EXCEPTION_LIST)
#include "cef/libcef/browser/browser_context.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/network_service_instance.h"
#include "content/public/browser/storage_partition.h"
#include "services/network/network_service.h"
#endif

#ifdef OHOS_I18N
#include "ui/base/ui_base_switches.h"
#endif

#ifdef OHOS_ITP
#include "cef/libcef/browser/anti_tracking/third_party_cookie_access_policy.h"
#endif

#if defined(OHOS_CLOUD_CONTROL)
#include "cef/libcef/browser/alloy/alloy_browser_context.h"
#endif

#ifdef OHOS_SUSPEND_ALL_TIMERS
#include "content/browser/ohos/content_view_statics_ohos.h"
#endif

#ifdef OHOS_ARKWEB_ADBLOCK
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#endif

#if defined(OHOS_SITE_ISOLATION)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

#if defined(OHOS_SITE_ISOLATION)
extern bool g_siteIsolationMode;
#endif

#ifdef OHOS_RENDER_PROCESS_MODE
#include "base/ohos/sys_info_utils.h"
#include "content/public/browser/render_process_host.h"
#endif // OHOS_RENDER_PROCESS_MODE

#if defined(OHOS_WARMUP_SERVICEWORKER)
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/service_worker_context.h"
#include "content/public/common/origin_util.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "url/gurl.h"
#include "url/origin.h"
#endif // defined(OHOS_WARMUP_SERVICEWORKER)

#if OHOS_URL_TRUST_LIST
#include "cef/libcef/browser/ohos_safe_browsing/ohos_url_trust_list_interface.h"
#endif

#if defined(OHOS_SITE_ISOLATION)
extern bool g_siteIsolationMode;
#endif

#ifdef OHOS_ARKWEB_ADBLOCK
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#include "cef/libcef/browser/subresource_filter/adblock_list.h"
#endif

#ifdef OHOS_NETWORK_LOAD
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#endif

#ifdef OHOS_WEB_LTPO
#include "content/browser/gpu/gpu_process_host.h"
#endif

#ifdef OHOS_USERAGENT
#include "components/embedder_support/user_agent_utils.h"
#endif

#if defined(OHOS_EDM_POLICY)
#include "components/policy/core/common/policy_loader_ohos.h"
#endif

#include "ohos_nweb/src/capi/nweb_devtools_message_handler.h"

#include "net/proxy_resolution/proxy_config_service_ohos.h"
#include "cef/libcef/browser/net_service/proxy_config_monitor.h"

#if defined(OHOS_EX_DOWNLOAD)
#include "cef/include/cef_app.h"
#endif

#ifdef OHOS_ARKWEB_EXTENSIONS
#include "cef_delegate/nweb_extension_context_menus_delegate_handler.h"
#include "cef/libcef/browser/menu_manager.h"
#include "cef_delegate/nweb_extension_window_delegate_handler.h"
#include "cef/libcef/browser/windows_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "cef/include/cef_request_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef_delegate/nweb_extension_tab_delegate_hander.h"
#include "cef/libcef/browser/web_extension_tab_manager.h"
#include "cef/libcef/browser/extensions/tab_extensions_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "extensions/common/extension.h"
#include "extensions/browser/uninstall_reason.h"
#include "extensions/browser/ui_util.h"
#include "extensions/browser/extension_registry_info_manager.h"
#include "extensions/browser/extension_system.h"
#include "nweb_extension_action_cef_delegate.h"
#include "nweb_extension_side_panel_cef_delegate.h"
#endif // OHOS_ARKWEB_EXTENSIONS

#if defined(OHOS_VIDEO_ASSISTANT)
OnReportStatisticLogFunc
    OHOS::NWeb::NWebImpl::on_report_statistic_log_callback_ = nullptr;
#endif  // defined(OHOS_VIDEO_ASSISTANT)
namespace {
uint32_t g_nweb_count = 0;
const uint32_t kSurfaceMaxWidth = 7680;
const uint32_t kSurfaceMaxHeight = 7680;
#if defined(OHOS_MEDIA_POLICY)
const int32_t kMaxResumeInterval = 60;
#endif  // defined(OHOS_MEDIA_POLICY)
const float richtextDisplayRatio = 1.0;

const int32_t WEB_RESIZE_CLOSE_DELAY_TIME = 500;
const int SOC_PERF_WEB_DRAG_RESIZE_ID = 10012;

#if defined(OHOS_NWEB_EX)
bool g_browser_service_api_enabled = false;
std::vector<std::string> g_browser_args = {};
int32_t g_browser_service_sdk_api_level = 0;
#endif  // defined(OHOS_NWEB_EX)

#if defined(REPORT_SYS_EVENT)
// For maximum count of nweb instance
uint32_t g_nweb_max_count = 0;
#endif

#if defined(OHOS_EX_GET_ZOOM_LEVEL)
static double default_zoom_factor = 1.0;
#endif

#if defined(OHOS_MEDIA_POLICY)
const int NWebPlaybackState_NONE = 0;
#endif

#if defined(OHOS_SOFTWARE_COMPOSITOR)
static bool set_whole_page_drawing = false;
#endif

#ifdef OHOS_LOGGER_REPORT
std::shared_ptr<NWebLoggerCallback> g_logger_callback;
#endif
bool g_logger_callback_initialized = false;

bool GetWebOptimizationValue() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetWebOptimizationValue();
}

static bool IsAdvancedSecurityMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.IsAdvancedSecurityMode();
}

static std::string GetOOPGPUStatus() {
   auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                     .GetSystemPropertiesInstance();
  return system_properties_adapter.GetOOPGPUStatus();
}

#if defined(OHOS_SITE_ISOLATION)
static std::string GetSiteIsolationMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  return system_properties_adapter.GetSiteIsolationMode();
}

static bool IsMultipleRenderProcess() {
  const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line && command_line->HasSwitch(switches::kRendererProcessLimit)) {
      int limit_value = std::stoi(command_line->GetSwitchValueASCII(switches::kRendererProcessLimit));
      return (limit_value > 1
#ifdef OHOS_RENDER_PROCESS_MODE
              && OHOS::NWeb::NWebImpl::GetRenderProcessMode() ==
                  OHOS::NWeb::RenderProcessMode::MULTIPLE_MODE
#endif
      );
  }
    return false;
}

static bool ShouldEnableSiteIsolation() {
  std::string isSiteIsolationMode = GetSiteIsolationMode();

  if (isSiteIsolationMode == "false") {
    return false;
  }

  const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (!command_line) {
    LOG(WARNING) << "web component is not initialized, unable to enable site isolation";
    return false;
  }

  //for judge PC&&Tablet devices
  bool isIgnoreLockdownMode = (*base::CommandLine::ForCurrentProcess()).HasSwitch(
            switches::kIgnoreLockdownMode);

  if (isIgnoreLockdownMode && IsMultipleRenderProcess()) {
    return true;
  }

  if (IsAdvancedSecurityMode() && IsMultipleRenderProcess()) {
    return true;
  }

  return false;
}
#endif

static std::string GetNetlogMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetNetlogMode();
}

bool GetIsPopup(std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsPopup() : false;
}

std::string GetDumpPath(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetDumpPath() : "";
}

bool GetIsFrameInfoDump(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsFrameInfoDump() : false;
}

bool GetIsEnhanceSurface(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsEnhanceSurface() : false;
}

bool GetIsMultiRendererProcess(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsMultiRendererProcess() : false;
}

std::list<std::string> GetArgsToAdd(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  if (!init_args) {
    std::list<std::string> temp;
    return temp;
  }

  return init_args->GetArgsToAdd();
}

std::list<std::string> GetArgsToDelete(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  if (!init_args) {
    std::list<std::string> temp;
    return temp;
  }

  return init_args->GetArgsToDelete();
}

#if defined(OHOS_RENDER_PROCESS_SHARE)
std::string GetSharedRenderProcessToken(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetSharedRenderProcessToken() : "";
}
#endif

#if defined(OHOS_SCROLLBAR)
float GetVirtualPixelRatioForScrollbar() {
  auto display_manager_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (display_manager_adapter == nullptr) {
    LOG(ERROR) << "display_manager_adapter is nullptr";
    return -1;
  }
  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (display == nullptr) {
    LOG(ERROR) << "display is nullptr";
    return -1;
  }
  float ratio = display->GetVirtualPixelRatio();
  LOG(DEBUG) << "GetVirtualPixelRatio ratio:" << std::to_string(ratio);
  return ratio;
}
#endif

#if defined(OHOS_API_INIT_WEB_ENGINE)
void InitialWebEngineArgs(
    std::list<std::string>& web_engine_args,
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  web_engine_args.clear();

  web_engine_args.emplace_back("/system/bin/web_render");
  web_engine_args.emplace_back("--in-process-gpu");
  web_engine_args.emplace_back("--disable-dev-shm-usage");
  web_engine_args.emplace_back("--off-screen-frame-rate=60");
  web_engine_args.emplace_back("--no-unsandboxed-zygote");
  web_engine_args.emplace_back("--no-zygote");
  web_engine_args.emplace_back("--enable-features=UseOzonePlatform");
  web_engine_args.emplace_back("-ozone-platform=headless");
  web_engine_args.emplace_back("--no-sandbox");
  web_engine_args.emplace_back("--use-mobile-user-agent");
  web_engine_args.emplace_back("--enable-gpu-rasterization");
  if (!base::ohos::IsPcDevice() || base::ohos::IsCompatibleMode()) {
    web_engine_args.emplace_back("--enable-viewport");
  }
  web_engine_args.emplace_back(
      "--browser-subprocess-path=/system/bin/web_render");
  web_engine_args.emplace_back("--zygote-cmd-prefix=/system/bin/web_render");
  web_engine_args.emplace_back("--remote-debugging-port=9222");
#if defined(OHOS_SCROLLBAR)
  static float ratio = -1.0f;
  if (ratio < 0) {
    ratio = GetVirtualPixelRatioForScrollbar();
  }
  if (ratio > 0) {
    web_engine_args.emplace_back("--virtual-pixel-ratio=" + std::to_string(ratio));
  }
#endif
  web_engine_args.emplace_back("--enable-touch-drag-drop");
  web_engine_args.emplace_back("--gpu-rasterization-msaa-sample-count=1");
  // enable aggressive domstorage flushing to minimize data loss
  // http://crbug.com/479767
  web_engine_args.emplace_back("--enable-aggressive-domstorage-flushing");
  web_engine_args.emplace_back("--ohos-enable-drdc");

  std::vector<std::string> modeVector = {"Default", "IncludeSensitive",
                                         "Everything"};
  if (std::find(modeVector.begin(), modeVector.end(), GetNetlogMode()) !=
      modeVector.end()) {
    web_engine_args.emplace_back("--net-log-capture-mode=" + GetNetlogMode());
    web_engine_args.emplace_back(
        "--log-net-log=/data/storage/el2/base/cache/web/netlog.json");
  }

  if (IsAdvancedSecurityMode()) {
    WVLOG_I(
        "In advanced security mode, some HTML5 features will be unavailable, including "
        "WebAssembly, WebGL, PDF viewer, MathML, speech recognition, etc.");
    web_engine_args.emplace_back("--js-flags=--jitless");
    web_engine_args.emplace_back("--disable-webgl");
    web_engine_args.emplace_back("--disable-webgl2");
    web_engine_args.emplace_back("--disable-pdf-extension");
    web_engine_args.emplace_back("--disable-blink-features=NonAdvancedSecurityMode");
#if defined(REPORT_SYS_EVENT)
    ReportLockdownModeStatus();
#endif
  }

  web_engine_args.emplace_back("--enable-media-stream");
  if (GetIsEnhanceSurface(init_args)) {
    WVLOG_I("is_enhance_surface is true");
    web_engine_args.emplace_back("--ohos-enhance-surface");
  }

  auto args_to_delete = GetArgsToDelete(init_args);
  bool xml_gpu = false;
  for (auto arg : args_to_delete) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(), arg);
    if (it != web_engine_args.end()) {
      if (*it == "--in-process-gpu") {
        xml_gpu = true;
        continue;
      }
      web_engine_args.erase(it);
    }
  }

  auto args_to_add = GetArgsToAdd(init_args);
  for (auto arg : args_to_add) {
    web_engine_args.emplace_back(arg);
  }

  std::string oop_gpu_enable = GetOOPGPUStatus();
  if ((xml_gpu && oop_gpu_enable != "false") || (!xml_gpu && oop_gpu_enable == "true")) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(), "--in-process-gpu");
    if (it != web_engine_args.end()) {
      web_engine_args.erase(it);
    }
  }

  if (GetIsMultiRendererProcess(init_args)) {
    web_engine_args.emplace_back("--enable-multi-renderer-process");
  }
#ifdef OHOS_NWEB_EX
  for (const std::string& arg : g_browser_args) {
    web_engine_args.emplace_back(arg);
  }
#endif  // OHOS_NWEB_EX
}
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)

#if defined(OHOS_EX_DOWNLOAD)
typedef void(*ReadDownloadDataCallback)(const char* guid, const void* buffer, const size_t size);
class NWebReadDownloadDataCallback : public CefReadDownloadDataCallback {
 public:
  NWebReadDownloadDataCallback(ReadDownloadDataCallback callback)
  : read_download_data_callback_(callback) { }
 
  ~NWebReadDownloadDataCallback() = default;
 
  NO_SANITIZE("cfi")
  void OnReadDownloadDataDone(const CefString& guid,
                              const CefRefPtr<CefBinaryValue>& buffer) override {
    if (!read_download_data_callback_) {
      return;
    }
    if (!buffer) {
      LOG(INFO) << "OnReadDownloadDataDone: buffer is nullptr.";
      read_download_data_callback_(guid.ToString().c_str(), NULL, 0);
      return;
    }
    size_t len = buffer->GetSize();
    std::vector<uint8_t> dataBuffer(len);
    buffer->GetData(&dataBuffer[0], len, 0);
    LOG(INFO) << "OnReadDownloadDataDone: guid: " << guid.ToString() << ", buffer len: " << len;
    read_download_data_callback_(guid.ToString().c_str(), (void *)(&dataBuffer[0]), len);
  }
 
 private:
  ReadDownloadDataCallback read_download_data_callback_;
 
  IMPLEMENT_REFCOUNTING(NWebReadDownloadDataCallback);
};
#endif

}  // namespace

namespace OHOS::NWeb {

// static
std::shared_ptr<NWeb> NWebImpl::CreateNWeb(
    std::shared_ptr<NWebCreateInfo> create_info) {
  if (!create_info) {
    return nullptr;
  }
  static uint32_t current_nweb_id = 0;
  if (current_nweb_id == 0) {
    StartObserveTraceEnable();
  }
  uint32_t nweb_id = ++current_nweb_id;
  TRACE_EVENT1("NWebImpl", "NWebImpl | CreateNWeb", "nweb_id", nweb_id);
  WVLOG_I("creating nweb %{public}u, size %{public}u*%{public}u", nweb_id,
          create_info->GetWidth(), create_info->GetHeight());
  bool is_enhance_surface =
      GetIsEnhanceSurface(create_info->GetEngineInitArgs());
  WVLOG_I("creating nweb use enhance surface %{public}d", is_enhance_surface);
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(nweb_id);
  if (nweb == nullptr) {
    WVLOG_E("fail to create nweb instance");
    return nullptr;
  }

  if (!nweb->Init(create_info)) {
    WVLOG_E("fail to init nweb");
    return nullptr;
  }

  nweb->AddNWebToMap(nweb_id, nweb);
  ++g_nweb_count;
#if defined(REPORT_SYS_EVENT)
  // Report nweb instance count
  if (g_nweb_count > g_nweb_max_count) {
    g_nweb_max_count = g_nweb_count;
  }
  ReportMultiInstanceStats(nweb_id, g_nweb_count, g_nweb_max_count);
#endif

  return nweb;
}

#ifdef OHOS_ARKWEB_ADBLOCK
void NWebImpl::UpdateAdblockEasyListRules(long adBlockEasyListVersion) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  bool replace_switch = OHOS::adblock::AdBlockConfig::GetInstance()
                            ->GetUserEasylistReplaceSwitch();
  if (replace_switch) {
    WVLOG_D("[adblock] replace switch is true, not update cloud easylist");
    return;
  }

  return nweb_delegate_->UpdateAdblockEasyListRules(adBlockEasyListVersion);
}
#endif

#if defined(OHOS_COOKIE)
// static
bool
NWebImpl::InitializeICUStatic(std::shared_ptr<NWebEngineInitArgs> init_args) {
  if (NWebApplication::GetDefault()->HasInitializedCef()) {
    return true;
  }
  WVLOG_I("will initialize icu.");
  (void)init_args;
#if !BUILDFLAG(IS_NACL)
  static bool g_init_icu = false;
  if (!g_init_icu) {
    std::list<std::string> web_engine_args;
    InitialWebEngineArgs(web_engine_args, init_args);
    int argc = web_engine_args.size();
    const char** argv = new const char*[argc];
    int i = 0;
    for (auto it = web_engine_args.begin(); i < argc; ++i, ++it) {
      argv[i] = it->c_str();
    }
    base::CommandLine::Init(argc, argv);
    content::RegisterPathProvider();
    if (!base::i18n::InitializeICU()) {
      WVLOG_E("initialize icu failed.");
      return false;
    }
    g_init_icu = true;
  }
#endif
  return true;
}
#endif // defined(OHOS_COOKIE)

#if defined(OHOS_API_INIT_WEB_ENGINE)
// static
void
NWebImpl::InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs> init_args) {
  std::list<std::string> web_engine_args;
  InitialWebEngineArgs(web_engine_args, init_args);
  int argc = web_engine_args.size();
  const char** argv = new const char*[argc];
  int i = 0;
  for (auto it = web_engine_args.begin(); i < argc; ++i, ++it) {
    argv[i] = it->c_str();
  }

  CefMainArgs mainargs(argc, const_cast<char**>(argv));
  CefSettings settings;
  settings.windowless_rendering_enabled = true;
  settings.log_severity = LOGSEVERITY_INFO;
  settings.multi_threaded_message_loop = false;

#if defined(OHOS_COOKIE)
  bool excludable_devices =
      base::ohos::IsTabletDevice() || base::ohos::IsPcDevice();
  settings.persist_session_cookies = !excludable_devices;
#endif // defined(OHOS_COOKIE)

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif
  NWebApplication::GetDefault()->InitializeCef(mainargs, settings);
  content::GetNetworkService();
}
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)

#if BUILDFLAG(IS_OHOS) && defined(OHOS_PERFORMANCE_INC_FREQ)
static constexpr int32_t SOC_PERF_LOADURL_CONFIG_ID = 10070;
static constexpr int32_t SOC_PERF_MOUSEWHEEL_CONFIG_ID = 10071;
#endif

// For NWebEx
typedef std::unordered_map<int32_t, std::weak_ptr<NWebImpl>> NWebMap;

base::LazyInstance<NWebMap>::DestructorAtExit g_nweb_map =
    LAZY_INSTANCE_INITIALIZER;
base::Lock OHOS::NWeb::NWebImpl::nweb_map_lock_;

void NWebImpl::AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb) {
  if (nweb) {
    base::AutoLock lock_scope(nweb_map_lock_);
    std::weak_ptr<NWebImpl> nweb_weak(nweb);
    g_nweb_map.Get().emplace(id, nweb_weak);
  }
}

NWebImpl* NWebImpl::FromID(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    auto nweb = it->second.lock();
    if (nweb) {
      return nweb.get();
    }
  }
  return nullptr;
}

std::shared_ptr<NWebImpl> NWebImpl::GetNWebSharedPtr(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap *map = g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    if (auto nweb = it->second.lock()) {
      return nweb;
    }
  }
  return nullptr;
}

NWebImpl::NWebImpl(uint32_t id) : nweb_id_(id) {
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, nweb_id_);
}

NWebImpl::~NWebImpl() {
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, nweb_id_);
  ReportLossFrame::GetInstance()->Reset();
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::STOP);
  base::AutoLock lock_scope(nweb_map_lock_);
  g_nweb_map.Get().erase(nweb_id_);
}

bool NWebImpl::Init(std::shared_ptr<NWebCreateInfo> create_info) {
  output_handler_ = NWebOutputHandler::Create(
      create_info->GetWidth(), create_info->GetHeight(),
      [create_info](const char *buffer, uint32_t width, uint32_t height) -> bool {
        std::shared_ptr<NWebOutputFrameCallback> callback = create_info->GetOutputFrameCallback();
        if (!callback) {
          return false;
        }
        return callback->Handle(buffer, width, height);
      });
  if (output_handler_ == nullptr) {
    return false;
  }

  incognito_mode_ = create_info->GetIsIncognitoMode();

  output_handler_->SetNWebId(nweb_id_);

#if defined(REPORT_SYS_EVENT)
  if (incognito_mode_) {
    ReportOpenPrivateMode();
  }
#endif

  ProcessInitArgs(create_info->GetEngineInitArgs());
  if (!InitWebEngine(create_info)) {
    WVLOG_E("web engine init fail");
    return false;
  }

  input_handler_ = NWebInputHandler::Create(nweb_delegate_);
  if (input_handler_ == nullptr) {
    return false;
  }

#ifdef OHOS_NETWORK_PROXY
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .StartListen();
#endif

#if defined(OHOS_SITE_ISOLATION)
  g_siteIsolationMode = ShouldEnableSiteIsolation();
  OHOS::NWeb::ResSchedClientAdapter::ReportSiteIsolationMode(g_siteIsolationMode);
#if defined(REPORT_SYS_EVENT)
  ReportSiteIsolationMode(std::to_string(g_siteIsolationMode));
#endif
#endif

  if (!g_logger_callback_initialized) {
    g_logger_callback_initialized = true;
#ifdef OHOS_LOGGER_REPORT
    NWebHandlerDelegate::RegisterLoggerCallback(g_logger_callback);
#endif
  }
  return true;
}

void NWebImpl::OnDestroy() {
  WVLOG_I("NWebImpl::OnDestroy, nweb_id = %{public}u", nweb_id_);

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::KEY_TASK);

  if (destroyCallback_ != nullptr) {
    WVLOG_I("NWebImpl::OnDestroy destroyCallback_ webName_ is %{public}s", webName_.c_str());
    (destroyCallback_)(webName_.c_str());
    destroyCallback_ = nullptr;
  } else if (nativeDestroyCallback_) {
    nativeDestroyCallback_();
   }

  if (g_nweb_count == 0) {
    return;
  }
  TRACE_EVENT1("NWebImpl", "NWebImpl | DestoryNWeb", "nweb_id", nweb_id_);

#ifdef OHOS_ITP
  EnableIntelligentTrackingPrevention(false);
#endif

#ifdef OHOS_ARKWEB_ADBLOCK
  EnableAdsBlock(false);
#endif  // OHOS_ARKWEB_ADBLOCK

  bool is_close_all = (--g_nweb_count) == 0 ? true : false;
  WVLOG_D("NWebImpl::OnDestroy, nweb_id = %{public}u, number = %{public}u", nweb_id_, g_nweb_count);
#ifdef OHOS_WEB_LTPO
  if (is_close_all) {
    if (auto* host = GpuProcessHost::Get()) {
      if (auto* host_impl = host->gpu_host()) {
        host_impl->SetVisible(false);
      }
    }
  }
#endif

  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->OnDestroy(is_close_all);
    nweb_delegate_ = nullptr;
  }

  if (input_handler_ != nullptr) {
    input_handler_->OnDestroy();
    input_handler_ = nullptr;
  }

#ifdef OHOS_NETWORK_PROXY
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .StopListen();
#endif

#if defined(REPORT_SYS_EVENT)
  // Report nweb instance count
  ReportMultiInstanceStats(nweb_id_, g_nweb_count, g_nweb_max_count);
#endif
}

void NWebImpl::ProcessInitArgs(std::shared_ptr<NWebEngineInitArgs> init_args) {
  std::string dump_path = GetDumpPath(init_args);
  if (!dump_path.empty() && output_handler_ != nullptr) {
    output_handler_->SetDumpPath(dump_path);
  }
  bool frame_info_dump = GetIsFrameInfoDump(init_args);
  if (frame_info_dump && output_handler_ != nullptr) {
    output_handler_->SetFrameInfoDump(frame_info_dump);
  }

  InitWebEngineArgs(init_args);
}

bool NWebImpl::SetVirtualDeviceRatio() {
  if (fabs(device_pixel_ratio_) < 1e-15) {
    auto display_manager_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
    if (display_manager_adapter == nullptr) {
      WVLOG_E("display_manager_adapter is nullptr.");
      return false;
    }
    std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
        display_manager_adapter->GetDefaultDisplay();
    if (display == nullptr) {
      WVLOG_E("display is nullptr.");
      return false;
    }
    if (is_richtext_value_) {
      // Created a richtext component
      device_pixel_ratio_ = richtextDisplayRatio;
    } else {
      device_pixel_ratio_ = display->GetVirtualPixelRatio();
    }
    if (device_pixel_ratio_ <= 0) {
      WVLOG_E("invalid ratio.");
      return false;
    }
    WVLOG_I("GetVirtualPixelRatio ratio: %{public}f", device_pixel_ratio_);
  }
  return true;
}

bool NWebImpl::InitWebEngine(std::shared_ptr<NWebCreateInfo> create_info) {
  if (output_handler_ == nullptr) {
    WVLOG_E("fail to init web engine, NWeb output handler is not ready");
    return false;
  }

  if (web_engine_args_.empty()) {
    WVLOG_E("fail to init web engine args");
    return false;
  }

  int argc = web_engine_args_.size();
  const char** argv = new const char*[argc];
  int i = 0;
  for (auto it = web_engine_args_.begin(); i < argc; ++i, ++it) {
    argv[i] = it->c_str();
    if (!strncmp(argv[i], "--init-richtext-data=", strlen("--init-richtext-data="))) {
      is_richtext_value_ = true;
    }
  }

  std::shared_ptr<NWebEngineInitArgs> init_args = create_info->GetEngineInitArgs();
  is_enhance_surface_ = GetIsEnhanceSurface(init_args);
  void* window = nullptr;
  if (is_enhance_surface_) {
    window = create_info->GetEnhanceSurfaceInfo();
  } else {
    window = output_handler_->GetNativeWindowFromSurface(
        create_info->GetProducerSurface());
  }

  if (window == nullptr) {
    WVLOG_E("fail to init web engine, get native window from surface failed");
    delete[] argv;
    return false;
  }

  window_ = reinterpret_cast<EGLNativeWindowType>(window);
  int32_t ret =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWindowAdapterInstance()
          .NativeWindowSetBufferGeometry(reinterpret_cast<void*>(window),
                                 create_info->GetWidth(), create_info->GetHeight());

  if (ret == OHOS::NWeb::GSErrorCode::GSERROR_OK) {
      WVLOG_I("native window opt for emulator in init, result = %{public}d", ret);
  } else {
      WVLOG_W("native window opt for emulator in init failed, result = %{public}d", ret);
  }

  bool is_popup = GetIsPopup(init_args);
#if defined(OHOS_RENDER_PROCESS_SHARE)
  std::string shared_render_process_token =
      GetSharedRenderProcessToken(init_args);
#endif
  WVLOG_D("nweb create_info.init_args.is_popup: %{public}d", is_popup);
  nweb_delegate_ = NWebDelegateAdapter::CreateNWebDelegate(
      argc, argv, is_enhance_surface_, window, is_popup
#if defined(OHOS_EX_DOWNLOAD)
      ,
      nweb_id_
#endif
#if defined(OHOS_INCOGNITO_MODE)
      ,
      create_info->GetIsIncognitoMode()
#endif
#if defined(OHOS_RENDER_PROCESS_SHARE)
      ,
      shared_render_process_token
#endif
  );
  WVLOG_D("nweb create_info.incognito_mode: %{public}d",
          create_info->GetIsIncognitoMode());
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to create nweb delegate of web engine");
    delete[] argv;
    return false;
  }

#if defined(OHOS_CLIPBOARD)
  nweb_delegate_->SetIsRichText(is_richtext_value_);
#endif

  if (!SetVirtualDeviceRatio()) {
    WVLOG_E("fail to set virtual device ratio");
    delete[] argv;
    return false;
  }

  uint32_t width, height;
  output_handler_->GetWindowInfo(width, height);
  nweb_delegate_->Resize(width, height);

  inputmethod_handler_ = new NWebInputMethodHandler();
  if (!inputmethod_handler_) {
    delete[] argv;
    WVLOG_E("inputmethod_handler_ is nullptr");
    return false;
  }
  nweb_delegate_->SetInputMethodClient(inputmethod_handler_);
  nweb_delegate_->SetNWebDelegateInterface(nweb_delegate_);
  inputmethod_handler_->SetVirtualDeviceRatio(device_pixel_ratio_);

  nweb_delegate_->SetNWebId(nweb_id_);
#if defined(OHOS_SOFTWARE_COMPOSITOR)
  if (set_whole_page_drawing) {
    nweb_delegate_->SetWholePageDrawing();
  }
#endif

#ifdef OHOS_I18N
  UpdateAcceptLanguageInternal();
#endif
  delete[] argv;
  return nweb_delegate_->IsReady();
}

void NWebImpl::InitWebEngineArgs(std::shared_ptr<NWebEngineInitArgs> init_args) {
  InitialWebEngineArgs(web_engine_args_, init_args);
}

void NWebImpl::PutDownloadCallback(
    std::shared_ptr<NWebDownloadCallback> downloadListener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set download callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->RegisterDownLoadListener(downloadListener);
}

void NWebImpl::SetNWebHandler(std::shared_ptr<NWebHandler> client) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set nweb handler failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_handle_ = client;
  nweb_delegate_->RegisterNWebHandler(client);
  client->SetNWeb(shared_from_this());
}

void NWebImpl::DisableBoost() {
  ResizeTime_--;
  if(ResizeTime_ <= 0) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, false);
    ResizeTime_ = 0;
  }
}

void NWebImpl::Resize(uint32_t width, uint32_t height, bool isKeyboard) {
  if (is_pause_) {
    LOG(INFO) << "web kernel in pause state, don't resize, store pending size, width = "
      << width << ", height = " << height << ", isKeyboard = " << isKeyboard << ", nweb_id = " << nweb_id_;
    ReSizeType temp_size;
    temp_size.width_ = width;
    temp_size.height_ = height;
    temp_size.is_keyboard_ = isKeyboard;
    pending_size_ = temp_size;
    return;
  }

  if (input_handler_ == nullptr || output_handler_ == nullptr) {
    return;
  }
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      WVLOG_E("size too large in surface mode (%{public}u , %{public}u)", width, height);
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resize failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .CreateSocPerfClientAdapter()
    ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
  ResizeTime_++;
  content::GetUIThreadTaskRunner({})->PostDelayedTask(
    FROM_HERE, base::BindOnce(&NWebImpl::DisableBoost, base::Unretained(this)),
    base::Milliseconds(WEB_RESIZE_CLOSE_DELAY_TIME));
  nweb_delegate_->Resize(width, height, isKeyboard);
  output_handler_->Resize(width, height);
}

void NWebImpl::ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard) {
#if defined(OHOS_INPUT_EVENTS)
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      WVLOG_E("size too large in surface mode (%{public}u , %{public}u)", width, height);
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resize failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }
  nweb_delegate_->ResizeVisibleViewport(width, height, isKeyboard);
#endif
}

void NWebImpl::SetDrawRect(int x, int y, int width, int height) {
  if (nweb_delegate_) {
    nweb_delegate_->SetDrawRect(x, y, width, height);
  }
}

void NWebImpl::SetDrawMode(int mode) {
  WVLOG_D("NWebImpl::SetDrawMode %{public}d", mode);
  draw_mode_ = mode;
  if (nweb_delegate_) {
    nweb_delegate_->SetDrawMode(mode);
  }
}

bool NWebImpl::GetPendingSizeStatus() {
  if (nweb_delegate_) {
    return nweb_delegate_->GetPendingSizeStatus();
  }
  return false;
}

void NWebImpl::SetFitContentMode(int mode) {
  WVLOG_D("NWebImpl::SetFitContentMode %{public}d", mode);
  if (nweb_delegate_) {
    nweb_delegate_->SetFitContentMode(mode);
  }
}

void NWebImpl::OnTouchPress(int32_t id, double x, double y, bool from_overlay) {
  WVLOG_D(
      "NWebImpl::OnTouchPress id=%{public}d, x=%{public}f, y=%{public}f, "
      "from_overlay=%{public}d",
      id, x, y, from_overlay);
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::CLICK, nweb_id_);
  input_handler_->OnTouchPress(id, x, y, from_overlay);

  if (nweb_delegate_) {
    nweb_delegate_->RefreshAccessibilityManagerClickEvent();
  }
}

void NWebImpl::OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) {
  WVLOG_D(
      "NWebImpl::OnTouchRelease id=%{public}d, x=%{public}f, y=%{public}f, "
      "from_overlay=%{public}d",
      id, x, y, from_overlay);
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnTouchRelease(id, x, y, from_overlay);
}

void NWebImpl::OnTouchMove(int32_t id, double x, double y, bool from_overlay) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnTouchMove(id, x, y, from_overlay);
}

void NWebImpl::OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                           bool from_overlay) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnTouchMove(touch_point_infos, from_overlay);
}

void NWebImpl::OnTouchCancel() {
  WVLOG_D("NWebImpl::OnTouchCancel");
  if (input_handler_ == nullptr) {
    return;
  }
  input_handler_->OnTouchCancel();
}

void NWebImpl::OnNavigateBack() {
  if (input_handler_ == nullptr) {
    return;
  }
  input_handler_->OnNavigateBack();
}

bool NWebImpl::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  if (input_handler_ == nullptr) {
    return false;
  }
  return input_handler_->SendKeyEvent(keyCode, keyAction);
}

void NWebImpl::SendTouchpadFlingEvent(double x, double y, double vx, double vy) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
}

void NWebImpl::SendMouseWheelEvent(double x,
                                   double y,
                                   double deltaX,
                                   double deltaY) {
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::SLIDE);

#if defined(OHOS_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

void NWebImpl::SendMouseEvent(int x, int y, int button, int action, int count) {
  if (input_handler_ == nullptr) {
    return;
  }

  if (action == MouseAction::PRESS) {
    ResSchedClientAdapter::ReportScene(
      ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::CLICK, nweb_id_);
  }
  input_handler_->SendMouseEvent(x, y, button, action, count);
}

int NWebImpl::Load(const std::string& url) {
  if (nweb_delegate_ == nullptr || output_handler_ == nullptr) {
    return NWEB_ERR;
  }

  if (!output_handler_->IsSizeValid()) {
    WVLOG_E("nweb size is invalid, stop Load");
    return NWEB_ERR;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::LOAD_URL, nweb_id_);

#if defined(OHOS_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_LOADURL_CONFIG_ID);
#endif

  int result = nweb_delegate_->Load(url);
  output_handler_->StartRenderOutput();
  return result;
}

bool NWebImpl::IsNavigatebackwardAllowed() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigatebackwardAllowed();
}

bool NWebImpl::IsNavigateForwardAllowed() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigateForwardAllowed();
}

bool NWebImpl::CanNavigateBackOrForward(int numSteps) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->CanNavigateBackOrForward(numSteps);
}

void NWebImpl::NavigateBack() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateBack();
}

void NWebImpl::NavigateForward() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateForward();
}

void NWebImpl::NavigateBackOrForward(int step) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateBackOrForward(step);
}

void NWebImpl::DeleteNavigateHistory() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->DeleteNavigateHistory();
}

void NWebImpl::ClearSslCache() {
  WVLOG_I("NWebImpl::ClearSslCache");
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearSslCache();
}

void NWebImpl::ClearClientAuthenticationCache() {
  WVLOG_I("NWebImpl::ClearClientAuthenticationCache");
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearClientAuthenticationCache();
}

void NWebImpl::Reload() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Reload();
}

int NWebImpl::Zoom(float zoomFactor) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Zoom(zoomFactor);
}

int NWebImpl::ZoomIn() {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomIn();
}

int NWebImpl::ZoomOut() {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomOut();
}

void NWebImpl::Stop() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Stop();
}

void NWebImpl::ExecuteJavaScript(const std::string& code) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code);
}

void NWebImpl::PutBackgroundColor(int color) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PutBackgroundColor(color);
}

void NWebImpl::InitialScale(float scale) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->InitialScale(scale);
}

void NWebImpl::PutOptimizeParserBudgetEnabled(bool enable)
{
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PutOptimizeParserBudgetEnabled(true);
}

void NWebImpl::OnPause() {
  if (!GetWebOptimizationValue()) {
    LOG(DEBUG) << "WebOptimization disabled.";
    return;
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  is_pause_ = true;
  nweb_delegate_->OnPause();

  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnPause";
    nweb_delegate_->GetCustomKeyboardHandler()->CloseFromWebStateChange(WebCustomKeyboardState::FROM_ONPAUSE);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  inputmethod_handler_->HideTextInput(
      nweb_id_, NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE);
}

void NWebImpl::OnContinue() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  if (is_pause_) {
    is_pause_ = false;
    if (pending_size_) {
      LOG(INFO) << "web kernel in continue state, continue resize, need resize" << ", nweb_id = " << nweb_id_;
      Resize(pending_size_.value().width_, pending_size_.value().height_, pending_size_.value().is_keyboard_);
      pending_size_.reset();
    }
  }

  nweb_delegate_->OnContinue();

  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnContinue and focus";
    auto handler = nweb_delegate_->GetCustomKeyboardHandler();
    if (handler && handler->AttachFromWebStateChange(WebCustomKeyboardState::FROME_ONCONTINUE)) {
      nweb_delegate_->OnFocus();
    }
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (inputmethod_handler_->Reattach(
          nweb_id_, NWebInputMethodHandler::ReattachType::FROM_CONTINUE)) {
    nweb_delegate_->OnFocus();
  }
}

void NWebImpl::OnDragAttach() {
  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  inputmethod_handler_->Reattach(
      nweb_id_, NWebInputMethodHandler::ReattachType::FROM_ONDRAG);
}

void NWebImpl::WebComponentsBlur() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  LOG(INFO) << "NWebImpl::WebComponentsBlur, Gesture back blur on.";
  nweb_delegate_->WebComponentsBlur();
}

void NWebImpl::OnOccluded() {
  if (!GetWebOptimizationValue()) {
    LOG(DEBUG) << "WebOptimization disabled.";
    return;
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnOccluded();
}

void NWebImpl::OnUnoccluded() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnUnoccluded();
}

void NWebImpl::SetEnableLowerFrameRate(bool enabled) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->SetEnableLowerFrameRate(enabled);
}

#if defined(OHOS_WEBRTC)
void NWebImpl::StopCameraSession() const {
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetForegroundFlag(false);
}

void NWebImpl::RestartCameraSession() const {
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetForegroundFlag(true);
  if (!OhosAdapterHelper::GetInstance()
           .GetCameraManagerAdapter()
           .IsExistCaptureTask()) {
    return;
  }
  WVLOG_I("RestartSession");
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().RestartSession();
}
#endif // defined(OHOS_WEBRTC)

void NWebImpl::MaximizeResize() {
  if (nweb_delegate_) {
    nweb_delegate_->MaximizeResize();
  }
}

std::shared_ptr<NWebPreference> NWebImpl::GetPreference() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->GetPreference();
}

std::string NWebImpl::Title() {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->Title();
}

void NWebImpl::ExecuteJavaScriptExt(
    const int fd,
    const size_t scriptLength,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
}

void NWebImpl::ExecuteCreatePDFExt(
    std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
    std::shared_ptr<NWebArrayBufferValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteCreatePDFExt(pdfConfig, callback);
}

#if defined(OHOS_MSGPORT)
void NWebImpl::ExecuteJavaScript(
    const std::string& code,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code, callback, extention);
}

std::vector<std::string> NWebImpl::CreateWebMessagePorts() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    std::vector<std::string> empty;
    return empty;
  }
  return nweb_delegate_->CreateWebMessagePorts();
}

void NWebImpl::PostWebMessage(const std::string& message,
                              const std::vector<std::string>& ports,
                              const std::string& targetUri) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostWebMessage(message, ports, targetUri);
}

void NWebImpl::ClosePort(const std::string& portHandle) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->ClosePort(portHandle);
}

void NWebImpl::PostPortMessage(const std::string& portHandle,
                               std::shared_ptr<NWebMessage> data) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostPortMessage(portHandle, data);
}

void NWebImpl::SetPortMessageCallback(
    const std::string& portHandle,
    std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->SetPortMessageCallback(portHandle, callback);
}
#endif  // defined(OHOS_MSGPORT)

void NWebImpl::SetAutofillCallback(std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->SetAutofillCallback(callback);
}

void NWebImpl::FillAutofillData(std::shared_ptr<NWebMessage> data) {
  LOG(INFO) << "NWebImpl::FillAutofillData";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->FillAutofillData(data);
}

void NWebImpl::OnAutofillCancel(const std::string& fillContent) {
  LOG(INFO) << "NWebImpl::OnAutofillCancel";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->ProcessAutofillCancel(fillContent);
}

uint32_t NWebImpl::GetWebId() {
  return nweb_id_;
}

std::shared_ptr<HitTestResult> NWebImpl::GetHitTestResult() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("get hit test result failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return std::make_shared<HitTestResultImpl>();
  }

  return nweb_delegate_->GetHitTestResult();
}

int NWebImpl::PageLoadProgress() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->PageLoadProgress();
}

int NWebImpl::ContentHeight() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->ContentHeight();
}

float NWebImpl::Scale() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->Scale();
}

int NWebImpl::Load(const std::string& url,
                   const std::map<std::string, std::string>& additionalHttpHeaders) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Load(url, additionalHttpHeaders);
}

int NWebImpl::PostUrl(const std::string& url,
                      const std::vector<char>& postData) {
#ifdef OHOS_POST_URL
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->PostUrl(url, postData);
#endif // defined(OHOS_POST_URL)
}

int NWebImpl::LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                     const std::string& data,
                                     const std::string& mimeType,
                                     const std::string& encoding,
                                     const std::string& historyUrl) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->LoadWithDataAndBaseUrl(baseUrl, data, mimeType,
                                                encoding, historyUrl);
}

int NWebImpl::LoadWithData(const std::string& data,
                           const std::string& mimeType,
                           const std::string& encoding) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->LoadWithData(data, mimeType, encoding);
}

void NWebImpl::RegisterNativeArkJSFunction(
    const char* objName,
    const std::vector<std::shared_ptr<NWebJsProxyCallback>> &callbacks) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeArkJSFunction(objName, callbacks);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeArkJSFunction(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFunc>&& callback,
    bool isAsync,
    const std::string& permission) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeJSProxy(objName, methodName,
                                          std::move(callback), isAsync, permission);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::UnRegisterNativeArkJSFunction(const char* objName) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->UnRegisterNativeArkJSFunction(objName);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback) {
  base::AutoLock lock_scope(state_lock_);
  webName_ = webName;
  validCallback_ = callback;
}
void NWebImpl::RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnDestroyCallback callback) {
  base::AutoLock lock_scope(state_lock_);
  webName_ = webName;
  destroyCallback_ = callback;
}

void NWebImpl::RegisterNativeDestroyCallback(std::function<void(void)>&& callback) {
  base::AutoLock lock_scope(state_lock_);
  nativeDestroyCallback_ = std::move(callback);
}

void NWebImpl::RegisterNativeLoadStartCallback(std::function<void(void)>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeLoadStartCallback(std::move(callback));
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeLoadEndCallback(std::function<void(void)>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeLoadEndCallback(std::move(callback));
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const int32_t object_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(object_name, method_list,
                                               std::vector<std::string>(), object_id, "");
}

void NWebImpl::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(object_name, method_list,
                                               async_method_list, object_id, "");
}

void NWebImpl::RegisterArkJSfunctionV2(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id,
    const std::string& permission) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(object_name, method_list,
                                               async_method_list, object_id, permission);
}

void NWebImpl::UnregisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->UnregisterArkJSfunction(object_name, method_list);
}

#if defined(OHOS_JSPROXY)
void NWebImpl::JavaScriptOnDocumentStart(const ScriptItems& scriptItems) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentStart(scriptItems);
}

void NWebImpl::JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentEnd(scriptItems);
}

void NWebImpl::JavaScriptOnDocumentStartByOrder(const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentStartByOrder(scriptItems, scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnDocumentEndByOrder(const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentEndByOrder(scriptItems, scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnHeadReadyByOrder(const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnHeadReadyByOrder(scriptItems, scriptItemsByOrder);
}
#endif

void NWebImpl::CallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string& h5_method_name,
    const std::vector<std::shared_ptr<NWebValue>>& args) {
  if (nweb_delegate_ == nullptr || h5_object_id < 0) {
    WVLOG_E("fail to call h5 function");
    return;
  }
  nweb_delegate_->CallH5Function(routing_id, h5_object_id, h5_method_name,
                                 args);
}

void NWebImpl::SetNWebJavaScriptResultCallBack(
    std::shared_ptr<NWebJavaScriptResultCallBack> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RegisterNWebJavaScriptCallBack(callback);
}

void NWebImpl::OnFocus(const FocusReason& focusReason) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnFocus";
    nweb_delegate_->GetCustomKeyboardHandler()->AttachFromWebStateChange(WebCustomKeyboardState::FROME_ONFOCUS);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (nweb_delegate_->OnFocus(focusReason)) {
    inputmethod_handler_->Reattach(
        nweb_id_, NWebInputMethodHandler::ReattachType::FROM_ONFOCUS);
  }
}

void NWebImpl::OnBlur(const BlurReason& blurReason) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnBlur();

  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnBlur";
    nweb_delegate_->GetCustomKeyboardHandler()->CloseFromWebStateChange(WebCustomKeyboardState::FROM_ONBLUR);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
#ifdef OHOS_FOCUS
  if (is_enhance_surface_ &&
      blurReason == OHOS::NWeb::BlurReason::WINDOW_BLUR) {
    return;
  }
#endif  // #ifdef OHOS_FOCUS
  if (blurReason == OHOS::NWeb::BlurReason::CLEAR_FOCUS) {
    inputmethod_handler_->HideTextInputForce();
  } else if (blurReason == OHOS::NWeb::BlurReason::FOCUS_SWITCH) {
    inputmethod_handler_->SetNeedReattachOnfocus();
  } else {
    inputmethod_handler_->HideTextInput(
        nweb_id_, NWebInputMethodClient::HideTextinputType::FROM_ONBLUR);
  }
  inputmethod_handler_->SetFocusStatus(false);
}

void NWebImpl::PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RegisterFindListener(findListener);
}

void NWebImpl::FindAllAsync(const std::string& search_string) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindAllAsync(search_string);
}

void NWebImpl::ClearMatches() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearMatches();
}

void NWebImpl::FindNext(const bool forward) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindNext(forward);
}

void NWebImpl::StoreWebArchive(
    const std::string& base_name,
    bool auto_name,
    std::shared_ptr<NWebStringValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->StoreWebArchive(base_name, auto_name, callback);
}

void NWebImpl::SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return;
  }
  DelegateDragEvent event;
  if (dragEvent) {
    event.action = static_cast<DelegateDragAction>(dragEvent->GetAction());
    event.x = dragEvent->GetX();
    event.y = dragEvent->GetY();
  }
  nweb_delegate_->SendDragEvent(event);
}

std::string NWebImpl::GetUrl() {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->GetUrl();
}

#ifdef OHOS_I18N
void NWebImpl::UpdateLocale(const std::string& language,
                            const std::string& region) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateLocale(language, region);
}
#endif  // ifdef OHOS_I18N

const std::string NWebImpl::GetOriginalUrl() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return std::string();
  }
  return nweb_delegate_->GetOriginalUrl();
}

#if BUILDFLAG(IS_OHOS)
bool NWebImpl::TerminateRenderProcess() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("TerminateRenderProcess failed, nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->TerminateRenderProcess();
}
#endif

bool NWebImpl::GetFavicon(const void** data,
                          size_t& width,
                          size_t& height,
                          ImageColorType& colorType,
                          ImageAlphaType& alphaType) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->GetFavicon(data, width, height, colorType, alphaType);
}

void NWebImpl::PutNetworkAvailable(bool available) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->PutNetworkAvailable(available);
}

void NWebImpl::HasImages(std::shared_ptr<NWebBoolValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI HasImages nweb_delegate_ is null");
    return;
  }

  nweb_delegate_->GetImages(callback);
}

void NWebImpl::RemoveCache(bool include_disk_files) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI RemoveCache nweb_delegate_ is null");
    return;
  }

  nweb_delegate_->RemoveCache(include_disk_files);
}

std::shared_ptr<NWebHistoryList> NWebImpl::GetHistoryList() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }

  return nweb_delegate_->GetHistoryList();
}

std::vector<uint8_t> NWebImpl::SerializeWebState() {
  if (nweb_delegate_ == nullptr) {
    std::vector<uint8_t> empty;
    return empty;
  }
  return nweb_delegate_->SerializeWebState();
}

bool NWebImpl::RestoreWebState(const std::vector<uint8_t>& state) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->RestoreWebState(state);
}

void NWebImpl::PutReleaseSurfaceCallback(
    std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set release surface callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->RegisterReleaseSurfaceListener(releaseSurfaceListener);
}

#ifdef OHOS_PAGE_UP_DOWN
void NWebImpl::PageUp(bool top) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->PageUp(top);
}

void NWebImpl::PageDown(bool bottom) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->PageDown(bottom);
}

#ifdef OHOS_GET_SCROLL_OFFSET
void NWebImpl::GetScrollOffset(float* offset_x, float* offset_y) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  float scroll_offset_x = 0;
  float scroll_offset_y = 0;
  float overscroll_offset_x = 0;
  float overscroll_offset_y = 0;
  float finalOffset_x = 0;
  float finalOffset_y = 0;
  nweb_delegate_->GetScrollOffset(&scroll_offset_x, &scroll_offset_y);
  #if defined(OHOS_INPUT_EVENTS)
  nweb_delegate_->GetOverScrollOffset(&overscroll_offset_x,
                                      &overscroll_offset_y);
  #endif
  finalOffset_x = scroll_offset_x + std::round(overscroll_offset_x);
  finalOffset_y = scroll_offset_y + std::round(overscroll_offset_y);

  if ((nullptr == offset_x) || (nullptr == offset_y)) {
    LOG(ERROR) << "offset_x or offset_y is nullptr";
    return;
  }
  *offset_x = finalOffset_x;
  *offset_y = finalOffset_y;
}
#endif
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
void NWebImpl::ScrollTo(float x, float y) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollTo(x, y);
}

void NWebImpl::ScrollBy(float delta_x, float delta_y) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollBy(delta_x, delta_y);
}

void NWebImpl::ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

void NWebImpl::SlideScroll(float vx, float vy) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->SlideScroll(vx, vy);
}

bool NWebImpl::WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                               const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    return false;
  }
  return input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
}

void NWebImpl::WebSendMouseWheelEvent(double x,
                                      double y,
                                      double deltaX,
                                      double deltaY,
                                      const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::SLIDE);

#if defined(OHOS_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
}

bool NWebImpl::WebSendMouseWheelEventV2(double x,
                                        double y,
                                        double deltaX,
                                        double deltaY,
                                        const std::vector<int32_t>& pressedCodes,
                                        int32_t source) {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendMouseWheelEventV2 input_handler_ is nullptr";
    return true;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::SLIDE);

#if defined(OHOS_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
  input_handler_->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);
  return true;
}

void NWebImpl::WebSendTouchpadFlingEvent(double x,
                                         double y,
                                         double vx,
                                         double vy,
                                         const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
}

bool NWebImpl::ScrollByWithResult(float delta_x, float delta_y) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->ScrollByWithResult(delta_x, delta_y);
}

void NWebImpl::ScrollToWithAnime(float x, float y, int32_t duration) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollToWithAnime(x, y, duration);
}

void NWebImpl::ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollByWithAnime(delta_x, delta_y, duration);
}

bool NWebImpl::SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
  if (input_handler_ == nullptr) {
    return false;
  }
  return input_handler_->SendKeyboardEvent(keyboardEvent);
}
#endif  // defined(OHOS_INPUT_EVENTS)

bool NWebImpl::GetCertChainDerData(std::vector<std::string>& certChainData,
                                   bool isSingleCert) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("get cert chain data failed, nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->GetCertChainDerData(certChainData, isSingleCert);
}

void NWebImpl::SetScreenOffSet(double x, double y) {
  if (inputmethod_handler_) {
    inputmethod_handler_->SetScreenOffSet(x, y);
  }
}

void NWebImpl::SetAudioMuted(bool muted) {
#if defined(OHOS_MEDIA_MUTE_AUDIO)
  WVLOG_D("SetAudioMuted invoked: %{public}s", (muted ? "true" : "false"));
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioMuted(muted);
  }
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)
}

#if defined(OHOS_COMPOSITE_RENDER)
void NWebImpl::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (nweb_delegate_) {
    nweb_delegate_->SetShouldFrameSubmissionBeforeDraw(should);
  }
}
#endif  // defined(OHOS_COMPOSITE_RENDER)

void NWebImpl::SetAudioResumeInterval(int32_t resumeInterval) {
#if defined(OHOS_MEDIA_POLICY)
  int32_t interval =
      resumeInterval > kMaxResumeInterval ? kMaxResumeInterval : resumeInterval;
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioResumeInterval(interval);
  }
#endif  // defined(OHOS_MEDIA_POLICY)
}

void NWebImpl::SetAudioExclusive(bool audioExclusive) {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioExclusive(audioExclusive);
  }
#endif  // defined(OHOS_MEDIA_POLICY)
}

void NWebImpl::CloseAllMediaPresentations() {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->CloseAllMediaPresentations();
#endif  // defined(OHOS_MEDIA_POLICY)
}

void NWebImpl::StopAllMedia() {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->StopAllMedia();
#endif  // defined(OHOS_MEDIA_POLICY)
}

void NWebImpl::ResumeAllMedia() {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ResumeAllMedia();
#endif  // defined(OHOS_MEDIA_POLICY)
}

void NWebImpl::PauseAllMedia() {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PauseAllMedia();
#endif  // defined(OHOS_MEDIA_POLICY)
}

int NWebImpl::GetMediaPlaybackState() {
#if defined(OHOS_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return NWebPlaybackState_NONE;
  }
  return nweb_delegate_->GetMediaPlaybackState();
#endif  // defined(OHOS_MEDIA_POLICY)
}

#ifdef OHOS_SCREEN_LOCK
void NWebImpl::RegisterScreenLockFunction(int32_t windowId,
                                          std::shared_ptr<NWebScreenLockCallback> callback) {
  if (nweb_delegate_) {
    nweb_delegate_->SetWakeLockCallback(windowId, callback);
  }
}

void NWebImpl::UnRegisterScreenLockFunction(int32_t windowId) {
  if (nweb_delegate_) {
    nweb_delegate_->SetWakeLockCallback(windowId, nullptr);
  }
}
#endif  // #ifdef OHOS_SCREEN_LOCK

void NWebImpl::NotifyMemoryLevel(int32_t level) {
#ifdef OHOS_PERFORMANCE_MEMORY_THRESHOLD
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;
  static constexpr int32_t kMemoryLevelModerate = 0;
  static constexpr base::TimeDelta kNotifyGapTime = base::Seconds(3);
  static MemoryPressureLevel last_memory_level =
      MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE;
  static base::Time last_notify_time;

  base::Time now = base::Time::Now();
  MemoryPressureLevel memory_pressure_level;
  if (level == kMemoryLevelModerate) {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE;
  } else {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL;
  }

  if (memory_pressure_level == last_memory_level &&
      (now - last_notify_time < kNotifyGapTime)) {
    LOG(INFO) << "The same memory level has been notified within three seconds";
    return;
  }
  last_memory_level = memory_pressure_level;
  last_notify_time = std::move(now);

  LOG(INFO) << "NWebImpl::NotifyMemoryLevel "
            << (level == kMemoryLevelModerate
                    ? "MEMORY_PRESSURE_LEVEL_MODERATE"
                    : "MEMORY_PRESSURE_LEVEL_CRITICAL");
  base::MemoryPressureListener::NotifyMemoryPressure(memory_pressure_level);
#endif  // OHOS_PERFORMANCE_MEMORY_THRESHOLD
}

void NWebImpl::OnWebviewHide() {
  WVLOG_D("NWebImpl::OnWebviewHide");
}

void NWebImpl::OnWebviewShow() {
  WVLOG_D("NWebImpl::OnWebviewShow");
#if defined(OHOS_CLOUD_CONTROL)
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    content::BrowserContext* browser_context =
        cef_browser_context->AsBrowserContext();

    if (browser_context && !browser_context->IsOffTheRecord()) {
      auto context = static_cast<AlloyBrowserContext*>(cef_browser_context);
      if (context) {
        context->OnWebViewShow();
      }
    }
  }
#endif
}

void NWebImpl::OnRenderToBackground() {
  TRACE_EVENT0("base", "OnRenderToBackground");
  WVLOG_D("NWebImpl::OnRenderToBackground");
#if defined(OHOS_WEBRTC)
  StopCameraSession();
#endif
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnRenderToBackground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnWindowHide();
}

void NWebImpl::OnRenderToForeground() {
  TRACE_EVENT0("base", "OnRenderToForeground");
  WVLOG_D("NWebImpl::OnRenderToForeground");
#if defined(OHOS_WEBRTC)
  RestartCameraSession();
#endif
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnRenderToForeground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnWindowShow();
}

void NWebImpl::OnOnlineRenderToForeground() {
  TRACE_EVENT0("base", "OnOnlineRenderToForeground");
  WVLOG_D("NWebImpl::OnOnlineRenderToForeground");
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnOnlineRenderToForeground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnOnlineRenderToForeground();
}

void NWebImpl::NotifyForNextTouchEvent() {
  TRACE_EVENT0("base", "NotifyForNextTouchEvent");
  WVLOG_D("NWebImpl::NotifyForNextTouchEvent");
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("NotifyForNextTouchEvent nweb delegate is null");
    return;
  }
  nweb_delegate_->NotifyForNextTouchEvent();
}

#if BUILDFLAG(IS_OHOS)
void NWebImpl::SetWindowId(uint32_t window_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetWindowId nweb delegate is null");
    return;
  }
  nweb_delegate_->SetWindowId(window_id);
  inputmethod_handler_->SetWindowIdForIME(window_id);
}

void NWebImpl::SetToken(void* token) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetToken nweb delegate is null");
    return;
  }
  nweb_delegate_->SetToken(token);
}

void* NWebImpl::CreateWebPrintDocumentAdapter(const std::string& jobName) {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->CreateWebPrintDocumentAdapter(jobName);
}

void NWebImpl::SetPrintBackground(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetPrintBackground(enable);
}

bool NWebImpl::GetPrintBackground() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetPrintBackground();
}

void NWebImpl::SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) {}

void NWebImpl::PrecompileJavaScript(const std::string& url,
                          const std::string& script,
                          std::shared_ptr<CacheOptions>& cacheOptions,
                          std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "PrecompileJavaScript: nweb delegate has not init.";
    return;
  }
  nweb_delegate_->PrecompileJavaScript(url, script, cacheOptions, callback);
}

void NWebImpl::InjectOfflineResource(const std::string& url,
                                     const std::string& origin,
                                     const std::vector<uint8_t>& resource,
                                     const std::map<std::string, std::string>& responseHeaders,
                                     const int type) {
  auto manager = web_cache::WebCacheManager::GetInstance();
  manager->AddResourceToCache(url, origin, resource, responseHeaders, type);
}
#endif

#if defined(OHOS_INPUT_EVENTS)
void NWebImpl::SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetVirtualKeyBoardArg nweb delegate is null");
    return;
  }
  nweb_delegate_->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool NWebImpl::ShouldVirtualKeyboardOverlay() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("ShouldVirtualKeyboardOverlay nweb delegate is null");
    return false;
  }
  return nweb_delegate_->ShouldVirtualKeyboardOverlay();
}
#endif

#ifdef OHOS_DRAG_DROP
std::shared_ptr<NWebDragData> NWebImpl::GetOrCreateDragData() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return nullptr;
  }

  return nweb_delegate_->GetOrCreateDragData();
}
#endif  // #ifdef OHOS_DRAG_DROP

#if defined(OHOS_NO_STATE_PREFETCH)
void NWebImpl::PrefetchPage(
    const std::string& url,
    const std::map<std::string, std::string>& additionalHttpHeaders) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PrefetchPage(url, additionalHttpHeaders);
}
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_NWEB_EX)
bool NWebImpl::CanStoreWebArchive() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  return nweb_delegate_->CanStoreWebArchive();
}

// static
const std::vector<std::string>& NWebImpl::GetCommandLineArgsForNWebEx() {
  return g_browser_args;
}

void NWebImpl::InitBrowserServiceApi(std::vector<std::string>& browser_args, int32_t api_level) {
  g_browser_args = browser_args;
  g_browser_service_api_enabled = true;
  g_browser_service_sdk_api_level = api_level;
}

bool NWebImpl::GetBrowserServiceApiEnabled() {
  return g_browser_service_api_enabled;
}

int32_t NWebImpl::GetBrowserServiceSdkAPILevel() {
  return g_browser_service_sdk_api_level;
}

void NWebImpl::ReloadOriginalUrl() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->ReloadOriginalUrl();
}

void NWebImpl::SetBrowserUserAgentString(const std::string& user_agent) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetBrowserUserAgentString(user_agent);
}

void NWebImpl::PutWebAppClientExtensionCallback(
    std::shared_ptr<NWebAppClientExtensionCallback>
        web_app_client_extension_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set web app client extension callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->RegisterWebAppClientExtensionListener(
      web_app_client_extension_listener);
}

void NWebImpl::RemoveWebAppClientExtensionCallback() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("remove web app client extension callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->UnRegisterWebAppClientExtensionListener();
}

void NWebImpl::GetImageFromContextNode() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->GetImageFromContextNode();
}

void NWebImpl::GetImageFromCache(const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->GetImageFromCache(url);
}

void NWebImpl::PutWebExtensionCallback(
    std::shared_ptr<NWebExtensionCallback> web_extension_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web extension callback failed, nweb delegate is nullptr, nweb_id "
        "= %{public}u",
        nweb_id_);
    return;
  }
  WVLOG_I("set web extension, nweb_id = %{public}u", nweb_id_);
  nweb_delegate_->RegisterWebExtensionListener(web_extension_listener);
}

void NWebImpl::RemoveWebExtensionCallback() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->UnRegisterWebExtensionListener();
}

#ifdef OHOS_ARKWEB_EXTENSIONS
void NWebImpl::PutWebExtensionApiCallback(
    std::shared_ptr<NWebExtensionApiCallback> web_extension_api_listener) {
  WVLOG_I("register web extension api listener");
  NWebHandlerDelegate::RegisterWebExtensionApiListener(
      web_extension_api_listener);

  NwebExtensionTabDelegateHandler::RegisterWebExtensionTabApiListener(
      web_extension_api_listener);
  NwebExtensionTabDelegateHandler* handler =
      NwebExtensionTabDelegateHandler::GetInstance();
  CefWebExtensionTabManager::GetInstance()->SetTabApiHandle(handler);
}

void NWebImpl::RemoveWebExtensionApiCallback() {
  WVLOG_I("unreqister web extension api listener");
  NWebHandlerDelegate::UnRegisterWebExtensionApiListener();
  NwebExtensionTabDelegateHandler::UnRegisterWebExtensionTabApiListener();
  CefWebExtensionTabManager::GetInstance()->SetTabApiHandle(nullptr);
}

void NWebImpl::PutExtensionContextMenusCallback(
      std::shared_ptr<NWebExtensionContextMenusCallback> extension_context_menus_callback) {
  WVLOG_I("register extension context menus listener");
  NweExtensionContextMenusDelegateHandler::RegisterExtensionContextMenusListener(
      extension_context_menus_callback);
  NweExtensionContextMenusDelegateHandler* handler = NweExtensionContextMenusDelegateHandler::GetInstance();
  CefMenuManager::SetContextMenusHandler(handler);
}
 
void NWebImpl::RemoveExtensionContextMenusCallback() {
  WVLOG_I("unregister extension context menus listener");
  NweExtensionContextMenusDelegateHandler::UnRegisterExtensionContextMenusListener();
  CefMenuManager::SetContextMenusHandler(nullptr);
}
 
// static
void NWebImpl::OnClickedExtensionContextMenus(const std::string& extension_id,
                                             ContextMenusOnClickedData& data,
                                             std::optional<NWebExtensionTab>& tab) {
  LOG(DEBUG) << "OnClickedExtensionContextMenus";
  CefMenuManager::OnClickedExtensionContextMenus(extension_id, data, tab);
}
 
// static
void NWebImpl::GetAllExtensionContextMenus(const std::vector<std::string>& extension_ids,
                                            std::vector<NWebContextMenusItem>& result) {
  LOG(DEBUG) << "GetAllExtensionContextMenus";
  std::vector<NWebContextMenusItem> menu_items = CefMenuManager::GetAllExtensionContextMenus(extension_ids);
  result = menu_items;
}
#endif // OHOS_ARKWEB_EXTENSIONS

#if defined(OHOS_ARKWEB_EXTENSIONS)
//static
void NWebImpl::PutWebExtensionActionApiCallback(
    std::shared_ptr<NWebExtensionActionApiCallback>
        action_api_listener) {
  WVLOG_I("register web extension action api listener");
  NWebExtensionActionCefDelegate::RegisterWebExtensionApiListener(
      action_api_listener);
}

//static
void NWebImpl::RemoveWebExtensionActionApiCallback() {
  WVLOG_I("unreqister web extension action api listener");
  NWebExtensionActionCefDelegate::UnRegisterWebExtensionApiListener();
}

void NWebImpl::WebExtensionActionClicked(std::string extension_id,
                                         const NWebExtensionTab* tab) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionActionClicked(extension_id, tab);
}

//  static
void NWebImpl::WebExtensionErasePopupWindowId(int popupNwebId) {
  extensions::CefExtensionWindowIdManager::ErasePopupWindowId(popupNwebId);
}

//  static
void NWebImpl::WebExtensionSetPopupWindowId(int popupNwebId, int windowId) {
  extensions::CefExtensionWindowIdManager::SetPopupWindowId(popupNwebId,
                                                             windowId);
}

//  static
void NWebImpl::WebExtensionEraseSidePanelWindowId(int sidePanelNwebId) {
  extensions::CefExtensionWindowIdManager::EraseSidePanelWindowId(
      sidePanelNwebId);
}

//  static
void NWebImpl::WebExtensionSetSidePanelWindowId(int sidePanelNwebId,
                                                int windowId) {
  extensions::CefExtensionWindowIdManager::SetSidePanelWindowId(
      sidePanelNwebId, windowId);
}

#endif  // OHOS_ARKWEB_EXTENSIONS

void NWebImpl::OpenDevtools(std::unique_ptr<OpenDevToolsParam> param) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "OpenDevtools failed, no nweb_delegate";
    return;
  }
  int32_t devtools_nweb_id = param->nweb_id;
  NWebImpl* nweb = NWebImpl::FromID(devtools_nweb_id);
  if (!nweb) {
    LOG(WARNING) << "OpenDevtools failed, no nweb";
    return;
  }
  nweb_delegate_->OpenDevtoolsWith(nweb->nweb_delegate_, std::move(param));
}

void NWebImpl::CloseDevtools() {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "CloseDevtools failed, no nweb_delegate";
    return;
  }
  nweb_delegate_->CloseDevtools();
}
#endif  // defined(OHOS_NWEB_EX)

#ifdef OHOS_EX_NETWORK_CONNECTION
// static
void NWebImpl::SetConnectTimeout(int32_t seconds) {
  content::GetNetworkService()->SetConnectTimeout(seconds);
}

void NWebImpl::SetConnectionTimeout(int32_t timeout) {
  if (g_nweb_count == 0) {
    WVLOG_I("nweb had not initiated. Will set timeout value after network service initialized.");
    return;
  }

  network::mojom::NetworkService* network_service = content::GetNetworkService();
  if (!network_service) {
    WVLOG_I("network_service is nullptr. Will set timeout value after network service initialized.");
    return;
  }

  content::GetNetworkService()->SetConnectTimeout(timeout);
  WVLOG_I("set connection timeout value in NetHelpers is: %{public}d",
      net_service::NetHelpers::connection_timeout);
}
#endif

#ifdef OHOS_EX_UA
// static
void NWebImpl::UpdateCloudUAConfig(const std::string& file_path,
                                   const std::string& version) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->UpdateCloudUAConfig(file_path,
                                                                    version);
}

// static
void NWebImpl::UpdateUAListConfig(const std::string& ua_name,
                                  const std::string& ua_string) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->UpdateUAListConfig(ua_name,
                                                                   ua_string);
}

// static
void NWebImpl::SetUAForHosts(const std::string& ua_name,
                             const std::vector<std::string>& hosts) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->SetUAForHosts(ua_name, hosts);
}

// static
std::string NWebImpl::GetUANameConfig(const std::string& host) {
  return nweb_ex::AlloyBrowserUAConfig::GetInstance()->GetUANameConfig(host);
}

// static
void NWebImpl::SetBrowserUA(const std::string& ua_name) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->SetBrowserUA(ua_name);
}
#endif  // OHOS_EX_UA

#if defined(OHOS_VIDEO_ASSISTANT)
// static
void NWebImpl::UpdateBrowserEngineConfig(const std::string& file_path, const std::string& version) {
  nweb_ex::AlloyBrowserEngineCloudConfig::GetInstance()->UpdateBrowserEngineCloudConfig(file_path, version);
}
#endif

#if defined(OHOS_I18N)
void NWebImpl::UpdateAcceptLanguageInternal() {
  const base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(::switches::kLang)) {
    return;
  }
  std::string lang = command_line.GetSwitchValueASCII(::switches::kLang);
  std::regex pattern("-");
  std::smatch match;
  if (std::regex_search(lang, match, pattern)) {
    std::string language = match.prefix();
    std::string region = match.suffix();
    UpdateLocale(language, region);
  }
}
#endif

#if defined(OHOS_EX_FREE_COPY)
void NWebImpl::ShowFreeCopyMenu() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ShowFreeCopyMenu();
}

bool NWebImpl::ShouldShowFreeCopyMenu() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->ShouldShowFreeCopyMenu();
}

#endif  // OHOS_EX_FREE_COPY

#ifdef OHOS_ARKWEB_ADBLOCK
// static
void NWebImpl::SetAdsBlockRules(const std::string& rulesFiles,
                                const bool replace) {
  LOG(DEBUG) << "[adblock] SetAdsBlockRules called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->SetAdsBlockRules(rulesFiles,
                                                                replace);

  ::adblock::AdBlockList::UpdateUserEasyListRules(rulesFiles);
}

// static
void NWebImpl::AddAdsBlockDisallowList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] AddAdsBlockDisallowList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockDisallowList(
      domainSuffixes);
}

// static
void NWebImpl::AddAdsBlockAllowList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] AddAdsBlockAllowList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockAllowList(
      domainSuffixes);
}

// static
void NWebImpl::RemoveAdsBlockDisallowedList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] RemoveAdsBlockDisallowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockDisallowedList(
      domainSuffixes);
}

// static
void NWebImpl::RemoveAdsBlockAllowedList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] RemoveAdsBlockAllowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockAllowedList(
      domainSuffixes);
}

// static
void NWebImpl::ClearAdsBlockDisallowedList() {
  LOG(DEBUG) << "[adblock] ClearAdsBlockDisallowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockDisallowedList();
}

// static
void NWebImpl::ClearAdsBlockAllowedList() {
  LOG(DEBUG) << "[adblock] ClearAdsBlockAllowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockAllowedList();
}

bool NWebImpl::IsAdsBlockEnabledForCurPage() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  LOG(DEBUG) << "[adblock] IsAdsBlockEnabledForCurPage called from ui";
  return nweb_delegate_->IsAdsBlockEnabledForCurPage();
}

bool NWebImpl::IsAdsBlockEnabled() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  LOG(DEBUG) << "[adblock] IsAdsBlockEnabled called from ui";
  return nweb_delegate_->IsAdsBlockEnabled();
}

void NWebImpl::EnableAdsBlock(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  LOG(DEBUG) << "[adblock] EnableAdsBlock called from ui";
  nweb_delegate_->EnableAdsBlock(enable);
}

// static
bool NWebImpl::IsAnyNWebAdblockEnabled() {
  NWebMap* map = g_nweb_map.Pointer();

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  bool replace_switch = OHOS::adblock::AdBlockConfig::GetInstance();

  for (auto it = map->begin(); it != map->end(); it++) {
    auto nweb_weak_ptr = it->second.lock();
    if (nweb_weak_ptr) {
      auto nweb = nweb_weak_ptr.get();

      if (nweb && nweb->IsAdsBlockEnabled() && !replace_switch) {
        return true;
      }
    }
  }
  return false;
}
#endif

#ifdef OHOS_EX_PASSWORD
bool NWebImpl::GetSavePasswordAutomatically() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetSavePasswordAutomatically();
}

void NWebImpl::SetSavePasswordAutomatically(bool enable) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetSavePasswordAutomatically(enable);
}

bool NWebImpl::GetSavePassword() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetSavePassword();
}

void NWebImpl::SetSavePassword(bool enable) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetSavePassword(enable);
}
void NWebImpl::SaveOrUpdatePassword(bool is_update) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SaveOrUpdatePassword(is_update);
}
#endif  // #ifdef OHOS_EX_PASSWORD

#if defined(OHOS_EX_PASSWORD) || (OHOS_DATALIST)
void NWebImpl::PasswordSuggestionSelected(int list_index) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->PasswordSuggestionSelected(list_index);
}
#endif

#if defined(OHOS_EX_FORCE_ZOOM)
void NWebImpl::SetForceEnableZoom(bool forceEnableZoom) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetForceEnableZoom(forceEnableZoom);
}

bool NWebImpl::GetForceEnableZoom() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetForceEnableZoom();
}
#endif //OHOS_EX_FORCE_ZOOM

void NWebImpl::PutWebDownloadDelegateCallback(
    std::shared_ptr<NWebDownloadDelegateCallback>
        web_download_delegate_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set web download delegate callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->RegisterWebDownloadDelegateListener(
      web_download_delegate_listener);
}

void NWebImpl::StartDownload(const char* url) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("start download failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->StartDownload(url);
}

void NWebImpl::ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resume download failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

  nweb_delegate_->ResumeDownload(web_download);
}

#ifdef OHOS_EX_DOWNLOAD
NWebDownloadItemState NWebImpl::GetDownloadItemState(long item_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("GetDownloadItemState failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }

  return nweb_delegate_->GetDownloadItemState(item_id);
}

// static
NWebDownloadItemState NWebImpl::GetDownloadItemStateByGuid(const std::string& guid) {
  LOG(DEBUG) << "get download item state " << guid;
  CefRefPtr<CefDownloadItem> download_item = CefGetDownloadItem(guid);
  if (!download_item) {
    LOG(ERROR) << "GetDownloadItemState failed, for download_item is nullptr, "
      << "guid " << guid;
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }

  return NWebDownloadItem::GetNWebState(download_item);
}
#endif

void NWebImpl::PutAccessibilityEventCallback(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibilityEventListener) {
  // Deprecated due to new accessibility architecture
}

void NWebImpl::PutAccessibilityIdGenerator(
    const AccessibilityIdGenerateFunc accessibilityIdGenerator) {
  // Deprecated due to new accessibility architecture
}

void NWebImpl::ExecuteAction(int64_t accessibilityId, uint32_t action) {
  // Deprecated due to new accessibility architecture
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                          bool isAccessibilityFocus) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetFocusedAccessibilityNodeInfo(
        accessibilityId, isAccessibilityFocus);
  }
  return nullptr;
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetAccessibilityNodeInfoById(int64_t accessibilityId) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoById(accessibilityId);
  }
  return nullptr;
}

bool NWebImpl::GetAccessibilityVisible(int64_t accessibilityId) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityVisible(accessibilityId);
  }
  return true;
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                              int32_t direction) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoByFocusMove(accessibilityId,
                                                               direction);
  }
  return nullptr;
}

void NWebImpl::SetAccessibilityState(bool state) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SetAccessibilityState(state ? STATE_ENABLED
                                                : STATE_DISABLED);
  }
}

#if defined(OHOS_VIDEO_ASSISTANT)
void NWebImpl::EnableVideoAssistant(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "nweb delegate is nullptr when enable video assistant";
    return;
  }
  nweb_delegate_->EnableVideoAssistant(enable);
}

void NWebImpl::ExecuteVideoAssistantFunction(const std::string& cmd_id) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING)
        << "nweb delegate is nullptr when execute video assistant function";
    return;
  }
  nweb_delegate_->ExecuteVideoAssistantFunction(cmd_id);
}

void NWebImpl::OnReportStatisticLog(const std::string& content) {
  if (on_report_statistic_log_callback_) {
    on_report_statistic_log_callback_(content.c_str());
  }
}

void NWebImpl::SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func) {
  on_report_statistic_log_callback_ = func;
}

void NWebImpl::CustomWebMediaPlayer(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "nweb delegate is nullptr when enable custom web media player";
    return;
  }
  nweb_delegate_->CustomWebMediaPlayer(enable);
}
#endif  // defined(OHOS_VIDEO_ASSISTANT)
#if defined(OHOS_ARKWEB_EXTENSIONS)
void NWebImpl::PutWebExtensionApiSidePanelCallback(
    std::shared_ptr<NWebExtensionSidePanelApiCallback> web_extension_api_listener) {
  WVLOG_I("register web extension api side panel listener");
  NWebExtensionSidePanelCefDelegate::RegisterWebExtensionApiListener(
      web_extension_api_listener);
}

void NWebImpl::RemoveWebExtensionApiSidePanelCallback() {
  WVLOG_I("unreqister web extension api side panel listener");
  NWebExtensionSidePanelCefDelegate::UnRegisterWebExtensionApiListener();
}

void NWebImpl::PutWebExtensionWindowsApiCallback(
      std::shared_ptr<NWebExtensionWindowsApiCallback> web_extension_windows_api_callback) {
  LOG(INFO) << "NWebImpl::PutWebExtensionWindowsApiCallback";
  WVLOG_I("register web extension windows api side panel listener");
  NweExtensionWindowDelegateHandler::RegisterWebExtensionWindowsApiListener(
      web_extension_windows_api_callback);
  NweExtensionWindowDelegateHandler* handler = NweExtensionWindowDelegateHandler::GetInstance();
  CefWindowsManager::GetInstance()->SetWindowHandler(handler);
}

void NWebImpl::RemoveWebExtensionWindowsApiCallback() {
  WVLOG_I("unreqister web extension windows api side panel listener");
  NweExtensionWindowDelegateHandler::UnRegisterWebExtensionWindowsApiListener();
}

content::BrowserContext* NWebImplGetGlobalBrowserContext() {
  CefRequestContextImpl* request_context =
      static_cast<CefRequestContextImpl*>(CefRequestContext::GetGlobalContext().get());
  if (!request_context) {
    LOG(ERROR) << "NWebImpl::GetGlobalBrowserContext request_context is null";
    return nullptr;
  }
  CefBrowserContext* cef_browser_context = request_context->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "NWebImpl::GetGlobalBrowserContext cef_browser_context is null";
    return nullptr;
  }
  return cef_browser_context->AsBrowserContext();
}

// static
void NWebImpl::PutWebExtensionManagerCallback(
    std::shared_ptr<NWebExtensionManagerCallBack> web_extension_manager_listener) {
  WVLOG_I("register web extension manager listener");
  extensions::ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener(web_extension_manager_listener);
}

// static
void NWebImpl::RemoveWebExtensionManagerCallback() {
  WVLOG_I("unreqister web extension manager listener");
  extensions::ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
}

// static
void NWebImpl::UnLoadWebExtension(const std::string& eid) {
  WVLOG_I("NWebImpl::UnLoadWebExtension %{public}s", eid.c_str());
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "NWebImpl::UnLoadWebExtension browser_context is null";
    return;
  }
  const extensions::Extension* current_extension =
      extensions::ExtensionRegistry::Get(browser_context)
      ->GetExtensionById(eid, extensions::ExtensionRegistry::EVERYTHING);
  std::u16string* error = nullptr;
  if (current_extension) {
    if (current_extension->was_installed_by_default()) {
      WVLOG_I("NWebImpl::UnLoadWebExtension RemovedDefaultInstalledExtension");
    }

    bool result = extensions::ExtensionSystem::Get(browser_context)
        ->extension_service()
        ->UninstallExtension(eid, extensions::UNINSTALL_REASON_COMPONENT_REMOVED, error);
    WVLOG_I("NWebImpl::UnLoadWebExtension result:%{public}d, error:%{public}s", result, error);
    return;
  }
  WVLOG_I("NWebImpl::UnLoadWebExtension extension not exist!");
}

// static
void NWebImpl::GetExtensionInfoByTabId(int32_t tabId, std::vector<WebExtensionInfo>& extensionsInfo) {
  WVLOG_I("NWebImpl::GetExtensionInfoByTabId, %{public}d", tabId);
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "NWebImpl::GetExtensionInfoByTabId browser_context is null";
    return;
  }

  const extensions::ExtensionRegistry* extensionRegistry = extensions::ExtensionRegistry::Get(browser_context);
  const extensions::ExtensionSet extensions = extensionRegistry->GenerateInstalledExtensionsSet();
  for (const auto& extension : extensions) {
    LOG(INFO) << "NWebImpl::GetExtensionInfoByTabId id=" << extension->id();
    if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
      continue;
    }
    WebExtensionInfo itemInfo;
    itemInfo.extensionId = extension->id();
    extensions::ExtensionRegistryInfoManager* manager =
                      extensions::ExtensionSystem::Get(browser_context)->GetExtensionRegistryInfoManager();
    if (manager) {
      itemInfo.action = manager->GetExtensionActionInfo(*extension, tabId);
      itemInfo.sidePanel = manager->GetExtensionSidePanelInfo(*extension, tabId);
      itemInfo.contextMenus = manager->GetAllExtensionContextMenus(extension->id());
    }
    extensionsInfo.push_back(itemInfo);
  }
}
#endif // OHOS_ARKWEB_EXTENSIONS

#ifdef OHOS_LOGGER_REPORT
void NWebImpl::PutLoggerCallback(
    std::shared_ptr<NWebLoggerCallback> logger_callback) {
  WVLOG_D("put logger callback");
  g_logger_callback = logger_callback;
}

void NWebImpl::RemoveLoggerCallback() {
  WVLOG_D("remove logger callback");
  NWebHandlerDelegate::UnRegisterLoggerCallback();
}
#endif

#if defined(OHOS_EX_NETWORK_CONNECTION)
// static
void NWebImpl::BindToNetwork(int network) {
  net_service::NetHelpers::network = network;
  if (g_nweb_count != 0) {
    network::mojom::NetworkService* network_service = content::GetNetworkService();
    if (network_service) {
        network_service->BindDnsToNetwork(network);
        WVLOG_I("bint to network %{public}d", net_service::NetHelpers::network);
        net::NetworkChangeNotifier::BindToNetwork(network);
    } else {
        WVLOG_E("net_work_service is nullptr");
    }
  }
}
#endif

#ifdef OHOS_EX_GET_ZOOM_LEVEL
void NWebImpl::SetBrowserZoomLevel(double zoom_factor) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetBrowserZoomLevel(zoom_factor);
}

double NWebImpl::GetBrowserZoomLevel() const {
  if (nweb_delegate_ == nullptr) {
    return default_zoom_factor;
  }
  return nweb_delegate_->GetBrowserZoomLevel();
}

// static
void NWebImpl::SetDefaultBrowserZoomLevel(double zoom_factor) {
  if (g_nweb_count == 0) {
    WVLOG_I("nweb had not initiated try to set default browser zoom level.");
    return;
  }
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    auto browser_context = cef_browser_context->AsProfile();
    if (!browser_context || browser_context->IsOffTheRecord()) {
      LOG(ERROR) << "SetDefaultBrowserZoomLevel null browser_context or browser_context is off the record.";
      return;
    }
    if (!browser_context->GetZoomLevelPrefs()) {
      LOG(ERROR) << "GetZoomLevelPrefs is nullptr.";
      return;
    }
    browser_context->GetZoomLevelPrefs()
        ->SetDefaultZoomLevelPref(
            blink::PageZoomFactorToZoomLevel(zoom_factor));
    default_zoom_factor = zoom_factor;
  }
}
#endif

bool NWebImpl::Discard() {
   if (nweb_delegate_ == nullptr) {
    WVLOG_E("Discard failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return false;
   }

  if (!nweb_delegate_->Discard()) {
    return false;
  }

  WVLOG_D("Discard: Notify the bufferq to clean all caches");
  return true;
}

bool NWebImpl::Restore() {
   if (nweb_delegate_ == nullptr) {
    WVLOG_E("Restore failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return false;
   }

   return nweb_delegate_->Restore();
}

#ifdef OHOS_EX_REFRESH_IFRAME
bool NWebImpl::WebExtensionContextMenuIsIframe()
{
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nullptr nweb_delegate_";
    return false;
  }
  return nweb_delegate_->WebExtensionContextMenuIsIframe();
}

void NWebImpl::WebExtensionContextMenuReloadFocusedFrame()
{
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nullptr nweb_delegate_";
    return;
  }
  return nweb_delegate_->WebExtensionContextMenuReloadFocusedFrame();
}
#endif

// static
void NWebImpl::ResumeDownloadStatic(
    std::shared_ptr<NWebDownloadItem> web_download) {
  CefResumeDownload(web_download->guid, web_download->url,
                    web_download->full_path, web_download->received_bytes,
                    web_download->total_bytes, web_download->etag,
                    web_download->mime_type, web_download->last_modified,
                    web_download->received_slices);
}

// static
void NWebImpl::SetFileRenameOption(const int file_rename_option) {
  WVLOG_I("NWebImpl::SetFileRenameOption: option: %{public}d", file_rename_option);
  CefSetFileRenameOption(file_rename_option);
}

#if defined(OHOS_EX_DOWNLOAD)
void NWebImpl::ReadDownloadData(const std::string& guid,
                                const int32_t read_size,
                                ReadDownloadDataCallback callback) {
  WVLOG_I("NWebImpl::ReadDownloadData: option: %{public}s", guid.c_str());
  CefRefPtr<NWebReadDownloadDataCallback> read_download_data_callback = 
      new NWebReadDownloadDataCallback(callback);
  CefReadDownloaData(guid, read_size, read_download_data_callback);
}
#endif  // BUILDFLAG(OHOS_EX_DOWNLOAD)
 
#if defined(OHOS_EX_TOPCONTROLS)
void NWebImpl::UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateBrowserControlsState(constraints, current, animate);
}

void NWebImpl::UpdateBrowserControlsHeight(int height, bool animate) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateBrowserControlsHeight(height, animate);
}
#endif

bool NWebImpl::NeedSoftKeyboard() {
  if (inputmethod_handler_) {
    return inputmethod_handler_->GetIsEditableNode();
  }
  return false;
}

#ifdef OHOS_EX_PERMISSION
void NWebImpl::Grant(NWebPermissionRequest* request,
                     int32_t resourse_id) {
  if (!request) {
    return;
  }
  if (!request->access_request) {
    return;
  }
  request->access_request->Agree(resourse_id);
}

void NWebImpl::Deny(NWebPermissionRequest* request) {
  if (!request) {
    return;
  }
  if (!request->access_request) {
    return;
  }
  request->access_request->Refuse();
}

std::string NWebImpl::GetOrigin(NWebPermissionRequest* request) {
  if (!request) {
    return "";
  }
  if (!request->access_request) {
    return "";
  }
  return request->access_request->Origin();
}

int32_t NWebImpl::GetResourceId(NWebPermissionRequest* request) {
  if (!request) {
    return -1;
  }
  if (!request->access_request) {
    return -1;
  }
 return request->access_request->ResourceAcessId();
}
#endif // OHOS_EX_PERMISSION

#if defined(OHOS_EX_NAVIGATION)
int NWebImpl::InsertBackForwardEntry(int offset, const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return NWebNavigationEntryUpdateResult::ERR_OTHER;
  }
  return nweb_delegate_->InsertBackForwardEntry(offset, url);
}

int NWebImpl::UpdateNavigationEntryUrl(int index, const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return NWebNavigationEntryUpdateResult::ERR_OTHER;
  }
  return nweb_delegate_->UpdateNavigationEntryUrl(index, url);
}

void NWebImpl::ClearForwardList() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearForwardList();
}
#endif

// static
std::shared_ptr<NWeb> NWebImpl::GetNWeb(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = OHOS::NWeb::g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    return it->second.lock();
  }

  return nullptr;
}

#if defined(OHOS_SCHEME_HANDLER)
// static
void NWebImpl::SetWebTag(int32_t nweb_id, const char* web_tag) {
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  OHOS::NWeb::ArkWebNativeObject::BindWebTagToWebInstance(nweb_id, web_tag);
  if (nweb) {
    nweb->SetWebTag(std::string(web_tag));
    return;
  }

  if (nweb_id == -1) {
    WVLOG_D("fail to find a valid nweb with id:%{public}d Tag:%{public}s", nweb_id, web_tag ? web_tag : " ");
  } else {
    WVLOG_E("fail to find a valid nweb with id:%{public}d Tag:%{public}s", nweb_id, web_tag ? web_tag : " ");
  }
}
#endif

// static
void NWebImpl::PrepareForPageLoad(const std::string &url,
                                  bool preconnectable,
                                  int32_t num_sockets) {
#if defined(OHOS_NO_STATE_PREFETCH)
  WVLOG_I("PrepareForPageLoad start.");
  predictor::PreconnectUrlInfo preconnectUrlInfo;
  preconnectUrlInfo.url = url;
  preconnectUrlInfo.num_sockets = num_sockets;
  preconnectUrlInfo.is_preconnectable = preconnectable;
  predictor::PredictorDatabase::preconnect_url_info_list.emplace_back(preconnectUrlInfo);

  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    return;
  }

  CefBrowserContext* context = browser_context_all[0];
  content::BrowserContext* browser_context = context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrepareForPageLoad null browser_context");
    return;
  }

  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    return;
  }

  std::vector<predictor::PreconnectUrlInfo> preconnect_url_infos =
      std::move(predictor::PredictorDatabase::preconnect_url_info_list);
  for(auto& preconnect_url_info : preconnect_url_infos) {
    loading_predictor->PrepareForPageLoad(
      GURL(preconnect_url_info.url), ohos_predictors::HintOrigin::OMNIBOX, preconnect_url_info.is_preconnectable,
      preconnect_url_info.num_sockets);
  }
#endif  // defined(OHOS_NO_STATE_PREFETCH)
}

#if BUILDFLAG(IS_OHOS)
bool NWebImpl::IsSafeBrowsingEnabled() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  return nweb_delegate_->IsSafeBrowsingEnabled();
}

void NWebImpl::EnableSafeBrowsing(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  return nweb_delegate_->EnableSafeBrowsing(enable);
}

void NWebImpl::EnableSafeBrowsingDetection(bool enable, bool strictMode) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->EnableSafeBrowsingDetection(enable, strictMode);
}
#endif

void NWebImpl::StartCamera() {
#if defined(OHOS_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->StartCamera();
  }
#endif  // defined(OHOS_WEBRTC)
}

void NWebImpl::StopCamera() {
#if defined(OHOS_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->StopCamera();
  }
#endif  // defined(OHOS_WEBRTC)
}

void NWebImpl::CloseCamera() {
#if defined(OHOS_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->CloseCamera();
  }
#endif  // defined(OHOS_WEBRTC)
}

void NWebImpl::PauseAllTimers() {
#if defined(OHOS_SUSPEND_ALL_TIMERS)
  if (content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()) {
    content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()
        ->PauseWebKitShardTimersFromOHOS();
  } else {
    WVLOG_I("NWebImpl::PauseAllTimers content_view_ is nullptr");
  }
#endif
}

void NWebImpl::ResumeAllTimers() {
#if defined(OHOS_SUSPEND_ALL_TIMERS)
  if (content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()) {
    content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()
        ->ResumeWebKitShardTimersFromOHOS();
  } else {
    WVLOG_I("NWebImpl::ResumeAllTimers content_view_ is nullptr");
  }
#endif
}

int NWebImpl::GetSecurityLevel() {
#if BUILDFLAG(IS_OHOS)
  if (nweb_delegate_ == nullptr) {
    return static_cast<int>(security_state::SecurityLevel::NONE);
  }

  return nweb_delegate_->GetSecurityLevel();
#else
  return static_cast<int>(security_state::SecurityLevel::NONE);
#endif
}

#if defined(OHOS_EX_EXCEPTION_LIST)
bool SetExceptionList(ContentSettingsType content_type,
                      const std::vector<std::string>& urls,
                      ContentSetting content_setting) {
  static constexpr unsigned int kMaxUrlExceptions = 10000;
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    content::BrowserContext* browser_context = cef_browser_context->AsBrowserContext();
    if (!browser_context) {
      WVLOG_E("SetUrlExceptionList null browser_context");
      return false;
    }

    HostContentSettingsMap* host_content_settings_map = HostContentSettingsMapFactory::GetForProfile(browser_context);
    if (!host_content_settings_map) {
      WVLOG_E("SetUrlExceptionList null host_content_settings_map");
      return false;
    }

    host_content_settings_map->ClearSettingsForOneType(content_type);
    for (unsigned int i = 0; i < urls.size() && kMaxUrlExceptions; ++i) {
      host_content_settings_map
          ->SetContentSettingCustomScope(
              ContentSettingsPattern::FromString(urls[i]),
              ContentSettingsPattern::Wildcard(),
              content_type, content_setting);
    }

    if (content_type == ContentSettingsType::COOKIES) {
      ContentSettingsForOneType cookies_settings;
      host_content_settings_map->GetSettingsForOneType(content_type, &cookies_settings);
      browser_context->ForEachLoadedStoragePartition(base::BindRepeating(
          [](ContentSettingsForOneType settings,content::StoragePartition* storage_partition) {
            storage_partition->GetCookieManagerForBrowserProcess()->SetContentSettings(settings);
          },
          cookies_settings));
    }
  }

  LOG(INFO) << "SetUrlExceptionList Add "
            << (content_setting == CONTENT_SETTING_BLOCK ? "Block" : "Allow")
            << " exception for content type: " << int32_t(content_type)
            << " with " << urls.size();

  return true;
}

// static
bool NWebImpl::SetExceptionListForJavaScriptEnabled(const std::vector<std::string>& urls, bool isEnabled) {
  ContentSetting content_setting = isEnabled ? CONTENT_SETTING_ALLOW : CONTENT_SETTING_BLOCK;
  return SetExceptionList(ContentSettingsType::JAVASCRIPT, urls, content_setting);
}

// static
bool NWebImpl::SetExceptionListForAcceptCookie(const std::vector<std::string>& urls, bool isEnabled) {
  ContentSetting content_setting = isEnabled ? CONTENT_SETTING_ALLOW : CONTENT_SETTING_BLOCK;
  return SetExceptionList(ContentSettingsType::COOKIES, urls, content_setting);
}
#endif

#ifdef OHOS_ITP
void NWebImpl::EnableIntelligentTrackingPrevention(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->EnableIntelligentTrackingPrevention(enable);
}

bool NWebImpl::IsIntelligentTrackingPreventionEnabled() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsIntelligentTrackingPreventionEnabled();
}

// static
bool NWebImpl::IsAnyNWebIntelligentTrackingPreventionEnabled() {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = g_nweb_map.Pointer();
  for (auto it = map->begin(); it != map->end(); it++) {
    auto nweb_weak_ptr = it->second.lock();
    if (nweb_weak_ptr) {
      auto nweb = nweb_weak_ptr.get();
      if (nweb && nweb->IsIntelligentTrackingPreventionEnabled()) {
        return true;
      }
    }
  }
  return false;
}
#endif

#if defined(OHOS_CLIPBOARD)
std::string NWebImpl::GetSelectInfo() {
  if (inputmethod_handler_) {
    return inputmethod_handler_->GetSelectInfo();
  }
  return std::string();
}
#endif

void NWebImpl::OnCreateNativeMediaPlayer(
    std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("set create custome media player callback failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return;
  }

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  nweb_delegate_->RegisterOnCreateNativeMediaPlayerListener(std::move(callback));
#endif // OHOS_CUSTOM_VIDEO_PLAYER
}

// static
void NWebImpl::AddIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
#ifdef OHOS_ITP
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()->
      AddITPBypassingList(hosts);
#endif
}

// static
void NWebImpl::RemoveIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
#ifdef OHOS_ITP
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()->
      RemoveITPBypassingList(hosts);
#endif
}

// static
void NWebImpl::ClearIntelligentTrackingPreventionBypassingList() {
#ifdef OHOS_ITP
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()->
      ClearITPBypassingList();
#endif
}

#if defined(OHOS_EX_SCREEN_CAPTURE)
void NWebImpl::StopScreenCapture(int32_t nweb_id, const char* session_id) {
  if (session_id == nullptr) {
    WVLOG_E("StopScreenCapture session_id is null");
    return;
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("StopScreenCapture nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->StopScreenCapture(nweb_id, session_id);
}

void NWebImpl::PutWebScreenCaptureDelegateCallback(
    std::shared_ptr<NWebScreenCaptureDelegateCallback> callback) {

  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web screen capture delegate callback failed, nweb delegate is nullptr, "
        "nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterScreenCaptureDelegateListener(callback);
}
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)

std::string NWebImpl::GetLastJavascriptProxyCallingFrameUrl() {
#if defined(OHOS_SECURE_JAVASCRIPT_PROXY)
  if (nweb_delegate_ == nullptr) {
    return "";
  }

  return nweb_delegate_->GetLastJavascriptProxyCallingFrameUrl();
#else
  return "";
#endif
}

// static
std::string NWebImpl::GetDefaultUserAgent() {
  return embedder_support::GetUserAgent();
}

int NWebImpl::ScaleGestureChange(double scale, double centerX, double centerY) {
  LOG(INFO) << "NWebImpl::ScaleGestureChange scale:" << scale << " centerX: " << centerX << " centerY: " << centerY;
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return NWEB_ERR;
  }
  return nweb_delegate_->ScaleGestureChange(scale, centerX, centerY);
}

#ifdef OHOS_RENDER_PROCESS_MODE
// static
void NWebImpl::SetRenderProcessMode(RenderProcessMode mode) {
  LOG(INFO) << "SetRenderProcessMode mode:" << (int)mode;
  content::RenderProcessHost::SetRenderProcessMode(
      static_cast<content::RenderProcessMode>(mode));

#if defined(OHOS_SITE_ISOLATION)
  g_siteIsolationMode = ShouldEnableSiteIsolation();
#if defined(REPORT_SYS_EVENT)
  ReportSiteIsolationMode(std::to_string(g_siteIsolationMode));
#endif
#endif
}

// static
RenderProcessMode NWebImpl::GetRenderProcessMode() {
  return static_cast<RenderProcessMode>(
      content::RenderProcessHost::render_process_mode());
}
#endif // OHOS_RENDER_PROCESS_MODE

#if defined(OHOS_SOFTWARE_COMPOSITOR)
// static
void NWebImpl::SetWholeWebDrawing() {
  set_whole_page_drawing = true;
}
#endif

}  // namespace OHOS::NWeb

using namespace OHOS::NWeb;

extern "C" OHOS_NWEB_EXPORT void WebDownloadManager_PutDownloadCallback(NWebDownloadDelegateCallback* callback) {
  if (!callback) {
    WVLOG_E("invalid callback");
    return;
  }
  WVLOG_I("[WebDownloadManager] put download callback.");
  CefRefPtr<NWebDownloadHandlerDelegate> delegate =
      new NWebDownloadHandlerDelegate(nullptr);
  delegate->RegisterWebDownloadDelegateListener(std::make_shared<NWebDownloadDelegateCallback>(*callback));
  CefSetDownloadHandler(delegate);
}

void NWebImpl::PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
                                const std::map<std::string, std::string>& additional_http_headers,
                                const std::string& cache_key,
                                const uint32_t& cache_valid_time) {
  auto it = additional_http_headers.find("Content-Type");
  if (it == additional_http_headers.end() || it->second != "application/x-www-form-urlencoded") {
    WVLOG_E("PrefetchResource post request has no content-type or it is not supported.");
    return;
  }

  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("PrefetchResource has no browser_context");
    return;
  }

  CefBrowserContext* context =browser_context_all[0];
  content::BrowserContext* browser_context =context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrefetchResource null browser_context");
    return;
  }
  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    WVLOG_E("PrefetchResource no load predictor");
    return;
  }
  std::shared_ptr<ohos_predictors::PreRequestInfo> request_info = std::make_shared<ohos_predictors::PreRequestInfo>();
  request_info->url = GURL(pre_args->GetUrl());
  request_info->method = pre_args->GetMethod();
  request_info->request_body = pre_args->GetFormData();

  loading_predictor->PrefetchResource(request_info, additional_http_headers, cache_key, cache_valid_time);
}

void NWebImpl::ClearPrefetchedResource(const std::vector<std::string>& cache_key_list) {
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("PrefetchResource has no browser_context");
    return;
  }

  CefBrowserContext* context =browser_context_all[0];
  content::BrowserContext* browser_context =context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrefetchResource null browser_context");
    return;
  }
  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    WVLOG_E("PrefetchResource no load predictor");
    return;
  }
  loading_predictor->ClearPrefetchedResource(cache_key_list);
}

// static
void NWebImpl::WarmupServiceWorker(const std::string &url) {
#if defined(OHOS_WARMUP_SERVICEWORKER)
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("WarmupServiceWorker has no browser_context.");
    return;
  }

  content::BrowserContext* browser_context =
      browser_context_all[0]->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("WarmupServiceWorker has null browser_context.");
    return;
  }

  content::StoragePartition* storage_partition =
      browser_context->GetDefaultStoragePartition();
  if (!storage_partition) {
    WVLOG_E("WarmupServiceWorker has null storage_partition.");
    return;
  }

  content::ServiceWorkerContext* service_worker_context =
      storage_partition->GetServiceWorkerContext();
  if (!service_worker_context) {
    WVLOG_E("WarmupServiceWorker has null service_worker_context.");
    return;
  }

  if (!content::OriginCanAccessServiceWorkers(GURL(url))) {
    WVLOG_E("Input url can not access service worker.");
    return;
  }

  const blink::StorageKey key =
      blink::StorageKey::CreateFirstParty(url::Origin::Create(GURL(url)));
  if (!service_worker_context->MaybeHasRegistrationForStorageKey(key)) {
    WVLOG_E("WarmupServiceWorker has no registration for storage key.");
    return;
  }

  WVLOG_I("Start to warm up service worker.");
  service_worker_context->WarmUpServiceWorker(GURL(url), key, base::DoNothing());
#endif
}

// static
void NWebImpl::SetHostIP(const std::string &hostName, const std::string &address, int32_t aliveTime) {
#if defined(OHOS_CUSTOM_DNS)
  net_service::NetHelpers::SetHostIP(hostName, address, aliveTime);
  auto it = net_service::NetHelpers::GetHostIP(hostName);
  if (it.size() == 0) {
    WVLOG_E("fail to set host IP.");
    return;
  }

  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    if (!cef_browser_context) {
      WVLOG_E("SetHostIP null browser_context");
      return;
    }
    cef_browser_context->GetNetworkContext()->SetHostIP(hostName, it, aliveTime);
  }
  WVLOG_I("Set host IP successfully.");
#endif
}

// static
void NWebImpl::ClearHostIP(const std::string &hostName) {
#if defined(OHOS_CUSTOM_DNS)
  auto it = net_service::NetHelpers::GetHostIP(hostName);
  if (it.size() == 0) {
    WVLOG_E("NWeb has not set the host IP.");
    return;
  }

  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    if (!cef_browser_context) {
      WVLOG_E("ClearHostIP null browser_context");
      return;
    }
    cef_browser_context->GetNetworkContext()->ClearHostIP(hostName);
  }
  net_service::NetHelpers::ClearHostIP(hostName);
  WVLOG_I("Clear host IP successfully.");
#endif
}

void NWebImpl::SuggestionSelected(int index) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->SuggestionSelected(index);
}

void NWebImpl::PutSpanstringConvertHtmlCallback(
    std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback) {
  ui::ClipboardOHOS::SetConvertHtmlCallback(callback);
}

#ifdef OHOS_AI
void NWebImpl::OnTextSelected() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnTextSelected();
}

void NWebImpl::OnDestroyImageAnalyzerOverlay() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnDestroyImageAnalyzerOverlay();
}

#endif

#ifdef OHOS_DISPLAY_CUTOUT
void NWebImpl::OnSafeInsetsChange(int left, int top, int right, int bottom) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "OnSafeInsetsChange failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }
  WVLOG_D(
      "OnSafeInsetsChange nweb_id:%{public}u "
      "%{public}d,%{public}d,%{public}d,%{public}d",
      nweb_id_, left, top, right, bottom);
  nweb_delegate_->OnSafeInsetsChange(left, top, right, bottom);
}
#endif

#ifdef OHOS_SOFTWARE_COMPOSITOR
bool NWebImpl::WebPageSnapshot(const char* id,
                               PixelUnit type,
                               int width,
                               int height,
                               const WebSnapshotCallback callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("WebPageSnapshot failed, nweb delegate is nullptr");
    return false;
  }
  return nweb_delegate_->WebPageSnapshot(id, type, width, height, callback);
}
#endif

#ifdef BUILDFLAG(IS_OHOS)
void NWebImpl::OnConfigurationUpdated(
    std::shared_ptr<NWebSystemConfiguration> configuration) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_I("NWebImpl::OnConfigurationUpdated nweb_delegate_ is nullptr");
    return;
  }
  if (configuration->GetThemeFlags() &
      static_cast<uint8_t>(SystemThemeFlags::THEME_FONT)) {
#ifdef OHOS_THEME_FONT
    for (content::RenderProcessHost::iterator host_iterator =
             content::RenderProcessHost::AllHostsIterator();
         !host_iterator.IsAtEnd(); host_iterator.Advance()) {
      content::RenderProcessHost* host = host_iterator.GetCurrentValue();
      if (host->IsInitializedAndNotDead()) {
        host->OnThemeFontChange();
      }
    }
#endif  // OHOS_THEME_FONT
  }
}
#endif  //  IS_OHOS

int NWebImpl::SetUrlTrustList(const std::string& urlTrustList) {
  return 0;
}

int NWebImpl::SetUrlTrustListWithErrMsg(
  const std::string& urlTrustList, std::string& detailErrMsg) {
#if OHOS_URL_TRUST_LIST
  if (nweb_delegate_ == nullptr) {
    return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
  }

  return nweb_delegate_->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsg);
#else
  return -1;
#endif
}

#ifdef OHOS_NETWORK_LOAD
void NWebImpl::SetPathAllowingUniversalAccess(
    const std::vector<std::string>& pathList,
    const std::vector<std::string>& moduleName,
    std::string& errorPath) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  if (pathList.empty()) {
    LOG(INFO) << "SetPathAllowingUniversalAccess empty";
    nweb_delegate_->SetPathAllowingUniversalAccess(pathList);
    return;
  }
  std::vector<base::FilePath> res_dir_path_list;
  std::vector<base::FilePath> file_dir_path_list;
  for (auto& name: moduleName) {
    res_dir_path_list.push_back(base::FilePath(
      "/data/storage/el1/bundle/" + name + "/resources/resfile"));
    file_dir_path_list.push_back(base::FilePath(
      "/data/storage/el2/base/haps/" + name + "/files"));
  }
  file_dir_path_list.push_back(base::FilePath("/data/storage/el2/base/files"));

  for (auto& p: pathList) {
    base::FilePath path(p);
    auto real_path = base::MakeAbsoluteFilePathNoResolveSymbolicLinks(path).value_or(base::FilePath());
    if (real_path.empty()) {
      errorPath = p;
      return;
    }
    bool valid = false;
    for (auto& res_dir: res_dir_path_list) {
      if (res_dir.IsParent(real_path) || res_dir == real_path) {
        valid = true;
        break;
      }
    }
    if (valid) {
      continue;
    }
    for (auto& file_dir: file_dir_path_list) {
      if (file_dir.IsParent(real_path)) {
        valid = true;
        break;
      }
    }
    if (!valid) {
      errorPath = p;
      return;
    }
  }
  nweb_delegate_->SetPathAllowingUniversalAccess(pathList);
}
#endif

void NWebImpl::PerformAction(int64_t accessibilityId, uint32_t action,
  const std::map<std::string, std::string>& actionArguments) {
  // Deprecated due to new accessibility architecture
}

bool NWebImpl::PerformActionV2(int64_t accessibilityId, uint32_t action,
      const std::map<std::string, std::string>& actionArguments) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->ExecuteAction(accessibilityId, action, actionArguments);
  }
  return false;
}

bool NWebImpl::GetAccessibilityNodeRectById(int64_t accessibilityId,
                                            int32_t* width,
                                            int32_t* height,
                                            int32_t* offsetX,
                                            int32_t* offsetY) {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeRectById(
        accessibilityId, width, height, offsetX, offsetY);
  }
  return false;
}

void NWebImpl::SendAccessibilityHoverEvent(int32_t x, int32_t y) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SendAccessibilityHoverEvent(x, y);
  }
}

#ifdef OHOS_MIXED_CONTENT
void NWebImpl::EnableMixedContentAutoUpgrades(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "EnableMixedContentAutoUpgrades failed,"
                  "for nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->EnableMixedContentAutoUpgrades(enable);
}

bool NWebImpl::IsMixedContentAutoUpgradesEnabled() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "IsMixedContentAutoUpgradesEnabled failed"
                  "for nweb_delegate_ is nullptr.";
    return false;
  }

  return nweb_delegate_->IsMixedContentAutoUpgradesEnabled();
}
#endif

#ifdef OHOS_ARKWEB_EXTENSIONS
void NWebImpl::WebExtensionTabCreateCallback(int request_id,
                                             const NWebExtensionTab* tab) {
  LOG(DEBUG) << "WebExtensionTabCreateCallback request_id= " << request_id;
  NWebHandlerDelegate::WebExtensionTabCreateCallback(request_id, tab);
}

void NWebImpl::WebExtensionTabCreated(int tab_id) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabCreated(tab_id);
}

void NWebImpl::WebExtensionTabRemoved(int tab_id) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabRemoved(tab_id);
}

void NWebImpl::WebExtensionTabUpdated(
    int tab_id,
    const std::vector<std::string>& changed_property_names,
    const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabUpdated(tab_id, changed_property_names, url);
}

void NWebImpl::WebExtensionTabUpdated(
    int tab_id,
    const std::vector<std::string>& changed_property_names,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabUpdated(tab_id, changed_property_names, std::move(changeInfo));
}

void NWebImpl::WebExtensionTabActivated(
    std::unique_ptr<NWebExtensionTabActiveInfo> activeInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabActivated(std::move(activeInfo));
}

void NWebImpl::WebExtensionTabAttached(
    std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabAttached(std::move(attachInfo));
}

void NWebImpl::WebExtensionTabDetached(
    std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabDetached(std::move(detachInfo));
}

void NWebImpl::WebExtensionTabHighlighted(int32_t tab_id, int32_t window_id) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabHighlighted(tab_id, window_id);
}

void NWebImpl::WebExtensionTabMoved(
    int32_t tab_id,
    std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabMoved(tab_id, std::move(moveInfo));
}

void NWebImpl::WebExtensionTabReplaced(int32_t addedTabId,
                                       int32_t removedTabId) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabReplaced(addedTabId, removedTabId);
}

void NWebImpl::WebExtensionTabZoomChange(
    std::unique_ptr<NWebExtensionTabZoomChangeInfo> tabZoomChangeInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabZoomChange(std::move(tabZoomChangeInfo));
}
#endif  // OHOS_ARKWEB_EXTENSIONS

void NWebImpl::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) {
#ifdef OHOS_BFCACHE
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to set back forward cache options. nweb_delegate is nullptr.");
    return;
  }

  nweb_delegate_->SetBackForwardCacheOptions(size, timeToLive);
#endif
}

#ifdef OHOS_MEDIA_NETWORK_TRAFFIC_PROMPT
void NWebImpl::EnableMediaNetworkTrafficPrompt(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "EnableMediaNetworkTrafficPrompt failed, no nweb_delegate_"
               << ", nweb_id_[" << nweb_id_ << "]";
    return;
  }
  LOG(INFO) << "NWebImpl::EnableMediaNetworkTrafficPrompt(" << enable
            << "), nweb_id_[" << nweb_id_ << "]";
  nweb_delegate_->EnableMediaNetworkTrafficPrompt(enable);
}
#endif // OHOS_MEDIA_NETWORK_TRAFFIC_PROMPT

void NWebImpl::SetSurfaceDensity(const double& density) {
  device_pixel_ratio_ = density;
  if (!inputmethod_handler_) {
    WVLOG_E("inputmethod_handler_ is nullptr");
    return;
  }
  inputmethod_handler_->SetVirtualDeviceRatio(device_pixel_ratio_);
  if(nweb_delegate_ == nullptr) {
    WVLOG_E("SetVirtualDeviceRatio failed, nweb_delegate is nullptr.");
    return;
  }
  nweb_delegate_->SetSurfaceDensity(density);
}

void NWebImpl::TrimMemoryByPressureLevel(int32_t memoryLevel) {
#ifdef OHOS_PERFORMANCE_MEMORY_THRESHOLD
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;
  static constexpr int32_t kMemoryLevelModerate = 0;
  static constexpr base::TimeDelta kNotifyGapTime = base::Seconds(3);
  static MemoryPressureLevel last_memory_level =
      MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE;
  static base::Time last_notify_time;

  base::Time now = base::Time::Now();
  MemoryPressureLevel memory_pressure_level;
  if (memoryLevel == kMemoryLevelModerate) {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE;
  } else {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL;
  }

  if (memory_pressure_level == last_memory_level &&
      (now - last_notify_time < kNotifyGapTime)) {
    LOG(INFO) << "The same memory level has been notified within three seconds";
    return;
  }
  last_memory_level = memory_pressure_level;
  last_notify_time = std::move(now);

  LOG(INFO) << "NWebImpl::NotifyMemoryLevel "
            << (memoryLevel == kMemoryLevelModerate
                    ? "MEMORY_PRESSURE_LEVEL_MODERATE"
                    : "MEMORY_PRESSURE_LEVEL_CRITICAL");
  base::MemoryPressureListener::NotifyMemoryPressure(memory_pressure_level);
#endif  // OHOS_PERFORMANCE_MEMORY_THRESHOLD
}

#if defined(OHOS_DISPATCH_BEFORE_UNLOAD)
bool NWebImpl::NeedToFireBeforeUnloadOrUnloadEvents() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->NeedToFireBeforeUnloadOrUnloadEvents();
}

void NWebImpl::DispatchBeforeUnload() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->DispatchBeforeUnload();
}

#endif // OHOS_DISPATCH_BEFORE_UNLOAD

void NWebImpl::SetProxyOverride(
    const std::vector<std::string>& proxyUrls,
    const std::vector<std::string>& proxySchemeFilters,
    const std::vector<std::string>& bypassRules,
    const bool& reverseBypass,
    std::shared_ptr<NWebProxyChangedCallback> callback) {
  std::vector<net::ProxyConfigServiceOHOS::ProxyOverrideRule> proxyRules;
  int size = proxySchemeFilters.size();
  DCHECK(proxySchemeFilters.size() == proxyUrls.size());
  proxyRules.reserve(size);
  for (int i = 0; i < size; i++) {
    proxyRules.emplace_back(proxySchemeFilters[i], proxyUrls[i]);
  }
  NWEB::ProxyConfigMonitor::GetInstance()->SetProxyOverride(proxyRules, bypassRules, reverseBypass,
      base::BindOnce([](std::shared_ptr<NWebProxyChangedCallback> napiCallback) {napiCallback->OnChanged();},
                     std::move(callback)));
}

void NWebImpl::RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback) {
  NWEB::ProxyConfigMonitor::GetInstance()->ClearProxyOverride(base::BindOnce(
        [](std::shared_ptr<NWebProxyChangedCallback> napiCallback) {napiCallback->OnChanged();},
          std::move(callback)));
}

#if defined(OHOS_EDM_POLICY)
// static
void NWebImpl::SetEnterprisePolicy(const std::string& policy, int version) {
  policy::BrowserPolicyHandler::GetInstance()->SetPolicyAndNotify(policy,
                                                                  version);
}
#endif

void NWebImpl::DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width) {
  LOG(DEBUG) << "start drag resize ";
  bool drag_bigger_height = false;
  bool drag_bigger_width = false;
  if (input_handler_ == nullptr || output_handler_ == nullptr) {
    return;
  }
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetDragResizeStart(true);
  if (pre_height > 0) {
    drag_bigger_height = true;
  }
  if (pre_width > 0) {
    drag_bigger_width = true;
  }
  if (drag_bigger_height) {
    height = OHOS::NWeb::NWebResizeHelper::GetInstance().GetResizeAdjustValue(height,
                                                                              pre_height,
                                                                              true);
  }
  if (drag_bigger_width) {
    width = OHOS::NWeb::NWebResizeHelper::GetInstance().GetResizeAdjustValue(width,
                                                                             pre_width,
                                                                             false);
  }
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetResizeHeightAndWidth(height, width);
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      OHOS::NWeb::NWebResizeHelper::GetInstance().RefreshParam();
      LOG(ERROR) << "size too large in surface mode width = " << width << "height = " << height;
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "resize failed, nweb delegate is nullptr, nweb_id = " << nweb_id_;
    return;
  }
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(SOC_PERF_WEB_DRAG_RESIZE_ID, true);
  nweb_delegate_->SetDrawMode(draw_mode_);
  nweb_delegate_->Resize(width, height, false);
  output_handler_->Resize(width, height);
}

bool NWebImpl::IsNWebEx() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kForBrowser)) {
    LOG(DEBUG) << "IsNWebEx is true";
    return true;
  }
  return false;
}
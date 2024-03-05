/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifdef OHOS_PERFORMANCE_MEMORY_THRESHOLD
#include "base/memory/memory_pressure_listener.h"
#include "base/time/time.h"
#endif  // OHOS_PERFORMANCE_MEMORY_THRESHOLD

#include "base/ohos/sys_info_utils.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"
#include "cef_delegate/nweb_download_handler_delegate.h"
#include "ndk/arkweb_native_object.h"
#include "nweb_delegate_adapter.h"
#include "nweb_export.h"
#include "nweb_handler.h"
#include "nweb_hilog.h"
#include "nweb_hit_test_result_impl.h"
#include "res_sched_client_adapter.h"

#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif
#if BUILDFLAG(IS_OHOS) && defined(OHOS_PERFORMANCE_INC_FREQ)
#include "soc_perf_client_adapter.h"
#endif

#ifdef OHOS_SCREEN_LOCK
#include "services/device/wake_lock/power_save_blocker/nweb_screen_lock_tracker.h"
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

#ifdef OHOS_EX_GET_ZOOM_LEVEL
#include "third_party/blink/public/common/page/page_zoom.h"
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "cef/libcef/browser/alloy/alloy_browser_context.h"
#endif

namespace {
uint32_t g_nweb_count = 0;
const uint32_t kSurfaceMaxWidth = 7680;
const uint32_t kSurfaceMaxHeight = 7680;
#if defined(OHOS_MEDIA_POLICY)
const int32_t kMaxResumeInterval = 60;
#endif  // defined(OHOS_MEDIA_POLICY)
const float richtextDisplayRatio = 1.0;

#if defined(OHOS_NWEB_EX)
bool g_browser_service_api_enabled = false;
base::LazyInstance<std::vector<std::string>>::DestructorAtExit g_browser_args =
    LAZY_INSTANCE_INITIALIZER;
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

bool GetWebOptimizationValue() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetWebOptimizationValue();
}

static bool GetLockdownModeStatus() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  return system_properties_adapter.GetLockdownModeStatus();
}

static std::string GetNetlogMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  return system_properties_adapter.GetNetlogMode();
}

#if defined(OHOS_API_INIT_WEB_ENGINE)
void InitialWebEngineArgs(std::list<std::string>& web_engine_args,
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
  web_engine_args.emplace_back("--enable-viewport");
  web_engine_args.emplace_back(
      "--browser-subprocess-path=/system/bin/web_render");
  web_engine_args.emplace_back("--zygote-cmd-prefix=/system/bin/web_render");
  web_engine_args.emplace_back("--remote-debugging-port=9222");
  web_engine_args.emplace_back("--enable-touch-drag-drop");
  web_engine_args.emplace_back("--gpu-rasterization-msaa-sample-count=1");
  // enable aggressive domstorage flushing to minimize data loss
  // http://crbug.com/479767
  web_engine_args.emplace_back("--enable-aggressive-domstorage-flushing");
  web_engine_args.emplace_back("--ohos-enable-drdc");

  std::vector<std::string> modeVector = {"Default", "IncludeSensitive", "Everything"};
  if (std::find(modeVector.begin(), modeVector.end(), GetNetlogMode()) != modeVector.end()) {
    web_engine_args.emplace_back("--net-log-capture-mode=" + GetNetlogMode());
    web_engine_args.emplace_back("--log-net-log=/data/storage/el2/base/cache/web/netlog.json");
  }

  if (GetLockdownModeStatus()) {
    WVLOG_W("In lockdown mode, some HTML5 features will be unavailable, including WebAssembly, WebGL, PDF viewer, MathML, speech recognition, etc.");
    web_engine_args.emplace_back("--js-flags=--jitless");
    web_engine_args.emplace_back("--disable-webgl");
    web_engine_args.emplace_back("--disable-webgl2");
    web_engine_args.emplace_back("--disable-pdf-extension");
    web_engine_args.emplace_back("--disable-blink-features=MathMLCore,ScriptedSpeechRecognition");
#if defined(REPORT_SYS_EVENT)
    ReportLockdownModeStatus();
#endif
  }

  web_engine_args.emplace_back("--enable-media-stream");
  if (init_args->GetIsEnhanceSurface()) {
    WVLOG_I("is_enhance_surface is true");
    web_engine_args.emplace_back("--ohos-enhance-surface");
  }

  auto args_to_delete = init_args->GetArgsToDelete();
  for (auto arg : args_to_delete) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(), arg);
    if (it != web_engine_args.end()) {
      web_engine_args.erase(it);
    }
  }
  auto args_to_add = init_args->GetArgsToAdd();
  for (auto arg : args_to_add) {
    web_engine_args.emplace_back(arg);
  }
  if (init_args->GetIsMultiRendererProcess()) {
    web_engine_args.emplace_back("--enable-multi-renderer-process");
  }
#ifdef OHOS_NWEB_EX
  auto args = g_browser_args.Get();
  for (const std::string& arg : args) {
    web_engine_args.emplace_back(arg);
  }
#endif  // OHOS_NWEB_EX
}
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)
}  // namespace

namespace OHOS::NWeb {

// static
std::shared_ptr<NWeb>
NWebImpl::CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info) {
  if (!create_info) {
    return nullptr;
  }
  static uint32_t current_nweb_id = 0;
  uint32_t nweb_id = ++current_nweb_id;
  TRACE_EVENT1("NWebImpl", "NWebImpl | CreateNWeb", "nweb_id", nweb_id);
  WVLOG_I("creating nweb %{public}u, size %{public}u*%{public}u", nweb_id,
          create_info->GetWidth(), create_info->GetHeight());
  std::shared_ptr<NWebEngineInitArgs> init_args = create_info->GetEngineInitArgs();
  bool is_enhance_surface = init_args ? init_args->GetIsEnhanceSurface() : false;
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
  if (!init_args) {
    return;
  }

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

void NWebImpl::AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb) {
  if (nweb) {
    std::weak_ptr<NWebImpl> nweb_weak(nweb);
    g_nweb_map.Get().emplace(id, nweb_weak);
  }
}

NWebImpl* NWebImpl::FromID(int32_t nweb_id) {
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

  return true;
}

void NWebImpl::OnDestroy() {
  WVLOG_I("NWebImpl::OnDestroy, nweb_id = %{public}u", nweb_id_);

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
  bool is_close_all = (--g_nweb_count) == 0 ? true : false;
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
  std::string dump_path = init_args->GetDumpPath();
  if (!dump_path.empty() && output_handler_ != nullptr) {
    output_handler_->SetDumpPath(dump_path);
  }
  bool frame_info_dump = init_args->GetIsFrameInfoDump();
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
#if BUILDFLAG(IS_OHOS)
      if (nweb_delegate_ && nweb_delegate_->GetBaseDisplayWidth() > 0) {
        device_pixel_ratio_ =
            display->GetWidth() / nweb_delegate_->GetBaseDisplayWidth();
      } else {
        device_pixel_ratio_ = display->GetVirtualPixelRatio();
      }
#else
      device_pixel_ratio_ = display->GetVirtualPixelRatio();
#endif
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
  if (!init_args) {
    return false;
  }

  is_enhance_surface_ = init_args->GetIsEnhanceSurface();
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

  WVLOG_D("nweb create_info.init_args.is_popup: %{public}d",
          init_args->GetIsPopup());
  nweb_delegate_ = NWebDelegateAdapter::CreateNWebDelegate(
      argc, argv, is_enhance_surface_, window, init_args->GetIsPopup()
#if defined(OHOS_EX_DOWNLOAD)
      , nweb_id_
#endif
#if defined(OHOS_INCOGNITO_MODE)
      , create_info->GetIsIncognitoMode()
#endif
      );
  WVLOG_D("nweb create_info.incognito_mode: %{public}d",
          create_info->GetIsIncognitoMode());
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to create nweb delegate of web engine");
    delete[] argv;
    return false;
  }

  if (!SetVirtualDeviceRatio()) {
    WVLOG_E("fail to set virtual device ratio");
    delete[] argv;
    return false;
  }

  std::weak_ptr<NWebOutputHandler> output_handler_weak(output_handler_);
  auto render_update_cb = [output_handler_weak](const char* buffer) -> void {
    if (!output_handler_weak.expired()) {
      output_handler_weak.lock()->OnRenderUpdate(buffer);
    }
  };

  uint32_t width, height;
  output_handler_->GetWindowInfo(width, height);
  nweb_delegate_->Resize(width, height);
  nweb_delegate_->RegisterRenderCb(render_update_cb);

  inputmethod_handler_ = new NWebInputMethodHandler();
  if (!inputmethod_handler_) {
    WVLOG_E("inputmethod_handler_ is nullptr");
    return false;
  }
  nweb_delegate_->SetInputMethodClient(inputmethod_handler_);
  nweb_delegate_->SetNWebDelegateInterface(nweb_delegate_);
  inputmethod_handler_->SetVirtualDeviceRatio(device_pixel_ratio_);

#if defined(REPORT_SYS_EVENT)
    nweb_delegate_->SetNWebId(nweb_id_);
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

void NWebImpl::Resize(uint32_t width, uint32_t height, bool isKeyboard) {
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
  nweb_delegate_->SetDrawMode(draw_mode_);
  nweb_delegate_->Resize(width, height, isKeyboard);
  output_handler_->Resize(width, height);
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

void NWebImpl::OnTouchPress(int32_t id, double x, double y, bool from_overlay) {
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::CLICK, nweb_id_);
  input_handler_->OnTouchPress(id, x, y, from_overlay);
}

void NWebImpl::OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) {
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

void NWebImpl::OnPause() {
  if (!GetWebOptimizationValue()) {
    LOG(DEBUG) << "WebOptimization disabled.";
    return;
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnPause();
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
  nweb_delegate_->OnContinue();
  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (inputmethod_handler_->Reattach(
          nweb_id_, NWebInputMethodHandler::ReattachType::FROM_CONTINUE)) {
    nweb_delegate_->OnFocus();
  }
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
    std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                    std::vector<size_t>&)>>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeJSProxy(objName, methodName,
                                          std::move(callback));
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
                                               object_id);
}

void NWebImpl::UnregisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->UnregisterArkJSfunction(object_name, method_list);
}

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
  inputmethod_handler_->HideTextInput(
      nweb_id_, NWebInputMethodClient::HideTextinputType::FROM_ONBLUR);
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

void NWebImpl::SendDragEvent(const DragEvent& dragEvent) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return;
  }
  DelegateDragEvent event;
  event.action = static_cast<DelegateDragAction>(dragEvent.action);
  event.x = dragEvent.x;
  event.y = dragEvent.y;
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

void NWebImpl::SlideScroll(float vx, float vy) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->SlideScroll(vx, vy);
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
  NWebScreenLockTracker::Instance().AddScreenLock(windowId, nweb_id_, [callback](bool key) {
    if (callback) {
      callback->Handle(key);
    }
  });
}

void NWebImpl::UnRegisterScreenLockFunction(int32_t windowId) {
  NWebScreenLockTracker::Instance().RemoveScreenLock(windowId, nweb_id_);
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

#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
void NWebImpl::SetEnableBlankTargetPopupIntercept(
    bool enableBlankTargetPopup) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetEnableBlankTargetPopupIntercept(enableBlankTargetPopup);
}
#endif

void NWebImpl::OnWebviewHide() {
#if defined(OHOS_WEBRTC)
  StopCameraSession();
#endif
}

void NWebImpl::OnWebviewShow() {
#if defined(OHOS_WEBRTC)
  RestartCameraSession();
#endif
}

#if BUILDFLAG(IS_OHOS)
void NWebImpl::SetWindowId(uint32_t window_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetWindowId nweb delegate is null");
    return;
  }
  nweb_delegate_->SetWindowId(window_id);
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
  return g_browser_args.Get();
}

void NWebImpl::InitBrowserServiceApi(std::vector<std::string>& browser_args) {
  auto args = g_browser_args.Pointer();
  args->clear();
  for (const std::string& arg : browser_args) {
    args->push_back(arg);
  }
  g_browser_service_api_enabled = true;
}

bool NWebImpl::GetBrowserServiceApiEnabled() {
  return g_browser_service_api_enabled;
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
#endif  // defined(OHOS_NWEB_EX)

#ifdef OHOS_EX_NETWORK_CONNECTION
// static
void NWebImpl::SetConnectTimeout(int32_t seconds) {
  content::GetNetworkService()->SetConnectTimeout(seconds);
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

#if defined(OHOS_EX_FREE_COPY)
void NWebImpl::SelectAndCopy() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SelectAndCopy();
}

bool NWebImpl::ShouldShowFreeCopy() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->ShouldShowFreeCopy();
}

#endif  // OHOS_EX_FREE_COPY

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

void NWebImpl::PasswordSuggestionSelected(int list_index) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->PasswordSuggestionSelected(list_index);
}
#endif  // #ifdef OHOS_EX_PASSWORD

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

void NWebImpl::PutAccessibilityEventCallback(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibilityEventListener) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterAccessibilityEventListener(
        accessibilityEventListener);
  }
}

void NWebImpl::PutAccessibilityIdGenerator(
    const AccessibilityIdGenerateFunc accessibilityIdGenerator) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterAccessibilityIdGenerator(accessibilityIdGenerator);
  }
}

void NWebImpl::ExecuteAction(int64_t accessibilityId, uint32_t action) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->ExecuteAction(accessibilityId, action);
  }
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

bool NWebImpl::Discard() {
   if (nweb_delegate_ == nullptr) {
    WVLOG_E("Discard failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return false;
   }

   return nweb_delegate_->Discard();
}
bool NWebImpl::Restore() {
   if (nweb_delegate_ == nullptr) {
    WVLOG_E("Restore failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return false;
   }

   return nweb_delegate_->Restore();
}

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

// static
void NWebImpl::ResumeDownloadStatic(
    std::shared_ptr<NWebDownloadItem> web_download) {
  CefResumeDownload(web_download->guid, web_download->url,
                    web_download->full_path, web_download->received_bytes,
                    web_download->total_bytes, web_download->etag,
                    web_download->mime_type, web_download->last_modified,
                    web_download->received_slices);
}

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

// static
std::shared_ptr<NWeb> NWebImpl::GetNWeb(int32_t nweb_id) {
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
  if (!nweb) {
    WVLOG_E("fail to find a valid nweb with %{public}d", nweb_id);
    return;
  }

  nweb->SetWebTag(std::string(web_tag));
}
#endif

// static
void NWebImpl::PrepareForPageLoad(const std::string &url,
                                  bool preconnectable,
                                  int32_t num_sockets) {
#if defined(OHOS_NO_STATE_PREFETCH)
  if (g_nweb_count != 0) {
    for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
      content::BrowserContext* browser_context =
          cef_browser_context->AsBrowserContext();
      if (!browser_context) {
        LOG(ERROR) << "PrepareForPageLoad null browser_context";
        return;
      }
      ohos_predictors::LoadingPredictor* loading_predictor =
          ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
              browser_context);
      if (loading_predictor) {
        loading_predictor->num_sockets_ = (int)num_sockets;
        loading_predictor->PrepareForPageLoad(
            GURL(url), ohos_predictors::HintOrigin::OMNIBOX, preconnectable);
      }
    }
  } else {
    WVLOG_I("nweb hadn't initiated try to prepare for page load later");
  }
#endif  // defined(OHOS_NO_STATE_PREFETCH)
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

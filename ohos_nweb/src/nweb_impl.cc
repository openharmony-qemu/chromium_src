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
#include <memory>
#include <string>
#include <thread>

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/memory_pressure_listener.h"
#include "base/report_loss_frame.h"
#include "base/time/time.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"
#include "camera_manager_adapter.h"
#include "cef/include/cef_app.h"
#include "cef/libcef/browser/devtools/devtools_manager_delegate.h"
#include "cef/libcef/browser/net_service/net_helpers.h"
#include "nweb_delegate_adapter.h"
#include "nweb_export.h"
#include "nweb_handler.h"
#include "nweb_hilog.h"
#include "ohos_adapter_helper.h"
#include "cef_delegate/nweb_download_handler_delegate.h"

#include "services/device/wake_lock/power_save_blocker/nweb_screen_lock_tracker.h"

#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif
#if BUILDFLAG(IS_OHOS)
#include "cef/libcef/browser/browser_context.h"
#include "cef_delegate/nweb_application.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/network_service_instance.h"
#include "content/public/browser/storage_partition.h"
#include "res_sched_client_adapter.h"
#include "services/network/network_service.h"
#include "soc_perf_client_adapter.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "cef/libcef/browser/alloy/alloy_browser_context.h"
#endif

#include "libcef/browser/predictors/loading_predictor.h"
#include "libcef/browser/predictors/loading_predictor_config.h"
#include "libcef/browser/predictors/loading_predictor_factory.h"

#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "content/public/common/content_paths.h"

#ifdef OHOS_NWEB_EX
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_ua_config.h"
#endif

namespace {
uint32_t g_nweb_count = 0;
const uint32_t kSurfaceMaxWidth = 7680;
const uint32_t kSurfaceMaxHeight = 7680;
const int32_t kMaxResumeInterval = 60;
const float richtextDisplayRatio = 1.0;

#ifdef OHOS_NWEB_EX
bool g_browser_service_api_enabled = false;
#endif  // OHOS_NWEB_EX

#if defined(REPORT_SYS_EVENT)
// For maximum count of nweb instance
uint32_t g_nweb_max_count = 0;
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

}  // namespace

namespace OHOS::NWeb {

#if BUILDFLAG(IS_OHOS)
static constexpr int32_t SOC_PERF_LOADURL_CONFIG_ID = 10070;
static constexpr int32_t SOC_PERF_MOUSEWHEEL_CONFIG_ID = 10071;
#endif

typedef std::unordered_map<int32_t, std::weak_ptr<NWebImpl>> NWebMap;
base::LazyInstance<NWebMap>::DestructorAtExit g_nweb_map =
    LAZY_INSTANCE_INITIALIZER;

#ifdef OHOS_NWEB_EX
base::LazyInstance<std::vector<std::string>>::DestructorAtExit g_browser_args =
    LAZY_INSTANCE_INITIALIZER;
static double default_zoom_factor = 1.0;
#endif  // OHOS_NWEB_EX

void InitialWebEngineArgs(std::list<std::string>& web_engine_args,
                          const OHOS::NWeb::NWebInitArgs& init_args) {
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

  if (GetLockdownModeStatus()) {
    web_engine_args.emplace_back("--js-flags=--jitless");
  }

  web_engine_args.emplace_back("--enable-media-stream");
  if (init_args.is_enhance_surface) {
    WVLOG_I("is_enhance_surface is true");
    web_engine_args.emplace_back("--ohos-enhance-surface");
  }
  for (auto arg : init_args.web_engine_args_to_delete) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(), arg);
    if (it != web_engine_args.end()) {
      web_engine_args.erase(it);
    }
  }
  for (auto arg : init_args.web_engine_args_to_add) {
    web_engine_args.emplace_back(arg);
  }
  if (init_args.multi_renderer_process) {
    web_engine_args.emplace_back("--enable-multi-renderer-process");
  }
#ifdef OHOS_NWEB_EX
  auto args = g_browser_args.Get();
  for (const std::string& arg : args) {
    web_engine_args.emplace_back(arg);
  }
#endif  // OHOS_NWEB_EX
}

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

NWebImpl::NWebImpl(uint32_t id) : nweb_id_(id) {
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                        nweb_id_);
}

NWebImpl::~NWebImpl() {
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT,
                                        nweb_id_);
  ReportLossFrame::GetInstance()->Reset();
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::STOP);
  g_nweb_map.Get().erase(nweb_id_);
}

bool NWebImpl::Init(const NWebCreateInfo& create_info) {
  output_handler_ = NWebOutputHandler::Create(
      create_info.width, create_info.height, create_info.output_render_frame);
  if (output_handler_ == nullptr) {
    return false;
  }
  output_handler_->SetNWebId(nweb_id_);

  ProcessInitArgs(create_info.init_args);

  if (!InitWebEngine(create_info)) {
    WVLOG_E("web engine init fail");
    return false;
  }

  input_handler_ = NWebInputHandler::Create(nweb_delegate_);
  if (input_handler_ == nullptr) {
    return false;
  }

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .StartListen();

  return true;
}

void NWebImpl::OnDestroy() {
  WVLOG_I("NWebImpl::OnDestroy, nweb_id = %{public}u", nweb_id_);
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

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetNetProxyInstance()
      .StopListen();

#if defined(REPORT_SYS_EVENT)
  // Report nweb instance count
  ReportMultiInstanceStats(nweb_id_, g_nweb_count, g_nweb_max_count);
#endif
}

void NWebImpl::ProcessInitArgs(const NWebInitArgs& init_args) {
  if (!init_args.dump_path.empty() && output_handler_ != nullptr) {
    output_handler_->SetDumpPath(init_args.dump_path);
  }
  if (init_args.frame_info_dump && output_handler_ != nullptr) {
    output_handler_->SetFrameInfoDump(init_args.frame_info_dump);
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

bool NWebImpl::InitWebEngine(const NWebCreateInfo& create_info) {
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

  if (!SetVirtualDeviceRatio()) {
    WVLOG_E("fail to set virtual device ratio");
    delete[] argv;
    return false;
  }

  is_enhance_surface_ = create_info.init_args.is_enhance_surface;
  void* window = nullptr;
  if (is_enhance_surface_) {
    window = create_info.enhance_surface_info;
  } else {
    window = output_handler_->GetNativeWindowFromSurface(
        create_info.producer_surface);
  }

  if (window == nullptr) {
    WVLOG_E("fail to init web engine, get native window from surface failed");
    delete[] argv;
    return false;
  }

  int32_t ret =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWindowAdapterInstance()
          .NativeWindowHandleOpt(reinterpret_cast<void*>(window),
                                 OHOS::NWeb::WindowAdapter::SET_BUFFER_GEOMETRY,
                                 create_info.width, create_info.height);

  if (ret == OHOS::NWeb::GSErrorCode::GSERROR_OK) {
      WVLOG_I("native window opt for emulator in init, result = %{public}d", ret);
  } else {
      WVLOG_W("native window opt for emulator in init failed, result = %{public}d", ret);
  }

  WVLOG_D("nweb create_info.init_args.is_popup: %{public}d",
          create_info.init_args.is_popup);
  nweb_delegate_ = NWebDelegateAdapter::CreateNWebDelegate(
      argc, argv, is_enhance_surface_, window, create_info.init_args.is_popup,
      nweb_id_);
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to create nweb delegate of web engine");
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

void NWebImpl::InitWebEngineArgs(const NWebInitArgs& init_args) {
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

void NWebImpl::PutAccessibilityEventCallback(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibilityEventListener) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterAccessibilityEventListener(
        accessibilityEventListener);
  }
}

void NWebImpl::PutAccessibilityIdGenerator(
    std::function<int32_t()> accessibilityIdGenerator) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterAccessibilityIdGenerator(accessibilityIdGenerator);
  }
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

const std::shared_ptr<NWebHandler> NWebImpl::GetNWebHandler() const {
  return nweb_handle_;
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

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::CLICK, nweb_id_);
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

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::SLIDE);

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);

  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

void NWebImpl::SendMouseEvent(int x, int y, int button, int action, int count) {
  if (input_handler_ == nullptr) {
    return;
  }

  if (action == MouseAction::PRESS) {
    ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                       ResSchedSceneAdapter::CLICK, nweb_id_);
  }
  input_handler_->SendMouseEvent(x, y, button, action, count);
}

int NWebImpl::Load(const std::string& url) const {
  if (nweb_delegate_ == nullptr || output_handler_ == nullptr) {
    return NWEB_ERR;
  }

  if (!output_handler_->IsSizeValid()) {
    WVLOG_E("nweb size is invalid, stop Load");
    return NWEB_ERR;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::LOAD_URL, nweb_id_);

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_LOADURL_CONFIG_ID);

  int result = nweb_delegate_->Load(url);
  output_handler_->StartRenderOutput();
  return result;
}

bool NWebImpl::IsNavigatebackwardAllowed() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigatebackwardAllowed();
}

bool NWebImpl::IsNavigateForwardAllowed() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigateForwardAllowed();
}

bool NWebImpl::CanNavigateBackOrForward(int numSteps) const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->CanNavigateBackOrForward(numSteps);
}

void NWebImpl::NavigateBack() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateBack();
}

void NWebImpl::NavigateForward() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateForward();
}

void NWebImpl::NavigateBackOrForward(int step) const {
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

void NWebImpl::Reload() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Reload();
}

int NWebImpl::Zoom(float zoomFactor) const {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Zoom(zoomFactor);
}

int NWebImpl::ZoomIn() const {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomIn();
}

int NWebImpl::ZoomOut() const {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomOut();
}

void NWebImpl::Stop() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Stop();
}

void NWebImpl::ExecuteJavaScript(const std::string& code) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code);
}

void NWebImpl::PutBackgroundColor(int color) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PutBackgroundColor(color);
}

void NWebImpl::InitialScale(float scale) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->InitialScale(scale);
}

void NWebImpl::OnPause() const {
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

void NWebImpl::OnContinue() const {
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

void NWebImpl::OnOccluded() const {
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

void NWebImpl::OnUnoccluded() const {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnUnoccluded();
}

void NWebImpl::SetEnableLowerFrameRate(bool enabled) const {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->SetEnableLowerFrameRate(enabled);
}

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

void NWebImpl::OnWebviewHide() const {
  StopCameraSession();
}

void NWebImpl::OnWebviewShow() const {
  RestartCameraSession();
}

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

void NWebImpl::SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) {}

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
const std::shared_ptr<NWebPreference> NWebImpl::GetPreference() const {
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

void NWebImpl::CreateWebMessagePorts(std::vector<std::string>& ports) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->CreateWebMessagePorts(ports);
}

void NWebImpl::PostWebMessage(std::string& message,
                              std::vector<std::string>& ports,
                              std::string& targetUri) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostWebMessage(message, ports, targetUri);
}

void NWebImpl::ClosePort(std::string& portHandle) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->ClosePort(portHandle);
}

void NWebImpl::PostPortMessage(std::string& portHandle,
                               std::shared_ptr<NWebMessage> data) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostPortMessage(portHandle, data);
}

void NWebImpl::SetPortMessageCallback(
    std::string& portHandle,
    std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->SetPortMessageCallback(portHandle, callback);
}

uint32_t NWebImpl::GetWebId() const {
  return nweb_id_;
}

HitTestResult NWebImpl::GetHitTestResult() const {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("get hit test result failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return HitTestResult();
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

int NWebImpl::Load(std::string& url,
                   std::map<std::string, std::string> additionalHttpHeaders) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Load(url, additionalHttpHeaders);
}

int NWebImpl::PostUrl(const std::string& url,
                      std::vector<char>& postData) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->PostUrl(url, postData);
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

void NWebImpl::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(object_name, method_list, -1);
}

void NWebImpl::RegisterArkJSfunctionExt(
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

void NWebImpl::SetNWebJavaScriptResultCallBack(
    std::shared_ptr<NWebJavaScriptResultCallBack> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RegisterNWebJavaScriptCallBack(callback);
}

void NWebImpl::ExecuteJavaScript(
    const std::string& code,
    std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback,
    bool extention) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code, callback, extention);
}

void NWebImpl::OnFocus(const FocusReason& focusReason) const {
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

void NWebImpl::OnBlur(const BlurReason& blurReason) const {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnBlur();
  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (is_enhance_surface_ &&
      blurReason == OHOS::NWeb::BlurReason::WINDOW_BLUR) {
    return;
  }
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

void NWebImpl::FindAllAsync(const std::string& search_string) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindAllAsync(search_string);
}

void NWebImpl::ClearMatches() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearMatches();
}

void NWebImpl::FindNext(const bool forward) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindNext(forward);
}

void NWebImpl::StoreWebArchive(
    const std::string& base_name,
    bool auto_name,
    std::shared_ptr<NWebValueCallback<std::string>> callback) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->StoreWebArchive(base_name, auto_name, callback);
}

void NWebImpl::SendDragEvent(const DragEvent& dragEvent) const {
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

std::shared_ptr<NWebDragData> NWebImpl::GetOrCreateDragData() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return nullptr;
  }

  return nweb_delegate_->GetOrCreateDragData();
}

std::string NWebImpl::GetUrl() const {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->GetUrl();
}

void NWebImpl::UpdateLocale(const std::string& language,
                            const std::string& region) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateLocale(language, region);
}

const std::string NWebImpl::GetOriginalUrl() const {
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

void NWebImpl::HasImages(std::shared_ptr<NWebValueCallback<bool>> callback) {
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

WebState NWebImpl::SerializeWebState() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->SerializeWebState();
}

bool NWebImpl::RestoreWebState(WebState state) {
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
  WVLOG_D("SetAudioMuted invoked: %{public}s", (muted ? "true" : "false"));
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioMuted(muted);
  }
}

void NWebImpl::SetAudioResumeInterval(int32_t resumeInterval) {
  int32_t interval =
      resumeInterval > kMaxResumeInterval ? kMaxResumeInterval : resumeInterval;
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioResumeInterval(interval);
  }
}

void NWebImpl::SetAudioExclusive(bool audioExclusive) {
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioExclusive(audioExclusive);
  }
}

#if defined(OHOS_NWEB_EX)
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

void NWebImpl::PasswordSuggestionSelected(int list_index) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->PasswordSuggestionSelected(list_index);
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

void NWebImpl::SetForceEnableZoom(bool forceEnableZoom) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetForceEnableZoom(forceEnableZoom);
}

void NWebImpl::SaveOrUpdatePassword(bool is_update) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SaveOrUpdatePassword(is_update);
}

bool NWebImpl::GetForceEnableZoom() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetForceEnableZoom();
}

void NWebImpl::SetEnableBlankTargetPopupIntercept(
    bool enableBlankTargetPopup) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetEnableBlankTargetPopupIntercept(enableBlankTargetPopup);
}

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

// static
void NWebImpl::SetDefaultBrowserZoomLevel(double zoom_factor) {
  if (g_nweb_count == 0) { 
    WVLOG_I("nweb had not initiated try to set default browser zoom level.");
    return;
  }
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    content::BrowserContext* browser_context =
        cef_browser_context->AsBrowserContext();
    if (!browser_context) {
      LOG(ERROR) << "SetDefaultBrowserZoomLevel null browser_context";
      return;
    }
    static_cast<AlloyBrowserContext*>(browser_context)
	->GetZoomLevelPrefs()
        ->SetDefaultZoomLevelPref(
	    blink::PageZoomFactorToZoomLevel(zoom_factor));
    default_zoom_factor = zoom_factor;
  }
}

// static
void NWebImpl::SetConnectTimeout(int32_t seconds) {
  content::GetNetworkService()->SetConnectTimeout(seconds);
}

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
#endif  // OHOS_NWEB_EX

void NWebImpl::PrefetchPage(
    std::string& url,
    std::map<std::string, std::string> additionalHttpHeaders) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PrefetchPage(url, additionalHttpHeaders);
}

void* NWebImpl::CreateWebPrintDocumentAdapter(const std::string& jobName) {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->CreateWebPrintDocumentAdapter(jobName);
}

void NWebImpl::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (nweb_delegate_) {
    nweb_delegate_->SetShouldFrameSubmissionBeforeDraw(should);
  }
}

void NWebImpl::RegisterScreenLockFunction(int32_t windowId,
                                          const SetKeepScreenOn&& handle) {
  NWebScreenLockTracker::Instance().AddScreenLock(windowId, nweb_id_, handle);
}

void NWebImpl::UnRegisterScreenLockFunction(int32_t windowId) {
  NWebScreenLockTracker::Instance().RemoveScreenLock(windowId, nweb_id_);
}

void NWebImpl::NotifyMemoryLevel(int32_t level) {
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
}

void NWebImpl::ExecuteAction(int32_t accessibilityId, uint32_t action) const {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->ExecuteAction(accessibilityId, action);
  }
}

bool NWebImpl::GetFocusedAccessibilityNodeInfo(
    int32_t accessibilityId,
    bool isAccessibilityFocus,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetFocusedAccessibilityNodeInfo(
        accessibilityId, isAccessibilityFocus, nodeInfo);
  }
  return false;
}

bool NWebImpl::GetAccessibilityNodeInfoById(
    int32_t accessibilityId,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoById(accessibilityId,
                                                        nodeInfo);
  }
  return false;
}

bool NWebImpl::GetAccessibilityNodeInfoByFocusMove(
    int32_t accessibilityId,
    int32_t direction,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoByFocusMove(
        accessibilityId, direction, nodeInfo);
  }
  return false;
}

void NWebImpl::SetAccessibilityState(bool state) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SetAccessibilityState(state ? STATE_ENABLED
                                                : STATE_DISABLED);
  }
}

bool NWebImpl::NeedSoftKeyboard() const {
  if (inputmethod_handler_) {
    return inputmethod_handler_->GetIsEditableNode();
  }
  return false;
}
}  // namespace OHOS::NWeb

using namespace OHOS::NWeb;
extern "C" OHOS_NWEB_EXPORT void CreateNWeb(const NWebCreateInfo& create_info,
                                            std::shared_ptr<NWebImpl>& nweb) {
  static uint32_t current_nweb_id = 0;
  uint32_t nweb_id = ++current_nweb_id;
  TRACE_EVENT1("NWebImpl", "NWebImpl | CreateNWeb", "nweb_id", nweb_id);
  WVLOG_I("creating nweb %{public}u, size %{public}u*%{public}u", nweb_id,
          create_info.width, create_info.height);
  WVLOG_I("creating nweb use enhance surface %{public}d",
          create_info.init_args.is_enhance_surface);
  nweb = std::make_shared<NWebImpl>(nweb_id);
  if (nweb == nullptr) {
    return;
  }

  if (!nweb->Init(create_info)) {
    WVLOG_E("fail to init nweb");
    return;
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
}

extern "C" OHOS_NWEB_EXPORT void GetNWeb(int32_t nweb_id,
                                         std::weak_ptr<NWebImpl>& nweb) {
  NWebMap* map = OHOS::NWeb::g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    nweb = it->second;
  }
}

bool NWebImpl::InitializeICUStatic(const NWebInitArgs& init_args) {
  if (NWebApplication::GetDefault()->HasInitializedCef()) {
    return true;
  }
  WVLOG_I("will initialize icu.");
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
  return true;
}

extern "C" OHOS_NWEB_EXPORT void InitializeWebEngine(
    const NWebInitArgs& init_args) {
  WVLOG_I("will initialize web engine and cef.");
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

  auto& system_properties_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance();
  OHOS::NWeb::ProductDeviceType deviceType =
      system_properties_adapter.GetProductDeviceType();
  bool is_pc_device =
      deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_TABLET ||
      deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_2IN1;
  settings.persist_session_cookies = !is_pc_device;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif
  NWebApplication::GetDefault()->InitializeCef(mainargs, settings);
  content::GetNetworkService();
}

extern "C" OHOS_NWEB_EXPORT void SetHttpDns(const NWebDOHConfig& config) {
  WVLOG_I("set http dns config mode:%{public}d config: %{public}s",
          config.doh_mode, config.doh_config.c_str());
  net_service::NetHelpers::doh_mode = config.doh_mode;
  net_service::NetHelpers::doh_config = config.doh_config;

  CefApplyHttpDns();
}

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

// static
void NWebImpl::ResumeDownloadStatic(
    std::shared_ptr<NWebDownloadItem> web_download) {
  CefResumeDownload(web_download->guid, web_download->url,
                    web_download->full_path, web_download->received_bytes,
                    web_download->total_bytes, web_download->etag,
                    web_download->mime_type, web_download->last_modified,
                    web_download->received_slices);
}

extern "C" OHOS_NWEB_EXPORT void PrepareForPageLoad(std::string url,
                                                    bool preconnectable,
                                                    int32_t num_sockets) {
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
}

extern "C" OHOS_NWEB_EXPORT void SetConnectionTimeout(const int& timeout) {
  net_service::NetHelpers::connection_timeout = timeout;
  if (content::GetNetworkService() != nullptr) {
      content::GetNetworkService()->SetConnectTimeout(net_service::NetHelpers::connection_timeout);
      WVLOG_I("set connection timeout value in NetHelpers is: %{public}d", net_service::NetHelpers::connection_timeout);
  } else {
      WVLOG_E("net_work_service is nullptr");
  }
}

extern "C" OHOS_NWEB_EXPORT void SetWebDebuggingAccess(bool isEnableDebug) {
  static bool isDebuggingEnabled = false;
  if (isEnableDebug && !isDebuggingEnabled) {
      CefDevToolsManagerDelegate::StartHttpHandler(nullptr);
      WVLOG_I("StartHttpHandler Enabled");
      isDebuggingEnabled = true;
  } else if (!isEnableDebug && isDebuggingEnabled) {
      CefDevToolsManagerDelegate::StopHttpHandler();
      WVLOG_I("StopHttpHandler Enabled");
      isDebuggingEnabled = false;
  }
}

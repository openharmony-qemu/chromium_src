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

#include "arkweb/chromium_ext/components/input/fling_controller_utils.h"
#include "components/input/fling_controller.h"

#include "arkweb/build/features/features.h"
#include "base/time/default_tick_clock.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/gestures/blink/web_gesture_curve_impl.h"
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
#include "arkweb/chromium_ext/base/report_loss_frame_ext.h"
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "content/browser/gpu/gpu_process_host.h"
#endif
#if BUILDFLAG(ARKWEB_D_VSYNC)
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace {
const int SOC_PERF_WEB_SLIDE_SCROLL = 10097;
}  // namespace

namespace input {

FlingControllerlUtils::FlingControllerlUtils(FlingController* impl) {
    this->flingController = impl;
}

#if BUILDFLAG(ARKWEB_FLING)
FlingController::~FlingController() {
  if (!fling_curve_) {
    return;
  }
  LOG(DEBUG) << "stop web page fling";
  auto frame_rate = base::ohos::SlidingObserver::GetInstance().StopFling();
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      host_impl->StopMonitor();
      if (frame_rate >= 0) {
        host_impl->ReportSlidingFrameRate(frame_rate);
      }
    }
  }
}
#else
FlingController::~FlingController() = default;
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
void FlingControllerlUtils::SetIsFling(const bool flag) {
  if (auto* host = content::GpuProcessHost::Get()) {
      if (auto* host_impl = host->gpu_host()) {
        host_impl->SetIsFling(flag);
      }
  }
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void FlingControllerlUtils::FlingUpdate(const gfx::Vector2dF &velocity) {
  int32_t preferredFrameRate =
      base::ohos::SlidingObserver::GetInstance().OnFlingUpdate(
          velocity.x(), velocity.y());
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      if (preferredFrameRate >= 0) {
        host_impl->ReportSlidingFrameRate(preferredFrameRate);
      }
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void FlingControllerlUtils::DealGestureFling(std::string &fling_string) {
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::START);
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .StartAsyncTrace(fling_string, 0);
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void FlingControllerlUtils::DealEndCurrentFling(std::string &fling_string) {
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::STOP);
  ReportLossFrame::GetInstance()->Report();
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .FinishAsyncTrace(fling_string, 0);
  LOG(DEBUG) << "stop web page fling";
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
void FlingControllerlUtils::ApplySocPerfConfig()
{
  int socPerfId = OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID;
#if BUILDFLAG(ARKWEB_D_VSYNC)
  if (base::ohos::IsPcDevice()) {
    socPerfId = SOC_PERF_WEB_SLIDE_SCROLL;
  }
#endif
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(socPerfId, true);
}
#endif
}
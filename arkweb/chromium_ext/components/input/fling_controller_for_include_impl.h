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

#ifndef COMPONENTS_INPUT_FLING_CONTROLLER_FOR_INCLUDE_IMPL_H_
#define COMPONENTS_INPUT_FLING_CONTROLLER_FOR_INCLUDE_IMPL_H_

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
#if BUILDFLAG(ARKWEB_D_VSYNC)
std::atomic<int> FlingController::instance_count_ = 0;
#endif

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

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void FlingController::DynamicFrameLossEvent(const std::string& sceneId, bool isStart) {
  event_sender_client_->DynamicFrameLossEvent(sceneId, isStart);
}
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
void FlingController::SetIsFlingFalse(const bool flag) {
  LOG(DEBUG) << "FlingController::ObserveAndMaybeConsumeGestureEvent::instance_count_: " << instance_count_;
  if (flag) {
    LOG(DEBUG) << "FlingController::ObserveAndMaybeConsumeGestureEvent::dvsyncSwitch=false";
    if (auto* host = content::GpuProcessHost::Get()) {
        if (auto* host_impl = host->gpu_host()) {
          host_impl->SetIsFling(false);
        }
    }
    TRACE_EVENT0("input", "ObserveAndMaybeConsumeGestureEvent::SetNeedDVsync=false, reason=kGestureFlingCancel");
  }
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void FlingController::StartWebPageFling() {
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  std::string fling_string = "WEB_LIST_FLING";
#endif
#if BUILDFLAG(ARKWEB_D_VSYNC)
  instance_count_++;
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::START);
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .StartAsyncTrace(fling_string, 0);
#endif
// https://open.codehub.huawei.com/OpenSourceCenter_CR/openharmony-tpc/oh-chromium/-/change_requests/1309
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  int socPerfId = OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID;
#if BUILDFLAG(ARKWEB_D_VSYNC)
  if (base::ohos::IsPcDevice()) {
    socPerfId = SOC_PERF_WEB_SLIDE_SCROLL;
  }
#endif
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(socPerfId, true);
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  LOG(DEBUG) << "start web page fling";
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      host_impl->StartMonitor();
      TRACE_EVENT0("input", "DynamicFrameLossEvent Start");
      content::GetUIThreadTaskRunner({})->PostTask(
          FROM_HERE, base::BindOnce(&FlingController::DynamicFrameLossEvent,
              weak_ptr_factory_.GetWeakPtr(), fling_string, true));
    }
  }
#endif
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
  if (base::ohos::IsMobileDevice() && instance_count_ == 1) {
      LOG(DEBUG) << "FlingController::ProgressFling::dvsyncSwitch=true";
      if (auto* host = content::GpuProcessHost::Get()) {
          if (auto* host_impl = host->gpu_host()) {
          host_impl->SetIsFling(true);
          }
      }
  }
  TRACE_EVENT0("input", "ObserveAndMaybeConsumeGestureEvent::SetNeedDVsync=true, reason=GestureFlingStart");
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  base::ohos::SlidingObserver::GetInstance().StartFling();
#endif
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void FlingController::FlingUpdate(base::TimeTicks current_time) {
  if ((current_time - current_fling_parameters_.start_time).InSecondsF() <= 0) {
    return;
  }
  int32_t preferredFrameRate =
      base::ohos::SlidingObserver::GetInstance().OnFlingUpdate(
          current_fling_parameters_.velocity.x(),
          current_fling_parameters_.velocity.y());
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      if (preferredFrameRate >= 0) {
        host_impl->ReportSlidingFrameRate(preferredFrameRate);
      }
    }
  }
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void FlingController::StopWebPageFling() {
#if BUILDFLAG(ARKWEB_D_VSYNC)
  instance_count_--;
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  std::string fling_string = "WEB_LIST_FLING";
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::STOP);
  ReportLossFrame::GetInstance()->Report();
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .FinishAsyncTrace(fling_string, 0);
  LOG(DEBUG) << "stop web page fling";
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  LOG(DEBUG) << "stop web page fling";
  auto frame_rate = base::ohos::SlidingObserver::GetInstance().StopFling();
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      host_impl->StopMonitor();
      if (frame_rate >= 0) {
        host_impl->ReportSlidingFrameRate(frame_rate);
      }
      TRACE_EVENT0("input", "DynamicFrameLossEvent End");
      content::GetUIThreadTaskRunner({})->PostTask(
          FROM_HERE,
          base::BindOnce(&FlingController::DynamicFrameLossEvent,
                         weak_ptr_factory_.GetWeakPtr(), fling_string, false));
    }
  }
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
  LOG(INFO) << "FlingController::EndCurrentFling::dvsyncSwitch=false";
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
        host_impl->SetIsFling(false);
    }
  }
  TRACE_EVENT0("input", "EndCurrentFling::SetNeedDVsync=false, reason=EndCurrentFling");
#endif
}
#endif
}
#endif
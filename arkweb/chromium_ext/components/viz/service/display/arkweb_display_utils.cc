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

#include "arkweb/chromium_ext/components/viz/service/display/arkweb_display_utils.h"
#include "base/time/time.h"
#include "base/timer/elapsed_timer.h"
#include "base/trace_event/trace_event.h"
#include "components/viz/service/display/direct_renderer.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display/display_client.h"
#include "ui/gfx/geometry/rect.h"

#if BUILDFLAG(ARKWEB_DFX_DUMP)
#include "arkweb/chromium_ext/components/viz/service/display/frame_dump_copy_output_request.h"
#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_VULKAN)
#include "gpu/config/gpu_finch_features.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/chromium_ext/components/viz/common/frame_sinks/arkweb_copy_output_request_utils.h"
#include "arkweb/chromium_ext/components/viz/service/display/frame_snapshot_copy_output_request.h"
#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_ext.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#endif

namespace viz {

#if BUILDFLAG(ARKWEB_DFX_DUMP)
const int DUMP_FRAME_FREQ = 60;

//LCOV_EXCL_START
class DumpFrameObserver : public OHOS::NWeb::SystemPropertiesObserver {
 public:
  DumpFrameObserver() = default;
  ~DumpFrameObserver() override = default;

  void PropertiesUpdate(const char* value) override {
    std::lock_guard<std::mutex> lock(mutex_);
    dump_param_list_.clear();
    if (strcmp(value, "true") == 0) {
      should_dump_ = true;
    } else if (strcmp(value, "false") == 0) {
      should_dump_ = false;
    } else {
      std::string str_value = std::string(value);
      std::stringstream origin_str(str_value);
      std::string item_str;
      while (std::getline(origin_str, item_str, '#')) {
        dump_param_list_.emplace_back(item_str);
      }
    }
  }

  bool ShouldDump() {
    std::lock_guard<std::mutex> lock(mutex_);
    return should_dump_ ||
           (dump_param_list_.size() > 0 && dump_param_list_[0] == "true");
  }

  bool ShouldDumpInFreq() {
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t dumpFreq = DUMP_FRAME_FREQ;
    if (dump_param_list_.size() > 1) {
      dumpFreq = std::stoi(dump_param_list_[1]);
    }

    if (dump_freq_count == dumpFreq) {
      dump_freq_count = 0;
      return true;
    } else {
      dump_freq_count++;
      return false;
    }
  }

  std::string DumpPath() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (dump_param_list_.size() > 2) {
      return dump_param_list_[2];
    }

    return "";
  }

 private:
  bool should_dump_ = false;
  int dump_freq_count = 0;
  std::vector<std::string> dump_param_list_;
  std::mutex mutex_;
};
#endif
//LCOV_EXCL_STOP

namespace {
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
const int MAX_SURFACE_SIZE = 8000;
const int MIN_FITCONTENT_SURFACE_SIZE = 6000;
#endif
#if BUILDFLAG(ARKWEB_DFX_DUMP)
static uint64_t g_dump_frame_id = 0;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
constexpr base::TimeDelta reset_state_delay = base::Milliseconds(800);
constexpr base::TimeDelta reenable_draw_delay = base::Milliseconds(3000);
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
const int kRectNumthreshold = 5;
#endif
}  // namespace

//LCOV_EXCL_START
ArkwebDisplayUtils::ArkwebDisplayUtils(Display* display) : display_(display) {
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  dump_frame_observer_ = std::make_unique<DumpFrameObserver>();
  // There is another switches in this file, only can distinguish for renderer
  // by uid, uid for browser is bigger than MAIN_PROCESS_ID_MIN, and gpu will
  // not come here.
  uid_t uid = getuid();
  auto app_mgr_client_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateAafwkAdapter();
  if (app_mgr_client_adapter == nullptr) {
    return;
  }
  if (app_mgr_client_adapter->IsRenderProcessByUid(static_cast<int>(uid))) {
    blink::SysPropRenderObserverClientRep sysproprender_;
    sysproprender_.AttachSysPropObserver(1, dump_frame_observer_.get());
  } else {
    auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetSystemPropertiesInstance();
    system_properties_adapter.AttachSysPropObserver(
        OHOS::NWeb::PropertiesKey::PROP_RENDER_DUMP,
        dump_frame_observer_.get());
  }
#endif
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  reset_init_timer_ = std::make_unique<base::RetainingOneShotTimer>(
      FROM_HERE, reset_state_delay,
      base::BindRepeating(&ArkwebDisplayUtils::RestoreRenderFitTimeElapsed,
                          weak_factory_.GetWeakPtr()));
  reenable_swap_timer_ = std::make_unique<base::RetainingOneShotTimer>(
      FROM_HERE, reenable_draw_delay,
      base::BindRepeating(&ArkwebDisplayUtils::RestoreRenderFitTimeElapsed,
                          weak_factory_.GetWeakPtr()));
#endif  // ARKWEB_MAXIMIZE_RESIZE
}

ArkwebDisplayUtils::~ArkwebDisplayUtils() {
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  uid_t uid = getuid();
  auto app_mgr_client_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateAafwkAdapter();
  if (app_mgr_client_adapter == nullptr) {
    return;
  }
  if (app_mgr_client_adapter->IsRenderProcessByUid(static_cast<int>(uid))) {
    blink::SysPropRenderObserverClientRep sysproprender_;
    sysproprender_.DetachSysPropObserver(1, dump_frame_observer_.get());
  } else {
    auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetSystemPropertiesInstance();
    system_properties_adapter.DetachSysPropObserver(
        OHOS::NWeb::PropertiesKey::PROP_RENDER_DUMP,
        dump_frame_observer_.get());
  }
#endif
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void ArkwebDisplayUtils::SetShouldFrameSubmissionBeforeDraw(bool should) {
  display_->scheduler_->SetShouldFrameSubmissionBeforeDraw(should);
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void ArkwebDisplayUtils::SetNativeInnerWeb(bool isInnerWeb) {
    display_->renderer_->SetNativeInnerWeb(isInnerWeb);
}
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void ArkwebDisplayUtils::SetBypassVsyncCondition(int32_t condition) {
    display_->renderer_->SetBypassVsyncCondition(condition);
}
#endif
//LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void ArkwebDisplayUtils::SetDrawRect(const gfx::Rect& new_rect) {
  if (draw_mode_ == 0) {
    LOG(ERROR) << "draw_mode =" << draw_mode_;
    return;
  }
  TRACE_EVENT1("viz", "Display::SetDrawRect", "new_rect", new_rect.ToString());
  draw_rect_ = new_rect;
  display_->current_surface_size_ = draw_rect_.size();
  display_->scheduler_->SetNeedsOneBeginFrame(true);
  LOG(INFO) << "SetDrawRect new_rect=" << new_rect.ToString();
}

//LCOV_EXCL_START
void ArkwebDisplayUtils::SetDrawMode(const int32_t mode) {
  LOG(INFO) << "SetDrawMode mode=" << mode;
  draw_mode_ = mode;
}
//LCOV_EXCL_STOP
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
//LCOV_EXCL_START
void ArkwebDisplayUtils::DisableSwapUntilMaximized() {
  temp_idle_state_ = TempIdleState::INIT;
  if (reset_init_timer_) {
    reset_init_timer_->Reset();
  }
}

void ArkwebDisplayUtils::RestoreRenderFitTimeElapsed() {
  LOG(INFO) << "Display::RestoreRenderFitTimeElapsed, frame_sink_id_: "
            << display_->frame_sink_id_.ToString();
  temp_idle_state_ = TempIdleState::RESTORE_RENDERFIT;
  if (display_->client_) {
    display_->client_->RestoreRenderFit(display_->frame_sink_id_);
  }
}
//LCOV_EXCL_STOP

void ArkwebDisplayUtils::ReenableSwapCheck(const SurfaceId& surface_id,
                                           int width,
                                           int height) {
  if (temp_idle_state_ != TempIdleState::DISABLE_SWAP) {
    return;
  }
  if (surface_id.is_valid() && surface_id != display_->CurrentSurfaceId()) {
    return;
  }
  if (width >= display_->current_surface_size_.width() &&
      height >= display_->current_surface_size_.height()) {
    LOG(INFO) << "Display::ReenableSwap, frame_sink_id_: "
              << display_->frame_sink_id_.ToString();
    if (reenable_swap_timer_ && reenable_swap_timer_->IsRunning()) {
      reenable_swap_timer_->Stop();
    }
    temp_idle_state_ = TempIdleState::REENABLE_SWAP;
  }
}

//LCOV_EXCL_START
void ArkwebDisplayUtils::RestoreRenderFit() {
  if (temp_idle_state_ == TempIdleState::REENABLE_SWAP && display_->client_) {
    LOG(INFO) << "Display RestoreRenderFit, frame_sink_id_: "
              << display_->frame_sink_id_.ToString();
    temp_idle_state_ = TempIdleState::RESTORE_RENDERFIT;
    display_->client_->RestoreRenderFit(display_->frame_sink_id_);
  }
}

bool ArkwebDisplayUtils::ShouldDisableSwap(bool should) {
  return should && (temp_idle_state_ != TempIdleState::DISABLE_SWAP);
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DFX_DUMP)
void ArkwebDisplayUtils::DrawAndSwapDump(AggregatedFrame& frame) {
  if (dump_frame_observer_ && dump_frame_observer_->ShouldDump()) {
    if (dump_frame_observer_->ShouldDumpInFreq()) {
      auto request = std::make_unique<FrameDumpCopyOutputRequest>(
          ++g_dump_frame_id, dump_frame_observer_->DumpPath());
      auto& root_render_pass = frame.render_pass_list.back();
      if (root_render_pass) {
        root_render_pass->copy_requests.push_back(std::move(request));
      }
    }
  }
}
#endif
//LCOV_EXCL_STOP

void ArkwebDisplayUtils::Resize(const gfx::Size& size) {
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  gfx::Size newSize = size;
  if (newSize.width() > MAX_SURFACE_SIZE) {
    newSize.set_width(MAX_SURFACE_SIZE);
  }
  if (newSize.height() > MAX_SURFACE_SIZE) {
    newSize.set_height(MAX_SURFACE_SIZE);
  }
  LOG(INFO) << "Display::Resize newSize = " << newSize.ToString();
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  if (temp_idle_state_ == TempIdleState::INIT &&
    (size.width() > display_->current_surface_size_.width() ||
    size.height() > display_->current_surface_size_.height())) {
    LOG(INFO) << "Display::Resize, disable swap, frame_sink_id_: "
              << display_->frame_sink_id_.ToString();
    if (reset_init_timer_ && reset_init_timer_->IsRunning()) {
      reset_init_timer_->Stop();
    }
    temp_idle_state_ = TempIdleState::DISABLE_SWAP;
    if (reenable_swap_timer_) {
      reenable_swap_timer_->Reset();
    }
  }
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  LOG(DEBUG) << "Display::Resize,current_surface_size is :"
             << display_->current_surface_size_.ToString().c_str();
  if (draw_mode_ &&
      display_->current_surface_size_.height() >= MIN_FITCONTENT_SURFACE_SIZE &&
      display_->current_surface_size_.height() <= MAX_SURFACE_SIZE) {
    display_->current_surface_size_.set_width(newSize.width());
  } else {
    display_->current_surface_size_ = newSize;
  }
#endif
}

#if BUILDFLAG(ARKWEB_VULKAN)
void ArkwebDisplayUtils::JudgePartialSwap() {
  if (features::IsUsingVulkan() && display_ && display_->renderer_) {
    if (base::ohos::IsPageScale()) {
      display_->renderer_->disable_partial_swap();
    } else {
      display_->renderer_->enable_partial_swap();
    }
  }
}
#endif

void ArkwebDisplayUtils::DrawAndSwap(AggregatedRenderPass& last_render_pass,
                                     gfx::Size current_surface_size,
                                     AggregatedFrame& frame) {
#if BUILDFLAG(ARKWEB_VULKAN)
  JudgePartialSwap();
#endif
  if (draw_mode_ ||
      (display_->settings_.auto_resize_output_surface &&
       last_render_pass.output_rect.size() != current_surface_size &&
       last_render_pass.damage_rect == last_render_pass.output_rect &&
       !current_surface_size.IsEmpty())) {
    LOG(INFO) << "resize_output_surface = " << current_surface_size.ToString();
    LOG(INFO) << "output_rect = " << last_render_pass.output_rect.ToString();
    // Resize the |output_rect| to the |current_surface_size| so that we won't
    // skip the draw and so that the GL swap won't stretch the output.
#if BUILDFLAG(ARKWEB_VULKAN)
    if (features::IsUsingVulkan()) {
      display_->renderer_->disable_partial_swap();
    }
#endif
    last_render_pass.output_rect.set_size(current_surface_size);
    last_render_pass.output_rect.set_y(draw_rect_.y());
    last_render_pass.output_rect.set_x(draw_rect_.x());
    last_render_pass.damage_rect = last_render_pass.output_rect;
    frame.surface_damage_rect_list_.push_back(last_render_pass.damage_rect);
    LOG(INFO) << "output_rect modify = "
              << last_render_pass.output_rect.ToString();
  }
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
//LCOV_EXCL_START
void ArkwebDisplayUtils::DumpSnapshotForBlankLess(AggregatedFrame& frame) {
  if (!gpu_service_impl_) {
    LOG(DEBUG) << "blankless DumpSnapshotForBlankLess, gpu_service_impl_ is nullptr";
    return;
  }
  uint64_t id = display_->frame_sink_id_.hash();
  base::ohos::BlanklessDumpInfo info;
  if (!gpu::GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(client_id_, id, info) || !info.dump_enabled) {
    LOG(DEBUG) << "blankless dump disable or no blankless info";
    return;
  }

  auto& root_render_pass = frame.render_pass_list.back();
  if (!root_render_pass) {
    LOG(ERROR) << "blankless no root render pass";
    return;
  }

  auto snapshot_request = std::make_unique<FrameSnapshotCopyOutputRequest>(
    base::BindOnce(&GpuServiceImpl::OnFrameSnapshotCopyOutputResult, gpu_service_impl_->GetWeakPtr()));
  if (!snapshot_request || !snapshot_request->copy_output_request_utils()) {
    LOG(ERROR) << "blankless snapshot_request is null";
    return;
  }
  info.info.width = root_render_pass->output_rect.width();
  info.info.height = root_render_pass->output_rect.height();
  snapshot_request->SetUniformScaleRatio(base::ohos::BlanklessController::SNAPSHOT_SCALE_FACTOR, 1);
  snapshot_request->copy_output_request_utils()->SetBlanklessInfo(info.info);
  LOG(DEBUG) << "blankless push copy render pass. nweb_id: " << info.info.nweb_id
              << ", blankless_key: " << info.info.blankless_key;
  root_render_pass->copy_requests.push_back(std::move(snapshot_request));
}

void ArkwebDisplayUtils::SetClientId(const uint32_t client_id) {
  client_id_ = client_id;
}

void ArkwebDisplayUtils::SetGpuServiceImpl(GpuServiceImpl* gpu_service_impl) {
  gpu_service_impl_ = gpu_service_impl;
}
//LCOV_EXCL_STOP
#endif
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void ArkwebDisplayUtils::DiscardBackbuffer() {
#if BUILDFLAG(ARKWEB_VULKAN)
  if (display_->renderer_) {
    display_->renderer_->ReallocatedFrameBuffers();
  }
#endif
  if (display_->output_surface_) {
    display_->output_surface_->DiscardBackbuffer();
  }
}
#endif

#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
void ArkwebDisplayUtils::SetIfNeedCleanBuffers(bool need_clean_buffers)
{
  TRACE_EVENT1("viz", "ArkwebDisplayUtils::SetIfNeedCleanBuffers ", "need_clean_buffers:", need_clean_buffers);
  if (display_ && display_->output_surface_) {
    display_->output_surface_->SetIfNeedCleanBuffers(need_clean_buffers);
    need_clean_buffers_ = need_clean_buffers;
  }
}

void ArkwebDisplayUtils::ReallocatedFrameBuffersIfNeed()
{
  if (!need_clean_buffers_) {
    return;
  }

#if BUILDFLAG(ARKWEB_VULKAN)
  if (display_ && display_->renderer_) {
    display_->renderer_->ReallocatedFrameBuffers();
  }
#endif
}
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
void ArkwebDisplayUtils::CleanBufferAfterSwapBuffer(bool delay_clean) {
  if (display_->output_surface_) {
    display_->output_surface_->CleanBufferAfterSwapBuffer(delay_clean);
  }
}
#endif
}  // namespace viz

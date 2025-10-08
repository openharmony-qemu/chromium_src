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
#include "arkweb/chromium_ext/ui/compositor/compositor_utils.h"
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_ARKWEB)
#include "mojo/public/cpp/bindings/sync_call_restrictions.h"
#endif
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "components/viz/host/host_frame_sink_client.h"
#include "arkweb/chromium_ext/components/viz/host/host_frame_sink_manager_utils.h"
namespace ui {

//LCOV_EXCL_START
CompositorUtils::CompositorUtils(Compositor* compositor) : compositor_(compositor) {}
//LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void CompositorUtils::SetDrawRect(const gfx::Rect& new_rect) {
  if (compositor_->display_private_) {
    TRACE_EVENT0("viz", "CompositorExt::SetDrawRect");
    mojo::SyncCallRestrictions::ScopedAllowSyncCall scoped_allow_sync_call;
    compositor_->display_private_->SetDrawRect(new_rect);
  }
}

void CompositorUtils::SetDrawMode(const int32_t& mode) {
  drawMode_ = mode;
  if (compositor_->display_private_) {
    TRACE_EVENT0("viz", "Compositor::SetDrawMode");
    mojo::SyncCallRestrictions::ScopedAllowSyncCall scoped_allow_sync_call;
    compositor_->display_private_->SetDrawMode(mode);
  }
}
#endif

//LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_SAME_LAYER)
void CompositorUtils::SetNativeInnerWeb(bool isInnerWeb) {
  isInnerWeb_ = isInnerWeb;
  if (compositor_->display_private_) {
    compositor_->display_private_->SetNativeInnerWeb(isInnerWeb);
  }
}
#endif
//LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void CompositorUtils::SetBypassVsyncCondition(int32_t condition) {
  condition_ = condition;
  if (compositor_->display_private_) {
    compositor_->display_private_->SetBypassVsyncCondition(condition);
  }
}
#endif

//LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void CompositorUtils::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (compositor_->display_private_) {
    TRACE_EVENT0("viz", "CompositorExt::SetShouldFrameSubmissionBeforeDraw");
    mojo::SyncCallRestrictions::ScopedAllowSyncCall scoped_allow_sync_call;
    compositor_->display_private_->SetShouldFrameSubmissionBeforeDraw(should);
  }
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CompositorUtils::SendInternalBeginFrame() {
  compositor_->context_factory_->SendInternalBeginFrame(compositor_->frame_sink_id());
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void CompositorUtils::SetEnableLowerFrameRate(bool enabled) {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->SetEnableLowerFrameRate(
      enabled, compositor_->frame_sink_id());
}

void CompositorUtils::SetEnableHalfFrameRate(bool enabled) {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->SetEnableHalfFrameRate(
      enabled, compositor_->frame_sink_id());
}

void CompositorUtils::EvictFrameBackBuffers() {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->EvictFrameBackBuffers(
      compositor_->frame_sink_id());
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void CompositorUtils::UpdateVSyncFrequency() {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->UpdateVSyncFrequency(
      compositor_->frame_sink_id());
}

void CompositorUtils::ResetVSyncFrequency() {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->ResetVSyncFrequency(
      compositor_->frame_sink_id());
}
#endif
//LCOV_EXCL_STOP

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void CompositorUtils::SetCurrentFrameSinkId(const viz::FrameSinkId& id) {
  if (compositor_->display_private_) {
    compositor_->display_private_->SetCurrentFrameSinkId(id);
  } else {
    LOG(ERROR) << "Compositor::SetCurrentDisplay display_private error";
  }
}
#endif

//LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void CompositorUtils::DisableSwapUntilMaximized() {
  if (compositor_->display_private_) {
    compositor_->display_private_->DisableSwapUntilMaximized();
  }
}

#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_PIP)
void CompositorUtils::SetPipActive(bool active) {
  compositor_->context_factory_->GetHostFrameSinkManager()->managerUtils->SetPipActive(
    active, compositor_->frame_sink_id());
}
#endif
//LCOV_EXCL_STOP

} // namespace ui
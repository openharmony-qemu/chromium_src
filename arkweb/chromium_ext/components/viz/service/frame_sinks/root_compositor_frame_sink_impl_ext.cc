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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/root_compositor_frame_sink_impl_ext.h"
#include "arkweb/chromium_ext/components/viz/service/display/arkweb_display_utils.h"

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

namespace viz {

RootCompositorFrameSinkImplExt::RootCompositorFrameSinkImplExt(
    FrameSinkManagerImpl* frame_sink_manager,
    const FrameSinkId& frame_sink_id,
    mojo::PendingAssociatedReceiver<mojom::CompositorFrameSink>
        frame_sink_receiver,
    mojo::PendingRemote<mojom::CompositorFrameSinkClient> frame_sink_client,
    mojo::PendingAssociatedReceiver<mojom::DisplayPrivate> display_receiver,
    mojo::Remote<mojom::DisplayClient> display_client,
    std::unique_ptr<SyntheticBeginFrameSource> synthetic_begin_frame_source,
    std::unique_ptr<ExternalBeginFrameSource> external_begin_frame_source,
    std::unique_ptr<Display> display,
    bool hw_support_for_multiple_refresh_rates)
    : RootCompositorFrameSinkImpl(frame_sink_manager,
                                  frame_sink_id,
                                  std::move(frame_sink_receiver),
                                  std::move(frame_sink_client),
                                  std::move(display_receiver),
                                  std::move(display_client),
                                  std::move(synthetic_begin_frame_source),
                                  std::move(external_begin_frame_source),
                                  std::move(display),
                                  hw_support_for_multiple_refresh_rates) {}

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void RootCompositorFrameSinkImplExt::SetDrawRect(const gfx::Rect& new_rect) {
  external_begin_frame_source_->SetDrawRect(new_rect);
  display_->display_utils()->SetDrawRect(new_rect);
}

void RootCompositorFrameSinkImplExt::SetDrawMode(int32_t mode) {
  LOG(INFO) << "rootCompositorFrameSinkImpl::SetDrawMode";
  display_->display_utils()->SetDrawMode(mode);
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void RootCompositorFrameSinkImplExt::SetNativeInnerWeb(bool isInnerWeb) {
   display_->display_utils()->SetNativeInnerWeb(isInnerWeb);
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void RootCompositorFrameSinkImplExt::SetShouldFrameSubmissionBeforeDraw(
    bool should,
    SetShouldFrameSubmissionBeforeDrawCallback callback) {
  TRACE_EVENT1(
      "viz", "RootCompositorFrameSinkImpl::SetShouldFrameSubmissionBeforeDraw",
      "should", should);
  display_->display_utils()->SetShouldFrameSubmissionBeforeDraw(should);
  if (callback) {
    std::move(callback).Run();
  }
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void RootCompositorFrameSinkImplExt::SetEnableLowerFrameRate(bool enabled) {
  external_begin_frame_source_->SetEnableLowerFrameRate(enabled);
}

void RootCompositorFrameSinkImplExt::SetEnableHalfFrameRate(bool enabled) {
  external_begin_frame_source_->SetEnableHalfFrameRate(enabled);
}

void RootCompositorFrameSinkImplExt::EvictFrameBackBuffers(bool invisible) {
  TRACE_EVENT1("viz", "RootCompositorFrameSinkImpl::EvictFrameBackBuffers",
               "invisible", invisible);
  if (invisible) {
    SetDisplayVisible(false);
  } else {
    SetDisplayVisible(true);
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void RootCompositorFrameSinkImplExt::UpdateVSyncFrequency(int frame_rate) {
  external_begin_frame_source_->UpdateVSyncFrequency(frame_rate);
}

void RootCompositorFrameSinkImplExt::ResetVSyncFrequency() {
  external_begin_frame_source_->ResetVSyncFrequency();
}
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void RootCompositorFrameSinkImplExt::SendInternalBeginFrame() {
  external_begin_frame_source_->SendInternalBeginFrame();
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void RootCompositorFrameSinkImplExt::SetCurrentFrameSinkId(
    const FrameSinkId& frame_sink_id) {
  external_begin_frame_source_->SetCurrentFrameSinkId(frame_sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void RootCompositorFrameSinkImplExt::DisableSwapUntilMaximized(
    DisableSwapUntilMaximizedCallback callback) {
  if (display_) {
    display_->display_utils()->DisableSwapUntilMaximized();
  }
  if (callback) {
    std::move(callback).Run();
  }
}

void RootCompositorFrameSinkImplExt::RestoreRenderFit(
    const FrameSinkId& frame_sink_id) {
  if (managerImplUtils) {
    managerImplUtils->RestoreRenderFit(frame_sink_id);
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_PIP)
void RootCompositorFrameSinkImplExt::SetPipActive(bool active) {
  external_begin_frame_source_->SetPipActive(active);
}
#endif

}  // namespace viz
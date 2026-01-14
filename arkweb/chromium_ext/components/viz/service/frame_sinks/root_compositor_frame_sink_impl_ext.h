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
 *
 * limitations under the License.
 */

#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_ROOT_COMPOSITOR_FRAME_SINK_IMPL_EXT_H_
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_ROOT_COMPOSITOR_FRAME_SINK_IMPL_EXT_H_

#include <memory>
#include <optional>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/service/frame_sinks/root_compositor_frame_sink_impl.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/viz/privileged/mojom/compositing/begin_frame_observer.mojom.h"
#include "services/viz/privileged/mojom/compositing/display_private.mojom.h"
#include "services/viz/privileged/mojom/compositing/frame_sink_manager.mojom.h"
#include "services/viz/public/mojom/compositing/compositor_frame_sink.mojom.h"

namespace viz {
class Display;
class RootCompositorFrameSinkImpl;
class ExternalBeginFrameSource;

class RootCompositorFrameSinkImplExt : public RootCompositorFrameSinkImpl {
 public:
  RootCompositorFrameSinkImplExt(const RootCompositorFrameSinkImplExt&) =
      delete;
  RootCompositorFrameSinkImplExt& operator=(
      const RootCompositorFrameSinkImplExt&) = delete;

  RootCompositorFrameSinkImplExt(
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
      bool hw_support_for_multiple_refresh_rates);

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(
      bool should,
      SetShouldFrameSubmissionBeforeDrawCallback callback) override;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void SetDrawRect(const gfx::Rect& new_rect) override;
  void SetDrawMode(int32_t mode) override;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeInnerWeb(bool isInnerWeb) override;
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void SetEnableLowerFrameRate(bool enabled);
  void SetEnableHalfFrameRate(bool enabled);
  void EvictFrameBackBuffers();
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  void SetIsOfflineWebComponentInactive(bool is_inactive);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void UpdateVSyncFrequency(int frame_rate);
  void ResetVSyncFrequency();
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void ReportKeyThreadIds(const std::vector<int32_t>& thread_ids,
                          int32_t process_id,
                          bool is_created) override {}
#endif  // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetCurrentFrameSinkId(const FrameSinkId& frame_sink_id) override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SendInternalBeginFrame();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void DisableSwapUntilMaximized() override;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_PIP)
  void SetPipActive(bool active);
#endif

  RootCompositorFrameSinkImplExt* AsExt() override { return this; }

 private:
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void RestoreRenderFit(const FrameSinkId& frame_sink_id) override;
#endif  // ARKWEB_MAXIMIZE_RESIZE
};
}  // namespace viz
#endif  // COMPONENTS_VIZ_SERVICE_FRAME_SINKS_ROOT_COMPOSITOR_FRAME_SINK_IMPL_EXT_H_
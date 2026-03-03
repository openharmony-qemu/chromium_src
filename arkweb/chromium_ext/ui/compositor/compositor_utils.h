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
#ifndef UI_ARKWEB_COMPOSITOR_UTILS_H_
#define UI_ARKWEB_COMPOSITOR_UTILS_H_

#include "ui/compositor/compositor.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "services/viz/privileged/mojom/compositing/display_private.mojom.h"
#include "services/viz/privileged/mojom/compositing/external_begin_frame_controller.mojom.h"
#include "services/viz/privileged/mojom/compositing/vsync_parameter_observer.mojom-forward.h"
#include "ui/gfx/display_color_spaces.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/vector2d.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/overlay_transform.h"

namespace ui {
class Compositor;

class CompositorUtils {
 public:
  CompositorUtils(Compositor* compositor);

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void SetDrawRect(const gfx::Rect& new_rect);
  void SetDrawMode(const int32_t& mode);
  int32_t drawMode_ = 0;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeInnerWeb(bool isInnerWeb);
  bool isInnerWeb_ = false;
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition);
  int32_t condition_ = 0;
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should);
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SendInternalBeginFrame();
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
  void UpdateVSyncFrequency();
  void ResetVSyncFrequency();
#endif

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetCurrentFrameSinkId(const viz::FrameSinkId& id);
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void DisableSwapUntilMaximized();
#endif

#if BUILDFLAG(ARKWEB_PIP)
  void SetPipActive(bool active);
#endif

 private:
  raw_ptr<Compositor> compositor_;
};
} // namespace ui

#endif  // UI_ARKWEB_COMPOSITOR_UTILS_H_
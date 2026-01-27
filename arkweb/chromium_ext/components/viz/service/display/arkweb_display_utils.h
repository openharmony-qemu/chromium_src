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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_ARKWEB_DISPLAY_UTILS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_ARKWEB_DISPLAY_UTILS_H_

#include <string>

#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "components/viz/service/display/display.h"
#if BUILDFLAG(ARKWEB_DFX_DUMP)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "components/viz/service/gl/gpu_service_impl.h"
#endif
namespace viz {

#if BUILDFLAG(ARKWEB_DFX_DUMP)
class DumpFrameObserver;
#endif
class Display;
class DisplayClient;

class ArkwebDisplayUtils {
 public:
  ArkwebDisplayUtils(Display* display);
  ~ArkwebDisplayUtils();

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should);
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void SetDrawRect(const gfx::Rect& new_rect);
  void SetDrawMode(const int32_t mode);
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeInnerWeb(bool isInnerWeb);
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition);
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void DisableSwapUntilMaximized();
  void RestoreRenderFitTimeElapsed();
  void ReenableSwapCheck(const SurfaceId& surface_id, int width, int height);
  void RestoreRenderFit();
  bool ShouldDisableSwap(bool should);
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  void DrawAndSwapDump(AggregatedFrame& frame);
#endif
  void Resize(const gfx::Size& size);
  void DrawAndSwap(AggregatedRenderPass& last_render_pass,
                   gfx::Size current_surface_size,
                   AggregatedFrame& frame);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void DumpSnapshotForBlankLess(AggregatedFrame& frame);

  void SetClientId(const uint32_t client_id);

  void SetGpuServiceImpl(GpuServiceImpl* gpu_service_impl);
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void DiscardBackbuffer();
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  void CleanBufferAfterSwapBuffer(bool delay_clean);
#endif

 private:
#if BUILDFLAG(ARKWEB_UNITTESTS)
  friend class ArkwebDisplayUtilsTest;
#endif

  raw_ptr<Display> display_;
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  gfx::Rect draw_rect_;
  int32_t draw_mode_ = 0;
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  std::unique_ptr<DumpFrameObserver> dump_frame_observer_;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  enum class TempIdleState : uint32_t {
    INIT,
    DISABLE_SWAP,
    REENABLE_SWAP,
    RESTORE_RENDERFIT,
  };
  TempIdleState temp_idle_state_ = TempIdleState::RESTORE_RENDERFIT;
  std::unique_ptr<base::RetainingOneShotTimer> reset_init_timer_;
  std::unique_ptr<base::RetainingOneShotTimer> reenable_swap_timer_;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  raw_ptr<GpuServiceImpl> gpu_service_impl_ {nullptr};
  uint32_t client_id_ = 0;
#endif
  base::WeakPtrFactory<ArkwebDisplayUtils> weak_factory_{this};
};

}  // namespace viz
#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_ARKWEB_DISPLAY_UTILS_H_
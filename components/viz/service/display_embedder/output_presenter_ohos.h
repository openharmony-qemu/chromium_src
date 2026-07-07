/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_OUTPUT_PRESENTER_OHOS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_OUTPUT_PRESENTER_OHOS_H_

#include <memory>

#include "base/containers/circular_deque.h"
#include "base/containers/flat_map.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/trace_event/trace_event.h"
#include "components/viz/service/display_embedder/output_presenter.h"
#include "components/viz/service/viz_service_export.h"

typedef struct OH_NativeBuffer OH_NativeBuffer;
typedef struct OH_SurfaceControl OH_SurfaceControl;
typedef struct OH_SurfaceTransaction OH_SurfaceTransaction;

struct OH_Rect {
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;
};
typedef struct OH_Rect OH_Rect;

enum DirtyBits {
  kName,
  kVisible,
  kBounds,
  kFrameGravity,
  kTranslate,
  kPivot,
  kBuffer,
  kBorderWidth,
  kBorderColor,
  kBorderStyle,
  kForegroundColor,
  kBackgroundColor,
  kAlpha,
  kZOrder,
  kBufferTransform,
  kSrcRect,
  kDisplayRect,
  kCount
};

/**
 * Parameter for OH_SurfaceTransaction_setVisibility().
 */
enum OH_SurfaceTransactionVisibility : int8_t {
  OH_SURFACE_TRANSACTION_VISIBILITY_HIDE = 0,
  OH_SURFACE_TRANSACTION_VISIBILITY_SHOW = 1,
};

enum OH_TRANSFORM : int32_t {
  OH_TRANSFORM_ROTATE_NONE = 0, /**< No rotation */
  OH_TRANSFORM_ROTATE_90,       /**< Rotation by 90 degrees */
  OH_TRANSFORM_ROTATE_180,      /**< Rotation by 180 degrees */
  OH_TRANSFORM_ROTATE_270,      /**< Rotation by 270 degrees */
  OH_TRANSFORM_FLIP_H,          /**< Flip horizontally */
  OH_TRANSFORM_FLIP_V,          /**< Flip vertically */
  OH_TRANSFORM_FLIP_H_ROT90,    /**< Flip horizontally and rotate 90 degrees */
  OH_TRANSFORM_FLIP_V_ROT90,    /**< Flip vertically and rotate 90 degrees */
  OH_TRANSFORM_FLIP_H_ROT180,   /**< Flip horizontally and rotate 180 degrees */
  OH_TRANSFORM_FLIP_V_ROT180,   /**< Flip vertically and rotate 180 degrees */
  OH_TRANSFORM_FLIP_H_ROT270,   /**< Flip horizontally and rotate 270 degrees */
  OH_TRANSFORM_FLIP_V_ROT270,   /**< Flip vertically and rotate 270 degrees */
  OH_TRANSFORM_ROTATE_LAST = OH_TRANSFORM_FLIP_V_ROT270,
};

namespace viz {
class SequencedTaskRunner;
class SkiaOutputSurfaceDependency;

class VIZ_SERVICE_EXPORT OutputPresenterOHOS : public OutputPresenter {
 public:
  explicit OutputPresenterOHOS(SkiaOutputSurfaceDependency* deps);
  ~OutputPresenterOHOS() override;

  static std::unique_ptr<OutputPresenterOHOS> Create(
      SkiaOutputSurfaceDependency* deps);
  void InitializeCapabilities(OutputSurface::Capabilities* capabilities) final;
  void Present(SwapCompletionCallback completion_callback,
               BufferPresentedCallback presentation_callback,
               gfx::FrameData data) final;
  bool Reshape(const ReshapeParams& params) final;
  void ScheduleOverlayPlane(
      const OutputPresenter::OverlayPlaneCandidate& overlay_plane_candidate,
      ScopedOverlayAccess* access) final override;
#if BUILDFLAG(ARKWEB_DELEGATE_COMPOSITING)
  void DiscardCompositingResources(bool is_clean_all) final override;
#endif

 private:
  class SurfaceControl;

  void OnComplete(uint64_t timestamp);
  void OnOverlayReleased(OH_NativeBuffer* buffer,
                         base::ScopedFD release_fence_fd);
  void TriggerOnCompleteCallbacks();

  const raw_ptr<SkiaOutputSurfaceDependency> dependency_;
  gfx::Size size_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  OH_SurfaceTransaction* transaction_;
  gfx::OverlayTransform transform_ = gfx::OVERLAY_TRANSFORM_NONE;

  OHOS::NWeb::SurfaceControlAdapter& surface_control_adapter_;
  OHOS::NWeb::SurfaceTransactionAdapter& transaction_adapter_;

  std::unique_ptr<SurfaceControl> root_surface_;
  ScopedOverlayAccess* root_overlay_access_ = nullptr;
  std::deque<std::unique_ptr<SurfaceControl>> pending_frame_;
  std::deque<std::unique_ptr<SurfaceControl>> previous_frame_;

  struct CompletionData {
    SwapCompletionCallback callback;
    ScopedOverlayAccess* access = nullptr;
    bool completed = false;
  };
  base::circular_deque<CompletionData> completion_datas_;
  base::flat_map<OH_NativeBuffer*, std::pair<ScopedOverlayAccess*, bool>>
      in_used_buffers_;
  base::circular_deque<BufferPresentedCallback> presentation_callbacks_;
  using OnCompleteCallback = base::RepeatingCallback<void(uint64_t timestamp)>;
  std::unique_ptr<OnCompleteCallback> on_complete_callback_;
  THREAD_CHECKER(thread_checker_);

  base::WeakPtrFactory<OutputPresenterOHOS> weak_factory_{this};
};

}  // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_OUTPUT_PRESENTER_OHOS_H_
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

#include <memory>
#include <optional>

#include "base/feature_list.h"
#include "base/trace_event/trace_event.h"
#include "cc/base/math_util.h"
#include "components/viz/common/features.h"
#include "components/viz/service/display/overlay_processor_ohos.h"
#include "components/viz/service/display/overlay_strategy_underlay.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/overlay_transform_utils.h"

namespace viz {
namespace {
constexpr float kMinScale = 0.5;
constexpr float kMaxScale = 30;
constexpr int32_t kMinDisplayWidth = 480;
constexpr int32_t kMaxDisplayWidth = 4096;
constexpr int32_t kMinDisplayHeight = 360;
constexpr int32_t kMaxDisplayHeight = 4096;
constexpr int32_t kMinSrcWidth = 480;
constexpr int32_t kMaxSrcWidth = 4096;
constexpr int32_t kMinSrcHeight = 360;
constexpr int32_t kMaxSrcHeight = 4096;

gfx::RectF ClipFromOrigin(gfx::RectF input) {
  if (input.x() < 0.f) {
    input.set_width(input.width() + input.x());
    input.set_x(0.f);
  }

  if (input.y() < 0.f) {
    input.set_height(input.height() + input.y());
    input.set_y(0.f);
  }

  return input;
}
}  // namespace

OverlayProcessorOhos::OverlayProcessorOhos() {
  // Android webview never sets |frame_sequence_number_| for the overlay
  // processor. Android Chrome does set this variable because it does call draw.
  // However, it also may not update this variable when displaying an overlay.
  // Therefore, our damage tracking for overlays is incorrect and we must ignore
  // the thresholding of prioritization.

  prioritization_config_.changing_threshold = false;
  prioritization_config_.damage_rate_threshold = false;

  strategies_.push_back(std::make_unique<OverlayStrategyUnderlay>(
      this, OverlayStrategyUnderlay::OpaqueMode::AllowTransparentCandidates));
}

OverlayProcessorOhos::~OverlayProcessorOhos() = default;

void OverlayProcessorOhos::AdjustPrimaryPlaneForDisplayTransform(
    OverlayCandidate& primary_plane) const {
  // Apply the display transform hint which will be non-identity if output
  // surface's `orientation_mode` is `kHardware` and the display's hardware
  // orientation does not match the OS's logical orientation. This will allow us
  // to draw into the primary plane in the orientation that allows the hardware
  // to make use of hardware overlays.
  DCHECK(
      std::holds_alternative<gfx::OverlayTransform>(primary_plane.transform));
  DCHECK_EQ(std::get<gfx::OverlayTransform>(primary_plane.transform),
            gfx::OVERLAY_TRANSFORM_NONE);
  DCHECK(primary_plane.display_rect ==
         ClipFromOrigin(primary_plane.display_rect));
  primary_plane.transform = display_transform_;
  const gfx::Transform display_inverse = gfx::OverlayTransformToTransform(
      gfx::InvertOverlayTransform(display_transform_),
      gfx::SizeF(viewport_size_));
  primary_plane.display_rect =
      display_inverse.MapRect(primary_plane.display_rect);
  primary_plane.display_rect =
      gfx::RectF(gfx::ToEnclosingRect(primary_plane.display_rect));
}

void OverlayProcessorOhos::CheckOverlaySupportImpl(
    const std::optional<OverlayCandidate>& primary_plane,
    OverlayCandidateList* candidates) {
  if (candidates == nullptr) {
    LOG(ERROR) << "CheckOverlaySupportImpl candidates is nullptr";
    return;
  }
  DCHECK(!candidates->empty());
  for (auto& candidate : *candidates) {
    if (!is_focused_) {
      TRACE_EVENT0("viz", "CheckOverlaySupportImpl reject due to lost focus");
      LOG(DEBUG) << "Overlay rejected due to candidates unfocus";
      candidate.overlay_handled = false;
      continue;
    }

    if (!is_single_screen_) {
      TRACE_EVENT0("viz", "CheckOverlaySupportImpl reject due to multi screen");
      LOG(DEBUG) << "Overlay rejected due to multi screen";
      candidate.overlay_handled = false;
      continue;
    }
    // Aggregator adds `display_transform_` to all quads, which is then added to
    // `candidate.transform` here. `display_transform_` only applies to content
    // on the main plane so it needs to be removed candidate it its own plane.
    gfx::OverlayTransform candidate_overlay_transform = OverlayTransformsConcat(
        std::get<gfx::OverlayTransform>(candidate.transform),
        InvertOverlayTransform(display_transform_));
    // Note the transform below using `candidate_overlay_transform` to compute
    // clipped and normalized `uv_rect` is only tested with NONE and
    // FLIP_VERTICAL.
    if (candidate_overlay_transform != gfx::OVERLAY_TRANSFORM_NONE &&
        candidate_overlay_transform != gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL) {
      TRACE_EVENT0("viz", "CheckOverlaySupportImpl due to transform invalid");
      LOG(DEBUG) << "Overlay rejected due to transform invalid";
      candidate.overlay_handled = false;
      continue;
    }
    candidate.transform = candidate_overlay_transform;

    gfx::RectF orig_display_rect = candidate.display_rect;
    gfx::RectF display_rect = orig_display_rect;
    if (candidate.clip_rect)
      display_rect.Intersect(gfx::RectF(*candidate.clip_rect));
    // The framework doesn't support display rects positioned at a negative
    // offset.
    display_rect = ClipFromOrigin(display_rect);
    if (display_rect.IsEmpty()) {
      LOG(DEBUG) << "Overlay rejected due to display_rect empty";
      candidate.overlay_handled = false;
      continue;
    }

    // The display rect above includes the |display_transform_| while the rects
    // sent to the platform API need to be in the logical screen space.
    const gfx::Transform display_inverse = gfx::OverlayTransformToTransform(
        gfx::InvertOverlayTransform(display_transform_),
        gfx::SizeF(viewport_size_));
    orig_display_rect = display_inverse.MapRect(orig_display_rect);
    display_rect = display_inverse.MapRect(display_rect);

    candidate.unclipped_display_rect = orig_display_rect;
    candidate.unclipped_uv_rect = candidate.uv_rect;

    candidate.display_rect = gfx::RectF(gfx::ToEnclosingRect(display_rect));

    // Transform `uv_rect` to display space, then clip, then transform back.
    candidate.uv_rect = gfx::OverlayTransformToTransform(
                            candidate_overlay_transform, gfx::SizeF(1, 1))
                            .MapRect(candidate.uv_rect);
    candidate.uv_rect = cc::MathUtil::ScaleRectProportional(
        candidate.uv_rect, orig_display_rect, candidate.display_rect);
    candidate.uv_rect = gfx::OverlayTransformToTransform(
        gfx::InvertOverlayTransform(candidate_overlay_transform),
        gfx::SizeF(1, 1)).MapRect(candidate.uv_rect);

    auto displayWidth = display_rect.width();
    auto displayHeight = display_rect.height();
    if (displayWidth < kMinDisplayWidth || displayWidth > kMaxDisplayWidth ||
      displayHeight < kMinDisplayHeight || displayHeight > kMaxDisplayHeight) {
      TRACE_EVENT2("viz", "CheckOverlaySupportImpl reject",
          "displayWidth", displayWidth, "displayHeight", displayHeight);
      LOG(DEBUG) << "Overlay rejected due to size limits. " <<
        "Display: " << displayWidth << "x" << displayHeight;
      candidate.overlay_handled = false;
      continue;
    }
    auto src_rect = gfx::ScaleRect(candidate.uv_rect,
                                   candidate.resource_size_in_pixels);
    auto srcWidth = src_rect.width();
    auto srcHeight = src_rect.height();
    if (srcWidth < kMinSrcWidth || srcWidth > kMaxSrcWidth ||
      srcHeight < kMinSrcHeight || srcHeight > kMaxSrcHeight) {
      TRACE_EVENT2("viz", "CheckOverlaySupportImpl reject",
                   "srcWidth", srcWidth, "srcHeight", srcHeight);
      LOG(DEBUG) << "Overlay rejected due to size limits. "
                 << "Src: " << srcWidth << " x " << srcHeight;
      candidate.overlay_handled = false;
      continue;
    }

    auto x_scale = displayWidth / srcWidth;
    auto y_scale = displayHeight / srcHeight;
    if (std::clamp(x_scale, kMinScale, kMaxScale) != x_scale ||
        std::clamp(y_scale, kMinScale, kMaxScale) != y_scale) {
      TRACE_EVENT2("viz", "CheckOverlaySupportImpl reject",
                   "x_scale", x_scale, "y_scale", y_scale);
      candidate.overlay_handled = false;
      continue;
    }

    candidate.overlay_handled = true;
    TRACE_EVENT0("viz", "Ohos CheckOverlaySupportImpl success");
  }
}

gfx::Rect OverlayProcessorOhos::GetOverlayDamageRectForOutputSurface(
    const OverlayCandidate& candidate) const {
  // Should only be called after ProcessForOverlays on handled candidates.
  DCHECK(candidate.overlay_handled);
  // We transform the candidate's display rect to the logical screen space (used
  // by the ui when preparing the frame) that the SurfaceControl expects it to
  // be in. So in order to provide a damage rect which maps to the
  // OutputSurface's main plane, we need to undo that transformation. But only
  // if the overlay is in handled state, since the modification above is only
  // applied when we mark the overlay as handled.
  gfx::Size viewport_size_pre_display_transform(viewport_size_.height(),
                                                viewport_size_.width());
  auto transform = gfx::OverlayTransformToTransform(
      display_transform_, gfx::SizeF(viewport_size_pre_display_transform));
  return transform.MapRect(gfx::ToEnclosingRect(candidate.display_rect));
}

void OverlayProcessorOhos::InsertPrimaryPlane(
    OverlayCandidate primary_plane,
    OverlayCandidateList& candidates) {
  AdjustPrimaryPlaneForDisplayTransform(primary_plane);

  const auto insert_position = candidates.begin();
  candidates.insert(insert_position, std::move(primary_plane));
}

bool OverlayProcessorOhos::IsOverlaySupported() const {
  return true;
}

bool OverlayProcessorOhos::NeedsSurfaceDamageRectList() const {
  return true;
}

void OverlayProcessorOhos::SetDisplayTransformHint(
    gfx::OverlayTransform transform) {
  display_transform_ = transform;
}

void OverlayProcessorOhos::SetViewportSize(
    const gfx::Size& viewport_size) {
  viewport_size_ = viewport_size;
}

bool OverlayProcessorOhos::SupportsFlipRotateTransform() const {
  return true;
}

#if BUILDFLAG(ARKWEB_DELEGATE_COMPOSITING)
void OverlayProcessorOhos::UpdateFocusAndScreenStatus(bool is_focused,
                                                     bool is_single_screen) {
  is_focused_ = is_focused;
  is_single_screen_ = is_single_screen;
}
#endif

}  // namespace viz

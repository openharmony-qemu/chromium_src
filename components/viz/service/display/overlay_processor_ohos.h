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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_OVERLAY_PROCESSOR_OHOS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_OVERLAY_PROCESSOR_OHOS_H_

#include <optional>

#include "components/viz/service/display/overlay_processor_using_strategy.h"

namespace viz {

class VIZ_SERVICE_EXPORT OverlayProcessorOhos
    : public OverlayProcessorUsingStrategy {
 public:
  OverlayProcessorOhos();
  ~OverlayProcessorOhos() override;

  void AdjustPrimaryPlaneForDisplayTransform(
      OverlayCandidate& primary_plane) const;
  void CheckOverlaySupportImpl(
      const std::optional<OverlayCandidate>& primary_plane,
      OverlayCandidateList* candidates) override;
  gfx::Rect GetOverlayDamageRectForOutputSurface(
      const OverlayCandidate& overlay) const override;
  void InsertPrimaryPlane(OverlayCandidate primary_plane,
                          OverlayCandidateList& candidates) override;
  bool IsOverlaySupported() const override;
  bool NeedsSurfaceDamageRectList() const override;
  void SetDisplayTransformHint(gfx::OverlayTransform transform) override;
  void SetViewportSize(const gfx::Size& size) override;
  bool SupportsFlipRotateTransform() const override;
  void UpdateFocusAndScreenStatus(bool is_focused,
                                  bool is_single_screen) override;

 private:
#if BUILDFLAG(ARKWEB_AI_HDR)
  void CalculateOverlayCandidateRects(OverlayCandidate& candidate);
#endif

  gfx::OverlayTransform display_transform_ = gfx::OVERLAY_TRANSFORM_NONE;
  gfx::Size viewport_size_;
  bool is_focused_ = false;
  bool is_single_screen_ = false;
};
}  // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_OVERLAY_PROCESSOR_OHOS_H_

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

#include "components/viz/service/display/overlay_processor_ohos.h"

#include <optional>

#include "components/viz/service/display/overlay_candidate.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/overlay_transform_utils.h"

namespace viz {
namespace {

#define CHECK_DELEGATE_ENABLED_OVERLAY()                                         \
  do {                                                                           \
    static bool is_delegated_enabled =                                           \
      OHOS::NWeb::OhosAdapterHelper::GetInstance()                               \
      .GetSystemPropertiesInstance()                                             \
      .GetBoolParameter("web.delegatedCompositing.enable", false);               \
    if (!is_delegated_enabled) {                                                 \
      GTEST_SKIP() << "delegated compositing is disabled, skip test";            \
    }                                                                            \
  } while (0)                                                                    \

// Helper: create a basic OverlayCandidate with default valid values.
OverlayCandidate MakeCandidate() {
  OverlayCandidate c;
  c.transform = gfx::OVERLAY_TRANSFORM_NONE;
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(800, 600);
  c.overlay_handled = false;
  return c;
}

}  // namespace

class OverlayProcessorOhosTest : public ::testing::Test {
 protected:
  std::unique_ptr<OverlayProcessorOhos> CreateProcessor() {
    return std::make_unique<OverlayProcessorOhos>();
 }
};

TEST_F(OverlayProcessorOhosTest, Constructor) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  EXPECT_TRUE(processor->IsOverlaySupported());
  EXPECT_TRUE(processor->NeedsSurfaceDamageRectList());
  EXPECT_TRUE(processor->SupportsFlipRotateTransform());
}

TEST_F(OverlayProcessorOhosTest, SetDisplayTransformHint) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90);

  OverlayCandidate primary;
  primary.transform = gfx::OVERLAY_TRANSFORM_NONE;
  primary.display_rect = gfx::RectF(0, 0, 1920, 1080);
  processor->AdjustPrimaryPlaneForDisplayTransform(primary);
  EXPECT_EQ(std::get<gfx::OverlayTransform>(primary.transform),
            gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90);
}

TEST_F(OverlayProcessorOhosTest, SetViewportSize) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));

  OverlayCandidate c = MakeCandidate();
  c.overlay_handled = true;
  c.display_rect = gfx::RectF(0, 0, 400, 300);
  gfx::Rect damage = processor->GetOverlayDamageRectForOutputSurface(c);
  EXPECT_FALSE(damage.IsEmpty());
}

TEST_F(OverlayProcessorOhosTest, UpdateFocusAndScreenStatus_FocusedSingleScreen) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  candidates.push_back(MakeCandidate());
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, UpdateFocusAndScreenStatus_Unfocused) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(false, true);

  OverlayCandidateList candidates;
  candidates.push_back(MakeCandidate());
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, UpdateFocusAndScreenStatus_MultiScreen) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, false);

  OverlayCandidateList candidates;
  candidates.push_back(MakeCandidate());
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_NullCandidates) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);
  processor->CheckOverlaySupportImpl(std::nullopt, nullptr);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_TransformNone) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_NONE;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_TransformFlipVertical) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_TransformRotate90) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_TransformRotate180) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_TransformRotate270) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest,
     CheckOverlaySupportImpl_TransformFlipVerticalClockwise90) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.transform = gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_90;
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_NegativeDisplayRectX) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(-100, 0, 800, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_NegativeDisplayRectY) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, -100, 800, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_EmptyDisplayRect) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(-900, 0, 100, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_DisplayWidthTooSmall) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 200, 600);
  c.resource_size_in_pixels = gfx::Size(200, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_DisplayHeightTooSmall) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 200);
  c.resource_size_in_pixels = gfx::Size(800, 200);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_DisplayWidthTooLarge) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(5000, 5000));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 5000, 600);
  c.resource_size_in_pixels = gfx::Size(5000, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_DisplayHeightTooLarge) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(5000, 5000));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 5000);
  c.resource_size_in_pixels = gfx::Size(800, 5000);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_SrcWidthTooSmall) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(200, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_SrcHeightTooSmall) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(800, 200);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_SrcWidthTooLarge) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(5000, 5000));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(5000, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_SrcHeightTooLarge) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(5000, 5000));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(800, 5000);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_ScaleTooSmall) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 800, 600);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(1920, 1080);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_ScaleTooLarge) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(0, 0, 1920, 1080);
  c.uv_rect = gfx::RectF(0, 0, 1, 1);
  c.resource_size_in_pixels = gfx::Size(48, 36);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_WithClipRect) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.clip_rect = gfx::Rect(0, 0, 800, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_ClipRectMakesEmpty) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  c.display_rect = gfx::RectF(800, 0, 800, 600);
  c.clip_rect = gfx::Rect(0, 0, 400, 600);
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_Success) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c = MakeCandidate();
  candidates.push_back(c);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_TRUE(candidates[0].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, CheckOverlaySupportImpl_MultipleCandidates) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetViewportSize(gfx::Size(1920, 1080));
  processor->UpdateFocusAndScreenStatus(true, true);

  OverlayCandidateList candidates;
  OverlayCandidate c1 = MakeCandidate();
  OverlayCandidate c2 = MakeCandidate();
  candidates.push_back(c1);
  candidates.push_back(c2);
  processor->UpdateFocusAndScreenStatus(false, true);
  processor->CheckOverlaySupportImpl(std::nullopt, &candidates);
  EXPECT_FALSE(candidates[0].overlay_handled);
  EXPECT_FALSE(candidates[1].overlay_handled);
}

TEST_F(OverlayProcessorOhosTest, AdjustPrimaryPlaneForDisplayTransform_None) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_NONE);
  processor->SetViewportSize(gfx::Size(1920, 1080));

  OverlayCandidate primary;
  primary.transform = gfx::OVERLAY_TRANSFORM_NONE;
  primary.display_rect = gfx::RectF(0, 0, 1920, 1080);
  processor->AdjustPrimaryPlaneForDisplayTransform(primary);
  EXPECT_EQ(std::get<gfx::OverlayTransform>(primary.transform),
            gfx::OVERLAY_TRANSFORM_NONE);
}

TEST_F(OverlayProcessorOhosTest, AdjustPrimaryPlaneForDisplayTransform_Rotate90) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90);
  processor->SetViewportSize(gfx::Size(1080, 1920));

  OverlayCandidate primary;
  primary.transform = gfx::OVERLAY_TRANSFORM_NONE;
  primary.display_rect = gfx::RectF(0, 0, 1080, 1920);
  processor->AdjustPrimaryPlaneForDisplayTransform(primary);
  EXPECT_EQ(std::get<gfx::OverlayTransform>(primary.transform),
            gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90);
}

TEST_F(OverlayProcessorOhosTest, AdjustPrimaryPlaneForDisplayTransform_Rotate180) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180);
  processor->SetViewportSize(gfx::Size(1920, 1080));

  OverlayCandidate primary;
  primary.transform = gfx::OVERLAY_TRANSFORM_NONE;
  primary.display_rect = gfx::RectF(0, 0, 1920, 1080);
  processor->AdjustPrimaryPlaneForDisplayTransform(primary);
  EXPECT_EQ(std::get<gfx::OverlayTransform>(primary.transform),
            gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180);
}

TEST_F(OverlayProcessorOhosTest, AdjustPrimaryPlaneForDisplayTransform_Rotate270) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270);
  processor->SetViewportSize(gfx::Size(1080, 1920));

  OverlayCandidate primary;
  primary.transform = gfx::OVERLAY_TRANSFORM_NONE;
  primary.display_rect = gfx::RectF(0, 0, 1080, 1920);
  processor->AdjustPrimaryPlaneForDisplayTransform(primary);
  EXPECT_EQ(std::get<gfx::OverlayTransform>(primary.transform),
            gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270);
}

TEST_F(OverlayProcessorOhosTest,
     GetOverlayDamageRectForOutputSurface_NoneTransform) {
  CHECK_DELEGATE_ENABLED_OVERLAY();
  auto processor = CreateProcessor();
  processor->SetDisplayTransformHint(gfx::OVERLAY_TRANSFORM_NONE);
  processor->SetViewportSize(gfx::Size(1920, 1080));
  OverlayCandidate c = MakeCandidate();
  c.overlay_handled = true;
  c.display_rect = gfx::RectF(100, 100, 400, 300);
  gfx::Rect damage = processor->GetOverlayDamageRectForOutputSurface(c);
  EXPECT_FALSE(damage.IsEmpty());
}
}  // namespace viz

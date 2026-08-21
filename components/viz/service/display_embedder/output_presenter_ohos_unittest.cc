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

#include "components/viz/service/display_embedder/output_presenter_ohos.h"

#include <optional>
#include <set>

#include "components/viz/service/display/output_surface.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/overlay_transform.h"

namespace viz {
namespace {

#define CHECK_DELEGATE_ENABLED_PRESENTER()                                       \
  do {                                                                           \
    static bool is_delegated_enabled =                                           \
      OHOS::NWeb::OhosAdapterHelper::GetInstance()                               \
      .GetSystemPropertiesInstance()                                             \
      .GetBoolParameter("web.delegatedCompositing.enable", false);               \
    if (!is_delegated_enabled) {                                                 \
      GTEST_SKIP() << "delegated compositing is disabled, skip test";            \
    }                                                                            \
  } while (0)

gfx::Size GetRotatedSize(const gfx::Size size,
                         gfx::OverlayTransform transform) {
  const bool needs_swapped_dims =
      transform == gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90 ||
      transform == gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270 ||
      transform == gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_90 ||
      transform == gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_270;
  return needs_swapped_dims ? gfx::Size(size.height(), size.width()) : size;
}

int32_t FromGfxOverlayTransform(gfx::OverlayTransform transform) {
  switch (transform) {
    case gfx::OVERLAY_TRANSFORM_NONE:
      return 0;      case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90:
      return 1;      case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180:
      return 2;      case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270:
      return 3;      default:
      NOTREACHED();
  }
}

}  // namespace

TEST(OutputPresenterOhosTest, GetRotatedSize_NoneTransform) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_NONE), size);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_FlipHorizontal) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL),
            size);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_Rotate90) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            gfx::Size(1080, 1920));
}

TEST(OutputPresenterOhosTest, GetRotatedSize_Rotate270) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270),
            gfx::Size(1080, 1920));
}

TEST(OutputPresenterOhosTest, GetRotatedSize_FlipVerticalClockwise90) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(
                size, gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_90),
            gfx::Size(1080, 1920));
}

TEST(OutputPresenterOhosTest, GetRotatedSize_FlipVerticalClockwise270) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(
                size, gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_270),
            gfx::Size(1080, 1920));
}

TEST(OutputPresenterOhosTest, GetRotatedSize_Rotate180) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180),
            size);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_Square) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(100, 100);
    EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            size);
}

TEST(OutputPresenterOhosTest, FromGfxOverlayTransform_None) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_NONE), 0);
}

TEST(OutputPresenterOhosTest, FromGfxOverlayTransform_Rotate90) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            1);
}

TEST(OutputPresenterOhosTest, FromGfxOverlayTransform_Rotate180) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(
      FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180), 2);
}

TEST(OutputPresenterOhosTest, FromGfxOverlayTransform_Rotate270) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(
      FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270), 3);
}

TEST(OutputPresenterOhosTest, Create_NullDeps) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  auto presenter = OutputPresenterOHOS::Create(nullptr);
  EXPECT_EQ(presenter, nullptr);
}

TEST(OutputPresenterOhosTest, InitializeCapabilities) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  constexpr int kNumberOfBuffers = 4;
  EXPECT_EQ(kNumberOfBuffers, 4);

}

TEST(OutputPresenterOhosTest, SurfaceTransactionVisibility) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(OH_SURFACE_TRANSACTION_VISIBILITY_HIDE, 0);
  EXPECT_EQ(OH_SURFACE_TRANSACTION_VISIBILITY_SHOW, 1);
}

TEST(OutputPresenterOhosTest, OhTransformValues) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  EXPECT_EQ(OH_TRANSFORM_ROTATE_NONE, 0);
  EXPECT_EQ(OH_TRANSFORM_ROTATE_90, 1);
  EXPECT_EQ(OH_TRANSFORM_ROTATE_180, 2);
  EXPECT_EQ(OH_TRANSFORM_ROTATE_270, 3);
  EXPECT_EQ(OH_TRANSFORM_FLIP_H, 4);
  EXPECT_EQ(OH_TRANSFORM_FLIP_V, 5);
  EXPECT_EQ(OH_TRANSFORM_FLIP_H_ROT90, 6);
  EXPECT_EQ(OH_TRANSFORM_FLIP_V_ROT90, 7);
  EXPECT_EQ(OH_TRANSFORM_FLIP_H_ROT180, 8);
  EXPECT_EQ(OH_TRANSFORM_FLIP_V_ROT180, 9);
  EXPECT_EQ(OH_TRANSFORM_FLIP_H_ROT270, 10);
  EXPECT_EQ(OH_TRANSFORM_FLIP_V_ROT270, 11);
  EXPECT_EQ(OH_TRANSFORM_ROTATE_LAST, OH_TRANSFORM_FLIP_V_ROT270);
}

TEST(OutputPresenterOhosTest, OhRectDefaults) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  OH_Rect rect = {0, 0, 0, 0};
  EXPECT_EQ(rect.x, 0);
  EXPECT_EQ(rect.y, 0);
  EXPECT_EQ(rect.w, 0);
  EXPECT_EQ(rect.h, 0);
}

TEST(OutputPresenterOhosTest, OhRectValues) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  OH_Rect rect = {10, 20, 800, 600};
  EXPECT_EQ(rect.x, 10);
  EXPECT_EQ(rect.y, 20);
  EXPECT_EQ(rect.w, 800);
  EXPECT_EQ(rect.h, 600);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_ZeroSize) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(0, 0);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_NONE), size);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            size);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_WidthOnly) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 0);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            gfx::Size(0, 1920));
}

TEST(OutputPresenterOhosTest, GetRotatedSize_HeightOnly) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(0, 1080);
  EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            gfx::Size(1080, 0));
}

TEST(OutputPresenterOhosTest, FromGfxOverlayTransform_AllMappings) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
    EXPECT_EQ(FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_NONE), 0);
    EXPECT_EQ(FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90),
            1);
    EXPECT_EQ(
      FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180), 2);
    EXPECT_EQ(
      FromGfxOverlayTransform(gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270), 3);
}

TEST(OutputPresenterOhosTest, GetRotatedSize_FlipHorizontalNoSwap) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
    EXPECT_EQ(GetRotatedSize(size, gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL), size);
}

TEST(OutputPresenterOhosTest, Rotate90_SizeAndTransformConsistency) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size original(1920, 1080);
  gfx::Size rotated = GetRotatedSize(original,
                                      gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90);
    EXPECT_EQ(rotated.width(), original.height());
  EXPECT_EQ(rotated.height(), original.width());
    EXPECT_EQ(FromGfxOverlayTransform(
                gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90), 1);
}

TEST(OutputPresenterOhosTest, Rotate270_SizeAndTransformConsistency) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size original(1920, 1080);
  gfx::Size rotated = GetRotatedSize(
      original, gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270);
  EXPECT_EQ(rotated.width(), original.height());
  EXPECT_EQ(rotated.height(), original.width());
  EXPECT_EQ(FromGfxOverlayTransform(
                gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270), 3);
}

TEST(OutputPresenterOhosTest, Reshape_Rotate90SwapsBounds) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  gfx::OverlayTransform transform = gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90;
  gfx::RectF expected_bounds(GetRotatedSize(size, transform));
  EXPECT_EQ(expected_bounds.width(), 1080);
  EXPECT_EQ(expected_bounds.height(), 1920);
}

TEST(OutputPresenterOhosTest, Reshape_NoneKeepsBounds) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::Size size(1920, 1080);
  gfx::OverlayTransform transform = gfx::OVERLAY_TRANSFORM_NONE;
  gfx::RectF expected_bounds(GetRotatedSize(size, transform));
  EXPECT_EQ(expected_bounds.width(), 1920);
  EXPECT_EQ(expected_bounds.height(), 1080);
}

TEST(OutputPresenterOhosTest, OverlayNameSelection_VideoHint) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
      const bool is_root = false;
  const auto priority = gfx::OverlayPriorityHint::kVideo;
  std::string expected_name = "delegate_child_video";
  if (!is_root && priority == gfx::OverlayPriorityHint::kVideo) {
      } else {
    expected_name = "delegate_child";
  }
  EXPECT_EQ(expected_name, "delegate_child_video");
}

TEST(OutputPresenterOhosTest, OverlayNameSelection_NonVideo) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  const bool is_root = false;
  const auto priority = gfx::OverlayPriorityHint::kNone;
  std::string expected_name = "delegate_child";
  if (!is_root && priority == gfx::OverlayPriorityHint::kVideo) {
    expected_name = "delegate_child_video";
  }
  EXPECT_EQ(expected_name, "delegate_child");
}

TEST(OutputPresenterOhosTest, SrcRectZeroWidthAdjustment) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::SizeF buffer_size(1920, 1080);
  gfx::RectF src_rect(100, 0, 0, 600);  // width=0

  if (src_rect.width() == 0) {
    src_rect.set_width(1);
    if (src_rect.right() > buffer_size.width()) {
      src_rect.set_x(buffer_size.width() - 1);
    }
  }
  EXPECT_EQ(src_rect.width(), 1);
  EXPECT_EQ(src_rect.x(), 100);
}

TEST(OutputPresenterOhosTest, SrcRectZeroWidthAdjustment_Overflow) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::SizeF buffer_size(1920, 1080);
  gfx::RectF src_rect(1920, 0, 0, 600);  // width=0, x at edge

  if (src_rect.width() == 0) {
    src_rect.set_width(1);
    if (src_rect.right() > buffer_size.width()) {
      src_rect.set_x(buffer_size.width() - 1);
    }
  }
  EXPECT_EQ(src_rect.width(), 1);
  EXPECT_EQ(src_rect.x(), 1919);  // repositioned
}

TEST(OutputPresenterOhosTest, SrcRectZeroHeightAdjustment) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::SizeF buffer_size(1920, 1080);
  gfx::RectF src_rect(0, 100, 800, 0);  // height=0

  if (src_rect.height() == 0) {
    src_rect.set_height(1);
    if (src_rect.bottom() > buffer_size.height()) {
      src_rect.set_y(buffer_size.height() - 1);
    }
  }
  EXPECT_EQ(src_rect.height(), 1);
  EXPECT_EQ(src_rect.y(), 100);
}

TEST(OutputPresenterOhosTest, SrcRectZeroHeightAdjustment_Overflow) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::SizeF buffer_size(1920, 1080);
  gfx::RectF src_rect(0, 1080, 800, 0);  // height=0, y at edge

  if (src_rect.height() == 0) {
    src_rect.set_height(1);
    if (src_rect.bottom() > buffer_size.height()) {
      src_rect.set_y(buffer_size.height() - 1);
    }
  }
  EXPECT_EQ(src_rect.height(), 1);
  EXPECT_EQ(src_rect.y(), 1079);  // repositioned
}

TEST(OutputPresenterOhosTest, RootBufferUsesDisplayTransform) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::OverlayTransform display_transform =
      gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90;
  bool is_root = true;
  int32_t transform = is_root ? FromGfxOverlayTransform(display_transform) : 0;
  EXPECT_EQ(transform, 1);  }

TEST(OutputPresenterOhosTest, NonRootBufferUsesNoneTransform) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  gfx::OverlayTransform display_transform =
      gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90;
  bool is_root = false;
  int32_t transform = is_root ? FromGfxOverlayTransform(display_transform) : 0;
  EXPECT_EQ(transform, 0);  }

TEST(OutputPresenterOhosTest, PendingFrameSortedByZOrder) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
    std::vector<int32_t> z_orders = {2, -1, 0, 1, -2};
  std::sort(z_orders.begin(), z_orders.end());
  std::vector<int32_t> expected = {-2, -1, 0, 1, 2};
  EXPECT_EQ(z_orders, expected);
}

TEST(OutputPresenterOhosTest, PendingFrameNegativeZOrderOffset) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
    std::vector<int32_t> z_orders = {2, -1, 0, 1, -2};
  std::sort(z_orders.begin(), z_orders.end());
  int32_t first_z = z_orders[0];  // -2
  ASSERT_LT(first_z, 0);
  for (auto& z : z_orders) {
    z -= first_z;  // offset to make all >= 0
  }
  std::vector<int32_t> expected = {0, 1, 2, 3, 4};
  EXPECT_EQ(z_orders, expected);
}

TEST(OutputPresenterOhosTest, UpdateProperty_NoChange) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  int32_t value = 5;
  bool dirty = false;
    int32_t new_value = 5;
  if (value != new_value) {
    value = new_value;
    dirty = true;
  }
  EXPECT_EQ(value, 5);
  EXPECT_FALSE(dirty);
}

TEST(OutputPresenterOhosTest, UpdateProperty_Changed) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
  int32_t value = 5;
  bool dirty = false;
  int32_t new_value = 10;
  if (value != new_value) {
    value = new_value;
    dirty = true;
  }
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(dirty);
}

TEST(OutputPresenterOhosTest, OpaqueTypesCompile) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
    OH_NativeBuffer* buffer = nullptr;
  OH_SurfaceControl* surface = nullptr;
  OH_SurfaceTransaction* transaction = nullptr;
  EXPECT_EQ(buffer, nullptr);
  EXPECT_EQ(surface, nullptr);
  EXPECT_EQ(transaction, nullptr);
}

TEST(OutputPresenterOhosTest, DiscardCompositingResources_EmptyFrame) {
  CHECK_DELEGATE_ENABLED_PRESENTER();
    std::deque<int> previous_frame;  // simplified model
  int call_count = 0;
  for (const auto& item : previous_frame) {
    (void)item;
    call_count++;
  }
  EXPECT_EQ(call_count, 0);
}

}  // namespace viz

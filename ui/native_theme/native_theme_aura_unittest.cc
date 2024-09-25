// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(OHOS_UNITTESTS)
#define protected public
#include "ui/native_theme/native_theme_aura.h"
#undef protected

#define private public
#include "third_party/skia/include/core/SkRRect.h"
#undef private

#include "cc/paint/paint_canvas.h"
#include "cc/paint/skottie_wrapper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/skia/include/core/SkTextBlob.h"
#include "ui/color/color_provider.h"
#include "ui/gfx/geometry/rrect_f.h"
#else  // OHOS_UNITTESTS
#include "ui/native_theme/native_theme_aura.h"
#endif  // OHOS_UNITTESTS

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkPath.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/native_theme/native_theme.h"

namespace ui {
namespace {

void VerifyPoint(SkPoint a, SkPoint b) {
  EXPECT_EQ(a.x(), b.x());
  EXPECT_EQ(a.y(), b.y());
}

void VerifyTriangle(SkPath actualPath, SkPoint p0, SkPoint p1, SkPoint p2) {
  EXPECT_EQ(3, actualPath.countPoints());
  VerifyPoint(p0, actualPath.getPoint(0));
  VerifyPoint(p1, actualPath.getPoint(1));
  VerifyPoint(p2, actualPath.getPoint(2));
}

}  // namespace

class NativeThemeAuraTest : public testing::Test {
 protected:
  NativeThemeAuraTest() = default;

  SkPath PathForArrow(const gfx::Rect& rect,
                      NativeTheme::Part direction) const {
    return theme_.PathForArrow(BoundingRectForArrow(rect), direction);
  }

  gfx::Rect BoundingRectForArrow(const gfx::Rect& rect) const {
    return theme_.BoundingRectForArrow(rect);
  }

 private:
  NativeThemeAura theme_{false, false};
};

TEST_F(NativeThemeAuraTest, VerticalArrows) {
  SkPath path;

  // Up arrow, sized for 1x.
  path =
      PathForArrow(gfx::Rect(100, 200, 17, 17), NativeTheme::kScrollbarUpArrow);
  VerifyTriangle(path, SkPoint::Make(105, 211), SkPoint::Make(112, 211),
                 SkPoint::Make(108.5, 207));

  // 1.25x, should be larger.
  path =
      PathForArrow(gfx::Rect(50, 70, 21, 21), NativeTheme::kScrollbarUpArrow);
  VerifyTriangle(path, SkPoint::Make(56, 84), SkPoint::Make(65, 84),
                 SkPoint::Make(60.5, 79));

  // Down arrow is just a flipped up arrow.
  path =
      PathForArrow(gfx::Rect(20, 80, 17, 17), NativeTheme::kScrollbarDownArrow);
  VerifyTriangle(path, SkPoint::Make(25, 86), SkPoint::Make(32, 86),
                 SkPoint::Make(28.5, 90));
}

TEST_F(NativeThemeAuraTest, HorizontalArrows) {
  SkPath path;

  // Right arrow, sized for 1x.
  path = PathForArrow(gfx::Rect(100, 200, 17, 17),
                      NativeTheme::kScrollbarRightArrow);
  VerifyTriangle(path, SkPoint::Make(107, 205), SkPoint::Make(107, 212),
                 SkPoint::Make(111, 208.5));

  // Button size for 1.25x, should be larger.
  path = PathForArrow(gfx::Rect(50, 70, 21, 21),
                      NativeTheme::kScrollbarRightArrow);
  VerifyTriangle(path, SkPoint::Make(58, 76), SkPoint::Make(58, 85),
                 SkPoint::Make(63, 80.5));

  // Left arrow is just a flipped right arrow.
  path =
      PathForArrow(gfx::Rect(20, 80, 17, 17), NativeTheme::kScrollbarLeftArrow);
  VerifyTriangle(path, SkPoint::Make(30, 85), SkPoint::Make(30, 92),
                 SkPoint::Make(26, 88.5));
}

TEST_F(NativeThemeAuraTest, ArrowForNonSquareButton) {
  SkPath path =
      PathForArrow(gfx::Rect(90, 80, 42, 37), NativeTheme::kScrollbarLeftArrow);
  VerifyTriangle(path, SkPoint::Make(116, 89), SkPoint::Make(116, 109),
                 SkPoint::Make(105, 99));
}

TEST_F(NativeThemeAuraTest, BoundingRectSquare) {
  gfx::Rect bounding_rect = BoundingRectForArrow(gfx::Rect(42, 61, 21, 21));
  EXPECT_EQ(48.f, bounding_rect.x());
  EXPECT_EQ(67.f, bounding_rect.y());
  EXPECT_EQ(9.f, bounding_rect.width());
  EXPECT_EQ(bounding_rect.width(), bounding_rect.height());
}

TEST_F(NativeThemeAuraTest, BoundingRectSlightlyRectangular) {
  // Stretched horzontally.
  gfx::Rect bounding_rect = BoundingRectForArrow(gfx::Rect(42, 61, 25, 20));
  EXPECT_EQ(49.f, bounding_rect.x());
  EXPECT_EQ(66.f, bounding_rect.y());
  EXPECT_EQ(11.f, bounding_rect.width());
  EXPECT_EQ(bounding_rect.width(), bounding_rect.height());

  // Stretched vertically.
  bounding_rect = BoundingRectForArrow(gfx::Rect(42, 61, 14, 10));
  EXPECT_EQ(46.f, bounding_rect.x());
  EXPECT_EQ(63.f, bounding_rect.y());
  EXPECT_EQ(6.f, bounding_rect.width());
  EXPECT_EQ(bounding_rect.width(), bounding_rect.height());
}

TEST_F(NativeThemeAuraTest, BoundingRectVeryRectangular) {
  // Stretched horzontally.
  gfx::Rect bounding_rect = BoundingRectForArrow(gfx::Rect(42, 61, 30, 8));
  EXPECT_EQ(53.f, bounding_rect.x());
  EXPECT_EQ(61.f, bounding_rect.y());
  EXPECT_EQ(8.f, bounding_rect.width());
  EXPECT_EQ(bounding_rect.width(), bounding_rect.height());

  // Stretched vertically.
  bounding_rect = BoundingRectForArrow(gfx::Rect(42, 61, 6, 44));
  EXPECT_EQ(42.f, bounding_rect.x());
  EXPECT_EQ(80.f, bounding_rect.y());
  EXPECT_EQ(6.f, bounding_rect.width());
  EXPECT_EQ(bounding_rect.width(), bounding_rect.height());
}

TEST_F(NativeThemeAuraTest, BoundingRectSnappedToWholePixels) {
  gfx::Rect bounding_rect = BoundingRectForArrow(gfx::Rect(0, 0, 9, 10));
  EXPECT_EQ(3.f, bounding_rect.x());

  bounding_rect = BoundingRectForArrow(gfx::Rect(0, 0, 10, 9));
  EXPECT_EQ(3.f, bounding_rect.y());
}

#if defined(OHOS_UNITTESTS)
class MockPaintCanvas : public cc::PaintCanvas {
 public:
  MOCK_METHOD(SkImageInfo, imageInfo, (), (const, override));
  MOCK_METHOD(void*,
              accessTopLayerPixels,
              (SkImageInfo*, size_t*, SkIPoint*),
              (override));
  MOCK_METHOD(void, flush, (), (override));
  MOCK_METHOD(int, save, (), (override));
  MOCK_METHOD(int, saveLayer, (const cc::PaintFlags&), (override));
  MOCK_METHOD(int,
              saveLayer,
              (const SkRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(int, saveLayerAlphaf, (float), (override));
  MOCK_METHOD(int, saveLayerAlphaf, (const SkRect&, float), (override));
  MOCK_METHOD(void, restore, (), (override));
  MOCK_METHOD(int, getSaveCount, (), (const, override));
  MOCK_METHOD(void, restoreToCount, (int), (override));
  MOCK_METHOD(void, translate, (SkScalar, SkScalar), (override));
  MOCK_METHOD(void, scale, (SkScalar, SkScalar), (override));
  MOCK_METHOD(void, rotate, (SkScalar), (override));
  MOCK_METHOD(void, concat, (const SkM44&), (override));
  MOCK_METHOD(void, setMatrix, (const SkM44&), (override));
  MOCK_METHOD(void, clipRect, (const SkRect&, SkClipOp, bool), (override));
  MOCK_METHOD(void, clipRRect, (const SkRRect&, SkClipOp, bool), (override));
  MOCK_METHOD(void,
              clipPath,
              (const SkPath&, SkClipOp, bool, cc::UsePaintCache),
              (override));
  MOCK_METHOD(bool, getLocalClipBounds, (SkRect*), (const, override));
  MOCK_METHOD(bool, getDeviceClipBounds, (SkIRect*), (const, override));
  MOCK_METHOD(void, drawColor, (SkColor4f, SkBlendMode), (override));
  MOCK_METHOD(void, clear, (SkColor4f), (override));
  MOCK_METHOD(void,
              drawLine,
              (SkScalar, SkScalar, SkScalar, SkScalar, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawRect,
              (const SkRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawIRect,
              (const SkIRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawOval,
              (const SkRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawRRect,
              (const SkRRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawDRRect,
              (const SkRRect&, const SkRRect&, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawRoundRect,
              (const SkRect&, SkScalar, SkScalar, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawPath,
              (const SkPath&, const cc::PaintFlags&, cc::UsePaintCache),
              (override));
  MOCK_METHOD(void,
              drawImage,
              (const cc::PaintImage&,
               SkScalar,
               SkScalar,
               const SkSamplingOptions&,
               const cc::PaintFlags*),
              (override));
  MOCK_METHOD(void,
              drawImageRect,
              (const cc::PaintImage&,
               const SkRect&,
               const SkRect&,
               const SkSamplingOptions&,
               const cc::PaintFlags*,
               SkCanvas::SrcRectConstraint),
              (override));

  MOCK_METHOD(void,
              drawSkottie,
              (scoped_refptr<cc::SkottieWrapper>,
               const SkRect&,
               float,
               cc::SkottieFrameDataMap,
               const cc::SkottieColorMap&,
               cc::SkottieTextPropertyValueMap),
              (override));

  MOCK_METHOD(void,
              drawTextBlob,
              (sk_sp<SkTextBlob>, SkScalar, SkScalar, const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void,
              drawTextBlob,
              (sk_sp<SkTextBlob>,
               SkScalar,
               SkScalar,
               cc::NodeId,
               const cc::PaintFlags&),
              (override));
  MOCK_METHOD(void, drawPicture, (cc::PaintRecord), (override));
  MOCK_METHOD(SkM44, getLocalToDevice, (), (const, override));
  MOCK_METHOD(bool, NeedsFlush, (), (const, override));
  MOCK_METHOD(void,
              Annotate,
              (AnnotationType, const SkRect&, sk_sp<SkData>),
              (override));
  MOCK_METHOD(void, recordCustomData, (uint32_t), (override));
  MOCK_METHOD(void, setNodeId, (int), (override));
};

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb001) {
  NativeThemeAura native_theme(true, false);
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarHorizontalThumb;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(10, 10);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect aroundRRect = gfx::Rect(0, 24, 10, 8);
            gfx::RRectF rounded_rect(gfx::RectF(aroundRRect), 6.0f, 6.0f, 6.0f,
                                     6.0f, 6.0f, 6.0f, 6.0f, 6.0f);
            ASSERT_EQ(rrect, static_cast<SkRRect>(rounded_rect));
          }));
  EXPECT_CALL(canvas, drawIRect(testing::_, testing::_))
      .WillOnce(
          testing::Invoke([](const SkIRect& rect, const cc::PaintFlags& flags) {
            gfx::Rect rect_(10, 10);
            ASSERT_EQ(rect, gfx::RectToSkIRect(rect_));
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb002) {
  NativeThemeAura native_theme(true, false);
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarDownArrow;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(10, 10);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect aroundRRect = gfx::Rect(24, 0, 8, 10);
            gfx::RRectF rounded_rect(gfx::RectF(aroundRRect), 6.0f, 6.0f, 6.0f,
                                     6.0f, 6.0f, 6.0f, 6.0f, 6.0f);
            ASSERT_EQ(rrect, static_cast<SkRRect>(rounded_rect));
          }));
  EXPECT_CALL(canvas, drawIRect(testing::_, testing::_))
      .WillOnce(
          testing::Invoke([](const SkIRect& rect, const cc::PaintFlags& flags) {
            gfx::Rect rect_(10, 10);
            ASSERT_EQ(rect, gfx::RectToSkIRect(rect_));
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb003) {
  NativeThemeAura native_theme(false, false);
  NativeThemeAura::State state = NativeThemeAura::State::kHovered;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarVerticalThumb;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(20, 10);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect thumb_rect(8, 0, 8, 10);
            SkRRect r_rect =
                SkRRect::MakeRectXY(gfx::RectToSkRect(thumb_rect), 4.0f, 4.0f);
            ASSERT_EQ(rrect.fType, r_rect.fType);
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb004) {
  NativeThemeAura native_theme(false, false);
  NativeThemeAura::State state = NativeThemeAura::State::kPressed;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarDownArrow;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(10, 20);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect thumb_rect(0, 8, 10, 8);
            SkRRect r_rect =
                SkRRect::MakeRectXY(gfx::RectToSkRect(thumb_rect), 4.0f, 4.0f);
            ASSERT_EQ(rrect.fType, r_rect.fType);
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb005) {
  NativeThemeAura native_theme(false, false);
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarVerticalThumb;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(10, 20);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect thumb_rect(2, 0, 4, 20);
            SkRRect r_rect =
                SkRRect::MakeRectXY(gfx::RectToSkRect(thumb_rect), 2.0f, 2.0f);
            ASSERT_EQ(rrect.fType, r_rect.fType);
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, PaintScrollbarThumb006) {
  NativeThemeAura native_theme(false, false);
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarDownArrow;
  MockPaintCanvas canvas;
  ColorProvider color_provider;
  gfx::Rect rect(10, 20);
  NativeThemeAura::ScrollbarOverlayColorTheme theme =
      NativeThemeAura::ScrollbarOverlayColorTheme::kDefault;
  NativeThemeAura::ColorScheme color_scheme =
      NativeThemeAura::ColorScheme::kDefault;
  SkColor scrollbar_color = 1024;
  EXPECT_CALL(canvas, drawRRect(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](const SkRRect& rrect, const cc::PaintFlags& flags) {
            gfx::Rect thumb_rect(0, 12, 10, 4);
            SkRRect r_rect =
                SkRRect::MakeRectXY(gfx::RectToSkRect(thumb_rect), 2.0f, 2.0f);
            ASSERT_EQ(rrect.fType, r_rect.fType);
          }));
  native_theme.PaintScrollbarThumb(&canvas, &color_provider, part, state, rect,
                                   theme, color_scheme, scrollbar_color);
}

TEST_F(NativeThemeAuraTest, GetPartSize001) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarDownArrow;
  NativeThemeAura::ExtraParams extra;
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  gfx::Size result = native_theme.GetPartSize(part, state, extra);
  EXPECT_EQ(result.width(), 15);
  EXPECT_EQ(result.height(), 0);
}

TEST_F(NativeThemeAuraTest, GetPartSize002) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarUpArrow;
  NativeThemeAura::ExtraParams extra;
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  gfx::Size result = native_theme.GetPartSize(part, state, extra);
  EXPECT_EQ(result.width(), 15);
  EXPECT_EQ(result.height(), 0);
}

TEST_F(NativeThemeAuraTest, GetPartSize003) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarLeftArrow;
  NativeThemeAura::ExtraParams extra;
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  gfx::Size result = native_theme.GetPartSize(part, state, extra);
  EXPECT_EQ(result.width(), 0);
  EXPECT_EQ(result.height(), 15);
}

TEST_F(NativeThemeAuraTest, GetPartSize004) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarRightArrow;
  NativeThemeAura::ExtraParams extra;
  NativeThemeAura::State state = NativeThemeAura::State::kNormal;
  gfx::Size result = native_theme.GetPartSize(part, state, extra);
  EXPECT_EQ(result.width(), 0);
  EXPECT_EQ(result.height(), 15);
}

TEST_F(NativeThemeAuraTest, GetNinePatchAperture001) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarHorizontalThumb;
  gfx::Rect result = native_theme.GetNinePatchAperture(part);
  EXPECT_EQ(result.x(), 8);
  EXPECT_EQ(result.y(), 0);
  EXPECT_EQ(result.width(), 20);
  EXPECT_EQ(result.height(), 36);
}

TEST_F(NativeThemeAuraTest, GetNinePatchAperture002) {
  NativeThemeAura native_theme{false, false};
  NativeThemeAura::Part part = NativeThemeAura::kScrollbarDownArrow;
  gfx::Rect result = native_theme.GetNinePatchAperture(part);
  EXPECT_EQ(result.x(), 0);
  EXPECT_EQ(result.y(), 8);
  EXPECT_EQ(result.width(), 36);
  EXPECT_EQ(result.height(), 20);
}
#endif  // OHOS_UNITTESTS
}  // namespace ui

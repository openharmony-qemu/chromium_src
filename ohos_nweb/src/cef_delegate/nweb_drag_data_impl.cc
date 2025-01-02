/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "nweb_drag_data_impl.h"

#include <cmath>
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "content/public/common/drop_data.h"
#include "ohos_nweb/include/nweb.h"

#include "cef/libcef/browser/image_impl.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/pathops/SkPathOps.h"
#include "third_party/skia/include/effects/SkImageFilters.h"
#include "ui/gfx/image/image_skia.h"

using namespace OHOS::NWeb;

namespace {
  constexpr int DEAFULT_ROUND_RECT_RATIO = 12;
  constexpr int IMAGE_ROUND_RECT_RATIO = 8;
  constexpr int ADD_BOUND_RECT_RATIO = 12;
  constexpr int TEXT_PADDING = 8;
  constexpr int DOUBLE_RATIO = 2;
  constexpr int DEFAULT_MARGIN = 5;
  constexpr int IMAGE_MIN_WIDTH = 64;
  constexpr int DEFAULT_COLOR_BG = 0xF2FFFFFF;
  constexpr int DEFAULT_COLOR_BG_DARK_MODE = 0xF2000000;
  constexpr int SHADOW_RADIUS = 5;
  constexpr int EXPAND_PADDING = 9;
  constexpr int SHADOW_COLOR = 0x33000000;
  constexpr int SHADOW_DX = 0;
  constexpr int SHADOW_DY = 4;
  constexpr int WEIRD_PADDING = 30;
  constexpr int IAMGE_SHADOW_RADIUS = 6;
  constexpr int IMAGE_EXPAND_PADDING = 11;
  constexpr int IMAGE_SHADOW_COLOR = 0xA0000000;
  constexpr int IMAGE_SHADOW_DY = 3;
  constexpr int DEFAULT_MIN_HEIGHT_THRESHOLD = 2;
}

namespace OHOS::NWeb {
float NWebDragDataImpl::ToOhCoordinate(int origin) {
  if (device_pixel_ratio_ > 0) {
    return origin * device_pixel_ratio_;
  }

  return 1;
}

SkPath NWebDragDataImpl::ClipLeftTopCorner(SkPath& origin_path, int pos_x, int pos_y, int radius) {
  SkPath rect_path;
  SkRect left_top_rect = SkRect::MakeXYWH(pos_x - radius, pos_y - radius, radius, radius);
  rect_path.addRect(left_top_rect);
  SkPath circle_path;
  circle_path.addCircle(pos_x, pos_y, radius);
  SkPath clipped_path;
  if (!Op(rect_path, circle_path, SkPathOp::kDifference_SkPathOp, &clipped_path)) {
    LOG(ERROR) << "clip left top path failed";
    return origin_path;
  }

  SkPath result_path = origin_path;
  if (!Op(origin_path, clipped_path, SkPathOp::kDifference_SkPathOp, &result_path)) {
    LOG(ERROR) << "clip left top path failed";
  }
  LOG(INFO) << "clip left top path sucess";
  return result_path;
}

SkPath NWebDragDataImpl::ClipRightBottomCorner(SkPath& origin_path, int pos_x, int pos_y, int radius) {
  SkPath rect_path;
  SkRect right_bottom_rect = SkRect::MakeXYWH(pos_x, pos_y, radius, radius);
  rect_path.addRect(right_bottom_rect);
  SkPath circle_path;
  circle_path.addCircle(pos_x, pos_y, radius);
  SkPath clipped_path;
  if (!Op(rect_path, circle_path, SkPathOp::kDifference_SkPathOp, &clipped_path)) {
    LOG(ERROR) << "clip RightBottom path failed";
    return origin_path;
  }

  SkPath result_path = origin_path;
  if (!Op(origin_path, clipped_path, SkPathOp::kDifference_SkPathOp, &result_path)) {
    LOG(ERROR) << "clip RightBottom path failed";
  }
  LOG(INFO) << "clip RightBottom path sucess";
  return result_path;
}

void NWebDragDataImpl::AddRightBottomCorner(SkPath& origin_path, int pos_x, int pos_y, int radius) {
  SkPath rect_path;
  SkRect right_bottom_rect = SkRect::MakeXYWH(pos_x, pos_y, radius, radius);
  rect_path.addRect(right_bottom_rect);
  SkPath circle_path;
  circle_path.addCircle(pos_x, pos_y, radius);
  SkPath clipped_path;
  if (!Op(rect_path, circle_path, SkPathOp::kDifference_SkPathOp, &clipped_path)) {
    LOG(ERROR) << "AddRightBottomCorner failed";
    return;
  }

  LOG(INFO) << "AddRightBottomCorner success";
  origin_path.addPath(clipped_path);
}

void NWebDragDataImpl::AddLeftTopCorner(SkPath& origin_path, int pos_x, int pos_y, int radius) {
  SkPath rect_path;
  SkRect right_bottom_rect = SkRect::MakeXYWH(pos_x - radius, pos_y - radius, radius, radius);
  rect_path.addRect(right_bottom_rect);
  SkPath circle_path;
  circle_path.addCircle(pos_x, pos_y, radius);
  SkPath clipped_path;
  if (!Op(rect_path, circle_path, SkPathOp::kDifference_SkPathOp, &clipped_path)) {
    LOG(ERROR) << "AddLeftTopCorner failed";
    return;
  }

  LOG(INFO) << "AddLeftTopCorner success";
  origin_path.addPath(clipped_path);
}

SkPath NWebDragDataImpl::GetClippedPath(bool is_start_line_compelete, bool is_end_line_compelete) {
  SkPath clipped_path;
  if (!is_start_line_compelete) {
    LOG(INFO) << "GetClippedPath start line is not compelete";
    SkRect left_top_rect = SkRect::MakeXYWH(0, 0, start_edge_top_.x - drag_image_origin_point_.x, start_edge_bottom_.y - start_edge_top_.y);
    clipped_path.addRect(left_top_rect);
  }

  if (!is_end_line_compelete) {
    LOG(INFO) << "GetClippedPath end line is not compelete";
    SkRect right_bottom_rect = SkRect::MakeXYWH(
      end_edge_top_.x + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO,
      drag_image_origin_point_.y + drag_clip_height_ + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - (end_edge_bottom_.y - end_edge_top_.y),
      drag_image_origin_point_.x + drag_clip_width_ - end_edge_top_.x,
      end_edge_bottom_.y - end_edge_top_.y);
    right_bottom_rect.offset(-drag_image_origin_point_.x, -drag_image_origin_point_.y);
    clipped_path.addRect(right_bottom_rect);
  }
  LOG(INFO) << "GetClippedPath return";
  return clipped_path;
}

SkPath NWebDragDataImpl::GetShadowPath(int width, int height) {
  SkPath out_path;
  SkRect out_rect = SkRect::MakeXYWH(0, 0, width, height);
  out_path.addRoundRect(out_rect, ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO), ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
  if (!is_useful_selection_) {
    // todo: need to process multi-row when width less min width
    LOG(INFO) << "unuseful selection info, return compelete round rect";
    return out_path;
  }

  auto ohMinHeightThreshold = ToOhCoordinate(DEFAULT_MIN_HEIGHT_THRESHOLD);
  bool is_oneline = ((start_edge_top_.y == end_edge_top_.y) && (start_edge_bottom_.y == end_edge_bottom_.y)) ||
    (std::abs(start_edge_bottom_.y - start_edge_top_.y - drag_clip_height_) < ohMinHeightThreshold &&
    drag_clip_height_ > ohMinHeightThreshold);

  bool is_both_out_clip_region = (start_edge_top_.y < 0) &&
    (std::abs(end_edge_bottom_.y - (drag_image_origin_point_.y + drag_clip_height_)) > ToOhCoordinate(WEIRD_PADDING));
  bool is_start_line_compelete = (start_edge_top_.y >= 0) &&
    (std::abs(start_edge_top_.x - drag_image_origin_point_.x) < ToOhCoordinate(DEFAULT_MARGIN + ADD_BOUND_RECT_RATIO));
  is_start_line_compelete = is_start_line_compelete || (start_edge_top_.y < 0);
  is_start_line_compelete = is_start_line_compelete ||
    (std::abs(start_edge_top_.y - drag_image_origin_point_.y) > ToOhCoordinate(WEIRD_PADDING));

  bool is_end_line_compelete = (std::abs(end_edge_bottom_.y - (drag_image_origin_point_.y + drag_clip_height_)) <= ToOhCoordinate(WEIRD_PADDING)) &&
    (std::abs(drag_image_origin_point_.x + drag_clip_width_ - end_edge_bottom_.x) < ToOhCoordinate(DEFAULT_MARGIN + ADD_BOUND_RECT_RATIO));
  is_end_line_compelete = is_end_line_compelete ||
    (std::abs(end_edge_bottom_.x - drag_image_origin_point_.x) < ToOhCoordinate(DEFAULT_MARGIN));
  is_end_line_compelete = is_end_line_compelete ||
    (std::abs(end_edge_bottom_.y - (drag_image_origin_point_.y + drag_clip_height_)) > ToOhCoordinate(WEIRD_PADDING));
  LOG(INFO) << "is one line = " << is_oneline << ", is_both_out_clip_region = " \
    << is_both_out_clip_region << ", is_start_line_compelete = " \
    << is_start_line_compelete << ", is_end_line_compelete = " << is_end_line_compelete;

  if (is_oneline || is_both_out_clip_region || (is_start_line_compelete && is_end_line_compelete)) {
    LOG(INFO) << "return compelete round rect";
    return out_path;
  } else {
    SkPath clipped_path;
    if (!Op(out_path, GetClippedPath(is_start_line_compelete, is_end_line_compelete), SkPathOp::kDifference_SkPathOp, &clipped_path)) {
      LOG(ERROR) << "get clipped path failed";
      clipped_path.addRoundRect(out_rect, ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO), ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
      return clipped_path;
    }
    SkPath final_path = clipped_path;
    if (!is_start_line_compelete) {
      final_path = ClipLeftTopCorner(final_path,
        start_edge_top_.x + ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO) - drag_image_origin_point_.x,
        ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
        ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
      final_path = ClipLeftTopCorner(final_path,
      ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
      start_edge_bottom_.y - start_edge_top_.y + ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
      ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
      AddRightBottomCorner(final_path,
      start_edge_top_.x - drag_image_origin_point_.x - ToOhCoordinate(ADD_BOUND_RECT_RATIO),
      start_edge_bottom_.y - start_edge_top_.y - ToOhCoordinate(ADD_BOUND_RECT_RATIO),
      ToOhCoordinate(ADD_BOUND_RECT_RATIO));
    }
    if (!is_end_line_compelete) {
      final_path = ClipRightBottomCorner(final_path,
        drag_clip_width_ + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
        drag_clip_height_ + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - (end_edge_bottom_.y - end_edge_top_.y) - ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
        ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
      final_path = ClipRightBottomCorner(final_path,
        end_edge_top_.x + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO) - drag_image_origin_point_.x,
        drag_clip_height_ + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO),
        ToOhCoordinate(DEAFULT_ROUND_RECT_RATIO));
      AddLeftTopCorner(final_path,
        end_edge_top_.x + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO + ToOhCoordinate(ADD_BOUND_RECT_RATIO) - drag_image_origin_point_.x,
        drag_clip_height_ + ToOhCoordinate(TEXT_PADDING)  * DOUBLE_RATIO - (end_edge_bottom_.y - end_edge_top_.y) + ToOhCoordinate(ADD_BOUND_RECT_RATIO),
        ToOhCoordinate(ADD_BOUND_RECT_RATIO));
    }
    return final_path;
  }
}

void NWebDragDataImpl::GenerateOhosDragBitmapFromOriginForImage(const SkBitmap& in_bitmap, SkBitmap& out_bitmap,
                                                                int& width, int& height) {
  LOG(INFO) << "process single picture drag";
  width = drag_clip_width_;
  height = drag_clip_height_;
  SkPath shadow_path;
  SkRect out_rect = SkRect::MakeXYWH(0, 0, width, height);

  width += ToOhCoordinate(IMAGE_EXPAND_PADDING) * DOUBLE_RATIO;
  height += ToOhCoordinate(IMAGE_EXPAND_PADDING) * DOUBLE_RATIO;
  shadow_path.addRoundRect(out_rect, ToOhCoordinate(IMAGE_ROUND_RECT_RATIO), ToOhCoordinate(IMAGE_ROUND_RECT_RATIO));
  auto imageInfo = SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType , SkAlphaType::kUnpremul_SkAlphaType);
  out_bitmap.allocPixels(imageInfo);
  SkCanvas bitmapCanvas(out_bitmap);
  bitmapCanvas.clear(SK_ColorTRANSPARENT);

  SkBitmap clip_image_bitmap;
  SkPath clip_image_path;
  SkRect image_rect = SkRect::MakeXYWH(0, 0, drag_clip_width_, drag_clip_height_);
  clip_image_path.addRoundRect(image_rect, ToOhCoordinate(IMAGE_ROUND_RECT_RATIO), ToOhCoordinate(IMAGE_ROUND_RECT_RATIO));
  auto clip_image_info = SkImageInfo::Make(drag_clip_width_, drag_clip_height_, SkColorType::kBGRA_8888_SkColorType , SkAlphaType::kUnpremul_SkAlphaType);
  clip_image_bitmap.allocPixels(clip_image_info);
  SkCanvas clip_image_canvas(clip_image_bitmap);
  clip_image_canvas.clear(SK_ColorTRANSPARENT);

  SkPaint clip_image_paint;
  clip_image_paint.setAntiAlias(true);
  if (dark_mode_enable_) {
    clip_image_paint.setColor(DEFAULT_COLOR_BG_DARK_MODE);
  } else {
    clip_image_paint.setColor(DEFAULT_COLOR_BG);
  }
  clip_image_paint.setBlendMode(SkBlendMode::kSrcOver);
  clip_image_paint.setStyle(SkPaint::kFill_Style);
  clip_image_canvas.drawPath(clip_image_path, clip_image_paint);

  SkPaint clip_bitmap_paint;
  clip_bitmap_paint.setAntiAlias(true);
  clip_bitmap_paint.setBlendMode(SkBlendMode::kSrcIn);
  clip_image_canvas.drawImage(in_bitmap.asImage(), 0, 0, SkSamplingOptions(), &clip_bitmap_paint);
  clip_image_canvas.readPixels(clip_image_bitmap, 0, 0);

  SkPaint bitmap_paint;
  bitmap_paint.setAntiAlias(true);
  auto shadow = SkImageFilters::DropShadow(ToOhCoordinate(SHADOW_DX), ToOhCoordinate(IMAGE_SHADOW_DY),
    ToOhCoordinate(IAMGE_SHADOW_RADIUS), ToOhCoordinate(IAMGE_SHADOW_RADIUS), IMAGE_SHADOW_COLOR, nullptr);
  bitmap_paint.setImageFilter(shadow);
  bitmapCanvas.drawImage(clip_image_bitmap.asImage(), ToOhCoordinate(IMAGE_EXPAND_PADDING - SHADOW_DX),
    ToOhCoordinate(IMAGE_EXPAND_PADDING - IMAGE_SHADOW_DY), SkSamplingOptions(), &bitmap_paint);
  bitmapCanvas.readPixels(out_bitmap, 0, 0);
}

void NWebDragDataImpl::GenerateOhosDragBitmapFromOriginForRichtext(const SkBitmap& in_bitmap, SkBitmap& out_bitmap,
                                                                int& width, int& height) {
  LOG(INFO) << "process richtext drag";
  width = drag_clip_width_ + ToOhCoordinate(TEXT_PADDING) * DOUBLE_RATIO;
  height = drag_clip_height_ + ToOhCoordinate(TEXT_PADDING) * DOUBLE_RATIO;

  bool is_need_process_min_image_width = false;
  if (width < ToOhCoordinate(IMAGE_MIN_WIDTH)) {
    is_need_process_min_image_width = true;
    width  = ToOhCoordinate(IMAGE_MIN_WIDTH);
  }

  SkPath shadow_path;
  shadow_path = GetShadowPath(width, height);
  width += ToOhCoordinate(EXPAND_PADDING) * DOUBLE_RATIO;
  height += ToOhCoordinate(EXPAND_PADDING) * DOUBLE_RATIO;
  auto imageInfo = SkImageInfo::Make(width, height,
    SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
  out_bitmap.allocPixels(imageInfo);
  SkCanvas bitmapCanvas(out_bitmap);
  // clear default black background
  bitmapCanvas.clear(SK_ColorTRANSPARENT);

  // draw shadow path
  shadow_path.offset(ToOhCoordinate(EXPAND_PADDING - SHADOW_DX), ToOhCoordinate(EXPAND_PADDING - SHADOW_DY));
  SkPaint shadow_layer_paint;
  shadow_layer_paint.setAntiAlias(true);
  shadow_layer_paint.setStyle(SkPaint::kFill_Style);
  if (dark_mode_enable_) {
    shadow_layer_paint.setColor(DEFAULT_COLOR_BG_DARK_MODE);
  } else {
    shadow_layer_paint.setColor(DEFAULT_COLOR_BG);
  }
  shadow_layer_paint.setBlendMode(SkBlendMode::kSrcOver);
  auto shadow = SkImageFilters::DropShadow(ToOhCoordinate(SHADOW_DX), ToOhCoordinate(SHADOW_DY),
    ToOhCoordinate(SHADOW_RADIUS), ToOhCoordinate(SHADOW_RADIUS), SHADOW_COLOR, nullptr);
  shadow_layer_paint.setImageFilter(shadow);
  bitmapCanvas.drawPath(shadow_path, shadow_layer_paint);

  SkPaint bitmap_paint;
  bitmap_paint.setAntiAlias(true);
  bitmap_paint.setBlendMode(SkBlendMode::kSrcOver);
  if (is_need_process_min_image_width) {
    bitmapCanvas.drawImage(in_bitmap.asImage(),
      ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING - SHADOW_DX) + (ToOhCoordinate(IMAGE_MIN_WIDTH) - ToOhCoordinate(TEXT_PADDING) * DOUBLE_RATIO - drag_clip_width_) / 2,
      ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING - SHADOW_DY), SkSamplingOptions(), &bitmap_paint);
  } else {
    bitmapCanvas.drawImage(in_bitmap.asImage(), ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING - SHADOW_DX),
      ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING - SHADOW_DY), SkSamplingOptions(), &bitmap_paint);
  }

  bitmapCanvas.readPixels(out_bitmap, 0, 0);
}

void NWebDragDataImpl::GenerateOhosDragBitmapFromOrigin(const SkBitmap& in_bitmap, SkBitmap& out_bitmap,
                                                        int& width, int& height) {
  if (drag_data_ && drag_data_->IsImageFileContents()) {
    GenerateOhosDragBitmapFromOriginForImage(in_bitmap, out_bitmap, width, height);
    return;
  }

  GenerateOhosDragBitmapFromOriginForRichtext(in_bitmap, out_bitmap, width, height);
}

NWebDragDataImpl::NWebDragDataImpl(CefRefPtr<CefDragData> drag_data)
    : drag_data_(drag_data) {}

NWebDragDataImpl::NWebDragDataImpl(CefRefPtr<CefDragData> drag_data, CefPoint& drag_touch_point,
    std::vector<CefPoint>& start_edge, std::vector<CefPoint>& end_edge, float device_pixel_ratio,
    bool is_useful_selection, bool dark_mode_enable, bool is_drag_new_style)
    : drag_data_(drag_data), is_useful_selection_(is_useful_selection) {
  device_pixel_ratio_ = device_pixel_ratio;
  if (device_pixel_ratio_ <= 0) {
    LOG(ERROR) << "invalid device pixel ratio, ratio = " << device_pixel_ratio_;
    return;
  }
  dark_mode_enable_ = dark_mode_enable;
  is_drag_new_style_ = is_drag_new_style;

  if (drag_data_) {
    drag_image_origin_point_.x = ToOhCoordinate(drag_touch_point.x - drag_data_->GetImageHotspot().x);
    drag_image_origin_point_.y = ToOhCoordinate(drag_touch_point.y - drag_data_->GetImageHotspot().y);
    auto image = drag_data_->GetImage();
    if (image) {
      drag_clip_width_ = ToOhCoordinate(image->GetWidth());
      drag_clip_height_ = ToOhCoordinate(image->GetHeight());
    }
  }
  if (is_useful_selection_) {
    if (start_edge.size() == 2 && end_edge.size() == 2) {
      start_edge_top_.x = start_edge[0].x;
      start_edge_top_.y = start_edge[0].y;
      start_edge_bottom_.x = start_edge[1].x;
      start_edge_bottom_.y = start_edge[1].y;
      end_edge_top_.x = end_edge[0].x;
      end_edge_top_.y = end_edge[0].y;
      end_edge_bottom_.x = end_edge[1].x;
      end_edge_bottom_.y = end_edge[1].y;
    }
  }

  auto image = drag_data_->GetImage();
  LOG(DEBUG) << "drag image width = " << drag_clip_width_
             << ", height = " << drag_clip_height_;
  LOG(DEBUG) << "start_edge_top_ x = " << start_edge_top_.x
             << ", start_edge_top_ y = " << start_edge_top_.y;
  LOG(DEBUG) << "start_edge_bottom_ x = " << start_edge_bottom_.x
             << ", start_edge_bottom_ y = " << start_edge_bottom_.y;
  LOG(DEBUG) << "end_edge_top_ x = " << end_edge_top_.x
             << ", end_edge_top_ y = " << end_edge_top_.y;
  LOG(DEBUG) << "end_edge_bottom_ x = " << end_edge_bottom_.x
             << ", end_edge_bottom_ y = " << end_edge_bottom_.y;
  LOG(DEBUG) << "origin rect x = " << drag_image_origin_point_.x
             << ", y = " << drag_image_origin_point_.y << ", right = "
             << ToOhCoordinate(drag_touch_point.x -
                               drag_data_->GetImageHotspot().x +
                               image->GetWidth())
             << ", bottom = "
             << ToOhCoordinate(drag_touch_point.y -
                               drag_data_->GetImageHotspot().y +
                               image->GetHeight())
             << ", dark mode enable = " << dark_mode_enable_;
}

std::string NWebDragDataImpl::GetLinkURL()
{
  if (!drag_data_) {
    return std::string();
  }
  return drag_data_->GetLinkURL();
}

std::string NWebDragDataImpl::GetFragmentText()
{
  if (!drag_data_) {
    return std::string();
  }
  return drag_data_->GetFragmentText();
}

std::string NWebDragDataImpl::GetFragmentHtml()
{
  if (!drag_data_) {
    return std::string();
  }
  return drag_data_->GetFragmentHtml();
}

bool IsTransparent(const SkBitmap& bitmap) {
    if (bitmap.isNull()) {
        return false;
    }

    if (bitmap.colorType() != kRGBA_8888_SkColorType && bitmap.colorType() != kBGRA_8888_SkColorType) {
        return false;
    }

    const SkColor* pixels = static_cast<const SkColor*>(bitmap.getPixels());
    if (!pixels) {
        return false;
    }

    for (int y = 0; y < bitmap.height(); ++y) {
        for (int x = 0; x < bitmap.width(); ++x) {
            SkColor pixel = pixels[y * bitmap.rowBytes() / sizeof(SkColor) + x];
            if (SkColorGetA(pixel) != 0) {
                return false;
            }
        }
    }

    return true;
}

bool NWebDragDataImpl::GetPixelMapSetting(const void** data, size_t& len, int& width, int& height) {
  if (!drag_data_) {
    return false;
  }
  auto image = drag_data_->GetImage();
  if (!image) {
    LOG(ERROR) << "drag data image invalid";
    return false;
  }

  gfx::ImageSkia image_skia = static_cast<CefImageImpl*>(image.get())->AsImageSkia();
  const SkBitmap *image_bitmap = image_skia.bitmap();
  if (image_bitmap == nullptr) {
    LOG(ERROR) << "image_bitmap is nullptr";
    return false;
  }

  SkBitmap out_bitmap;
  width = image_bitmap->width();
  height = image_bitmap->height();
  bool isTransparent = IsTransparent(*image_bitmap);
  if (!isTransparent && width != 0 && height != 0) {
    TRACE_EVENT0("base", "NWebDragDataImpl::GenerateOhosDragBitmapFromOrigin");
    GenerateOhosDragBitmapFromOrigin(*image_bitmap, out_bitmap, width, height);
  } else {
    out_bitmap = *image_bitmap;
  }

  LOG(INFO) << "bitmap color type = " << static_cast<int>(out_bitmap.colorType()) << ", alpha type = " \
    << static_cast<int>(out_bitmap.alphaType()) << "out_bitmap.computeByteSize() = " << out_bitmap.computeByteSize()
    << ", isTransparent = " << isTransparent;
  auto bitmap = CefBinaryValue::Create(out_bitmap.getPixels(), out_bitmap.computeByteSize());
  if (!bitmap) {
    LOG(ERROR) << "drag data bitmap invalid";
    return false;
  }

  size_t data_size = bitmap->GetSize();
  void* buffer = calloc(1, data_size);
  if (!buffer) {
    LOG(ERROR) << "calloc failed";
    return false;
  }
  size_t read_size = bitmap->GetData(buffer, data_size, 0);
  if (read_size != data_size) {
    free(buffer);
    LOG(ERROR) << "get data from bitmap failed";
    return false;
  }
  len = read_size;
  *data = buffer;
  if (image_buffer_) {
    free(image_buffer_);
    image_buffer_ = nullptr;
  }
  image_buffer_ = buffer;

  LOG(INFO) << "drag image width : " << width << ", height : " << height
            << ", buffer size : " << read_size;
  return true;
}

void NWebDragDataImpl::FreePixlMapData()
{
  if (image_buffer_) {
    free(image_buffer_);
    image_buffer_ = nullptr;
  }
}

bool NWebDragDataImpl::SetLinkURL(const std::string& url)
{
  if (!drag_data_) {
    drag_data_ = new CefDragDataImpl();
    if (!drag_data_) {
      return false;
    }
  }

  if (drag_data_->IsReadOnly()) {
    drag_data_->SetReadOnly(false);
    drag_data_->SetLinkURL(url);
    drag_data_->SetReadOnly(true);
  } else {
    drag_data_->SetLinkURL(url);
  }
  return true;
}

bool NWebDragDataImpl::SetFragmentText(const std::string& text)
{
  if (!drag_data_) {
    drag_data_ = new CefDragDataImpl();
    if (!drag_data_) {
      return false;
    }
  }

  if (drag_data_->IsReadOnly()) {
    drag_data_->SetReadOnly(false);
    drag_data_->SetFragmentText(text);
    drag_data_->SetReadOnly(true);
  } else {
    drag_data_->SetFragmentText(text);
  }
  return true;
}

bool NWebDragDataImpl::SetFragmentHtml(const std::string& html)
{
  if (!drag_data_) {
    drag_data_ = new CefDragDataImpl();
    if (!drag_data_) {
      return false;
    }
  }

  if (drag_data_->IsReadOnly()) {
    drag_data_->SetReadOnly(false);
    drag_data_->SetFragmentHtml(html);
    drag_data_->SetReadOnly(true);
  } else {
    drag_data_->SetFragmentHtml(html);
  }
  return true;
}

std::string NWebDragDataImpl::GetLinkTitle()
{
  if (!drag_data_) {
    return std::string();
  }
  return drag_data_->GetLinkTitle();
}

bool NWebDragDataImpl::SetLinkTitle(const std::string& title)
{
  if (!drag_data_) {
    drag_data_ = new CefDragDataImpl();
    if (!drag_data_) {
      return false;
    }
  }
  drag_data_->SetLinkTitle(title);
  return true;
}

bool NWebDragDataImpl::SetPixelMapSetting(const void* data, size_t len, int width, int height)
{
  content::DropData dropdata;
  CefRefPtr<CefImage> image = CefImage::CreateImage();
  // default params
  image->AddBitmap(1.0, width, height, CEF_COLOR_TYPE_BGRA_8888, CEF_ALPHA_TYPE_OPAQUE, data, len);

  if (!drag_data_) {
    drag_data_ = new CefDragDataImpl(dropdata, image, CefPoint(0,0));
    if (!drag_data_) {
      return false;
    }
  }
  return true;
}

void NWebDragDataImpl::GetDragStartPosition(int& x, int& y) {
  if (drag_data_ && drag_data_->IsImageFileContents()) {
    x = drag_image_origin_point_.x - ToOhCoordinate(IMAGE_EXPAND_PADDING);
    y = drag_image_origin_point_.y - ToOhCoordinate(IMAGE_EXPAND_PADDING);
  } else {
    x = drag_image_origin_point_.x - ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING);
    y = drag_image_origin_point_.y - ToOhCoordinate(TEXT_PADDING + EXPAND_PADDING);
    int32_t width = drag_clip_width_ + ToOhCoordinate(TEXT_PADDING) * DOUBLE_RATIO;
    if (width < ToOhCoordinate(IMAGE_MIN_WIDTH)) {
      x = static_cast<int32_t>(x - (ToOhCoordinate(IMAGE_MIN_WIDTH) - width) / DOUBLE_RATIO);
    }
  }
}

bool NWebDragDataImpl::IsSingleImageContent() {
  if (drag_data_) {
    return drag_data_->IsImageFileContents();
  }

  return false;
}

bool NWebDragDataImpl::SetFileUri(const std::string& uri) {
  if (!drag_data_) {
    return false;
  }
  base::FilePath fPath(uri);
  if (drag_data_->IsReadOnly()) {
    drag_data_->SetReadOnly(false);
    drag_data_->AddFile(uri, fPath.BaseName().AsUTF8Unsafe());
    drag_data_->SetReadOnly(true);
  } else {
    drag_data_->AddFile(uri, fPath.BaseName().AsUTF8Unsafe());
  }

  return true;
}

std::string NWebDragDataImpl::GetImageFileName() {
 if (drag_data_) {
    return drag_data_->GetFileName();
  }
  return "";
}

void NWebDragDataImpl::ClearImageFileNames() {
  if (drag_data_) {
    if (drag_data_->IsReadOnly()) {
      drag_data_->SetReadOnly(false);
      drag_data_->ClearFilenames();
      drag_data_->ResetFileContents();
      drag_data_->SetReadOnly(true);
    } else {
      drag_data_->ClearFilenames();
    }
  }
}

}  // namespace OHOS::NWeb

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>

#include "base/bits.h"
#include "base/check_op.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/numerics/safe_conversions.h"
#include "base/numerics/safe_math.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/raster_interface.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "media/base/limits.h"
#include "media/base/video_frame.h"
#include "media/base/video_frame_pool.h"
#include "media/base/video_types.h"
#include "media/base/encoder_status.h"
#include "media/base/wait_and_replace_sync_token_client.h"
#include "third_party/libyuv/include/libyuv.h"
#include "third_party/skia/include/core/SkAlphaType.h"
#include "third_party/skia/include/core/SkColorType.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkYUVAPixmaps.h"
#include "third_party/skia/include/gpu/GpuTypes.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace media  {

EncoderStatus ConvertAndScaleFrame(const VideoFrame& src_frame,
                                   VideoFrame& dst_frame,
                                   std::vector<uint8_t>& tmp_buf) {
  TRACE_EVENT2("media", "ConvertAndScaleFrame", "src_format",
               VideoPixelFormatToString(src_frame.format()), "dst_format",
               VideoPixelFormatToString(dst_frame.format()));
  constexpr auto kDefaultFiltering = libyuv::kFilterBox;
  if (!src_frame.IsMappable() || !dst_frame.IsMappable()) {
    return EncoderStatus::Codes::kUnsupportedFrameFormat;
  }

  if ((dst_frame.format() == PIXEL_FORMAT_I420A ||
       dst_frame.format() == PIXEL_FORMAT_I420 ||
       dst_frame.format() == PIXEL_FORMAT_NV12 ||
       dst_frame.format() == PIXEL_FORMAT_NV12A) &&
      (src_frame.format() == PIXEL_FORMAT_XBGR ||
       src_frame.format() == PIXEL_FORMAT_XRGB ||
       src_frame.format() == PIXEL_FORMAT_ABGR ||
       src_frame.format() == PIXEL_FORMAT_ARGB)) {
    if (!media::IsOpaque(dst_frame.format()) &&
        media::IsOpaque(src_frame.format())) {
      // We can drop an alpha channel, but we don't add it out of nothing.
      return EncoderStatus::Codes::kUnsupportedFrameFormat;
    }

    // libyuv's RGB to YUV methods always output BT.601.
    dst_frame.set_color_space(gfx::ColorSpace::CreateREC601());

    size_t src_stride = src_frame.stride(VideoFrame::Plane::kARGB);
    const uint8_t* src_data = src_frame.visible_data(VideoFrame::Plane::kARGB);
    if (src_frame.visible_rect().size() != dst_frame.visible_rect().size()) {
      size_t tmp_buffer_size = VideoFrame::AllocationSize(
          src_frame.format(), dst_frame.coded_size());
      if (tmp_buf.size() < tmp_buffer_size) {
        tmp_buf.resize(tmp_buffer_size);
      }

      size_t stride =
          VideoFrame::RowBytes(VideoFrame::Plane::kARGB, src_frame.format(),
                               dst_frame.visible_rect().width());
      int error = libyuv::ARGBScale(
          src_data, src_stride, src_frame.visible_rect().width(),
          src_frame.visible_rect().height(), tmp_buf.data(), stride,
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      if (error) {
        return EncoderStatus::Codes::kScalingError;
      }
      src_data = tmp_buf.data();
      src_stride = stride;
    }

    if (dst_frame.format() == PIXEL_FORMAT_I420 ||
        dst_frame.format() == PIXEL_FORMAT_I420A) {
      auto convert_fn = (src_frame.format() == PIXEL_FORMAT_XBGR ||
                         src_frame.format() == PIXEL_FORMAT_ABGR)
                            ? libyuv::ABGRToI420
                            : libyuv::ARGBToI420;
      int error = convert_fn(
          src_data, src_stride,
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
          dst_frame.stride(VideoFrame::Plane::kU),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
          dst_frame.stride(VideoFrame::Plane::kV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
      if (error) {
        return EncoderStatus::Codes::kFormatConversionError;
      }

      if (dst_frame.format() == PIXEL_FORMAT_I420A) {
        // Convert alpha channel separately
        libyuv::ARGBExtractAlpha(
            src_data, src_stride,
            dst_frame.GetWritableVisibleData(media::VideoFrame::Plane::kA),
            dst_frame.stride(media::VideoFrame::Plane::kA),
            dst_frame.visible_rect().width(),
            dst_frame.visible_rect().height());
      }

      return OkStatus();
    }

    auto convert_fn = (src_frame.format() == PIXEL_FORMAT_XBGR ||
                       src_frame.format() == PIXEL_FORMAT_ABGR)
                          ? libyuv::ABGRToNV12
                          : libyuv::ARGBToNV12;
    int error = convert_fn(
        src_data, src_stride,
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
        dst_frame.stride(VideoFrame::Plane::kY),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
        dst_frame.stride(VideoFrame::Plane::kUV),
        dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
    if (error) {
      return EncoderStatus::Codes::kFormatConversionError;
    }

    if (dst_frame.format() == PIXEL_FORMAT_NV12A) {
      // Convert alpha channel separately.
      libyuv::ARGBExtractAlpha(
          src_data, src_stride,
          dst_frame.GetWritableVisibleData(
              media::VideoFrame::Plane::kATriPlanar),
          dst_frame.stride(media::VideoFrame::Plane::kATriPlanar),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
    }

    return OkStatus();
  }

  // Converting between YUV formats doesn't change the color space.
  dst_frame.set_color_space(src_frame.ColorSpace());

  // Both frames are I420 or I420A, only scaling is required.
  if ((dst_frame.format() == PIXEL_FORMAT_I420 ||
       dst_frame.format() == PIXEL_FORMAT_I420A) &&
      (src_frame.format() == PIXEL_FORMAT_I420 ||
       src_frame.format() == PIXEL_FORMAT_I420A)) {
    if (dst_frame.format() == PIXEL_FORMAT_I420A) {
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kA),
          src_frame.stride(VideoFrame::Plane::kA),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kA),
          dst_frame.stride(VideoFrame::Plane::kA),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
    }
    int error = libyuv::I420Scale(
        src_frame.visible_data(VideoFrame::Plane::kY),
        src_frame.stride(VideoFrame::Plane::kY),
        src_frame.visible_data(VideoFrame::Plane::kU),
        src_frame.stride(VideoFrame::Plane::kU),
        src_frame.visible_data(VideoFrame::Plane::kV),
        src_frame.stride(VideoFrame::Plane::kV),
        src_frame.visible_rect().width(), src_frame.visible_rect().height(),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
        dst_frame.stride(VideoFrame::Plane::kY),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
        dst_frame.stride(VideoFrame::Plane::kU),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
        dst_frame.stride(VideoFrame::Plane::kV),
        dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
        kDefaultFiltering);
    if (error) {
      return EncoderStatus::Codes::kScalingError;
    }
    return OkStatus();
  }

  // Both frames are NV12 or NV12A, only scaling is required.
  if ((dst_frame.format() == PIXEL_FORMAT_NV12 ||
       dst_frame.format() == PIXEL_FORMAT_NV12A) &&
      (src_frame.format() == PIXEL_FORMAT_NV12 ||
       src_frame.format() == PIXEL_FORMAT_NV12A)) {
    if (dst_frame.format() == PIXEL_FORMAT_NV12A) {
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kATriPlanar),
          src_frame.stride(VideoFrame::Plane::kATriPlanar),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kATriPlanar),
          dst_frame.stride(VideoFrame::Plane::kATriPlanar),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
    }
    int error = libyuv::NV12Scale(
        src_frame.visible_data(VideoFrame::Plane::kY),
        src_frame.stride(VideoFrame::Plane::kY),
        src_frame.visible_data(VideoFrame::Plane::kUV),
        src_frame.stride(VideoFrame::Plane::kUV),
        src_frame.visible_rect().width(), src_frame.visible_rect().height(),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
        dst_frame.stride(VideoFrame::Plane::kY),
        dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
        dst_frame.stride(VideoFrame::Plane::kUV),
        dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
        kDefaultFiltering);
    if (error) {
      return EncoderStatus::Codes::kScalingError;
    }
    return OkStatus();
  }

  if (dst_frame.format() == PIXEL_FORMAT_I420 &&
      src_frame.format() == PIXEL_FORMAT_NV12) {
    if (src_frame.visible_rect().size() == dst_frame.visible_rect().size()) {
      // Both frames have the same size, only NV12-to-I420 conversion is
      // required.
      int error = libyuv::NV12ToI420(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          src_frame.visible_data(VideoFrame::Plane::kUV),
          src_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
          dst_frame.stride(VideoFrame::Plane::kU),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
          dst_frame.stride(VideoFrame::Plane::kV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
      if (error) {
        return EncoderStatus::Codes::kFormatConversionError;
      }
      return OkStatus();
    } else {
      // Both resize and NV12-to-I420 conversion are required.
      // First, split UV planes into two, basically producing a I420 frame.
      const int tmp_uv_width = (src_frame.visible_rect().width() + 1) / 2;
      const int tmp_uv_height = (src_frame.visible_rect().height() + 1) / 2;
      size_t tmp_buffer_size = tmp_uv_width * tmp_uv_height * 2;
      if (tmp_buf.size() < tmp_buffer_size) {
        tmp_buf.resize(tmp_buffer_size);
      }

      uint8_t* tmp_u = tmp_buf.data();
      uint8_t* tmp_v = tmp_u + tmp_uv_width * tmp_uv_height;
      libyuv::SplitUVPlane(src_frame.visible_data(VideoFrame::Plane::kUV),
                           src_frame.stride(VideoFrame::Plane::kUV), tmp_u,
                           tmp_uv_width, tmp_v, tmp_uv_width, tmp_uv_width,
                           tmp_uv_height);

      // Second, scale resulting I420 frame into the destination.
      int error = libyuv::I420Scale(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          tmp_u,  // Temporary U-plane for src UV-plane.
          tmp_uv_width,
          tmp_v,  // Temporary V-plane for src UV-plane.
          tmp_uv_width, src_frame.visible_rect().width(),
          src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
          dst_frame.stride(VideoFrame::Plane::kU),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
          dst_frame.stride(VideoFrame::Plane::kV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      if (error) {
        return EncoderStatus::Codes::kScalingError;
      }
      return OkStatus();
    }
  }

  if (dst_frame.format() == PIXEL_FORMAT_I420A &&
      src_frame.format() == PIXEL_FORMAT_NV12A) {
    if (src_frame.visible_rect().size() == dst_frame.visible_rect().size()) {
      // Both frames have the same size, only NV12-to-I420 conversion is
      // required.
      int error = libyuv::NV12ToI420(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          src_frame.visible_data(VideoFrame::Plane::kUV),
          src_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
          dst_frame.stride(VideoFrame::Plane::kU),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
          dst_frame.stride(VideoFrame::Plane::kV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
      if (error) {
        return EncoderStatus::Codes::kFormatConversionError;
      }
      // Convert alpha channel separately.
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kATriPlanar),
          src_frame.stride(VideoFrame::Plane::kATriPlanar),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kA),
          dst_frame.stride(VideoFrame::Plane::kA),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      return OkStatus();
    } else {
      // Both resize and NV12-to-I420 conversion are required.
      // First, split UV planes into two, basically producing a I420 frame.
      const int tmp_uv_width = (src_frame.visible_rect().width() + 1) / 2;
      const int tmp_uv_height = (src_frame.visible_rect().height() + 1) / 2;
      size_t tmp_buffer_size = tmp_uv_width * tmp_uv_height * 2;
      if (tmp_buf.size() < tmp_buffer_size) {
        tmp_buf.resize(tmp_buffer_size);
      }

      uint8_t* tmp_u = tmp_buf.data();
      uint8_t* tmp_v = tmp_u + tmp_uv_width * tmp_uv_height;
      DCHECK_EQ(tmp_buf.data() + tmp_buffer_size,
                tmp_v + (tmp_uv_width * tmp_uv_height));
      libyuv::SplitUVPlane(src_frame.visible_data(VideoFrame::Plane::kUV),
                           src_frame.stride(VideoFrame::Plane::kUV), tmp_u,
                           tmp_uv_width, tmp_v, tmp_uv_width, tmp_uv_width,
                           tmp_uv_height);

      // Second, scale resulting I420 frame into the destination.
      int error = libyuv::I420Scale(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          tmp_u,  // Temporary U-plane for src UV-plane.
          tmp_uv_width,
          tmp_v,  // Temporary V-plane for src UV-plane.
          tmp_uv_width, src_frame.visible_rect().width(),
          src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kU),
          dst_frame.stride(VideoFrame::Plane::kU),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kV),
          dst_frame.stride(VideoFrame::Plane::kV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      if (error) {
        return EncoderStatus::Codes::kScalingError;
      }
      // Convert alpha channel separately.
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kATriPlanar),
          src_frame.stride(VideoFrame::Plane::kATriPlanar),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kA),
          dst_frame.stride(VideoFrame::Plane::kA),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      return OkStatus();
    }
  }

  if (dst_frame.format() == PIXEL_FORMAT_NV12 &&
      src_frame.format() == PIXEL_FORMAT_I420) {
    if (src_frame.visible_rect().size() == dst_frame.visible_rect().size()) {
      // Both frames have the same size, only I420-to-NV12 conversion is
      // required.
      int error = libyuv::I420ToNV12(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          src_frame.visible_data(VideoFrame::Plane::kU),
          src_frame.stride(VideoFrame::Plane::kU),
          src_frame.visible_data(VideoFrame::Plane::kV),
          src_frame.stride(VideoFrame::Plane::kV),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
          dst_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
      if (error) {
        return EncoderStatus::Codes::kFormatConversionError;
      }
      return OkStatus();
    } else {
      // Both resize and I420-to-NV12 conversion are required.
      // First, merge U and V planes into one, basically producing a NV12 frame.
      const size_t tmp_uv_width = (src_frame.visible_rect().width() + 1) / 2;
      const size_t tmp_uv_height = (src_frame.visible_rect().height() + 1) / 2;
      size_t tmp_buffer_size = tmp_uv_width * tmp_uv_height * 2;
      if (tmp_buf.size() < tmp_buffer_size) {
        tmp_buf.resize(tmp_buffer_size);
      }

      uint8_t* tmp_uv = tmp_buf.data();
      size_t stride_uv = tmp_uv_width * 2;
      libyuv::MergeUVPlane(src_frame.visible_data(VideoFrame::Plane::kU),
                           src_frame.stride(VideoFrame::Plane::kU),
                           src_frame.visible_data(VideoFrame::Plane::kV),
                           src_frame.stride(VideoFrame::Plane::kV),
                           tmp_uv,     // Temporary for merged UV-plane
                           stride_uv,  // Temporary stride
                           tmp_uv_width, tmp_uv_height);

      // Second, scale resulting NV12 frame into the destination.
      int error = libyuv::NV12Scale(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          tmp_uv,     // Temporary for merged UV-plane
          stride_uv,  // Temporary stride
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
          dst_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      if (error) {
        return EncoderStatus::Codes::kScalingError;
      }
      return OkStatus();
    }
  }

  if (dst_frame.format() == PIXEL_FORMAT_NV12A &&
      src_frame.format() == PIXEL_FORMAT_I420A) {
    if (src_frame.visible_rect().size() == dst_frame.visible_rect().size()) {
      // Both frames have the same size, only I420-to-NV12 conversion is
      // required.
      int error = libyuv::I420ToNV12(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          src_frame.visible_data(VideoFrame::Plane::kU),
          src_frame.stride(VideoFrame::Plane::kU),
          src_frame.visible_data(VideoFrame::Plane::kV),
          src_frame.stride(VideoFrame::Plane::kV),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
          dst_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height());
      if (error) {
        return EncoderStatus::Codes::kFormatConversionError;
      }
      // Convert alpha channel separately.
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kA),
          src_frame.stride(VideoFrame::Plane::kA),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kATriPlanar),
          dst_frame.stride(VideoFrame::Plane::kATriPlanar),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      return OkStatus();
    } else {
      // Both resize and I420-to-NV12 conversion are required.
      // First, merge U and V planes into one, basically producing a NV12 frame.
      const size_t tmp_uv_width = (src_frame.visible_rect().width() + 1) / 2;
      const size_t tmp_uv_height = (src_frame.visible_rect().height() + 1) / 2;
      size_t tmp_buffer_size = tmp_uv_width * tmp_uv_height * 2;
      if (tmp_buf.size() < tmp_buffer_size) {
        tmp_buf.resize(tmp_buffer_size);
      }

      uint8_t* tmp_uv = tmp_buf.data();
      size_t stride_uv = tmp_uv_width * 2;
      libyuv::MergeUVPlane(src_frame.visible_data(VideoFrame::Plane::kU),
                           src_frame.stride(VideoFrame::Plane::kU),
                           src_frame.visible_data(VideoFrame::Plane::kV),
                           src_frame.stride(VideoFrame::Plane::kV),
                           tmp_uv,     // Temporary for merged UV-plane
                           stride_uv,  // Temporary stride
                           tmp_uv_width, tmp_uv_height);

      // Second, scale resulting NV12 frame into the destination.
      int error = libyuv::NV12Scale(
          src_frame.visible_data(VideoFrame::Plane::kY),
          src_frame.stride(VideoFrame::Plane::kY),
          tmp_uv,     // Temporary for merged UV-plane
          stride_uv,  // Temporary stride
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kY),
          dst_frame.stride(VideoFrame::Plane::kY),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kUV),
          dst_frame.stride(VideoFrame::Plane::kUV),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      if (error) {
        return EncoderStatus::Codes::kScalingError;
      }
      // Convert alpha channel separately.
      libyuv::ScalePlane(
          src_frame.visible_data(VideoFrame::Plane::kA),
          src_frame.stride(VideoFrame::Plane::kA),
          src_frame.visible_rect().width(), src_frame.visible_rect().height(),
          dst_frame.GetWritableVisibleData(VideoFrame::Plane::kATriPlanar),
          dst_frame.stride(VideoFrame::Plane::kATriPlanar),
          dst_frame.visible_rect().width(), dst_frame.visible_rect().height(),
          kDefaultFiltering);
      return OkStatus();
    }
  }

  return EncoderStatus(EncoderStatus::Codes::kUnsupportedFrameFormat)
      .WithData("src", src_frame.AsHumanReadableString())
      .WithData("dst", dst_frame.AsHumanReadableString());
}

}
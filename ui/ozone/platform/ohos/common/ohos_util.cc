/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/ozone/platform/ohos/common/ohos_util.h"

#include "base/check.h"
#include "base/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/aura/window_tree_host.h"
#include "ui/display/screen.h"

using WindowAdapter = ohos::adapter::xcomponent::WindowAdapter;

namespace ui {
namespace util {

const std::string kWindowPrefix = "browser";

void* GetWindow(std::string& window_id) {
  auto window = WindowAdapter::GetInstance().GetWindow(window_id);
  DCHECK(window);
  if (!window) {
    LOG(ERROR) << "Get window is not exists, windowId:" << window_id;
    return nullptr;
  }
  return window;
}

void* GetWindowFromWidget(gfx::AcceleratedWidget widget_id) {
  DCHECK(widget_id);
  std::string window_id(kWindowPrefix);
  window_id += std::to_string(widget_id);
  return ui::util::GetWindow(window_id);
}

int32_t GetNextWindowWidgetId() {
  return WindowAdapter::GetInstance().NextWindowWidgetId();
}

std::string ConvertWidgetIdToWindowId(const gfx::AcceleratedWidget widget_id) {
  DCHECK(widget_id);
  return kWindowPrefix + std::to_string(widget_id);
}

gfx::AcceleratedWidget ConvertWindowIdToWidgetId(std::string& window_id) {
  if (!window_id.starts_with(kWindowPrefix) ||
      !std::all_of(window_id.begin() + kWindowPrefix.size(), window_id.end(),
          [](char i) { return isdigit(i); })) {
    return gfx::kNullAcceleratedWidget;
  }
  std::string windowIdString = window_id.substr(kWindowPrefix.size());
  return std::stoi(windowIdString);
}

void UpdateOhNativeWindow(gfx::ColorSpace frame_color_space,
                          OHNativeWindow* window) {
  int32_t window_format = GetOhBufferFormatFromColorSpace(frame_color_space);
  int ret = OH_NativeWindow_NativeWindowHandleOpt(
      reinterpret_cast<OHNativeWindow*>(window), SET_FORMAT, window_format);
  if (ret != 0) {
    LOG(ERROR) << __func__
               << " [HDRVivid] OH_NativeWindow_NativeWindowHandleOpt fail ret="
               << ret;
  }

  int32_t color_gamut = GetOhColorGamutFromColorSpace(frame_color_space);
  ret = OH_NativeWindow_NativeWindowHandleOpt(
      reinterpret_cast<OHNativeWindow*>(window), SET_COLOR_GAMUT, color_gamut);
  if (ret != 0) {
    LOG(ERROR) << __func__
               << " [HDRVivid] OH_NativeWindow_NativeWindowHandleOpt fail ret="
               << ret;
  }

  OH_NativeBuffer_ColorSpace colorSpace =
      ConvertToSystemColorSpace(frame_color_space);

  ret = OH_NativeWindow_SetColorSpace(reinterpret_cast<OHNativeWindow*>(window),
                                      colorSpace);
  if (ret != 0) {
    LOG(ERROR) << __func__
               << " [HDRVivid] OH_NativeWindow_SetColorSpace fail ret=" << ret;
  }
}

int32_t GetOhColorGamutFromColorSpace(gfx::ColorSpace frame_color_space) {
  int32_t color_gamut = NATIVEBUFFER_COLOR_GAMUT_SRGB;
  switch (frame_color_space.GetPrimaryID()) {
    case gfx::ColorSpace::PrimaryID::BT709:
      break;
    case gfx::ColorSpace::PrimaryID::P3:
      color_gamut = NATIVEBUFFER_COLOR_GAMUT_DISPLAY_P3;
      break;
    case gfx::ColorSpace::PrimaryID::BT2020:
      color_gamut = NATIVEBUFFER_COLOR_GAMUT_DISPLAY_BT2020;
      break;
    default:
      LOG(WARNING) << __func__ << " [HDRVivid] not support color gurmat: "
                   << static_cast<int>(frame_color_space.GetPrimaryID());
  }
  return color_gamut;
}

OH_NativeBuffer_ColorSpace ConvertToSystemColorSpace(
    gfx::ColorSpace frame_color_space) {
  OH_NativeBuffer_ColorSpace color_space = OH_COLORSPACE_BT709_FULL;
  if (frame_color_space.GetPrimaryID() == gfx::ColorSpace::PrimaryID::BT709) {
    return color_space;
  }
  if (frame_color_space.GetPrimaryID() == gfx::ColorSpace::PrimaryID::P3 &&
      frame_color_space.GetTransferID() == gfx::ColorSpace::TransferID::PQ) {
    color_space = OH_COLORSPACE_P3_PQ_FULL;
  } else if (frame_color_space.GetPrimaryID() ==
                 gfx::ColorSpace::PrimaryID::P3 &&
             frame_color_space.GetTransferID() ==
                 gfx::ColorSpace::TransferID::HLG) {
    color_space = OH_COLORSPACE_P3_HLG_FULL;
  } else if (frame_color_space.GetPrimaryID() ==
                 gfx::ColorSpace::PrimaryID::BT2020 &&
             frame_color_space.GetTransferID() ==
                 gfx::ColorSpace::TransferID::PQ) {
    color_space = OH_COLORSPACE_BT2020_PQ_FULL;
  } else if (frame_color_space.GetPrimaryID() ==
                 gfx::ColorSpace::PrimaryID::BT2020 &&
             frame_color_space.GetTransferID() ==
                 gfx::ColorSpace::TransferID::HLG) {
    color_space = OH_COLORSPACE_BT2020_HLG_FULL;
  } else {
    LOG(WARNING) << __func__ << " [HDRVivid] not support color sapce: "
                 << frame_color_space.ToString();
  }

  return color_space;
}

int32_t GetOhBufferFormatFromColorSpace(gfx::ColorSpace frame_color_space) {
  if (frame_color_space.IsHDR()) {
    return NATIVEBUFFER_PIXEL_FMT_RGBA_1010102;
  }
  return NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
}

}  // namespace util
}  // namespace ui

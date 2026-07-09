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

#ifndef UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_
#define UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_

#include <native_buffer/buffer_common.h>
#include <native_buffer/native_buffer.h>
#include <native_image/native_image.h>

#include <string>

#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/display/display.h"
#include "ui/gfx/native_widget_types.h"

namespace ui {

namespace util {

using NativeWindowType = ohos::adapter::xcomponent::NativeWindowType;

NativeWindowType GetWindow(std::string& window_id);

NativeWindowType GetWindowFromWidget(gfx::AcceleratedWidget widget_id);

int32_t GetNextWindowWidgetId();

std::string ConvertWidgetIdToWindowId(const gfx::AcceleratedWidget widget_id);

gfx::AcceleratedWidget ConvertWindowIdToWidgetId(std::string& window_id);

void UpdateOhNativeWindow(gfx::ColorSpace frame_color_space,
                          OHNativeWindow* window);

int32_t GetOhColorGamutFromColorSpace(gfx::ColorSpace frame_color_space);
OH_NativeBuffer_ColorSpace ConvertToSystemColorSpace(
    gfx::ColorSpace frame_color_space);
int32_t GetOhBufferFormatFromColorSpace(gfx::ColorSpace frame_color_space);

}  // namespace util
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMMON_OHOSUTIL_H_
#define COMMON_OHOSUTIL_H_


#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <array>
#include <cstdint>

#include "angle_gl.h"

namespace angle
{

namespace ohos
{

// clang-format off
/**
 * Buffer pixel formats mirrored from OpenHarmony.
 *
 * Taken from
 * https://gitee.com/openharmony/graphic_graphic_surface/blob/master/interfaces/inner_api/surface/surface_type.h
 */
enum {
    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_CLUT8
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_CLUT8         = 0,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_CLUT1
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_CLUT1         = 1,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_CLUT4
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_CLUT4         = 2,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGB_565
     *   Vulkan: VK_FORMAT_R5G6B5_UNORM_PACK16
     *   OpenGL ES: GL_RGB565
     */
    ANGLE_OHOS_FORMAT_RGB_565       = 3,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA_5658
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGBA_5658     = 4,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBX_4444
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGBX_4444     = 5,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA_4444
     *   Vulkan: VK_FORMAT_R4G4B4A4_UNORM_PACK16
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGBA_4444     = 6,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGB_444
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGB_444       = 7,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBX_5551
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGBX_5551     = 8,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA_5551
     *   Vulkan: VK_FORMAT_R5G5B5A1_UNORM_PACK16
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGBA_5551     = 9,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGB_555
     *   Vulkan: VK_FORMAT_R5G5B5_UNORM_PACK16
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RGB_555       = 10,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBX_8888
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    ANGLE_OHOS_FORMAT_RGBX_8888     = 11,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA_8888
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGBA8
     */
    ANGLE_OHOS_FORMAT_RGBA_8888     = 12,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGB_888
     *   Vulkan: VK_FORMAT_R8G8B8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    ANGLE_OHOS_FORMAT_RGB_888       = 13,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGR_565
     *   Vulkan: VK_FORMAT_B5G6R5_UNORM_PACK16
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BGR_565       = 14,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRX_4444
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BGRX_4444     = 15,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRA_4444
     *   Vulkan: VK_FORMAT_B4G4R4A4_UNORM_PACK16
     *   OpenGL ES: GL_RGBA4
     */
    ANGLE_OHOS_FORMAT_BGRA_4444     = 16,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRX_5551
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BGRX_5551     = 17,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRA_5551
     *   Vulkan: VK_FORMAT_B5G5R5A1_UNORM_PACK16
     *   OpenGL ES: GL_RGB5_A1
     */
    ANGLE_OHOS_FORMAT_BGRA_5551     = 18,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRX_8888
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BGRX_8888     = 19,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BGRA_8888
     *   Vulkan: VK_FORMAT_B8G8R8A8_UNORM
     *   OpenGL ES: GL_BGRA8_EXT
     */
    ANGLE_OHOS_FORMAT_BGRA_8888     = 20,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YUV_422_I
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YUV_422_I     = 21,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCBCR_422_SP
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCBCR_422_SP  = 22,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCRCB_422_SP
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCRCB_422_SP  = 23,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCBCR_420_SP
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCBCR_420_SP  = 24,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCRCB_420_SP
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCRCB_420_SP  = 25,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCBCR_422_P
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCBCR_422_P   = 26,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCRCB_422_P
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCRCB_422_P   = 27,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCBCR_420_P
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCBCR_420_P   = 28,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCRCB_420_P
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCRCB_420_P   = 29,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YUYV_422_PKG
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YUYV_422_PKG  = 30,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_UYVY_422_PKG
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_UYVY_422_PKG  = 31,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YVYU_422_PKG
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YVYU_422_PKG  = 32,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_VYUY_422_PKG
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_VYUY_422_PKG  = 33,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA_1010102
     *   Vulkan: VK_FORMAT_A2B10G10R10_UNORM_PACK32
     *   OpenGL ES: GL_RGB10_A2
     */
    ANGLE_OHOS_FORMAT_RGBA_1010102  = 34,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCBCR_P010
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCBCR_P010   = 35,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_YCRCB_P010
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_YCRCB_P010   = 36,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RAW10
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_RAW10        = 37,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BLOB
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BLOB        = 38,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_RGBA16_FLOAT
     *   Vulkan: VK_FORMAT_R16G16B16A16_SFLOAT
     *   OpenGL ES: GL_RGBA16F
     */
    ANGLE_OHOS_FORMAT_RGBA16_FLOAT  = 39,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_VERNDER_MASK
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_VENDER_MASK   = 0X7FFF0000,

    /**
     * Corresponding formats:
     *   OHOS: GRAPHIC_PIXEL_FMT_BUTT
     *   Vulkan: ??
     *   OpenGL ES: ??
     */
    ANGLE_OHOS_FORMAT_BUTT          = 0X7FFFFFFF
};
// clang-format on


void *ClientBufferToOHNativeBuffer(EGLClientBuffer clientBuffer);

void GetOHNativeBufferProperties(void *buffer,
                                      int *width,
                                      int *height,
                                      int *depth,
                                      int *pixelFormat,
                                      int *usage);

GLenum NativePixelFormatToGLInternalFormat(int pixelFormat);

bool NativePixelFormatIsYUV(int pixelFormat);

}  // namespace ohos
}  // namespace angle

#endif // COMMON_OHOSUTIL_H_



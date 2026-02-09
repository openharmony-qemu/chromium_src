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

#include "../../../../../../../../../src/third_party/angle/src/common/platform.h"
#include "ohos_util.h"

#if defined(ANGLE_PLATFORM_OHOS)
#include "../../../../../../../../../src/third_party/ohos_ndk/includes/ohos_adapter/hilog_adapter.h"
#include "../../../../../../../../../src/third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace angle
{

namespace ohos
{

namespace {

#if defined(ANGLE_PLATFORM_OHOS)
class OHNativeBufferConfigAdapter : public OHOS::NWeb::NativeBufferConfigAdapter {
public:
    OHNativeBufferConfigAdapter() = default;

    ~OHNativeBufferConfigAdapter() override = default;

    int GetBufferWidth() override { return mWidth; }

    int GetBufferHeight() override { return mHeight; }

    int GetBufferFormat() override { return mFormat; }

    int GetBufferUsage() override { return mUsage; }

    int GetBufferStride() override { return mStride; }

    void SetBufferWidth(int width) override { mWidth = width; }

    void SetBufferHeight(int height) override { mHeight = height; }

    void SetBufferFormat(int format) override { mFormat = format; }

    void SetBufferUsage(int usage) override { mUsage = usage; }

    void SetBufferStride(int stride) override { mStride = stride; }

private:
    int mWidth = 0;
    int mHeight = 0;
    int mFormat = 0;
    int mUsage = 0;
    int mStride = 0;
};
#endif

GLenum GetPixelFormatInfo(int pixelFormat, bool *isYUV) {
    *isYUV = false;
    switch (pixelFormat) {
        case ANGLE_OHOS_FORMAT_RGBA_8888:
            return GL_RGBA8;
        case ANGLE_OHOS_FORMAT_RGBX_8888:
            return GL_RGB8;
        case ANGLE_OHOS_FORMAT_RGB_888:
            return GL_RGB8;
        case ANGLE_OHOS_FORMAT_RGB_565:
            return GL_RGB565;
        case ANGLE_OHOS_FORMAT_BGRA_8888:
            return GL_BGRA8_EXT;
        case ANGLE_OHOS_FORMAT_BGRA_5551:
            return GL_RGB5_A1;
        case ANGLE_OHOS_FORMAT_BGRA_4444:
            return GL_RGBA4;
        case ANGLE_OHOS_FORMAT_RGBA16_FLOAT:
            return GL_RGBA16F;
        case ANGLE_OHOS_FORMAT_RGBA_1010102:
            return GL_RGB10_A2;
        case ANGLE_OHOS_FORMAT_BLOB:
            return GL_NONE;
        case ANGLE_OHOS_FORMAT_YCBCR_420_SP:
            *isYUV = true;
            return GL_RGB8;
        default:
            // Treat unknown formats as RGB. They are vendor-specific YUV formats that would sample
            // as RGB.
            *isYUV = true;
            return GL_RGB8;
    }
}

}  // anonymous namespace

void *ClientBufferToOHNativeBuffer(EGLClientBuffer clientBuffer) {
#if defined(ANGLE_PLATFORM_OHOS)
    void* buffer;
    if (!OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter()
    .NativeBufferFromNativeWindowBuffer(clientBuffer, &buffer)) {
      return buffer;
    } else {
      OHOS::NWeb::HiLogAdapter::PrintLog(OHOS::NWeb::LogLevelAdapter::ERROR, "ANGLE-chromium ", "%{public}s",
      "NativeBufferFromNativeWindowBuffer fails");
    }

#endif
    return nullptr;
}

void GetOHNativeBufferProperties(void *buffer,
                                      int *width,
                                      int *height,
                                      int *depth,
                                      int *pixelFormat,
                                      int *usage) {
#if defined(ANGLE_PLATFORM_OHOS)
    if (!buffer) {
        OHOS::NWeb::HiLogAdapter::PrintLog(OHOS::NWeb::LogLevelAdapter::ERROR, "ANGLE-chromium ", "%{public}s",
            "No buffer");
        return;
    }

    auto config = std::make_shared<OHNativeBufferConfigAdapter>();
    if (!config) {
        OHOS::NWeb::HiLogAdapter::PrintLog(OHOS::NWeb::LogLevelAdapter::ERROR, "ANGLE-chromium ", "%{public}s",
            "No config");
        return;
    }

    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter().Describe(config, buffer);
    *width = config->GetBufferWidth();
    *height = config->GetBufferHeight();
    *depth = 1; // TODO
    *pixelFormat = config->GetBufferFormat();
    *usage = config->GetBufferUsage();
#endif
}

GLenum NativePixelFormatToGLInternalFormat(int pixelFormat) {
    bool isYuv = false;
    return GetPixelFormatInfo(pixelFormat, &isYuv);
}

bool NativePixelFormatIsYUV(int pixelFormat) {
    bool isYuv = false;
    GetPixelFormatInfo(pixelFormat, &isYuv);
    return isYuv;
}

}  // namespace ohos
}  // namespace angle

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

#include "ohos_image_encoder_adapter.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <multimedia/image_framework/image/image_common.h>
#include <native_color_space_manager/native_color_space_manager.h>

namespace OHOS {
namespace NWeb {
OhosImageEncoderAdapter& OhosImageEncoderAdapter::GetInstance()
{
    static OhosImageEncoderAdapter instance;
    return instance;
}

std::shared_ptr<OH_PackingOptions> OhosImageEncoderAdapter::CreatePackingOptions()
{
    OH_PackingOptions* options = nullptr;
    Image_ErrorCode errorCode = OH_PackingOptions_Create(&options);
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_PackingOptions_Create failed");
        return nullptr;
    }

    std::shared_ptr<OH_PackingOptions> optionsPtr(options, [](OH_PackingOptions* options) {
        OH_PackingOptions_Release(options);
    });

    // 100 is the number of image quality
    OH_PackingOptions_SetQuality(optionsPtr.get(), 100);
    Image_MimeType format = { .data = const_cast<char*>(MIME_TYPE_HEIC), .size = strlen(MIME_TYPE_HEIC) };
    OH_PackingOptions_SetMimeType(optionsPtr.get(), &format);
    return optionsPtr;
}

std::shared_ptr<OH_PixelmapNative> OhosImageEncoderAdapter::CreatePixelmap(
    const SkBitmap& bitmap)
{
    OH_Pixelmap_InitializationOptions* createOpts = nullptr;
    Image_ErrorCode errorCode = OH_PixelmapInitializationOptions_Create(&createOpts);
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_PixelmapInitializationOptions_Create failed");
        return nullptr;
    }

    std::shared_ptr<OH_Pixelmap_InitializationOptions> optionsPtr(createOpts,
        [](OH_Pixelmap_InitializationOptions* createOpts) {
            OH_PixelmapInitializationOptions_Release(createOpts);
    });

    OH_PixelmapInitializationOptions_SetSrcPixelFormat(optionsPtr.get(), PIXEL_FORMAT_RGBA_8888);
    OH_PixelmapInitializationOptions_SetWidth(optionsPtr.get(), bitmap.width());
    OH_PixelmapInitializationOptions_SetHeight(optionsPtr.get(), bitmap.height());
    OH_PixelmapInitializationOptions_SetPixelFormat(optionsPtr.get(), PIXEL_FORMAT_RGBA_8888);
    OH_PixelmapInitializationOptions_SetAlphaType(optionsPtr.get(), PIXELMAP_ALPHA_TYPE_PREMULTIPLIED);

    OH_PixelmapNative* pixelmap = nullptr;
    size_t dataSize = bitmap.height() * bitmap.rowBytes();
    errorCode = OH_PixelmapNative_CreatePixelmap(static_cast<uint8_t*>(bitmap.getPixels()),
        dataSize, optionsPtr.get(), &pixelmap);
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_PixelmapNative_CreatePixelmap failed, errorCode: %{public}d", errorCode);
        return nullptr;
    }

    std::shared_ptr<OH_PixelmapNative> pixelmapPtr(pixelmap, [](OH_PixelmapNative* pixelmap) {
        OH_PixelmapNative_Release(pixelmap);
    });

    return pixelmapPtr;
}

bool OhosImageEncoderAdapter::Encode(const SkBitmap& bitmap, const std::string& path)
{
    WVLOG_I("blankless Encode start.");
    // create Pixelmap
    std::shared_ptr<OH_PixelmapNative> pixelmap = CreatePixelmap(bitmap);
    if (pixelmap == nullptr) {
        WVLOG_E("blankless CreatePixelmap failed");
        return false;
    }

    ColorSpaceName setColorSpaceName = SRGB;
    OH_NativeColorSpaceManager* setColorSpaceNative = OH_NativeColorSpaceManager_CreateFromName(setColorSpaceName);
    if (setColorSpaceNative == nullptr) {
        WVLOG_E("blankless OH_NativeColorSpaceManager_CreateFromName failed");
        return false;
    }
    std::shared_ptr<OH_NativeColorSpaceManager> colorSpaceNativePtr(setColorSpaceNative,
        [](OH_NativeColorSpaceManager* setColorSpaceNative) {
            OH_NativeColorSpaceManager_Destroy(setColorSpaceNative);
    });

    Image_ErrorCode errorCode = OH_PixelmapNative_SetColorSpaceNative(pixelmap.get(), colorSpaceNativePtr.get());
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_PixelmapNative_SetColorSpaceNative failed, errorCode: %{public}d", errorCode);
        return false;
    }

    OH_ImagePackerNative* imagePacker = nullptr;
    errorCode = OH_ImagePackerNative_Create(&imagePacker);
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_ImagePackerNative_Create failed, errorCode: %{public}d", errorCode);
        return false;
    }
    std::shared_ptr<OH_ImagePackerNative> imagePackerPtr(imagePacker, [](OH_ImagePackerNative* imagePacker) {
        OH_ImagePackerNative_Release(imagePacker);
    });

    std::shared_ptr<OH_PackingOptions> optionsPtr = CreatePackingOptions();
    if (optionsPtr == nullptr) {
        WVLOG_E("blankless CreatePackingOptions failed");
        return false;
    }

    int fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        WVLOG_E("blankless open failed, errno: %{public}d", errno);
        return false;
    }
    errorCode = OH_ImagePackerNative_PackToFileFromPixelmap(imagePackerPtr.get(), optionsPtr.get(), pixelmap.get(), fd);
    close(fd);
    if (errorCode != IMAGE_SUCCESS) {
        WVLOG_E("blankless OH_ImagePackerNative_PackToFileFromPixelmap failed, errorCode: %{public}d", errorCode);
        return false;
    }
    WVLOG_D("blankless Encode success, path:%{public}s", path.c_str());
    return true;
}

}  // namespace NWeb
}  // namespace OHOS
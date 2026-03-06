/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_image_decoder_adapter_impl.h"
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <unordered_map>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "fstream"
#include "istream"
#include "sstream"
#include "string"
#include "third_party/bounds_checking_function/include/securec.h"

namespace OHOS {
namespace NWeb {

const int64_t OHMEDIA_NAME_SIZE = 256;
const std::unordered_map<AllocatorType, IMAGE_ALLOCATOR_TYPE> ALLOC_TYPE_MAP = {
    { AllocatorType::kDmaAlloc, IMAGE_ALLOCATOR_TYPE_DMA },
    { AllocatorType::kShareMemAlloc, IMAGE_ALLOCATOR_TYPE_SHARE_MEMORY },
};

IMAGE_ALLOCATOR_TYPE GetImageAllocType(AllocatorType type)
{
    auto item = ALLOC_TYPE_MAP.find(type);
    if (item == ALLOC_TYPE_MAP.end()) {
        WVLOG_E("allocator type not found");
        return IMAGE_ALLOCATOR_TYPE_AUTO;
    }
    return item->second;
}

const std::string IMAGE_HEIF = "image/heif";

void ReleaseDecodeOptions(OH_DecodingOptions* decodeOptions)
{
    if (decodeOptions) {
        Image_ErrorCode errorCode = OH_DecodingOptions_Release(decodeOptions);
        decodeOptions = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] ReleaseDecodeOptions failed, errorCode %{public}d", errorCode);
        }
        WVLOG_D("[HeifSupport] ReleaseDecodeOptions success");
    }
    WVLOG_D("[HeifSupport] ReleaseDecodeOptions options is null, do not need release");
}

// LCOV_EXCL_START
void OhosImageDecoderAdapterImpl::NativeBufferFromPixelMap()
{
    if (pixelMap_) {
        if (nativeBuffer_) {
            WVLOG_I("[HeifSupport] NativeBufferFromPixelMap nativeBuffer_ already exists, return");
            return;
        }
        Image_ErrorCode errorCode = OH_PixelmapNative_GetNativeBuffer(pixelMap_, &nativeBuffer_);
        if (errorCode == Image_ErrorCode::IMAGE_SUCCESS) {
            return;
        }
        nativeBuffer_ = nullptr;
        WVLOG_E("[HeifSupport] NativeBufferFromPixelMap GetNativeBuffer failed, errorCode %{public}d", errorCode);
        return;
    }
    WVLOG_E("[HeifSupport] NativeBufferFromPixelMap GetNativeBuffer pixelMap_ not exists");
}
// LCOV_EXCL_STOP

bool OhosImageDecoderAdapterImpl::ParseRawData(const uint8_t* data, uint32_t size)
{
    if (data == nullptr || size == 0) {
        WVLOG_E("[HeifSupport] invalid raw data.");
        return false;
    }
    Image_ErrorCode errorCode = OH_ImageSourceNative_CreateFromData(const_cast<uint8_t*>(data), size, &imageSource_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] ParseRawData create imageSource failed, errorCode %{public}d", errorCode);
        return false;
    }
    if (!imageInfo_) {
        errorCode = OH_ImageSourceInfo_Create(&imageInfo_);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] ParseRawData create imageInfo failed, errorCode %{public}d", errorCode);
            return false;
        }
    }
    errorCode = OH_ImageSourceNative_GetImageInfo(imageSource_, 0, imageInfo_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] ParseRawData GetImageInfo failed, errorCode %{public}d", errorCode);
        return false;
    }
    return true;
}

OhosImageDecoderAdapterImpl::OhosImageDecoderAdapterImpl()
{
    has_lock_pixelmap_ = false;
    Image_ErrorCode errorCode = OH_PixelmapInitializationOptions_Create(&opt_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] init create options failed, errorCode %{public}d", errorCode);
    }
    errorCode = OH_ImageSourceInfo_Create(&imageInfo_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] init create imageInfo failed, errorCode %{public}d", errorCode);
    }
}

OhosImageDecoderAdapterImpl::~OhosImageDecoderAdapterImpl()
{
    ReleasePixelMap();
}

bool OhosImageDecoderAdapterImpl::ParseImageInfo(const uint8_t* data, uint32_t size)
{
    WVLOG_D("[HeifSupport] ParseImageInfo size = %{public}u", size);
    return ParseRawData(data, size);
}

std::string OhosImageDecoderAdapterImpl::GetEncodedFormat()
{
    WVLOG_W("[HeifSupport] GetEncodedFormat do not implement, return default = %{public}s", IMAGE_HEIF.c_str());
    return IMAGE_HEIF;
}

int32_t OhosImageDecoderAdapterImpl::GetImageWidth()
{
    uint32_t width = 0;
    Image_ErrorCode errorCode = OH_ImageSourceInfo_GetWidth(imageInfo_, &width);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] GetImageWidth failed, errorCode %{public}d", errorCode);
    }
    WVLOG_D("[HeifSupport] GetImageWidth = %{public}u", width);
    return width;
}

int32_t OhosImageDecoderAdapterImpl::GetImageHeight()
{
    uint32_t height = 0;
    Image_ErrorCode errorCode = OH_ImageSourceInfo_GetHeight(imageInfo_, &height);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] GetImageHeight failed, errorCode %{public}d", errorCode);
    }
    WVLOG_D("[HeifSupport] GetImageHeight = %{public}u", height);
    return height;
}

bool OhosImageDecoderAdapterImpl::DecodeToPixelMap(const uint8_t* data, uint32_t size)
{
    WVLOG_D("[HeifSupport] DecodeToPixelMap size = %{public}d", size);
    return Decode(data, size, AllocatorType::kDmaAlloc, false);
}

bool OhosImageDecoderAdapterImpl::Decode(const uint8_t* data,
                                         uint32_t size,
                                         AllocatorType type,
                                         bool useYuv)
{
    WVLOG_D("[HeifSupport] Decode size = %{public}u, type = %{public}d, useYuv = %{public}d",
        size, static_cast<int>(type), useYuv);
    if (!ParseRawData(data, size)) {
        WVLOG_E("[HeifSupport] Decode, fail to get image source.");
        return false;
    }
    OH_DecodingOptions* decodeOptions;
    Image_ErrorCode errorCode = OH_DecodingOptions_Create(&decodeOptions);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode create decode options failed, errorCode %{public}d", errorCode);
        return false;
    }
    errorCode = OH_DecodingOptions_SetPixelFormat(decodeOptions,
        useYuv ? PIXEL_FORMAT::PIXEL_FORMAT_NV12 : PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode set pixel format failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    errorCode = OH_ImageSourceNative_CreatePixelmapUsingAllocator(imageSource_, decodeOptions,
        GetImageAllocType(type), &pixelMap_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode create pixel map failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    SetMemoryName(type);
    ReleaseDecodeOptions(decodeOptions);
    return true;
}

void OhosImageDecoderAdapterImpl::SetMemoryName(AllocatorType type) {
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] SetMemoryName pixelMap_ is null.");
        return;
    }
    std::string width = std::to_string(GetImageWidth());
    std::string height = std::to_string(GetImageHeight());
    std::string memoryNameSrc = "web-";
    memoryNameSrc.append(width + "x" + height + "-heif");
    size_t nameLen = memoryNameSrc.length();
    if (nameLen > OHMEDIA_NAME_SIZE - 1) {
        WVLOG_E("[HeifSupport] Error: Name Size is too large to set");
        return;
    }
    char memoryName[OHMEDIA_NAME_SIZE];
    errno_t err = strncpy_s(memoryName, OHMEDIA_NAME_SIZE, memoryNameSrc.c_str(), nameLen);
    if (err != EOK) {
        WVLOG_E("[HeifSupport] Copy name error!");
        return;
    }
    auto errorCode = OH_PixelmapNative_SetMemoryName(pixelMap_, memoryName, &nameLen);
    WVLOG_I("[HeifSupport] pixel map type: %{public}d. Name is %{public}s", GetImageAllocType(type), memoryName);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] set memory name failed, errorCode %{public}d", errorCode);
        return;
    }
}

// LCOV_EXCL_START
int32_t OhosImageDecoderAdapterImpl::GetFd()
{
    if (!GetBufferHandle()) {
        WVLOG_E("[HeifSupport] GetFd bufferHandle is null.");
        return -1;
    }
    WVLOG_D("[HeifSupport] GetFd %{public}d", bufferHandle_->fd);
    return bufferHandle_->fd;
}

int32_t OhosImageDecoderAdapterImpl::GetStride()
{
    if (pixelMap_) {
        WVLOG_D("[HeifSupport] OhosImageDecoderAdapterImpl::GetStride. share mem get row stride.");
        OH_Pixelmap_ImageInfo *srcInfo = nullptr;
        OH_PixelmapImageInfo_Create(&srcInfo);
        OH_PixelmapNative_GetImageInfo(pixelMap_, srcInfo);
        uint32_t rowStride;
        OH_PixelmapImageInfo_GetRowStride(srcInfo, &rowStride);
        OH_PixelmapImageInfo_Release(srcInfo);
        return rowStride;
    }
    return 0;
}

int32_t OhosImageDecoderAdapterImpl::GetOffset()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] GetOffset. PixelMap is null.");
        return 0;
    }
    NativeBufferFromPixelMap();
    if (nativeBuffer_) {
        OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
        void* virAddr;
        int32_t errorCode = OH_NativeBuffer_MapPlanes(nativeBuffer_, &virAddr, nativeBufferPlanes);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] GetOffset get planes failed, errorCode %{public}d", errorCode);
            delete nativeBufferPlanes;
            return 0;
        }
        int32_t offset = nativeBufferPlanes->planes[0].offset;
        delete nativeBufferPlanes;
        WVLOG_D("[HeifSupport] GetOffset %{public}d", offset);
        return offset;
    }
    WVLOG_E("[HeifSupport] GetOffset native buffer is null");
    return 0;
}

uint64_t OhosImageDecoderAdapterImpl::GetSize()
{
    if (!GetBufferHandle()) {
        WVLOG_E("[HeifSupport] GetSize bufferHandle is null.");
        return 0;
    }
    WVLOG_D("[HeifSupport] GetSize %{public}d", bufferHandle_->size);
    return bufferHandle_->size;
}

void* OhosImageDecoderAdapterImpl::GetNativeWindowBuffer()
{
    if (!CreateNativeWindowBuffer()) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer failed, return nullptr");
        return nullptr;
    }
    return static_cast<void*>(nativeWindowBuffer_);
}

// Used for NV12
int32_t OhosImageDecoderAdapterImpl::GetPlanesCount()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] GetPlanesCount. PixelMap is null.");
        return 0;
    }
    NativeBufferFromPixelMap();
    if (nativeBuffer_) {
        OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
        void* virAddr;
        int32_t errorCode = OH_NativeBuffer_MapPlanes(nativeBuffer_, &virAddr, nativeBufferPlanes);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] GetPlanesCount get planes failed, errorCode %{public}d", errorCode);
            delete nativeBufferPlanes;
            return 0;
        }
        int32_t planeCount = static_cast<int32_t>(nativeBufferPlanes->planeCount);
        delete nativeBufferPlanes;
        WVLOG_D("[HeifSupport] GetPlanesCount %{public}d", planeCount);
        return planeCount;
    }
    WVLOG_E("[HeifSupport] GetPlanesCount native buffer is null");
    return 0;
}

void OhosImageDecoderAdapterImpl::ReleasePixelMap()
{
    WVLOG_I("[HeifSupport] OhosImageDecoderAdapterImpl release pixelmap and native window buffer.");
    if (imageSource_) {
        Image_ErrorCode errorCode = OH_ImageSourceNative_Release(imageSource_);
        imageSource_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_ImageSourceNative_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (imageInfo_) {
        Image_ErrorCode errorCode = OH_ImageSourceInfo_Release(imageInfo_);
        imageInfo_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_ImageSourceInfo_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (pixelMap_) {
        if (has_lock_pixelmap_) {
            has_lock_pixelmap_ = false;
            Image_ErrorCode errorCode = OH_PixelmapNative_UnaccessPixels(pixelMap_);
            if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
                WVLOG_E("[HeifSupport] OH_PixelmapNative_UnaccessPixels failed, errorCode = %{public}d", errorCode);
            }
        }
        Image_ErrorCode errorCode = OH_PixelmapNative_Release(pixelMap_);
        pixelMap_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_PixelmapNative_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (nativeWindowBuffer_) {
        OH_NativeWindow_DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
    if (nativeBuffer_) {
        int errorCode = OH_NativeBuffer_Unreference(nativeBuffer_);
        nativeBuffer_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_NativeBuffer_Unreference failed, errorCode = %{public}d", errorCode);
        }
    }
    if (opt_) {
        Image_ErrorCode errorCode = OH_PixelmapInitializationOptions_Release(opt_);
        opt_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_PixelmapInitializationOptions_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (bufferHandle_) {
        WVLOG_I("[HeifSupport] OhosImageDecoderAdapterImpl bufferHandle_ unreference");
        int32_t errorCode = OH_NativeWindow_NativeObjectUnreference(bufferHandle_);
        bufferHandle_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_NativeWindow_DestroyNativeWindowBuffer failed, errorCode = %{public}d", errorCode);
        }
    }
}

bool OhosImageDecoderAdapterImpl::CreateNativeWindowBuffer()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer PixelMap is nullptr");
        return false;
    }
    NativeBufferFromPixelMap();
    if (!nativeBuffer_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer nativeBuffer is nullptr");
        return false;
    }
    nativeWindowBuffer_ = OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(nativeBuffer_);
    if (!nativeWindowBuffer_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer create nativeWindowBuffer failed.");
        return false;
    }
    return true;
}

bool OhosImageDecoderAdapterImpl::GetBufferHandle()
{
    if (bufferHandle_) {
        return true;
    }
    if (!nativeWindowBuffer_ && !CreateNativeWindowBuffer()) {
        WVLOG_E("[HeifSupport] GetBufferHandle, nativeWindowBuffer get and create both failed.");
        return false;
    }
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(nativeWindowBuffer_);
    if (!bufferHandle_) {
        WVLOG_E("[HeifSupport] GetBufferHandle, get bufferHandle failed.");
        return false;
    }
    return true;
}

void* OhosImageDecoderAdapterImpl::GetDecodeData()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] OhosImageDecoderAdapterImpl::GetDecodeData. PixelMap is null.");
        return nullptr;
    }

    void* ptr = nullptr;
    Image_ErrorCode errorCode = OH_PixelmapNative_AccessPixels(pixelMap_, &ptr);
    has_lock_pixelmap_ = true;
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] OhosImageDecoderAdapterImpl::GetDecodeData. get PixelMap data fail.");
        return nullptr;
    }

    return ptr;
}
// LCOV_EXCL_STOP

bool OhosImageDecoderAdapterImpl::DecodeByPath(const std::string& path, AllocatorType type)
{
    if (path.empty()) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath path is empty");
        return false;
    }

    WVLOG_D("[HeifSupport] blankless DecodeByPath path =%{public}s", path.c_str());
    char fullPath[PATH_MAX];
    if (realpath(path.c_str(), fullPath) == nullptr) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath realpath failed, errno = %{public}d", errno);
        return false;
    }

    size_t size = strlen(fullPath);
    WVLOG_D("[HeifSupport] blankless DecodeByPath fullPath =%{public}s, size = %{public}zu", fullPath, size);
    Image_ErrorCode errorCode = OH_ImageSourceNative_CreateFromUri(fullPath, size, &imageSource_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath create imageSource from uri failed, errorCode %{public}d",
                errorCode);
        return false;
    }
    if (!imageInfo_) {
        errorCode = OH_ImageSourceInfo_Create(&imageInfo_);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] blankless DecodeByPath create imageinfo failed, errorCode %{public}d", errorCode);
            return false;
        }
    }
    errorCode = OH_ImageSourceNative_GetImageInfo(imageSource_, 0, imageInfo_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath GetImageInfo failed, errorCode %{public}d", errorCode);
        return false;
    }

    OH_DecodingOptions* decodeOptions;
    errorCode = OH_DecodingOptions_Create(&decodeOptions);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath create decode options failed, errorCode %{public}d", errorCode);
        return false;
    }
    errorCode = OH_DecodingOptions_SetPixelFormat(decodeOptions, PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath set pixel format failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    errorCode = OH_ImageSourceNative_CreatePixelmapUsingAllocator(imageSource_, decodeOptions,
        GetImageAllocType(type), &pixelMap_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] blankless DecodeByPath create pixel map failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    SetMemoryName(type);
    ReleaseDecodeOptions(decodeOptions);
    return true;
}

}  // namespace NWeb
}  // namespace OHOS
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

#ifdef SK_HAS_HEIF_LIBRARY
#include "SkHeifDecoderAPIOhos.h"

#include <sys/mman.h>
#include <fstream>

#include "base/logging.h"
#include "base/containers/span.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

std::unique_ptr<OHOS::NWeb::OhosImageDecoderAdapter> HeifDecoder::decoderAdapter_;

#define HEIF_BYTES_PER_PIXEL_RGBA_8888 4

bool HeifDecoder::init(std::unique_ptr<SkStream> stream, HeifFrameInfo* heifInfo)
{
    auto skData = SkData::MakeFromStream(stream.get(), stream->getLength());
    if (skData == nullptr) {
        LOG(ERROR) << "[HeifSupport] HeifDecoder::Init skData is null.";
        return false;
    }

    base::span<const uint8_t> encodedData = base::make_span(skData->bytes(), skData->size());
    if (!getDecoderAdapter()->ParseImageInfo(encodedData.data(), (uint32_t)encodedData.size())) {
        LOG(ERROR) << "[HeifSupport] HeifDecoder::Init ParseImageInfo failed.";
        return false;
    }

    heifInfo->mHeight = getDecoderAdapter()->GetImageHeight();
    heifInfo->mWidth = getDecoderAdapter()->GetImageWidth();

    data_ = std::move(skData);
    return true;
}

bool HeifDecoder::decode(HeifFrameInfo* heifInfo)
{
    if (!heifInfo) {
        return false;
    }
    bool useYuv = (colorFormat_ == kHeifColorFormat_RGBA_8888) ? false : true;
    if (!useYuv) {
        heifInfo->mBytesPerPixel = HEIF_BYTES_PER_PIXEL_RGBA_8888;
    }

    return getDecoderAdapter()->Decode((const uint8_t*)data_->bytes(), (uint32_t)data_->size(),
                                       OHOS::NWeb::AllocatorType::kDmaAlloc, useYuv);
}

bool HeifDecoder::setOutputColor(HeifColorFormat colorFormat)
{
    colorFormat_ = colorFormat;
    return true;
}

void* HeifDecoder::getDecodeData(uint64_t& size)
{
    size = getDecoderAdapter()->GetSize();
    void* ptr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE,
                     getDecoderAdapter()->GetFd(), getDecoderAdapter()->GetOffset());
    if (ptr == MAP_FAILED) {
        return nullptr;
    }
    return ptr;
}

int32_t HeifDecoder::getStride()
{
    return getDecoderAdapter()->GetStride();
}

void HeifDecoder::closeDecodeData(void* ptr, uint64_t size)
{
    munmap(ptr, size);
    return;
}

OHOS::NWeb::OhosImageDecoderAdapter* HeifDecoder::getDecoderAdapter()
{
    if (!decoderAdapter_) {
        decoderAdapter_ = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateOhosImageDecoderAdapter();
    }

    return decoderAdapter_.get();
}

#endif // SK_HAS_HEIF_LIBRARY
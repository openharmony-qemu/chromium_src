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

#include "arkweb/ohos_adapter_ndk/ohos_image_adapter/ohos_image_decoder_adapter_impl.h"
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
const std::string DEFAULT_MOUSE_DRAG_IMAGE { "/system/etc/device_status/drag_icon/Copy_Drag.svg" };

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;
std::shared_ptr<OhosImageDecoderAdapter> CreateDecoderAdapter()
{
    return OhosAdapterHelper::GetInstance().CreateOhosImageDecoderAdapter();
}



bool ProcessImageSource(const uint8_t* data, size_t size)
{
    FuzzedDataProvider dataProvider(data, size);

    constexpr uint32_t MIN_VALID_SIZE = 16;
    auto validData = std::make_unique<uint8_t[]>(MIN_VALID_SIZE);
    validData[0] = 0x89; 
    validData[1] = 0x50;
    validData[2] = 0x4E;
    validData[3] = 0x47;
    std::shared_ptr<OhosImageDecoderAdapterImpl> adapter =
        std::make_shared<OhosImageDecoderAdapterImpl>();
    if (!adapter) {
        return false;
    }

    adapter->ParseImageInfo(validData.get(), MIN_VALID_SIZE);
    adapter->DecodeToPixelMap(validData.get(), MIN_VALID_SIZE);

    adapter->GetEncodedFormat();
    adapter->GetImageWidth();
    adapter->GetImageHeight();
    adapter->GetFd();
    adapter->GetStride();
    adapter->GetOffset();
    adapter->GetSize();
    adapter->GetNativeWindowBuffer();
    adapter->GetPlanesCount();
    adapter->GetDecodeData();
    adapter->GetPixelMap();
    adapter->ReleasePixelMap();

    return true;
}

bool ApplyOhosImageDecoderAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }

    return ProcessImageSource(data, size);
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyOhosImageDecoderAdapterFuzzTest(data, size);
    return 0;
}
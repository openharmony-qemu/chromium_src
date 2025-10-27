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

#include "ohosimagedecoder_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "image_source.h"
#include "image_type.h"
#include "media_errors.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
const std::string DEFAULT_MOUSE_DRAG_IMAGE { "/system/etc/device_status/drag_icon/Copy_Drag.svg" };

namespace OHOS {
std::shared_ptr<OhosImageDecoderAdapter> CreateDecoderAdapter()
{
    return OhosAdapterHelper::GetInstance().CreateOhosImageDecoderAdapter();
}

bool GetParametersFuzzTest(const uint8_t* data, size_t size)
{
    auto adapter = CreateDecoderAdapter();
    if (!adapter) {
        return false;
    }
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
    adapter->ReleasePixelMap();
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::GetParametersFuzzTest(data, size);
    return 0;
}

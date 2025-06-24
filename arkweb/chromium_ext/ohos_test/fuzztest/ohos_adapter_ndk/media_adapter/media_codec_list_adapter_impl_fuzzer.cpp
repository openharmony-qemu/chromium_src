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

#include "arkweb/ohos_adapter_ndk/media_adapter/include/media_codec_list_adapter_impl.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {
bool MediaCodecListAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    FuzzedDataProvider fdp(data, size);
    OHOS::NWeb::MediaCodecListAdapterImpl& mediaCodecListAdapterImpl =
        OHOS::NWeb::MediaCodecListAdapterImpl::GetInstance();

    std::string mime = fdp.ConsumeRandomLengthString(32);
    bool isEncoder = fdp.ConsumeBool();

    std::shared_ptr<OHOS::NWeb::CapabilityDataAdapter> capabilityAdapter =
        mediaCodecListAdapterImpl.GetCodecCapability(mime, isEncoder);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::MediaCodecListAdapterImplFuzzTest(data, size);
    return 0;
}
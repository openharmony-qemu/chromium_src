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

#include "arkweb/ohos_adapter_ndk/media_adapter/audio_cenc_info_adapter_impl.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 255;

bool AudioCencInfoAdapterFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<AudioCencInfoAdapterImpl> adapter =
        std::make_shared<AudioCencInfoAdapterImpl>();
    adapter->GetKeyId();
    adapter->GetKeyIdLen();
    adapter->GetIv();
    adapter->GetIvLen();
    adapter->GetAlgo();
    adapter->GetEncryptedBlockCount();
    adapter->GetSkippedBlockCount();

    adapter->GetFirstEncryptedOffset();
    adapter->GetClearHeaderLens();
    adapter->GetPayLoadLens();
    adapter->GetMode();

    uint32_t rawValue = fdp.ConsumeIntegral<uint32_t>();
    adapter->SetKeyIdLen(rawValue);

    adapter->SetIvLen(rawValue);
    adapter->SetAlgo(rawValue);
    adapter->SetEncryptedBlockCount(rawValue);
    adapter->SetSkippedBlockCount(rawValue);
    adapter->SetFirstEncryptedOffset(rawValue);
    adapter->SetMode(rawValue);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr || size == 0) {
        return 0;
    }
    OHOS::AudioCencInfoAdapterFuzzTest(data, size);
    return 0;
}
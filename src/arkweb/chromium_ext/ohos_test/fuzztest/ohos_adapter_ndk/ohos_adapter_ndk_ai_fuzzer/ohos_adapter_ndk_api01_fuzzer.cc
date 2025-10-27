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

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#define private public
#include "arkweb/ohos_adapter_ndk/media_adapter/media_codec_decoder_adapter_impl.h"
#undef private

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);

    OHOS::NWeb::MediaCodecDecoderAdapterImpl decoderAdapter;
    void *window = provider.ComsumeBool() ? nullptr : reinterpret_cast<void*>(provider.ConsumeIntegral<intptr_t>());
    decoderAdapter.SetOutputSurface(window);
    decoderAdapter.PrepareDecoder();
    decoderAdapter.StartDecoder();

    uint32_t index = provider.ConsumeIntegral<uint32_t>();
    int64_t presentationTimeUs = provider.ConsumeIntegral<int64_t>();
    int size = provider.ConsumeIntegral<int32_t>();
    int offset = provider.ConsumeIntegral<int32_t>();
    OHOS::NWeb::BufferFlag flag = static_cast<OHOS::NWeb::BufferFlag>(provider.ConsumeIntegral<int32_t>());
    decoderAdapter.QueueInputBufferDec(index, presentationTimeUs, size, offset, flag);

    decoderAdapter.FlushDecoder();
    decoderAdapter.StopDecoder();
    decoderAdapter.ResetDecoder();
    decoderAdapter.ReleaseDecoder();
    return 0;
}

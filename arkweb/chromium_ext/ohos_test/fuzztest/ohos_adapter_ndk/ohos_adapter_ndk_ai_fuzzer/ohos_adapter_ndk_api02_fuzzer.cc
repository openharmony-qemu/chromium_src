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
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#undef private

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < 10) {
        return 0;
    }

    OHOS::NWeb::MediaCodecDecoderAdapterImpl *decoderAdapter = new OHOS::NWeb::MediaCodecDecoderAdapterImpl();
    OHOS::NWeb::AudioDecoderCallbackManager *callbackManager = new OHOS::NWeb::AudioDecoderCallbackManager();
    OHOS::NWeb::AudioDecoderFormatAdapterImpl *formatAdapter = new OHOS::NWeb::AudioDecoderFormatAdapterImpl();

    decoderAdapter->PrepareDecoder();
    if (decoderAdapter.StartDecoder() != OHOS::NWeb::DecoderAdapterCode::DECODER_OK) {
        delete decoderAdapter;
        delete callbackManager;
        delete formatAdapter;
        return 0;
    }

    formatAdapter.SetChannelCount(static_cast<int32_t>data[0]);

    OH_AVCodec *codec = nullptr;
    uint32_t index = static_cast<uint32_t>(data[1]);
    OH_AVBuffer *buffer = nullptr;
    callbackManager.OnOutputBufferAvailable(codec, index, buffer, nullptr);

    int32_t errorCode = static_cast<int32_t>(data[2]);
    callbackManager->OnError(codec, errorCode, nullptr);

    decoderAdapter->ResetDecoder();
    decoderAdapter->StopDecoder();

    if (decoderAdapter.ReleaseDecoder() != OHOS::NWeb::DecoderAdapterCode::DECODER_OK) {
        delete decoderAdapter;
        delete callbackManager;
        delete formatAdapter;
        return 0;
    }
    delete decoderAdapter;
    delete callbackManager;
    delete formatAdapter;
    return 0;
}

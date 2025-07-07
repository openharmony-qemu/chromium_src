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

#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>


using namespace OHOS::NWeb;
 
namespace OHOS {
constexpr int MAX_SET_TIMESTAMP = 10000000;

bool OH_SurfaceBufferAdapterImplFuzzTest(FuzzedDataProvider* fdp)
{
    int32_t capacity = fdp->ConsumeIntegralInRange<int32_t>(1, 8);
    OH_AVBuffer* buffer = OH_AVBuffer_Create(capacity);
    OH_NativeBuffer* nativeBuffer = OH_AVBuffer_GetNativeBuffer(buffer);
    OH_NativeBuffer_Config config;
    OH_NativeBuffer_GetConfig(nativeBuffer, &config);
    auto surfaceBufferImpl = std::make_shared<OH_SurfaceBufferAdapterImpl>(buffer, config);
    surfaceBufferImpl->GetFileDescriptor();
    surfaceBufferImpl->GetWidth();
    surfaceBufferImpl->GetHeight();
    surfaceBufferImpl->GetStride();
    surfaceBufferImpl->GetFormat();
    surfaceBufferImpl->GetSize();
    surfaceBufferImpl->GetVirAddr();
    OH_AVBuffer_Destroy(buffer);
    OH_NativeBuffer_Unreference(nativeBuffer);

    return true;
}

bool OH_AudioBufferAdapterImplFuzzTest(FuzzedDataProvider* fdp)
{
    int32_t capacity = fdp->ConsumeIntegralInRange<int32_t>(1, 8);
    OH_AVBuffer* buffer = OH_AVBuffer_Create(capacity);
    int32_t timestamp = fdp->ConsumeIntegralInRange<int32_t>(1, MAX_SET_TIMESTAMP);
    int32_t sourceTypeValue = fdp->ConsumeIntegralInRange<int32_t>(-1, 3);
    OH_AudioCaptureSourceType audioSource =
        static_cast<OH_AudioCaptureSourceType>(sourceTypeValue);
    auto audioBufferImpl = std::make_shared<OH_AudioBufferAdapterImpl>(
        buffer, timestamp, audioSource);
    audioBufferImpl->SetBuffer(audioBufferImpl->avBuffer_);
    int32_t audioLength = fdp->ConsumeIntegralInRange<int32_t>(1, 1000);
    audioBufferImpl->SetLength(audioLength);
    int32_t audioTimestamp = fdp->ConsumeIntegralInRange<int32_t>(1, MAX_SET_TIMESTAMP);
    audioBufferImpl->SetTimestamp(audioTimestamp);
    int32_t audioSourceTypeValue = fdp->ConsumeIntegralInRange<int32_t>(-1, 3);
    AudioCaptureSourceTypeAdapter audioCaptureSource =
        static_cast<AudioCaptureSourceTypeAdapter>(audioSourceTypeValue);
    audioBufferImpl->SetSourcetype(audioCaptureSource);
    audioBufferImpl->GetBuffer();
    audioBufferImpl->GetLength();
    audioBufferImpl->GetTimestamp();
    audioBufferImpl->GetSourcetype();
    OH_AVBuffer_Destroy(buffer);

    return true;
}

}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    OHOS::OH_SurfaceBufferAdapterImplFuzzTest(&fdp);
    OHOS::OH_AudioBufferAdapterImplFuzzTest(&fdp);
 
    return 0;
}
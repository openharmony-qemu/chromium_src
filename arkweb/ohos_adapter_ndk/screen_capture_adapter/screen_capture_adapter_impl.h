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

#ifndef SCREEN_CAPTURE_ADAPTER_IMPL_H
#define SCREEN_CAPTURE_ADAPTER_IMPL_H

#include <graphic_adapter.h>
#include <arkweb/ohos_adapter_ndk/interfaces/screen_capture_adapter.h>
#include <multimedia/player_framework/native_avscreen_capture.h>
#include <native_buffer/native_buffer.h>
#include <queue>

namespace OHOS::NWeb {
void ScreenCaptureCallbackOnError(OH_AVScreenCapture *capture, int32_t errorCode, void* userData);

void ScreenCaptureCallbackOnBufferAvailable(OH_AVScreenCapture *capture, OH_AVBuffer *buffer,
    OH_AVScreenCaptureBufferType bufferType, int64_t timestamp, void* userData);

void ScreenCaptureCallbackOnStateChange(struct OH_AVScreenCapture *capture,
    OH_AVScreenCaptureStateCode stateCode, void* userData);

class OH_SurfaceBufferAdapterImpl : public SurfaceBufferAdapter {
private:
    void* avBuffer_ = nullptr;
    OH_NativeBuffer_Config config_;
    uint32_t size_ = 0;

public:
    explicit OH_SurfaceBufferAdapterImpl(OH_AVBuffer* avBuffer, OH_NativeBuffer_Config config);

    ~OH_SurfaceBufferAdapterImpl() override;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    void* GetVirAddr() override;
};

class OH_AudioBufferAdapterImpl : public AudioBufferAdapter {
public:
    explicit OH_AudioBufferAdapterImpl(
        OH_AVBuffer* avBuffer, int64_t timestamp, OH_AudioCaptureSourceType sourcetype);

    ~OH_AudioBufferAdapterImpl() override;

    uint8_t* GetBuffer() override;

    int32_t GetLength() override;

    int64_t GetTimestamp() override;

    AudioCaptureSourceTypeAdapter GetSourcetype() override;

    void SetBuffer(uint8_t* buffer) override;

    void SetLength(int32_t length) override;

    void SetTimestamp(int64_t timestamp) override;

    void SetSourcetype(AudioCaptureSourceTypeAdapter sourcetype) override;

    uint8_t* avBuffer_ = nullptr;
    int32_t length_ = 0;
    int64_t timestamp_ = 0;
    OH_AudioCaptureSourceType sourcetype_ = OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT;
};

struct CallbackInfo {
    std::shared_ptr<ScreenCaptureCallbackAdapter> callback;
    int nweb_id;
};

class ScreenCaptureAdapterImpl : public ScreenCaptureAdapter {
public:
    ScreenCaptureAdapterImpl() = default;
    ~ScreenCaptureAdapterImpl() override;

    int32_t Init(const std::shared_ptr<ScreenCaptureConfigAdapter> config) override {return 0;}

    int32_t SetMicrophoneEnable(bool enable) override;

    int32_t StartCapture() override;

    int32_t StopCapture() override;

    int32_t SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback) override;

    std::shared_ptr<SurfaceBufferAdapter> AcquireVideoBuffer() override;

    int32_t ReleaseVideoBuffer() override;

    int32_t AcquireAudioBuffer(
        std::shared_ptr<AudioBufferAdapter> audiobuffer, AudioCaptureSourceTypeAdapter type) override;

    int32_t ReleaseAudioBuffer(AudioCaptureSourceTypeAdapter type) override;

    int32_t InitV2(const std::shared_ptr<ScreenCaptureConfigAdapter> config, int nweb_id) override;

    void ClearBufferQueue(int nwebId) override;
private:
    void Release();

private:
    OH_AVScreenCapture *screenCapture_;
    CallbackInfo callback_info_ = {nullptr, 0};
};

}  // namespace OHOS::NWeb

#endif  // SCREEN_CAPTURE_ADAPTER_IMPL_H

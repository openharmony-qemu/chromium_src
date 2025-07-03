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

#ifndef MEDIA_OHOS_AUDIO_BASE_AUDIO_CAPTURER_SOURCE_H_
#define MEDIA_OHOS_AUDIO_BASE_AUDIO_CAPTURER_SOURCE_H_

#include "base/files/file_util.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/audio_capturer_source.h"
#include "media/base/media_export.h"
#include "content/common/content_export.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_read_callback.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_source_bridge.h"

namespace base {
class SingleThreadTaskRunner;
}  // namespace base

namespace webrtc {

using namespace OHOS::NWeb;
using namespace media;

using OnAudioReadDataCallback = base::RepeatingCallback<void(OHOS::NWeb::AudioCaptureSourceTypeAdapter)>;

class BaseScreenCaptureReadCallback;

constexpr int kMaxNumOfBuffer = 2;

class AudioScreenCapturerReadCallback : public BaseScreenCaptureReadCallback {
 public:
  AudioScreenCapturerReadCallback(const OnAudioReadDataCallback& readDataCallback);

  ~AudioScreenCapturerReadCallback();

  void OnReadData() override {}

  void OnReadData(OHOS::NWeb::AudioCaptureSourceTypeAdapter type) override;

  void OnDisplaySelected(uint64_t displayId) override;
 private:
  OnAudioReadDataCallback readDataCallback_;
};

class BaseAudioCapturerSource : public AudioCapturerSource {
public:

    BaseAudioCapturerSource(
        scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner, int nwebId);

    BaseAudioCapturerSource(const BaseAudioCapturerSource&) = delete;

    BaseAudioCapturerSource& operator=(const BaseAudioCapturerSource&) = delete;

    // AudioCaptureSource implementation.
    void Initialize(const AudioParameters& params,
                    CaptureCallback* callback) override;

    void Start() override;

    void Stop() override;

    void SetVolume(double volume) override;

    void SetAutomaticGainControl(bool enable) override;

    void SetOutputDeviceForAec(const std::string& output_device_id) override;

    void SetScreenCaptureState(const OHOS::NWeb::ScreenCaptureStateCodeAdapter& stateCode);

    void HandleAudioBuffer(OHOS::NWeb::AudioCaptureSourceTypeAdapter type);

private:

    ~BaseAudioCapturerSource() override;

    void NotifyCaptureError(const std::string& error);

    void NotifyCaptureStarted();

    void ReportError(const std::string& message);

    scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner_;
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;
    AudioParameters params_;
    CaptureCallback* base_callback_ = nullptr;
    base::Lock base_callback_lock_;
    FILE* dumpFile_ = nullptr;

    bool isStart_ = false;
    bool portal_init_failed_ = false;
    OHOS::NWeb::ScreenCaptureStateCodeAdapter capture_state_code_
        = OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;

    std::shared_ptr<AudioScreenCapturerReadCallback> AudioScreenCapturerReadCallback_ = nullptr;

    int nwebId_ = 0;
    base::WeakPtrFactory<BaseAudioCapturerSource> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_OHOS_AUDIO_BASE_AUDIO_CAPTURER_SOURCE_H_
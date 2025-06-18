/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MEDIA_OHOS_AUDIO_OHOS_AUDIO_CAPTURER_SOURCE_H_
#define MEDIA_OHOS_AUDIO_OHOS_AUDIO_CAPTURER_SOURCE_H_

#include "base/files/file_util.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/audio_capturer_source.h"
#include "media/base/media_export.h"
#include "ohaudio/native_audiocapturer.h"
#include "ohaudio/native_audiostreambuilder.h"

namespace base {
class SingleThreadTaskRunner;
}  // namespace base

namespace media {

using OnReadDataCallback = base::RepeatingCallback<void(void)>;

constexpr int kMaxNumOfBuffer = 2;

class AudioCapturerReadCallback {
 public:
  AudioCapturerReadCallback(const OnReadDataCallback& readDataCallback);

  ~AudioCapturerReadCallback();

  void OnReadData(size_t length);

 private:
  OnReadDataCallback readDataCallback_;
};

class MEDIA_EXPORT OHOSAudioCapturerSource final : public AudioCapturerSource {
 public:
  OHOSAudioCapturerSource(
      scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner);
  OHOSAudioCapturerSource(const OHOSAudioCapturerSource&) = delete;
  OHOSAudioCapturerSource& operator=(const OHOSAudioCapturerSource&) = delete;

  // AudioCaptureSource implementation.
  void Initialize(const AudioParameters& params,
                  CaptureCallback* callback) override;
  void Start() override;
  void Stop() override;
  void SetVolume(double volume) override;
  void SetAutomaticGainControl(bool enable) override;
  void SetOutputDeviceForAec(const std::string& output_device_id) override;
  void ReadData(void* buffer, int32_t length);

 private:
  ~OHOSAudioCapturerSource() override;

  // Called in Initialize(), create a OHOS audio capturer.
  void InitializeOnCapturerThread();

  // Called in Start(), start OHOS audio capturer.
  void StartOnCapturerThread();

  // Called in Stop(); stop OHOS audio capturer.
  void StopOnCapturerThread();

  // Called in ReadData() when the OH_AudioCapturer_OnReadData callback is
  // triggered. |buffer| is the data obtained from the microphone, and |length|
  // is the buffer length.
  void ReadDataOnCapturerThread(void* buffer, int32_t length);

  void NotifyCaptureError(const std::string& error);

  void NotifyCaptureStarted();

  // reports an error to |callback_|.
  void ReportError(const std::string& message);

  RAW_PTR_EXCLUSION OH_AudioCapturer* audio_capturer_ = nullptr;

  RAW_PTR_EXCLUSION OH_AudioStreamBuilder* audio_stream_builder_ = nullptr;

  scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner_;

  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

  AudioParameters params_;

  raw_ptr<CaptureCallback> callback_ = nullptr;

  std::shared_ptr<AudioCapturerReadCallback> audioCapturerReadCallback_ =
      nullptr;

  base::Lock callback_lock_;
};

}  // namespace media

#endif  // MEDIA_OHOS_AUDIO_OHOS_AUDIO_CAPTURER_SOURCE_H_

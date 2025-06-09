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

#include "media/audio/ohos/ohos_audio_input_stream.h"

#include "base/logging.h"
#include "media/audio/audio_device_description.h"
#include "media/audio/ohos/ohos_audio_manager.h"

namespace media {

class OHOSAudioInputStream::CaptureCallbackAdapter
    : public AudioCapturerSource::CaptureCallback {
 public:
  CaptureCallbackAdapter(AudioInputCallback* callback) : callback_(callback) {}

  void Capture(const AudioBus* audio_source,
               base::TimeTicks audio_capture_time,
               const AudioGlitchInfo& glitch_info,
               double volume,
               bool key_pressed) override {
    callback_->OnData(audio_source, audio_capture_time, volume, {});
  }

  void OnCaptureError(AudioCapturerSource::ErrorCode code,
                      const std::string& message) override {
    callback_->OnError();
  }

  void OnCaptureMuted(bool is_muted) override {}

 private:
  AudioInputCallback* callback_;
};

OHOSAudioInputStream::OHOSAudioInputStream(OHOSAudioManager* manager,
                                           const AudioParameters& parameters)
    : manager_(manager), parameters_(parameters) {}

OHOSAudioInputStream::~OHOSAudioInputStream() = default;

AudioInputStream::OpenOutcome OHOSAudioInputStream::Open() {
  return OpenOutcome::kSuccess;
}

void OHOSAudioInputStream::Start(AudioInputCallback* callback) {
  capturer_source_ =
      base::MakeRefCounted<OHOSAudioCapturerSource>(manager_->GetTaskRunner());
  callback_adapter_ = std::make_unique<CaptureCallbackAdapter>(callback);
  capturer_source_->Initialize(parameters_, callback_adapter_.get());
  capturer_source_->Start();
}

void OHOSAudioInputStream::Stop() {
  if (capturer_source_) {
    capturer_source_->Stop();
    capturer_source_ = nullptr;
  }
}

void OHOSAudioInputStream::Close() {
  Stop();
  manager_->ReleaseInputStream(this);
}

double OHOSAudioInputStream::GetMaxVolume() {
  return 1.0;
}

void OHOSAudioInputStream::SetVolume(double volume) {
  capturer_source_->SetVolume(volume);
  volume_ = volume;
}

double OHOSAudioInputStream::GetVolume() {
  return volume_;
}

bool OHOSAudioInputStream::SetAutomaticGainControl(bool enabled) {
  NOTIMPLEMENTED();
  return false;
}

bool OHOSAudioInputStream::GetAutomaticGainControl() {
  return automatic_gain_control_;
}

bool OHOSAudioInputStream::IsMuted() {
  return volume_ == 0.0;
}

void OHOSAudioInputStream::SetOutputDeviceForAec(
    const std::string& output_device_id) {
  NOTIMPLEMENTED();
}

}  // namespace media

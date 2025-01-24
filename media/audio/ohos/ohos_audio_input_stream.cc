// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_input_stream.h"

#include "base/logging.h"
#include "media/audio/audio_device_description.h"
#include "media/audio/ohos/ohos_audio_manager.h"

namespace media {
namespace {
// ID of the virtual device used to share system audio
constexpr std::string_view kScreenSystemAudioDeviceId = "screen:systemAudio:-2:0";
}

class OHOSAudioInputStream::CaptureCallbackAdapter
    : public AudioCapturerSource::CaptureCallback {
 public:
  CaptureCallbackAdapter(AudioInputCallback* callback) : callback_(callback) {}

  void Capture(const AudioBus* audio_source,
               base::TimeTicks audio_capture_time,
               double volume,
               bool key_pressed) override {
    AudioGlitchInfo audio_glitch_info;
    callback_->OnData(audio_source, audio_capture_time, volume, audio_glitch_info);
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
  LOG(INFO) << "OHOSAudioInputStream::Open";
  return OpenOutcome::kSuccess;
}

void OHOSAudioInputStream::Start(AudioInputCallback* callback) {
  std::string device_id = manager_->GetSelectAudioDeviceId();
  LOG(INFO) << "OHOSAudioInputStream::Start, format: " 
            << (int32_t)parameters_.format()
            << " , deviceid: "
            << device_id;
  
  if (device_id == kScreenSystemAudioDeviceId) {
    base_capturer_source_ =
      base::MakeRefCounted<BaseAudioCapturerSource>(manager_->GetTaskRunner());
    base_callback_adapter_ = std::make_unique<CaptureCallbackAdapter>(callback);
    base_capturer_source_->Initialize(parameters_, base_callback_adapter_.get());
    base_capturer_source_->Start();
  } else {
    capturer_source_ =
       base::MakeRefCounted<OHOSAudioCapturerSource>(manager_->GetTaskRunner());
    callback_adapter_ = std::make_unique<CaptureCallbackAdapter>(callback);
    capturer_source_->Initialize(parameters_, callback_adapter_.get());
    capturer_source_->Start();
  }
}

void OHOSAudioInputStream::Stop() {
  LOG(INFO) << "OHOSAudioInputStream::Stop";
  if (capturer_source_) {
    capturer_source_->Stop();
    capturer_source_ = nullptr;
  }
  if (base_capturer_source_) {
    base_capturer_source_->Stop();
    base_capturer_source_ = nullptr;
  }
}

void OHOSAudioInputStream::Close() {
  LOG(INFO) << "OHOSAudioInputStream::Close";
  Stop();
  manager_->ReleaseInputStream(this);
}

double OHOSAudioInputStream::GetMaxVolume() {
  return 1.0;
}

void OHOSAudioInputStream::SetVolume(double volume) {
  LOG(INFO) << "OHOSAudioInputStream::SetVolume volume is:" << volume;
  if (capturer_source_) {
    capturer_source_->SetVolume(volume);
  }
  if (base_capturer_source_) {
    base_capturer_source_->SetVolume(volume);
  }
  volume_ = volume;
}

double OHOSAudioInputStream::GetVolume() {
  return volume_;
}

bool OHOSAudioInputStream::SetAutomaticGainControl(bool enabled) {
  if (capturer_source_) {
    capturer_source_->SetAutomaticGainControl(enabled);
  }
  if (base_capturer_source_) {
    base_capturer_source_->SetAutomaticGainControl(enabled);
  }
  automatic_gain_control_ = enabled;
  return true;
}

bool OHOSAudioInputStream::GetAutomaticGainControl() {
  return automatic_gain_control_;
}

bool OHOSAudioInputStream::IsMuted() {
  return volume_ == 0.0;
}

void OHOSAudioInputStream::SetOutputDeviceForAec(
    const std::string& output_device_id) {
  if (capturer_source_) {
    capturer_source_->SetOutputDeviceForAec(output_device_id);
  }
  if (base_capturer_source_) {
    base_capturer_source_->SetOutputDeviceForAec(output_device_id);
  }
}

}  // namespace media

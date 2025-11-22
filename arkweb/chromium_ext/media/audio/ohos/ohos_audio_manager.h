// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on audio_manager_alsa.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_
#define MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_

#include "arkweb/build/features/features.h"
#include "media/audio/audio_manager_base.h"
#include "ohos_audio_input_stream.h"
#include "ohos_audio_output_stream.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_system_manager_adapter.h"

namespace media {
extern const int32_t AUDIO_DEFAULT_DEVICE_ID;

class AudioManagerDeviceChangeCallback
    : public AudioManagerDeviceChangeCallbackAdapter {
 public:
  AudioManagerDeviceChangeCallback(base::RepeatingClosure cb);
  ~AudioManagerDeviceChangeCallback();

  void OnDeviceChange() override;

 private:
  const base::RepeatingClosure outputDeviceChangeListenerCallback_;
};

class MEDIA_EXPORT OHOSAudioManager : public AudioManagerBase {
 public:
  OHOSAudioManager(std::unique_ptr<AudioThread> audio_thread,
                   AudioLogFactory* audio_log_factory);
  ~OHOSAudioManager() override;

  OHOSAudioManager(const OHOSAudioManager&) = delete;
  OHOSAudioManager& operator=(const OHOSAudioManager&) = delete;

  // Implementation of AudioManager.
  bool HasAudioOutputDevices() override;
  bool HasAudioInputDevices() override;
  void GetAudioOutputDeviceNames(AudioDeviceNames* device_names) override;
#if BUILDFLAG(ARKWEB_WEBRTC)
  void GetAudioInputDeviceNames(AudioDeviceNames* device_names) override;
  void ReleaseInputStream(AudioInputStream* stream) override;
  std::string GetSelectAudioDeviceId() override;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
  const char* GetName() override;

  // Implementation of AudioManagerBase.
  AudioOutputStream* MakeLinearOutputStream(
      const AudioParameters& params,
      const LogCallback& log_callback) override;
  AudioOutputStream* MakeLowLatencyOutputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;
  AudioInputStream* MakeLinearInputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;
  AudioInputStream* MakeLowLatencyInputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;

 protected:
  AudioParameters GetPreferredOutputStreamParameters(
      const std::string& output_device_id,
      const AudioParameters& input_params) override;

#if BUILDFLAG(ARKWEB_WEBRTC)
  AudioParameters GetPreferredInputStreamParameters(
      const std::string& input_device_id) override;

  void SelectAudioDevice(const std::string& device_id, bool isInput);
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

 private:
  bool isCommunication_ = false;
  std::shared_ptr<AudioManagerDeviceChangeCallback>
      outputDeviceChangeCallback_ = nullptr;
#if BUILDFLAG(ARKWEB_WEBRTC)
  std::string device_id_ = "";
#endif // defined(OHOS_WEBRTC)
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_

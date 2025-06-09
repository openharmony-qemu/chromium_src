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

#ifndef MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_
#define MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_

#include "media/audio/audio_manager_base.h"

namespace media {

// OHOS implementation of the AudioManager singleton.
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
  void GetAudioInputDeviceNames(AudioDeviceNames* device_names) override;
  void ReleaseInputStream(AudioInputStream* stream) override;
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
  void ReleaseOutputStream(
      AudioOutputStream* stream) override;

 protected:
  AudioParameters GetPreferredOutputStreamParameters(
      const std::string& output_device_id,
      const AudioParameters& input_params) override;

  AudioParameters GetInputStreamParameters(
      const std::string& input_device_id) override;

  void SelectAudioDevice(const std::string& device_id, bool isInput);

};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_MANAGER_H_

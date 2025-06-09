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

#include "media/audio/ohos/ohos_audio_manager.h"

#include <stdlib.h>
#include "base/command_line.h"
#include "base/logging.h"
#include "media/audio/ohos/ohos_audio_input_stream.h"
#include "media/audio/ohos/ohos_audio_output_stream.h"
#include "media/base/audio_parameters.h"
#include "media/base/media_switches.h"

namespace media {

constexpr int kDefaultSampleRate = 48000;
constexpr int kMinimumOutputBufferSize = 2048;
constexpr int kMinimumInputBufferSize = 960;
const char kAudioDefaultDeviceId[] = "defaultId";
const char AUDIO_DEFAULT_DEVICE_NAME[] = "(default)";
static const char AUDIO_MANAGER_NAME[] = "OHOS";

std::unique_ptr<AudioManager> CreateAudioManager(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory) {
  return std::make_unique<OHOSAudioManager>(std::move(audio_thread),
                                            audio_log_factory);
}

OHOSAudioManager::OHOSAudioManager(std::unique_ptr<AudioThread> audio_thread,
                                   AudioLogFactory* audio_log_factory)
    : AudioManagerBase(std::move(audio_thread), audio_log_factory) {}

OHOSAudioManager::~OHOSAudioManager() = default;

// Implementation of AudioManager.
bool OHOSAudioManager::HasAudioOutputDevices() {
  return true;
}

bool OHOSAudioManager::HasAudioInputDevices() {
  return true;
}

void OHOSAudioManager::GetAudioOutputDeviceNames(
    AudioDeviceNames* device_names) {
  AudioDeviceName device_name;
  device_name.unique_id = std::string(kAudioDefaultDeviceId);
  device_name.device_name = std::string(AUDIO_DEFAULT_DEVICE_NAME);
  device_names->push_front(device_name);
}

void OHOSAudioManager::GetAudioInputDeviceNames(
    AudioDeviceNames* device_names) {
  AudioDeviceName device_name;
  device_name.unique_id = std::string(kAudioDefaultDeviceId);
  device_name.device_name = std::string(AUDIO_DEFAULT_DEVICE_NAME);
  device_names->push_front(device_name);
}

const char* OHOSAudioManager::GetName() {
  return AUDIO_MANAGER_NAME;
}

// Implementation of AudioManagerBase.
AudioOutputStream* OHOSAudioManager::MakeLinearOutputStream(
    const AudioParameters& params,
    const LogCallback& log_callback) {
  NOTREACHED();
  return nullptr;
}

AudioOutputStream* OHOSAudioManager::MakeLowLatencyOutputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback) {
  SelectAudioDevice(device_id, false);
  return new OHOSAudioOutputStream(this, params);
}

AudioInputStream* OHOSAudioManager::MakeLinearInputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback) {
  NOTREACHED();
  return nullptr;
}

AudioInputStream* OHOSAudioManager::MakeLowLatencyInputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback) {
  SelectAudioDevice(device_id, true);
  return new OHOSAudioInputStream(this, params);
}

AudioParameters OHOSAudioManager::GetPreferredOutputStreamParameters(
    const std::string& output_device_id,
    const AudioParameters& input_params) {
  SelectAudioDevice(output_device_id, false);
  return AudioParameters(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_STEREO>(),
      kDefaultSampleRate, kMinimumOutputBufferSize);
}

AudioParameters OHOSAudioManager::GetInputStreamParameters(
    const std::string& input_device_id) {
  AudioParameters params =
      AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                      ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_STEREO>(),
                      kDefaultSampleRate, kMinimumInputBufferSize);
  params.set_effects(AudioParameters::ECHO_CANCELLER |
                     AudioParameters::NOISE_SUPPRESSION |
                     AudioParameters::AUTOMATIC_GAIN_CONTROL);
  return params;
}

void OHOSAudioManager::ReleaseInputStream(AudioInputStream* stream) {
  AudioManagerBase::ReleaseInputStream(stream);
}

void OHOSAudioManager::ReleaseOutputStream(AudioOutputStream* stream) {
  AudioManagerBase::ReleaseOutputStream(stream);
  stream = nullptr;
}

void OHOSAudioManager::SelectAudioDevice(const std::string& device_id,
                                         bool isInput) {}

}  // namespace media

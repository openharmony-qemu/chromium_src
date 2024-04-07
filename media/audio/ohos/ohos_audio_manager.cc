// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_manager.h"

#include <stdlib.h>
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/system/system_monitor.h"
#include "base/task/bind_post_task.h"
#include "media/base/media_switches.h"
#include "ohos_adapter_helper.h"

namespace media {

constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumOutputBufferSize = 2048;
#if defined(OHOS_WEBRTC)
constexpr int kMinimumInputBufferSize = 2048;
#endif  // defined(OHOS_WEBRTC)
const int32_t AUDIO_DEFAULT_DEVICE_ID = 1000000;
const char* AUDIO_DEFAULT_DEVICE_NAME = "(default)";
static const char* AUDIO_MANAGER_NAME = "OHOS";

AudioManagerDeviceChangeCallback::AudioManagerDeviceChangeCallback(
    base::RepeatingClosure cb)
    : outputDeviceChangeListenerCallback_(cb) {}

AudioManagerDeviceChangeCallback::~AudioManagerDeviceChangeCallback() {}

void AudioManagerDeviceChangeCallback::OnDeviceChange() {
  LOG(INFO) << "AudioManagerDeviceChangeCallback::OnDeviceChange";
  outputDeviceChangeListenerCallback_.Run();
  if (auto* monitor = base::SystemMonitor::Get()) {
    monitor->ProcessDevicesChanged(base::SystemMonitor::DEVTYPE_AUDIO);
  }
}

std::unique_ptr<AudioManager> CreateAudioManager(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory) {
  return std::make_unique<OHOSAudioManager>(std::move(audio_thread),
                                            audio_log_factory);
}

OHOSAudioManager::OHOSAudioManager(std::unique_ptr<AudioThread> audio_thread,
                                   AudioLogFactory* audio_log_factory)
    : AudioManagerBase(std::move(audio_thread), audio_log_factory) {}

OHOSAudioManager::~OHOSAudioManager() {
  int32_t ret = OhosAdapterHelper::GetInstance()
                    .GetAudioSystemManager()
                    .UnsetDeviceChangeCallback();
  if (ret != 0) {
    LOG(ERROR) << "OHOSAudioManager::UnsetDeviceChangeCallback failed. ret: "
               << ret;
  }
}

// Implementation of AudioManager.
bool OHOSAudioManager::HasAudioOutputDevices() {
  return OhosAdapterHelper::GetInstance()
      .GetAudioSystemManager()
      .HasAudioOutputDevices();
}

bool OHOSAudioManager::HasAudioInputDevices() {
  return OhosAdapterHelper::GetInstance()
      .GetAudioSystemManager()
      .HasAudioInputDevices();
}

void OHOSAudioManager::GetAudioOutputDeviceNames(
    AudioDeviceNames* device_names) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableAudioOutput)) {
    return;
  }
  auto audioDeviceList =
      OhosAdapterHelper::GetInstance().GetAudioSystemManager().GetDevices(
          AdapterDeviceFlag::OUTPUT_DEVICES_FLAG);
  for (auto audioDevice : audioDeviceList) {
    if (audioDevice) {
      device_names->emplace_back(
          audioDevice->GetDeviceName(),
          base::NumberToString(audioDevice->GetDeviceId()));
    }
  }
  auto defaultOutputDevice = OhosAdapterHelper::GetInstance()
                                 .GetAudioSystemManager()
                                 .GetDefaultOutputDevice();
  if (!defaultOutputDevice) {
    LOG(ERROR) << "OHOSAudioManager::GetAudioOutputDeviceNames "
                  "defaultOutputDevice is null";
    return;
  }

  std::string defaultOutputDeviceName =
      AUDIO_DEFAULT_DEVICE_NAME + defaultOutputDevice->GetDeviceName();
  AudioDeviceName device_name;
  device_name.unique_id = base::NumberToString(AUDIO_DEFAULT_DEVICE_ID);
  device_name.device_name = defaultOutputDeviceName;
  device_names->push_front(device_name);
}

#if defined(OHOS_WEBRTC)
void OHOSAudioManager::GetAudioInputDeviceNames(
    AudioDeviceNames* device_names) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableAudioInput)) {
    return;
  }
  auto audioDeviceList =
      OhosAdapterHelper::GetInstance().GetAudioSystemManager().GetDevices(
          AdapterDeviceFlag::INPUT_DEVICES_FLAG);
  for (auto audioDevice : audioDeviceList) {
    if (audioDevice) {
      device_names->emplace_back(
          audioDevice->GetDeviceName(),
          base::NumberToString(audioDevice->GetDeviceId()));
    }
  }
  auto defaultInputDevice = OhosAdapterHelper::GetInstance()
                                .GetAudioSystemManager()
                                .GetDefaultInputDevice();

  if (!defaultInputDevice) {
    LOG(ERROR) << "OHOSAudioManager::GetAudioInputDeviceNames "
                  "defaultInputDeviceName is null";
    return;
  }

  std::string defaultInputDeviceName =
      AUDIO_DEFAULT_DEVICE_NAME + defaultInputDevice->GetDeviceName();
  AudioDeviceName device_name;
  device_name.unique_id = base::NumberToString(AUDIO_DEFAULT_DEVICE_ID);
  device_name.device_name = defaultInputDeviceName;
  device_names->push_front(device_name);
}
#endif  // defined(OHOS_WEBRTC)

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
  LOG(INFO) << "OHOSAudioManager::MakeLowLatencyOutputStream";
  SelectAudioDevice(device_id, false);
  if (!outputDeviceChangeCallback_) {
    outputDeviceChangeCallback_ =
        std::make_shared<AudioManagerDeviceChangeCallback>(
            base::BindPostTaskToCurrentDefault(base::BindRepeating(
                &OHOSAudioManager::NotifyAllOutputDeviceChangeListeners,
                base::Unretained(this))));
    int32_t ret = OhosAdapterHelper::GetInstance()
                      .GetAudioSystemManager()
                      .SetDeviceChangeCallback(outputDeviceChangeCallback_);
    if (ret != 0) {
      LOG(ERROR) << "OHOSAudioManager::SetDeviceChangeCallback failed. ret: "
                 << ret;
    }
  }
  return new OHOSAudioOutputStream(this, params, isCommunication_);
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
  LOG(INFO) << "OHOSAudioManager::MakeLowLatencyInputStream";
  isCommunication_ = true;
  SelectAudioDevice(device_id, true);
  return new OHOSAudioInputStream(this, params);
}

AudioParameters OHOSAudioManager::GetPreferredOutputStreamParameters(
    const std::string& output_device_id,
    const AudioParameters& input_params) {
  LOG(INFO) << "OHOSAudioManager::GetPreferredOutputStreamParameters";
  SelectAudioDevice(output_device_id, false);
  return AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                         ChannelLayoutConfig::Guess(kDefaultChannelCount),
                         kDefaultSampleRate, kMinimumOutputBufferSize);
}

#if defined(OHOS_WEBRTC)
AudioParameters OHOSAudioManager::GetPreferredInputStreamParameters(
    const std::string& input_device_id) {
  LOG(INFO) << "OHOSAudioManager::GetPreferredInputStreamParameters";
  AudioParameters params =
      AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                      ChannelLayoutConfig::Guess(kDefaultChannelCount),
                      kDefaultSampleRate, kMinimumInputBufferSize);
  params.set_effects(AudioParameters::ECHO_CANCELLER |
                     AudioParameters::NOISE_SUPPRESSION |
                     AudioParameters::AUTOMATIC_GAIN_CONTROL);
  return params;
}

void OHOSAudioManager::ReleaseInputStream(AudioInputStream* stream) {
  AudioManagerBase::ReleaseInputStream(stream);
  isCommunication_ = false;
}

void OHOSAudioManager::SelectAudioDevice(const std::string& device_id,
                                         bool isInput) {
  if (device_id.empty()) {
    LOG(ERROR) << "OHOSAudioManager::SelectAudioDevice device_id is empty.";
    return;
  }
  LOG(INFO) << "OHOSAudioManager::SelectAudioDevice device_id is: "
            << device_id;
  int deviceId = 0;
  base::StringToInt(device_id, &deviceId);
  int32_t ret = OhosAdapterHelper::GetInstance()
                    .GetAudioSystemManager()
                    .SelectAudioDeviceById(deviceId, isInput);
  if (ret != 0) {
    LOG(ERROR) << "OHOSAudioManager::SelectAudioDevice failed. ret: " << ret;
  }
}
#endif  // defined(OHOS_WEBRTC)
}  // namespace media

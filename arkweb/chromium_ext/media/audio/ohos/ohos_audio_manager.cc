// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_audio_manager.h"

#include <stdlib.h>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/system/system_monitor.h"
#include "base/task/bind_post_task.h"
#include "media/base/media_switches.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace media {
constexpr int kPcModeMaxOutputStreams = 65536;
constexpr int kDefaultMaxOutputStreams = 10;
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumOutputBufferSize = 2048;
constexpr int kLowLatencyOutputBufferSize = 960;
#if BUILDFLAG(ARKWEB_WEBRTC)
constexpr int kMinimumInputBufferSize = 2048;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
const int32_t AUDIO_DEFAULT_DEVICE_ID = 1000000;
const char* AUDIO_DEFAULT_DEVICE_NAME = "(default)";
static const char* AUDIO_MANAGER_NAME = "OHOS";
constexpr std::string_view kScreenSystemAudioDeviceId = "screen:systemAudio:-2:0";

AudioManagerDeviceChangeCallback::AudioManagerDeviceChangeCallback(
    base::RepeatingClosure cb)
    : outputDeviceChangeListenerCallback_(cb) {}

AudioManagerDeviceChangeCallback::~AudioManagerDeviceChangeCallback() {}

void AudioManagerDeviceChangeCallback::OnDeviceChange() {
  // this callback listens the all device change event,
  // web don't need to deal with it.
  LOG(INFO) << "AudioManagerDeviceChangeCallback::OnDeviceChange";
}

std::unique_ptr<AudioManager> CreateAudioManager(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory) {
  return std::make_unique<OHOSAudioManager>(std::move(audio_thread),
                                            audio_log_factory);
}

OHOSAudioManager::OHOSAudioManager(std::unique_ptr<AudioThread> audio_thread,
                                   AudioLogFactory* audio_log_factory)
    : AudioManagerBase(std::move(audio_thread), audio_log_factory) {
      if (base::ohos::IsPcDevice() || base::ohos::IsPcMode()) {
        LOG(INFO) << "OHOSAudioManager Pc";
        SetMaxOutputStreamsAllowed(kPcModeMaxOutputStreams);
      } else {
        SetMaxOutputStreamsAllowed(kDefaultMaxOutputStreams);
      }
    }

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
  auto defaultOutputDevice = OhosAdapterHelper::GetInstance()
                                 .GetAudioSystemManager()
                                 .GetDefaultOutputDevice();
  for (auto audioDevice : audioDeviceList) {
    if (!audioDevice || !device_names) {
      LOG(ERROR) << "GetAudioOutputDeviceNames audioDevice is nullptr.";
      return;
    }
    if (!defaultOutputDevice) {
      LOG(ERROR) << "OHOSAudioManager::GetAudioOutputDeviceNames "
                    "defaultOutputDevice is null";
      device_names->emplace_back(
          audioDevice->GetDeviceName(),
          base::NumberToString(audioDevice->GetDeviceId()));
    } else {
      if (defaultOutputDevice->GetDeviceName() ==
          audioDevice->GetDeviceName()) {
        device_names->emplace_front(
            audioDevice->GetDeviceName(),
            base::NumberToString(audioDevice->GetDeviceId()));
      } else {
        device_names->emplace_back(
            audioDevice->GetDeviceName(),
            base::NumberToString(audioDevice->GetDeviceId()));
      }
    }
  }
}

#if BUILDFLAG(ARKWEB_WEBRTC)
void OHOSAudioManager::GetAudioInputDeviceNames(
    AudioDeviceNames* device_names) {
  if (!device_names) {
    LOG(ERROR) << "GetAudioInputDeviceNames audioDevice is nullptr.";
    return;
  }
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableAudioInput)) {
    return;
  }

  if (!device_names) {
    LOG(ERROR) << "OHOSAudioManager::GetAudioInputDeviceNames device_names is null";
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
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

const char* OHOSAudioManager::GetName() {
  return AUDIO_MANAGER_NAME;
}

// Implementation of AudioManagerBase.
AudioOutputStream* OHOSAudioManager::MakeLinearOutputStream(
    const AudioParameters& params,
    const LogCallback& log_callback) {
  NOTREACHED();
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
  if (!params.effects()) {
    isCommunication_ = false;
  }
  int buffer_size = kLowLatencyOutputBufferSize;
  if (input_params.latency_tag() == AudioLatency::Type::kPlayback) {
    buffer_size = kMinimumOutputBufferSize;
  }
  return AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                         ChannelLayoutConfig::Guess(kDefaultChannelCount),
                         kDefaultSampleRate, buffer_size);
}

#if BUILDFLAG(ARKWEB_WEBRTC)
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
    LOG(WARNING) << "OHOSAudioManager::SelectAudioDevice device_id is empty.";
    return;
  }
  device_id_ = device_id;
  LOG(INFO) << "OHOSAudioManager::SelectAudioDevice device_id is: "
            << device_id;
  if (device_id_ == std::string(kScreenSystemAudioDeviceId)) {
    LOG(INFO) << "OHOSAudioManager::SelectAudioDevice is: SystemAudioDevice";
    return;
  }
  int deviceId = 0;
  base::StringToInt(device_id, &deviceId);
  int32_t ret = OhosAdapterHelper::GetInstance()
                    .GetAudioSystemManager()
                    .SelectAudioDeviceById(deviceId, isInput);
  if (ret != 0) {
    LOG(ERROR) << "OHOSAudioManager::SelectAudioDevice failed. ret: " << ret;
  }
}

// LCOV_EXCL_START
std::string OHOSAudioManager::GetSelectAudioDeviceId() {
  if (device_id_.empty()) {
    LOG(WARNING) << "OHOSAudioManager::SelectAudioDevice device_id is empty.";
    return "";
  }
  return device_id_;
}
// LCOV_EXCL_STOP
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}  // namespace media

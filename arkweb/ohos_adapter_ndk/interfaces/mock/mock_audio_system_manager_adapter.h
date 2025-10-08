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

#ifndef MOCK_AUDIO_SYSTEM_MANAGER_ADAPTER_H
#define MOCK_AUDIO_SYSTEM_MANAGER_ADAPTER_H

#include <gmock/gmock.h>

#include "adapter_base.h"
#include "audio_system_manager_adapter.h"

namespace OHOS::NWeb {

class MockAudioSystemManagerAdapter : public AudioSystemManagerAdapter {
 public:
  MOCK_METHOD(bool, HasAudioOutputDevices, (), (override));
  MOCK_METHOD(bool, HasAudioInputDevices, (), (override));
  MOCK_METHOD(int32_t,
              RequestAudioFocus,
              (const std::shared_ptr<AudioInterruptAdapter> audioInterrupt),
              (override));
  MOCK_METHOD(int32_t,
              AbandonAudioFocus,
              (const std::shared_ptr<AudioInterruptAdapter> audioInterrupt),
              (override));
  MOCK_METHOD(int32_t,
              SetAudioManagerInterruptCallback,
              (std::shared_ptr<AudioManagerCallbackAdapter> callback),
              (override));
  MOCK_METHOD(int32_t, UnsetAudioManagerInterruptCallback, (), (override));
  MOCK_METHOD(std::vector<std::shared_ptr<AudioDeviceDescAdapter>>,
              GetDevices,
              (AdapterDeviceFlag flag),
              (override));
  MOCK_METHOD(int32_t,
              SelectAudioDeviceById,
              (int32_t deviceId, bool isInput),
              (override));
  MOCK_METHOD(
      int32_t,
      SetDeviceChangeCallback,
      (std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> callback),
      (override));
  MOCK_METHOD(int32_t, UnsetDeviceChangeCallback, (), (override));
  MOCK_METHOD(std::shared_ptr<AudioDeviceDescAdapter>,
              GetDefaultOutputDevice,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<AudioDeviceDescAdapter>,
              GetDefaultInputDevice,
              (),
              (override));
  MOCK_METHOD(bool, SetLanguage, (const std::string& language), (override));
};

}  // namespace OHOS::NWeb

#endif  // MOCK_AUDIO_SYSTEM_MANAGER_ADAPTER_H

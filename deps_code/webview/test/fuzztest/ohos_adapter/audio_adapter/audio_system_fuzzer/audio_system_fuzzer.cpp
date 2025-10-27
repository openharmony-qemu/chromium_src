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

#include "audio_system_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "audio_renderer_adapter_impl.h"
#define private public
#include "audio_system_manager_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {

class AudioManagerCallbackAdapterMock : public AudioManagerCallbackAdapter {
public:
    AudioManagerCallbackAdapterMock() = default;
    void OnSuspend() {}
    void OnResume() {}
};
class AudioManagerDeviceChangeCallbackAdapterMock : public AudioManagerDeviceChangeCallbackAdapter {
public:
    AudioManagerDeviceChangeCallbackAdapterMock() = default;
    void OnDeviceChange() {}
};

bool AudioGetDeviceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t deviceId =  fdp.ConsumeIntegralInRange<int32_t>(1000000, 1000005);
    bool randBool = fdp.ConsumeBool();
    auto language = fdp.ConsumeRandomLengthString(32);
    
    AudioSystemManagerAdapterImpl::GetInstance().GetDeviceName(DeviceType::DEVICE_TYPE_INVALID);
    AudioSystemManagerAdapterImpl::GetInstance().GetDevices(AdapterDeviceFlag::OUTPUT_DEVICES_FLAG);
    AudioSystemManagerAdapterImpl::GetInstance().SelectAudioDeviceById(deviceId, randBool);
    AudioSystemManagerAdapterImpl::GetInstance().GetDefaultOutputDevice();
    AudioSystemManagerAdapterImpl::GetInstance().GetDefaultInputDevice();
    AudioSystemManagerAdapterImpl::GetInstance().UnsetDeviceChangeCallback();
    AudioSystemManagerAdapterImpl::GetInstance().SetLanguage(language);

    std::shared_ptr<AudioManagerCallbackAdapter> callback =
        std::make_shared<AudioManagerCallbackAdapterMock>();
    AudioSystemManagerAdapterImpl::GetInstance().SetAudioManagerInterruptCallback(callback);
    AudioSystemManagerAdapterImpl::GetInstance().UnsetDeviceChangeCallback();

    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> managerCallBack = 
        std::make_shared<AudioManagerDeviceChangeCallbackAdapterMock>();
    AudioSystemManagerAdapterImpl::GetInstance().SetDeviceChangeCallback(managerCallBack);
    std::vector<std::shared_ptr<AudioDeviceDescriptor>> defaultOutputDevice;
    AudioSystemManagerAdapterImpl::GetInstance().SelectAudioOutputDevice(randBool, defaultOutputDevice);
    AudioSystemManagerAdapterImpl::GetInstance().SelectDefaultAudioDevice(randBool);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioGetDeviceFuzzTest(data, size);
    return 0;
}
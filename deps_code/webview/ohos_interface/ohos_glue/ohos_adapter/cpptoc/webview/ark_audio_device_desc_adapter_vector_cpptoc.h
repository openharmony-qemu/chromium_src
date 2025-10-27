/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_AUDIO_DEVICE_DESC_ADAPTER_VECTOR_CPPTOC_H
#define ARK_AUDIO_DEVICE_DESC_ADAPTER_VECTOR_CPPTOC_H

#pragma once

#include "audio_system_manager_adapter.h"
#include "ohos_adapter/include/ark_audio_device_desc_adapter_vector.h"

namespace OHOS::ArkWeb {

ArkAudioDeviceDescAdapterVector ArkAudioDeviceDescAdapterVectorClassToStruct(
    const std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>>& class_value);

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_DEVICE_DESC_ADAPTER_VECTOR_CPPTOC_H_

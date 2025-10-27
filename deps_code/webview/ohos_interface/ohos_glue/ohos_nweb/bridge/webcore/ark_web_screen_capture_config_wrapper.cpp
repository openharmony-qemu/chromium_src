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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_screen_capture_config_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebScreenCaptureConfigWrapper::ArkWebScreenCaptureConfigWrapper(
    ArkWebRefPtr<ArkWebScreenCaptureConfig> ark_web_screen_capture_config)
    : ark_web_screen_capture_config_(ark_web_screen_capture_config)
{}

int32_t ArkWebScreenCaptureConfigWrapper::GetMode()
{
    return ark_web_screen_capture_config_->GetMode();
}

int32_t ArkWebScreenCaptureConfigWrapper::GetSourceId()
{
    return ark_web_screen_capture_config_->GetSourceId();
}

} // namespace OHOS::ArkWeb

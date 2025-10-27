/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_screen_capture_config_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_audio_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_recorder_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_video_info_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkScreenCaptureConfigAdapterWrapper::ArkScreenCaptureConfigAdapterWrapper(
    ArkWebRefPtr<ArkScreenCaptureConfigAdapter> ref)
    : ctocpp_(ref)
{}

NWeb::CaptureModeAdapter ArkScreenCaptureConfigAdapterWrapper::GetCaptureMode()
{
    return (NWeb::CaptureModeAdapter)ctocpp_->GetCaptureMode();
}

NWeb::DataTypeAdapter ArkScreenCaptureConfigAdapterWrapper::GetDataType()
{
    return (NWeb::DataTypeAdapter)ctocpp_->GetDataType();
}

std::shared_ptr<NWeb::AudioInfoAdapter> ArkScreenCaptureConfigAdapterWrapper::GetAudioInfo()
{
    ArkWebRefPtr<ArkAudioInfoAdapter> adapter = ctocpp_->GetAudioInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioInfoAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::VideoInfoAdapter> ArkScreenCaptureConfigAdapterWrapper::GetVideoInfo()
{
    ArkWebRefPtr<ArkVideoInfoAdapter> adapter = ctocpp_->GetVideoInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkVideoInfoAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::RecorderInfoAdapter> ArkScreenCaptureConfigAdapterWrapper::GetRecorderInfo()
{
    ArkWebRefPtr<ArkRecorderInfoAdapter> adapter = ctocpp_->GetRecorderInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkRecorderInfoAdapterWrapper>(adapter);
}

} // namespace OHOS::ArkWeb

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

#include "ohos_adapter/bridge/ark_screen_capture_config_adapter_impl.h"

#include "ohos_adapter/bridge/ark_audio_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_recorder_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_video_info_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
ArkScreenCaptureConfigAdapterImpl::ArkScreenCaptureConfigAdapterImpl(
    std::shared_ptr<OHOS::NWeb::ScreenCaptureConfigAdapter> ref)
    : real_(ref)
{}

int32_t ArkScreenCaptureConfigAdapterImpl::GetCaptureMode()
{
    return (int32_t)real_->GetCaptureMode();
}

int32_t ArkScreenCaptureConfigAdapterImpl::GetDataType()
{
    return (int32_t)real_->GetDataType();
}

ArkWebRefPtr<ArkAudioInfoAdapter> ArkScreenCaptureConfigAdapterImpl::GetAudioInfo()
{
    std::shared_ptr<NWeb::AudioInfoAdapter> info = real_->GetAudioInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkAudioInfoAdapterImpl(info);
}

ArkWebRefPtr<ArkVideoInfoAdapter> ArkScreenCaptureConfigAdapterImpl::GetVideoInfo()
{
    std::shared_ptr<NWeb::VideoInfoAdapter> info = real_->GetVideoInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkVideoInfoAdapterImpl(info);
}

ArkWebRefPtr<ArkRecorderInfoAdapter> ArkScreenCaptureConfigAdapterImpl::GetRecorderInfo()
{
    std::shared_ptr<NWeb::RecorderInfoAdapter> info = real_->GetRecorderInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkRecorderInfoAdapterImpl(info);
}

} // namespace OHOS::ArkWeb

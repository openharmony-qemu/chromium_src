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

#include "ohos_adapter/bridge/ark_audio_info_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_audio_capture_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_enc_info_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioInfoAdapterWrapper::ArkAudioInfoAdapterWrapper(ArkWebRefPtr<ArkAudioInfoAdapter> ref) : ctocpp_(ref) {}

std::shared_ptr<NWeb::AudioCaptureInfoAdapter> ArkAudioInfoAdapterWrapper::GetMicCapInfo()
{
    ArkWebRefPtr<ArkAudioCaptureInfoAdapter> adapter = ctocpp_->GetMicCapInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioCaptureInfoAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::AudioCaptureInfoAdapter> ArkAudioInfoAdapterWrapper::GetInnerCapInfo()
{
    ArkWebRefPtr<ArkAudioCaptureInfoAdapter> adapter = ctocpp_->GetInnerCapInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioCaptureInfoAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::AudioEncInfoAdapter> ArkAudioInfoAdapterWrapper::GetAudioEncInfo()
{
    ArkWebRefPtr<ArkAudioEncInfoAdapter> adapter = ctocpp_->GetAudioEncInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioEncInfoAdapterWrapper>(adapter);
}

} // namespace OHOS::ArkWeb

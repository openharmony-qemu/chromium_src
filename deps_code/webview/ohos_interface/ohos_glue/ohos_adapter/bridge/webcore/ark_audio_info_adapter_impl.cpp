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

#include "ohos_adapter/bridge/ark_audio_info_adapter_impl.h"

#include "ohos_adapter/bridge/ark_audio_capture_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_enc_info_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioInfoAdapterImpl::ArkAudioInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioInfoAdapter> ref) : real_(ref) {}

ArkWebRefPtr<ArkAudioCaptureInfoAdapter> ArkAudioInfoAdapterImpl::GetMicCapInfo()
{
    std::shared_ptr<NWeb::AudioCaptureInfoAdapter> info = real_->GetMicCapInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkAudioCaptureInfoAdapterImpl(info);
}

ArkWebRefPtr<ArkAudioCaptureInfoAdapter> ArkAudioInfoAdapterImpl::GetInnerCapInfo()
{
    std::shared_ptr<NWeb::AudioCaptureInfoAdapter> info = real_->GetInnerCapInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkAudioCaptureInfoAdapterImpl(info);
}

ArkWebRefPtr<ArkAudioEncInfoAdapter> ArkAudioInfoAdapterImpl::GetAudioEncInfo()
{
    std::shared_ptr<NWeb::AudioEncInfoAdapter> info = real_->GetAudioEncInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkAudioEncInfoAdapterImpl(info);
}

} // namespace OHOS::ArkWeb

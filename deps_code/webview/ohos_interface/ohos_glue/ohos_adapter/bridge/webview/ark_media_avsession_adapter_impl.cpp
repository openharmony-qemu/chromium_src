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

#include "ohos_adapter/bridge/ark_media_avsession_adapter_impl.h"

#include "ohos_adapter/bridge/ark_media_avsession_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_avsession_metadata_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_avsession_position_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaAVSessionAdapterImpl::ArkMediaAVSessionAdapterImpl(std::shared_ptr<OHOS::NWeb::MediaAVSessionAdapter> ref)
    : real_(ref)
{}

bool ArkMediaAVSessionAdapterImpl::CreateAVSession(int32_t type)
{
    return real_->CreateAVSession((OHOS::NWeb::MediaAVSessionType)type);
}

void ArkMediaAVSessionAdapterImpl::DestroyAVSession()
{
    real_->DestroyAVSession();
}

bool ArkMediaAVSessionAdapterImpl::RegistCallback(ArkWebRefPtr<ArkMediaAVSessionCallbackAdapter> callbackAdapter)
{
    if (!(CHECK_REF_PTR_IS_NULL(callbackAdapter))) {
        return real_->RegistCallback(std::make_shared<ArkMediaAVSessionCallbackAdapterWrapper>(callbackAdapter));
    }
    return false;
}

bool ArkMediaAVSessionAdapterImpl::IsActivated()
{
    return real_->IsActivated();
}

bool ArkMediaAVSessionAdapterImpl::Activate()
{
    return real_->Activate();
}

void ArkMediaAVSessionAdapterImpl::DeActivate()
{
    real_->DeActivate();
}

void ArkMediaAVSessionAdapterImpl::SetMetadata(const ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> metadata)
{
    if (CHECK_REF_PTR_IS_NULL(metadata)) {
        real_->SetMetadata(nullptr);
    } else {
        real_->SetMetadata(std::make_shared<ArkMediaAVSessionMetadataAdapterWrapper>(metadata));
    }
}

void ArkMediaAVSessionAdapterImpl::SetPlaybackState(int32_t state)
{
    real_->SetPlaybackState((OHOS::NWeb::MediaAVSessionPlayState)state);
}

void ArkMediaAVSessionAdapterImpl::SetPlaybackPosition(const ArkWebRefPtr<ArkMediaAVSessionPositionAdapter> position)
{
    if (CHECK_REF_PTR_IS_NULL(position)) {
        real_->SetPlaybackPosition(nullptr);
    } else {
        real_->SetPlaybackPosition(std::make_shared<ArkMediaAVSessionPositionAdapterWrapper>(position));
    }
}

} // namespace OHOS::ArkWeb

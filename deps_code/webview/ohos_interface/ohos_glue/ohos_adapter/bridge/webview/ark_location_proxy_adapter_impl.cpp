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

#include "ohos_adapter/bridge/ark_location_proxy_adapter_impl.h"

#include "ohos_adapter/bridge/ark_location_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_location_request_config_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkLocationProxyAdapterImpl::ArkLocationProxyAdapterImpl(std::shared_ptr<OHOS::NWeb::LocationProxyAdapter> ref)
    : real_(ref)
{}

int32_t ArkLocationProxyAdapterImpl::StartLocating(
    ArkWebRefPtr<ArkLocationRequestConfig> requestConfig, ArkWebRefPtr<ArkLocationCallbackAdapter> callback)
{
    ArkLocationRequestConfigImpl* imp = static_cast<ArkLocationRequestConfigImpl*>(requestConfig.get());

    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->StartLocating(imp->real_, nullptr);
    }

    return real_->StartLocating(imp->real_, std::make_shared<ArkLocationCallbackAdapterWrapper>(callback));
}

bool ArkLocationProxyAdapterImpl::StopLocating(int32_t callbackId)
{
    return real_->StopLocating(callbackId);
}

bool ArkLocationProxyAdapterImpl::EnableAbility(bool isEnabled)
{
    return real_->EnableAbility(isEnabled);
}

bool ArkLocationProxyAdapterImpl::IsLocationEnabled()
{
    return real_->IsLocationEnabled();
}

} // namespace OHOS::ArkWeb

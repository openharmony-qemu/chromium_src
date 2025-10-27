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

#include "ohos_adapter/bridge/ark_location_proxy_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_location_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_location_request_config_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkLocationProxyAdapterWrapper::ArkLocationProxyAdapterWrapper(ArkWebRefPtr<ArkLocationProxyAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkLocationProxyAdapterWrapper::StartLocating(std::shared_ptr<OHOS::NWeb::LocationRequestConfig> requestConfig,
    std::shared_ptr<OHOS::NWeb::LocationCallbackAdapter> callback)
{
    std::shared_ptr<ArkLocationRequestConfigWrapper> config =
        std::static_pointer_cast<ArkLocationRequestConfigWrapper>(requestConfig);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->StartLocating(config->ctocpp_, nullptr);
    }

    return ctocpp_->StartLocating(config->ctocpp_, new ArkLocationCallbackAdapterImpl(callback));
}

bool ArkLocationProxyAdapterWrapper::StopLocating(int32_t callbackId)
{
    return ctocpp_->StopLocating(callbackId);
}

bool ArkLocationProxyAdapterWrapper::EnableAbility(bool isEnabled)
{
    return ctocpp_->EnableAbility(isEnabled);
}

bool ArkLocationProxyAdapterWrapper::IsLocationEnabled()
{
    return ctocpp_->IsLocationEnabled();
}

} // namespace OHOS::ArkWeb

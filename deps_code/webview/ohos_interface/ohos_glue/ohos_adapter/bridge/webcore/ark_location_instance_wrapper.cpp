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

#include "ohos_adapter/bridge/ark_location_instance_wrapper.h"

#include "ohos_adapter/bridge/ark_location_proxy_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_location_request_config_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::NWeb {
LocationInstance& LocationInstance::GetInstance()
{
    static ArkWeb::ArkLocationInstanceWrapper instance(ArkWeb::ArkLocationInstance::GetInstance());
    return instance;
}

} // namespace OHOS::NWeb

namespace OHOS::ArkWeb {

ArkLocationInstanceWrapper::ArkLocationInstanceWrapper(ArkWebRefPtr<ArkLocationInstance> ref) : ctocpp_(ref) {}

std::shared_ptr<NWeb::LocationProxyAdapter> ArkLocationInstanceWrapper::CreateLocationProxyAdapter()
{
    ArkWebRefPtr<ArkLocationProxyAdapter> ref = ctocpp_->CreateLocationProxyAdapter();
    if (CHECK_REF_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return std::make_shared<ArkLocationProxyAdapterWrapper>(ref);
}

std::shared_ptr<NWeb::LocationRequestConfig> ArkLocationInstanceWrapper::CreateLocationRequestConfig()
{
    ArkWebRefPtr<ArkLocationRequestConfig> ref = ctocpp_->CreateLocationRequestConfig();
    if (CHECK_REF_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return std::make_shared<ArkLocationRequestConfigWrapper>(ref);
}

} // namespace OHOS::ArkWeb

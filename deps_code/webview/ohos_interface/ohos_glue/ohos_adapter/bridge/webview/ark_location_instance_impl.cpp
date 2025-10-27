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

#include "ohos_adapter/bridge/ark_location_instance_impl.h"

#include "ohos_adapter/bridge/ark_location_proxy_adapter_impl.h"
#include "ohos_adapter/bridge/ark_location_request_config_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebRefPtr<ArkLocationInstance> ArkLocationInstance::GetInstance()
{
    static NWeb::LocationInstance& instance = NWeb::LocationInstance::GetInstance();
    static ArkWebRefPtr<ArkLocationInstance> impl = new ArkLocationInstanceImpl(instance);
    return impl;
}

ArkLocationInstanceImpl::ArkLocationInstanceImpl(NWeb::LocationInstance& ref) : real_(ref) {}

ArkWebRefPtr<ArkLocationProxyAdapter> ArkLocationInstanceImpl::CreateLocationProxyAdapter()
{
    std::shared_ptr<OHOS::NWeb::LocationProxyAdapter> adapter = real_.CreateLocationProxyAdapter();
    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return new ArkLocationProxyAdapterImpl(adapter);
}

ArkWebRefPtr<ArkLocationRequestConfig> ArkLocationInstanceImpl::CreateLocationRequestConfig()
{
    std::shared_ptr<OHOS::NWeb::LocationRequestConfig> config = real_.CreateLocationRequestConfig();
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        return nullptr;
    }
    return new ArkLocationRequestConfigImpl(config);
}

} // namespace OHOS::ArkWeb

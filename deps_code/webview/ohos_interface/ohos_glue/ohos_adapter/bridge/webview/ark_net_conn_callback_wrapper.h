/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_NET_CONN_CALLBACK_WRAPPER_H
#define ARK_NET_CONN_CALLBACK_WRAPPER_H
#pragma once

#include "net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetConnCallbackWrapper : public OHOS::NWeb::NetConnCallback {
public:
  explicit  ArkNetConnCallbackWrapper(ArkWebRefPtr<ArkNetConnCallback>);
    int32_t NetAvailable() override;
    int32_t NetCapabilitiesChange(const OHOS::NWeb::NetConnectType& netConnectType,
        const OHOS::NWeb::NetConnectSubtype& netConnectSubtype) override;
    int32_t NetConnectionPropertiesChange() override;
    int32_t NetUnavailable() override;
    int32_t OnNetCapabilitiesChanged(
        const std::shared_ptr<NWeb::NetCapabilitiesAdapter> capabilities) override;
    int32_t OnNetConnectionPropertiesChanged(
        const std::shared_ptr<NWeb::NetConnectionPropertiesAdapter> properties) override;

private:
    ArkWebRefPtr<ArkNetConnCallback> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_CONN_CALLBACK_WRAPPER_H

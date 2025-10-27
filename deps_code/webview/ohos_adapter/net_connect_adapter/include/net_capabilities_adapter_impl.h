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

#ifndef NET_CAPABILITIES_ADAPTER_IMPL_H
#define NET_CAPABILITIES_ADAPTER_IMPL_H

#include "net_connect_adapter.h"

namespace OHOS::NWeb {

class NetCapabilitiesAdapterImpl : public NetCapabilitiesAdapter {
public:
    NetCapabilitiesAdapterImpl() = default;

    ~NetCapabilitiesAdapterImpl() override = default;

    int32_t GetNetId() override;

    NetConnectType GetConnectType() override;

    NetConnectSubtype GetConnectSubtype() override;

    void SetNetId(int32_t netId);

    void SetConnectType(NetConnectType type);

    void SetConnectSubtype(NetConnectSubtype subtype);

private:
    int32_t netId_ = -1;
    NetConnectType type_ = NetConnectType::CONNECTION_UNKNOWN;
    NetConnectSubtype subtype_ = NetConnectSubtype::SUBTYPE_UNKNOWN;
};

} // namespace OHOS::NWeb

#endif // NET_CAPABILITIES_ADAPTER_IMPL_H

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

#ifndef NET_CONNECT_UTILS_H
#define NET_CONNECT_UTILS_H

#include <string>

#include "net_all_capabilities.h"
#include "net_connect_adapter.h"
#include "network_search_types.h"

namespace OHOS::NWeb {
using namespace OHOS::NetManagerStandard;
using namespace OHOS::Telephony;
class NetConnectUtils {
public:
    NetConnectUtils() = default;
    virtual ~NetConnectUtils() = default;

    static NetConnectSubtype ConvertToConnectsubtype(const RadioTech &subtype);
    static NetConnectType ConvertToConnectType(const NetBearType &netBearType, const RadioTech &subtype);
    static std::string ConnectTypeToString(const NetConnectType &type);
private:
    static NetConnectType ConvertToConnectTypeInner(const RadioTech &subtype);
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_UTILS_H
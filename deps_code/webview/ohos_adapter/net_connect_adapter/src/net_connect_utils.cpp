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

#include "net_connect_utils.h"

namespace OHOS::NWeb {
NetConnectType NetConnectUtils::ConvertToConnectTypeInner(const RadioTech &subtype)
{
    switch (subtype) {
        case RadioTech::RADIO_TECHNOLOGY_UNKNOWN:
        case RadioTech::RADIO_TECHNOLOGY_IWLAN:
            return NetConnectType::CONNECTION_UNKNOWN;
        case RadioTech::RADIO_TECHNOLOGY_GSM:
        case RadioTech::RADIO_TECHNOLOGY_1XRTT:
            return NetConnectType::CONNECTION_2G;
        case RadioTech::RADIO_TECHNOLOGY_WCDMA:
        case RadioTech::RADIO_TECHNOLOGY_HSPA:
        case RadioTech::RADIO_TECHNOLOGY_HSPAP:
        case RadioTech::RADIO_TECHNOLOGY_TD_SCDMA:
        case RadioTech::RADIO_TECHNOLOGY_EVDO:
        case RadioTech::RADIO_TECHNOLOGY_EHRPD:
            return NetConnectType::CONNECTION_3G;
        case RadioTech::RADIO_TECHNOLOGY_LTE:
        case RadioTech::RADIO_TECHNOLOGY_LTE_CA:
            return NetConnectType::CONNECTION_4G;
        case RadioTech::RADIO_TECHNOLOGY_NR:
            return NetConnectType::CONNECTION_5G;
        default:
            return NetConnectType::CONNECTION_UNKNOWN;
    }
}

NetConnectSubtype NetConnectUtils::ConvertToConnectsubtype(const RadioTech &subtype)
{
    switch (subtype) {
        case RadioTech::RADIO_TECHNOLOGY_UNKNOWN:
        case RadioTech::RADIO_TECHNOLOGY_IWLAN:
        case RadioTech::RADIO_TECHNOLOGY_WCDMA:
        case RadioTech::RADIO_TECHNOLOGY_TD_SCDMA:
        case RadioTech::RADIO_TECHNOLOGY_EVDO:
        case RadioTech::RADIO_TECHNOLOGY_NR:
            return NetConnectSubtype::SUBTYPE_UNKNOWN;
        case RadioTech::RADIO_TECHNOLOGY_GSM:
            return NetConnectSubtype::SUBTYPE_GSM;
        case RadioTech::RADIO_TECHNOLOGY_1XRTT:
            return NetConnectSubtype::SUBTYPE_1XRTT;
        case RadioTech::RADIO_TECHNOLOGY_HSPA:
            return NetConnectSubtype::SUBTYPE_HSPA;
        case RadioTech::RADIO_TECHNOLOGY_HSPAP:
            return NetConnectSubtype::SUBTYPE_HSPAP;
        case RadioTech::RADIO_TECHNOLOGY_EHRPD:
            return NetConnectSubtype::SUBTYPE_EHRPD;
        case RadioTech::RADIO_TECHNOLOGY_LTE:
            return NetConnectSubtype::SUBTYPE_LTE;
        case RadioTech::RADIO_TECHNOLOGY_LTE_CA:
            return NetConnectSubtype::SUBTYPE_LTE_ADVANCED;
        default:
            return NetConnectSubtype::SUBTYPE_UNKNOWN;
    }
}

NetConnectType NetConnectUtils::ConvertToConnectType(const NetBearType &netBearType, const RadioTech &subtype)
{
    switch (netBearType) {
        case BEARER_CELLULAR:
            return ConvertToConnectTypeInner(subtype);
        case BEARER_WIFI:
            return NetConnectType::CONNECTION_WIFI;
        case BEARER_BLUETOOTH:
            return NetConnectType::CONNECTION_BLUETOOTH;
        case BEARER_ETHERNET:
            return NetConnectType::CONNECTION_ETHERNET;
        case BEARER_VPN:
        case BEARER_WIFI_AWARE:
        case BEARER_DEFAULT:
            return NetConnectType::CONNECTION_UNKNOWN;
        default:
            return NetConnectType::CONNECTION_UNKNOWN;
    }
}

std::string NetConnectUtils::ConnectTypeToString(const NetConnectType &type)
{
    switch (type) {
        case NetConnectType::CONNECTION_UNKNOWN:
            return "unknown";
        case NetConnectType::CONNECTION_ETHERNET:
            return "ethernet";
        case NetConnectType::CONNECTION_WIFI:
            return "wifi";
        case NetConnectType::CONNECTION_2G:
            return "2g";
        case NetConnectType::CONNECTION_3G:
            return "3g";
        case NetConnectType::CONNECTION_4G:
            return "4g";
        case NetConnectType::CONNECTION_NONE:
            return "none";
        case NetConnectType::CONNECTION_BLUETOOTH:
            return "bluetooth";
        case NetConnectType::CONNECTION_5G:
            return "5g";
    }
}
}  // namespace OHOS::NWeb
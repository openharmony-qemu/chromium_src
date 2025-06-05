/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_NET_CONNECTION_ADAPTER_H
#define OHOS_ADAPTER_NET_CONNECTION_ADAPTER_H

#include <memory>
#include <string>
#include <napi/native_api.h>
#include "ohos/adapter/export.h"

namespace ohos::adapter::net {
enum class ADAPTER_EXPORT_API NetBearType {
  BEARER_CELLULAR = 0,
  BEARER_WIFI = 1,
  BEARER_ETHERNET = 3
};

enum class ADAPTER_EXPORT_API NetConnectionType {
  CONNECTION_UNKNOWN = 0,
  CONNECTION_ETHERNET = 1,
  CONNECTION_WIFI = 2,
  CONNECTION_2G = 3,
  CONNECTION_3G = 4,
  CONNECTION_4G = 5,
  CONNECTION_NONE = 6,
  CONNECTION_BLUETOOTH = 7,
  CONNECTION_5G = 8,
  CONNECTION_LAST = CONNECTION_5G
};

enum class ADAPTER_EXPORT_API NetConnectionSubtype {
  SUBTYPE_UNKNOWN = 0,
  SUBTYPE_NONE,
  SUBTYPE_OTHER,
  SUBTYPE_GSM,
  SUBTYPE_IDEN,
  SUBTYPE_CDMA,
  SUBTYPE_1XRTT,
  SUBTYPE_GPRS,
  SUBTYPE_EDGE,
  SUBTYPE_UMTS,
  SUBTYPE_EVDO_REV_0,
  SUBTYPE_EVDO_REV_A,
  SUBTYPE_HSPA,
  SUBTYPE_EVDO_REV_B,
  SUBTYPE_HSDPA,
  SUBTYPE_HSUPA,
  SUBTYPE_EHRPD,
  SUBTYPE_HSPAP,
  SUBTYPE_LTE,
  SUBTYPE_LTE_ADVANCED,
  SUBTYPE_BLUETOOTH_1_2,
  SUBTYPE_BLUETOOTH_2_1,
  SUBTYPE_BLUETOOTH_3_0,
  SUBTYPE_BLUETOOTH_4_0,
  SUBTYPE_ETHERNET,
  SUBTYPE_FAST_ETHERNET,
  SUBTYPE_GIGABIT_ETHERNET,
  SUBTYPE_10_GIGABIT_ETHERNET,
  SUBTYPE_WIFI_B,
  SUBTYPE_WIFI_G,
  SUBTYPE_WIFI_N,
  SUBTYPE_WIFI_AC,
  SUBTYPE_WIFI_AD,
  SUBTYPE_LAST = SUBTYPE_WIFI_AD
};

class ADAPTER_EXPORT_API NetConnectionCallback {
public:
  virtual int32_t NetAvailable() = 0;
  virtual int32_t NetCapabilitiesChange(
    const NetConnectionType& netConnectType,
    const NetConnectionSubtype& netConnectSubtype) = 0;
  virtual int32_t NetConnectionPropertiesChange() = 0;
  virtual int32_t NetUnavailable() = 0;
  virtual int32_t NetLost() = 0;
};

class ADAPTER_EXPORT_API NetConnectionAdapter {
public:
  NetConnectionAdapter() = default;
  virtual ~NetConnectionAdapter() = default;
  virtual int32_t RegisterNetConnectionCallback(std::shared_ptr<NetConnectionCallback> netConnCallback);
  virtual int32_t UnregisterNetConnectionCallback(std::shared_ptr<NetConnectionCallback> netConnCallback);
};

class ADAPTER_EXPORT_API NetConnectionAdapterHelper {
public:
  static NetConnectionAdapterHelper& GetInstance();
  virtual ~NetConnectionAdapterHelper() = default;
  std::unique_ptr<NetConnectionAdapter> CreateNetConnectionAdapter();
private:
  NetConnectionAdapterHelper() = default;
};

template <typename nativeType, typename OhosType>
nativeType ConvertOhosConnTypeToNativeConnType(const OhosType& type) {
  return static_cast<nativeType>(type);
}
} // namespace ohos::adapter::net

#endif  // OHOS_ADAPTER_NET_CONNECTION_ADAPTER_H

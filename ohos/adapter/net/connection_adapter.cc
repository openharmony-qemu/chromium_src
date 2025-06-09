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

#include "connection_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::net {
NetConnectionType ConvertBearTypeToNetConnType(const NetBearType& bearType) {
  if (bearType == NetBearType::BEARER_CELLULAR)
    return NetConnectionType::CONNECTION_4G;
  if (bearType == NetBearType::BEARER_WIFI)
    return NetConnectionType::CONNECTION_WIFI;
  if (bearType == NetBearType::BEARER_ETHERNET)
    return NetConnectionType::CONNECTION_ETHERNET;
  return NetConnectionType::CONNECTION_UNKNOWN;
}

int32_t NetConnectionAdapter::RegisterNetConnectionCallback(
    std::shared_ptr<NetConnectionCallback> netConnCallback) {
  if (auto netAvailableFunc = ohos::adapter::GetJSFunction("NetConnection.NetAvailable")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetAvailable();
    };
    netAvailableFunc->Invoke<void>(callback);
  }

  if (auto netCapabilitiesChangeFunc = ohos::adapter::GetJSFunction("NetConnection.NetCapabilitiesChange")) {
    std::function<void(NetBearType, int)> callback = [=](NetBearType bearType, int capType) {
      netConnCallback->NetCapabilitiesChange(
        ConvertBearTypeToNetConnType(bearType),
        NetConnectionSubtype::SUBTYPE_WIFI_B);
    };
    netCapabilitiesChangeFunc->Invoke<void>(callback);
  }

  if (auto netConnectionPropertiesChangeFunc =
    ohos::adapter::GetJSFunction("NetConnection.NetConnectionPropertiesChange")) {
      std::function<void(void)> callback = [=]() {
        netConnCallback->NetConnectionPropertiesChange();
      };
      netConnectionPropertiesChangeFunc->Invoke<void>(callback);
  }

  if (auto netUnavailableFunc = ohos::adapter::GetJSFunction("NetConnection.NetUnavailable")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetUnavailable();
    };
    netUnavailableFunc->Invoke<void>(callback);
  }

  if (auto netLostFunc = ohos::adapter::GetJSFunction("NetConnection.NetLost")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetLost();
    };
    netLostFunc->Invoke<void>(callback);
  }

  return 0;
}

int32_t NetConnectionAdapter::UnregisterNetConnectionCallback(
    std::shared_ptr<NetConnectionCallback> netConnCallback) {
  if (auto registerFunc = ohos::adapter::GetJSFunction("NetConnection.UnregisterAll")) {
    registerFunc->Invoke<void>();
  }
  return 0;
}

NetConnectionAdapterHelper& NetConnectionAdapterHelper::GetInstance() {
  static NetConnectionAdapterHelper helper;
  return helper;
}

std::unique_ptr<NetConnectionAdapter> NetConnectionAdapterHelper::CreateNetConnectionAdapter() {
  return std::make_unique<NetConnectionAdapter>();
}
} // namespace ohos::adapter::net

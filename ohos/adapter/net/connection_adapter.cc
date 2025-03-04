// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "connection_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::net {
NetConnectionType ConvertBearTypeToNetConnType(const NetBearType& bearType) {
  if (bearType == NetBearType::BEARER_CELLULAR) {
    return NetConnectionType::CONNECTION_4G;
  }
  if (bearType == NetBearType::BEARER_WIFI) {
    return NetConnectionType::CONNECTION_WIFI;
  }
  if (bearType == NetBearType::BEARER_ETHERNET) {
    return NetConnectionType::CONNECTION_ETHERNET;
  }
  return NetConnectionType::CONNECTION_UNKNOWN;
}

int32_t NetConnectionAdapter::RegisterNetConnectionCallback(
    std::shared_ptr<NetConnectionCallback> netConnCallback) {
  if (auto netAvailableFunc =
          ohos::adapter::GetJSFunction("NetConnection.NetAvailable")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetAvailable();
    };
    netAvailableFunc->Invoke<void>(callback);
  }

  if (auto netCapabilitiesChangeFunc =
          ohos::adapter::GetJSFunction("NetConnection.NetCapabilitiesChange")) {
    std::function<void(NetBearType, int)> callback = [=](NetBearType bearType,
                                                         int capType) {
      netConnCallback->NetCapabilitiesChange(
          ConvertBearTypeToNetConnType(bearType),
          NetConnectionSubtype::SUBTYPE_WIFI_B);
    };
    netCapabilitiesChangeFunc->Invoke<void>(callback);
  }

  if (auto netConnectionPropertiesChangeFunc = ohos::adapter::GetJSFunction(
          "NetConnection.NetConnectionPropertiesChange")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetConnectionPropertiesChange();
    };
    netConnectionPropertiesChangeFunc->Invoke<void>(callback);
  }

  if (auto netUnavailableFunc =
          ohos::adapter::GetJSFunction("NetConnection.NetUnavailable")) {
    std::function<void(void)> callback = [=]() {
      netConnCallback->NetUnavailable();
    };
    netUnavailableFunc->Invoke<void>(callback);
  }

  if (auto netLostFunc =
          ohos::adapter::GetJSFunction("NetConnection.NetLost")) {
    std::function<void(void)> callback = [=]() { netConnCallback->NetLost(); };
    netLostFunc->Invoke<void>(callback);
  }

  return 0;
}

int32_t NetConnectionAdapter::UnregisterNetConnectionCallback(
    std::shared_ptr<NetConnectionCallback> netConnCallback) {
  if (auto registerFunc =
          ohos::adapter::GetJSFunction("NetConnection.UnregisterAll")) {
    registerFunc->Invoke<void>();
  }
  return 0;
}

NetConnectionAdapterHelper& NetConnectionAdapterHelper::GetInstance() {
  static NetConnectionAdapterHelper helper;
  return helper;
}

std::unique_ptr<NetConnectionAdapter>
NetConnectionAdapterHelper::CreateNetConnectionAdapter() {
  return std::make_unique<NetConnectionAdapter>();
}
}  // namespace ohos::adapter::net

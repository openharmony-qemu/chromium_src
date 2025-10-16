/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_OHOS_ADAPTER_NDK_INTERFACES_NET_EVENT_ADAPTER_H_
#define ARKWEB_OHOS_ADAPTER_NDK_INTERFACES_NET_EVENT_ADAPTER_H_

#include <functional>
#include <memory>
#include <vector>

namespace OHOS::NWeb {
enum class NetBearType : int32_t {
  BEARER_CELLULAR = 0,
  BEARER_WIFI = 1,
  BEARER_BLUETOOTH = 2,
  BEARER_ETHERNET = 3,
  BEARER_VPN = 4,
  BEARER_WIFI_AWARE = 5,
  BEARER_DEFAULT  // The maximum value of NetBearType. Do not exceed the limit.
                  // No actual meaning.
};

enum class NetConnState : int32_t {
  NET_CONN_STATE_UNKNOWN = 0,
  NET_CONN_STATE_IDLE = 1,
  NET_CONN_STATE_CONNECTING = 2,
  NET_CONN_STATE_CONNECTED = 3,
  NET_CONN_STATE_DISCONNECTING = 4,
  NET_CONN_STATE_DISCONNECTED = 5,
};

class NetCommonEventListener {
 public:
  NetCommonEventListener() = default;
  virtual ~NetCommonEventListener() = default;

  virtual void OnConnectivityChanged(NetBearType bearType,
                                     NetConnState connState) = 0;
};

class NetEventAdapter {
 public:
  NetEventAdapter() = default;
  virtual ~NetEventAdapter() = default;

  virtual void RegisterCommonEventListener(
      std::shared_ptr<NetCommonEventListener> commonEventListener) = 0;

  virtual void UnRegisterCommonEventListener() = 0;
};

}  // namespace OHOS::NWeb

#endif  // ARKWEB_OHOS_ADAPTER_NDK_INTERFACES_NET_EVENT_ADAPTER_H_
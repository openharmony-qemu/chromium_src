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

#ifndef ARKWEB_OHOS_ADAPTER_NDK_NET_EVENT_ADAPTER_IMPL_H_
#define ARKWEB_OHOS_ADAPTER_NDK_NET_EVENT_ADAPTER_IMPL_H_

#include <cstdint>
#include <mutex>
#include <string>

#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/core_service/telephony_radio_type.h>

#include "arkweb/ohos_adapter_ndk/interfaces/net_event_adapter.h"

namespace OHOS::NWeb {
class NetEventAdapterImpl final : public NetEventAdapter {
 public:
  NetEventAdapterImpl() = default;
  ~NetEventAdapterImpl() override = default;

  void RegisterCommonEventListener(
      std::shared_ptr<NetCommonEventListener> commonEventListener) override;

  void UnRegisterCommonEventListener() override;

  static void OnReceiveEvent(const CommonEvent_RcvData* data);
  static bool RegisterNetCommonEvent();
  static bool UnRegisterCommonEvent();

 private:
  static CommonEvent_SubscribeInfo* commonEventSubscribeInfo_;
  static CommonEvent_Subscriber* commonEventSubscriber_;
  static std::shared_ptr<NetCommonEventListener> commonEventListener_;
  static std::mutex commonEventListenerMutex_;
};

}  // namespace OHOS::NWeb

#endif  // ARKWEB_OHOS_ADAPTER_NDK_NET_EVENT_ADAPTER_IMPL_H_

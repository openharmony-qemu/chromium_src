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

#include "arkweb/ohos_adapter_ndk/net_event_adapter/net_event_adapter_impl.h"

#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/cellular_data/telephony_data.h>
#include <telephony/core_service/telephony_radio.h>
#include <telephony/core_service/telephony_radio_type.h>

#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_capabilities_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_utils.h"
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connection_properties_adapter_impl.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {

namespace {
const int32_t DEFAULT_VALUE = -1;
static const char* COMMON_EVENT_KEY_NET_TYPE = "NetType";
}  // namespace

CommonEvent_SubscribeInfo* NetEventAdapterImpl::commonEventSubscribeInfo_ =
    nullptr;
CommonEvent_Subscriber* NetEventAdapterImpl::commonEventSubscriber_ = nullptr;
std::mutex NetEventAdapterImpl::commonEventListenerMutex_;
std::shared_ptr<NetCommonEventListener>
    NetEventAdapterImpl::commonEventListener_ = nullptr;

// static
void NetEventAdapterImpl::OnReceiveEvent(const CommonEvent_RcvData* data) {
  if (!OH_CommonEvent_GetEventFromRcvData(data)) {
    return;
  }

  if (strcmp(OH_CommonEvent_GetEventFromRcvData(data),
             COMMON_EVENT_CONNECTIVITY_CHANGE)) {
    return;
  }

  const CommonEvent_Parameters* para =
      OH_CommonEvent_GetParametersFromRcvData(data);
  if (!para) {
    WVLOG_E("failed to get patameters");
    return;
  }

  int32_t code = OH_CommonEvent_GetCodeFromRcvData(data);
  int32_t netType = DEFAULT_VALUE;
  if (OH_CommonEvent_HasKeyInParameters(para, COMMON_EVENT_KEY_NET_TYPE)) {
    netType = OH_CommonEvent_GetIntFromParameters(
        para, COMMON_EVENT_KEY_NET_TYPE, DEFAULT_VALUE);
  } else {
    WVLOG_E("failed to get netType");
  }
  WVLOG_I(
      "receive COMMON_EVENT_CONNECTIVITY_CHANGE type: %{public}d code: "
      "%{public}d",
      netType, code);
  if (netType == DEFAULT_VALUE) {
    WVLOG_E("net vpn default net type");
    return;
  }

  NetBearType netBearType = static_cast<NetBearType>(netType);
  NetConnState netConnState = static_cast<NetConnState>(code);
  {
    std::lock_guard<std::mutex> lock(commonEventListenerMutex_);
    if (commonEventListener_) {
      commonEventListener_->OnConnectivityChanged(netBearType, netConnState);
    }
  }
}

// static
bool NetEventAdapterImpl::RegisterNetCommonEvent() {
  const char* events[] = {COMMON_EVENT_CONNECTIVITY_CHANGE};
  int count = sizeof(events) / sizeof(events[0]);
  commonEventSubscribeInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
  if (!commonEventSubscribeInfo_) {
    WVLOG_E("Create SubscribeInfo failed.");
    return false;
  }

  commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(
      commonEventSubscribeInfo_, OnReceiveEvent);
  if (!commonEventSubscriber_) {
    OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
    commonEventSubscribeInfo_ = nullptr;
    WVLOG_E("Create Subscriber failed.");
    return false;
  }

  CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
  if (ret != COMMONEVENT_ERR_OK) {
    OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
    OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
    commonEventSubscribeInfo_ = nullptr;
    commonEventSubscriber_ = nullptr;
    WVLOG_E("Subscribe failed. ret = %{public}d", ret);
    return false;
  }
  return true;
}

// static
bool NetEventAdapterImpl::UnRegisterCommonEvent() {
  WVLOG_I("stop common event listen");
  if (commonEventSubscriber_) {
    CommonEvent_ErrCode errorCode =
        OH_CommonEvent_UnSubscribe(commonEventSubscriber_);
    if (errorCode == COMMONEVENT_ERR_OK) {
      OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
      OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
      commonEventSubscriber_ = nullptr;
      commonEventSubscribeInfo_ = nullptr;
      return true;
    } else {
      WVLOG_E("stop common event listen, unsubscribe common event failed");
    }
  }
  return false;
}

void NetEventAdapterImpl::RegisterCommonEventListener(
    std::shared_ptr<NetCommonEventListener> commonEventListener) {
  if (!commonEventListener) {
    WVLOG_I("net common event subscriber register failed due no cb");
    return;
  }

  {
    std::lock_guard<std::mutex> lock(commonEventListenerMutex_);
    commonEventListener_ = commonEventListener;
  }

  if (!commonEventSubscriber_) {
    if (!RegisterNetCommonEvent()) {
      return;
    }
  }
}

void NetEventAdapterImpl::UnRegisterCommonEventListener() {
  {
    std::lock_guard<std::mutex> lock(commonEventListenerMutex_);
    commonEventListener_ = nullptr;
  }
  UnRegisterCommonEvent();
}

}  // namespace OHOS::NWeb
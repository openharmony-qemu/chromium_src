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
#include "net_connect_fuzz_mock.h"
#include <memory>

static const char* const COMMON_EVENT_CONNECTIVITY_CHANGE =
    "usual.event.CONNECTIVITY_CHANGE";

const char* OH_CommonEvent_GetEventFromRcvData(
    const CommonEvent_RcvData* rcvData) {
  return rcvData->event.c_str();
}

const CommonEvent_Parameters* OH_CommonEvent_GetParametersFromRcvData(
    const CommonEvent_RcvData* rcvData) {
  return rcvData->parameters;
}

int32_t OH_CommonEvent_GetCodeFromRcvData(const CommonEvent_RcvData* rcvData) {
  return rcvData->code;
}

bool OH_CommonEvent_HasKeyInParameters(const CommonEvent_Parameters* para,
                                       const char* key) {
  return true;
}

int OH_CommonEvent_GetIntFromParameters(const CommonEvent_Parameters* para,
                                        const char* key,
                                        const int defaultValue) {
  const CArrParameters* p = reinterpret_cast<const CArrParameters*>(para);
  return p->netType;
}

std::shared_ptr<NetConn_NetConnCallback> g_netConnCallback;

int32_t OH_NetConn_RegisterDefaultNetConnCallback(
    NetConn_NetConnCallback* netConnCallback,
    uint32_t* callbackId) {
  g_netConnCallback =
      std::make_shared<NetConn_NetConnCallback>(*netConnCallback);
  return 0;
}

int32_t OH_NetConn_UnregisterNetConnCallback(uint32_t callBackId) {
  return 0;
}

int32_t OH_NetConn_GetDefaultNet(NetConn_NetHandle* netHandle) {
  if (g_netConnCallback == nullptr) {
    return 0;
  }
  g_netConnCallback->onNetworkAvailable(netHandle);
  NetConn_NetCapabilities netCap;
  netCap.bearerTypesSize = 1;
  netCap.bearerTypes[0] = NETCONN_BEARER_CELLULAR;
  g_netConnCallback->onNetCapabilitiesChange(netHandle, &netCap);
  NetConn_ConnectionProperties connProp;
  g_netConnCallback->onConnetionProperties(netHandle, &connProp);
  g_netConnCallback->onNetLost(netHandle);
  g_netConnCallback->onNetUnavailable();
  g_netConnCallback->onNetBlockStatusChange(netHandle, true);
  return 0;
}

int32_t OH_NetConn_GetNetCapabilities(NetConn_NetHandle* netHandle,
                                      NetConn_NetCapabilities* netCapacities) {
  return 0;
}

int32_t OH_Telephony_GetDefaultCellularDataSlotId(void) {
  return -1;
}

Telephony_RadioResult OH_Telephony_GetNetworkStateForSlot(
    int32_t slotId,
    Telephony_NetworkState* state) {
  state->cfgTech_ = TEL_RADIO_TECHNOLOGY_GSM;
  return TEL_RADIO_SUCCESS;
}

int32_t OH_NetConn_GetAllNets(NetConn_NetHandleList* netHandleList) {
  netHandleList->netHandleListSize = 1;
  netHandleList->netHandles[0].netId = 0;
  return 0;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetNativeBufferPropertiesOHOS(VkDevice device,
                                const struct OH_NativeBuffer* buffer,
                                VkNativeBufferPropertiesOHOS* pProperties) {
  return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSurfaceOHOS(VkInstance instance,
                    const VkSurfaceCreateInfoOHOS* pCreateInfo,
                    const VkAllocationCallbacks* pAllocator,
                    VkSurfaceKHR* pSurface) {
  return VK_SUCCESS;
}
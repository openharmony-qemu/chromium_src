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

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"

namespace MockNdkApi {

#ifdef __cplusplus
extern "C" {
#endif

std::function<CommonEvent_SubscribeInfo* (const char* events[],
		int32_t eventsNum)> g_mock_OH_CommonEvent_CreateSubscribeInfo;
CommonEvent_SubscribeInfo* __wrap_OH_CommonEvent_CreateSubscribeInfo(const char* events[], int32_t eventsNum) {
  return g_mock_OH_CommonEvent_CreateSubscribeInfo?g_mock_OH_CommonEvent_CreateSubscribeInfo(events, eventsNum)
                                                  :__real_OH_CommonEvent_CreateSubscribeInfo(events, eventsNum);
}

std::function<CommonEvent_Subscriber*(const CommonEvent_SubscribeInfo* info,
		CommonEvent_ReceiveCallback callback)> g_mock_OH_CommonEvent_CreateSubscriber;
CommonEvent_Subscriber* __wrap_OH_CommonEvent_CreateSubscriber(const CommonEvent_SubscribeInfo* info,
    CommonEvent_ReceiveCallback callback) {
  return g_mock_OH_CommonEvent_CreateSubscriber?g_mock_OH_CommonEvent_CreateSubscriber(info, callback)
                                               :__real_OH_CommonEvent_CreateSubscriber(info, callback);
}

std::function<CommonEvent_ErrCode (const CommonEvent_Subscriber* subscriber)> g_mock_OH_CommonEvent_Subscribe;
CommonEvent_ErrCode __wrap_OH_CommonEvent_Subscribe(const CommonEvent_Subscriber* subscriber) {
  return g_mock_OH_CommonEvent_Subscribe?g_mock_OH_CommonEvent_Subscribe(subscriber)
                                        :__real_OH_CommonEvent_Subscribe(subscriber);
}

std::function<int32_t (NetConn_HttpProxy *httpProxy)> g_mock_OH_NetConn_GetDefaultHttpProxy;
int32_t __wrap_OH_NetConn_GetDefaultHttpProxy(NetConn_HttpProxy *httpProxy) {
  return g_mock_OH_NetConn_GetDefaultHttpProxy?g_mock_OH_NetConn_GetDefaultHttpProxy(httpProxy)
                                              :__real_OH_NetConn_GetDefaultHttpProxy(httpProxy);
}

std::function<CommonEvent_ErrCode (const CommonEvent_Subscriber* subscriber)> g_mock_OH_CommonEvent_UnSubscribe;
CommonEvent_ErrCode __wrap_OH_CommonEvent_UnSubscribe(const CommonEvent_Subscriber* subscriber) {
  return g_mock_OH_CommonEvent_UnSubscribe?g_mock_OH_CommonEvent_UnSubscribe(subscriber)
                                          :__real_OH_CommonEvent_UnSubscribe(subscriber);
}

std::function<const char* (const CommonEvent_RcvData* rcvData)> g_mock_OH_CommonEvent_GetEventFromRcvData;
const char* __wrap_OH_CommonEvent_GetEventFromRcvData(const CommonEvent_RcvData* rcvData) {
  return g_mock_OH_CommonEvent_GetEventFromRcvData?g_mock_OH_CommonEvent_GetEventFromRcvData(rcvData)
                                                  :__real_OH_CommonEvent_GetEventFromRcvData(rcvData);
}

std::function<int32_t (void)> g_mock_OH_Telephony_GetDefaultCellularDataSlotId;
int32_t __wrap_OH_Telephony_GetDefaultCellularDataSlotId(void) {
  return g_mock_OH_Telephony_GetDefaultCellularDataSlotId?g_mock_OH_Telephony_GetDefaultCellularDataSlotId()
                                                         :__real_OH_Telephony_GetDefaultCellularDataSlotId();
  }

std::function<Telephony_RadioResult (int32_t slotId,
    Telephony_NetworkState *state)> g_mock_OH_Telephony_GetNetworkStateForSlot;
Telephony_RadioResult __wrap_OH_Telephony_GetNetworkStateForSlot(int32_t slotId, Telephony_NetworkState *state) {
  return g_mock_OH_Telephony_GetNetworkStateForSlot?g_mock_OH_Telephony_GetNetworkStateForSlot(slotId, state)
  																								 :__real_OH_Telephony_GetNetworkStateForSlot(slotId, state);
}

std::function<int32_t (NetConn_NetConnCallback *netConnCallback,
		uint32_t *callbackId)> g_mock_OH_NetConn_RegisterDefaultNetConnCallback;
int32_t __wrap_OH_NetConn_RegisterDefaultNetConnCallback(NetConn_NetConnCallback *netConnCallback,
    uint32_t *callbackId) {
  return g_mock_OH_NetConn_RegisterDefaultNetConnCallback
			?g_mock_OH_NetConn_RegisterDefaultNetConnCallback(netConnCallback, callbackId)
  		:__real_OH_NetConn_RegisterDefaultNetConnCallback(netConnCallback, callbackId);
}

std::function<int32_t (uint32_t callbackId)> g_mock_OH_NetConn_UnregisterNetConnCallback;
int32_t __wrap_OH_NetConn_UnregisterNetConnCallback(uint32_t callbackId) {
  return g_mock_OH_NetConn_UnregisterNetConnCallback?g_mock_OH_NetConn_UnregisterNetConnCallback(callbackId)
  																									:__real_OH_NetConn_UnregisterNetConnCallback(callbackId);
}

std::function<int32_t (NetConn_NetHandle *netHandle,
		NetConn_ConnectionProperties *prop)> g_mock_OH_NetConn_GetConnectionProperties;
int32_t __wrap_OH_NetConn_GetConnectionProperties(NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *prop) {
  return g_mock_OH_NetConn_GetConnectionProperties?g_mock_OH_NetConn_GetConnectionProperties(netHandle, prop)
  																								:__real_OH_NetConn_GetConnectionProperties(netHandle, prop);
}

std::function<int32_t (NetConn_NetHandle *netHandle)> g_mock_OH_NetConn_GetDefaultNet;
int32_t __wrap_OH_NetConn_GetDefaultNet(NetConn_NetHandle *netHandle) {
  return g_mock_OH_NetConn_GetDefaultNet?g_mock_OH_NetConn_GetDefaultNet(netHandle)
  																			:__real_OH_NetConn_GetDefaultNet(netHandle);
}

std::function<int32_t (NetConn_NetHandle *netHandle,
		NetConn_NetCapabilities *netCapabilities)> g_mock_OH_NetConn_GetNetCapabilities;
int32_t __wrap_OH_NetConn_GetNetCapabilities(NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCapabilities) {
  return g_mock_OH_NetConn_GetNetCapabilities?g_mock_OH_NetConn_GetNetCapabilities(netHandle, netCapabilities)
  																					 :__real_OH_NetConn_GetNetCapabilities(netHandle, netCapabilities);
}

std::function<int32_t (NetConn_NetHandleList *netHandleList)> g_mock_OH_NetConn_GetAllNets;
int32_t __wrap_OH_NetConn_GetAllNets(NetConn_NetHandleList *netHandleList) {
  return g_mock_OH_NetConn_GetAllNets?g_mock_OH_NetConn_GetAllNets(netHandleList)
  																	 :__real_OH_NetConn_GetAllNets(netHandleList);
}

std::function<Input_DeviceInfo* ()> g_mock_OH_Input_CreateDeviceInfo;
Input_DeviceInfo* __wrap_OH_Input_CreateDeviceInfo() {
  return g_mock_OH_Input_CreateDeviceInfo?g_mock_OH_Input_CreateDeviceInfo()
  																			 :__real_OH_Input_CreateDeviceInfo();
}

std::function<Input_Result (int32_t *deviceIds, int32_t inSize, int32_t *outSize)> g_mock_OH_Input_GetDeviceIds;
Input_Result __wrap_OH_Input_GetDeviceIds(int32_t *deviceIds, int32_t inSize, int32_t *outSize) {
  return g_mock_OH_Input_GetDeviceIds?g_mock_OH_Input_GetDeviceIds(deviceIds, inSize, outSize)
  																	 :__real_OH_Input_GetDeviceIds(deviceIds, inSize, outSize);
}

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi 

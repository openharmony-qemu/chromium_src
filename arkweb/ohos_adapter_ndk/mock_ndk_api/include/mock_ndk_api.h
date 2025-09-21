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

#ifndef MOCK_NDK_API_H
#define MOCK_NDK_API_H

#include <functional>
#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <network/netmanager/net_connection_type.h>
#include <network/netmanager/net_connection.h>
#include <telephony/core_service/telephony_radio_type.h>
#include <multimodalinput/oh_input_manager.h>

namespace MockNdkApi {

#ifdef __cplusplus
extern "C" {
#endif

extern std::function<CommonEvent_SubscribeInfo* (const char* events[],
		int32_t eventsNum)> g_mock_OH_CommonEvent_CreateSubscribeInfo;
CommonEvent_SubscribeInfo* __real_OH_CommonEvent_CreateSubscribeInfo(const char* events[], int32_t eventsNum);
CommonEvent_SubscribeInfo* __wrap_OH_CommonEvent_CreateSubscribeInfo(const char* events[], int32_t eventsNum);

extern std::function<CommonEvent_Subscriber*(const CommonEvent_SubscribeInfo* info,
		CommonEvent_ReceiveCallback callback)> g_mock_OH_CommonEvent_CreateSubscriber;
CommonEvent_Subscriber* __real_OH_CommonEvent_CreateSubscriber(const CommonEvent_SubscribeInfo* info,
    CommonEvent_ReceiveCallback callback);
CommonEvent_Subscriber* __wrap_OH_CommonEvent_CreateSubscriber(const CommonEvent_SubscribeInfo* info,
    CommonEvent_ReceiveCallback callback);

extern std::function<CommonEvent_ErrCode (const CommonEvent_Subscriber* subscriber)> g_mock_OH_CommonEvent_Subscribe;
CommonEvent_ErrCode __real_OH_CommonEvent_Subscribe(const CommonEvent_Subscriber* subscriber);
CommonEvent_ErrCode __wrap_OH_CommonEvent_Subscribe(const CommonEvent_Subscriber* subscriber);

extern std::function<int32_t (NetConn_HttpProxy *httpProxy)> g_mock_OH_NetConn_GetDefaultHttpProxy;
int32_t __real_OH_NetConn_GetDefaultHttpProxy(NetConn_HttpProxy *httpProxy);
int32_t __wrap_OH_NetConn_GetDefaultHttpProxy(NetConn_HttpProxy *httpProxy);

extern std::function<CommonEvent_ErrCode (const CommonEvent_Subscriber* subscriber)> g_mock_OH_CommonEvent_UnSubscribe;
CommonEvent_ErrCode __real_OH_CommonEvent_UnSubscribe(const CommonEvent_Subscriber* subscriber);
CommonEvent_ErrCode __wrap_OH_CommonEvent_UnSubscribe(const CommonEvent_Subscriber* subscriber);

extern std::function<const char* (const CommonEvent_RcvData* rcvData)> g_mock_OH_CommonEvent_GetEventFromRcvData;
const char* __real_OH_CommonEvent_GetEventFromRcvData(const CommonEvent_RcvData* rcvData);
const char* __wrap_OH_CommonEvent_GetEventFromRcvData(const CommonEvent_RcvData* rcvData);

extern std::function<int32_t (void)> g_mock_OH_Telephony_GetDefaultCellularDataSlotId;
int32_t __real_OH_Telephony_GetDefaultCellularDataSlotId(void);
int32_t __wrap_OH_Telephony_GetDefaultCellularDataSlotId(void);

extern std::function<Telephony_RadioResult (int32_t slotId,
    Telephony_NetworkState *state)> g_mock_OH_Telephony_GetNetworkStateForSlot;
Telephony_RadioResult __real_OH_Telephony_GetNetworkStateForSlot(int32_t slotId, Telephony_NetworkState *state);
Telephony_RadioResult __wrap_OH_Telephony_GetNetworkStateForSlot(int32_t slotId, Telephony_NetworkState *state);

extern std::function<int32_t (NetConn_NetConnCallback *netConnCallback,
		uint32_t *callbackId)> g_mock_OH_NetConn_RegisterDefaultNetConnCallback;
int32_t __real_OH_NetConn_RegisterDefaultNetConnCallback(NetConn_NetConnCallback *netConnCallback,
    uint32_t *callbackId);
int32_t __wrap_OH_NetConn_RegisterDefaultNetConnCallback(NetConn_NetConnCallback *netConnCallback,
    uint32_t *callbackId);

extern std::function<int32_t (uint32_t callbackId)> g_mock_OH_NetConn_UnregisterNetConnCallback;
int32_t __real_OH_NetConn_UnregisterNetConnCallback(uint32_t callbackId);
int32_t __wrap_OH_NetConn_UnregisterNetConnCallback(uint32_t callbackId);

extern std::function<int32_t (NetConn_NetHandle *netHandle,
		NetConn_ConnectionProperties *prop)> g_mock_OH_NetConn_GetConnectionProperties;
int32_t __real_OH_NetConn_GetConnectionProperties(NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *prop);
int32_t __wrap_OH_NetConn_GetConnectionProperties(NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *prop);

extern std::function<int32_t (NetConn_NetHandle *netHandle)> g_mock_OH_NetConn_GetDefaultNet;
int32_t __real_OH_NetConn_GetDefaultNet(NetConn_NetHandle *netHandle);
int32_t __wrap_OH_NetConn_GetDefaultNet(NetConn_NetHandle *netHandle);

extern std::function<int32_t (NetConn_NetHandle *netHandle,
		NetConn_NetCapabilities *netCapabilities)> g_mock_OH_NetConn_GetNetCapabilities;
int32_t __real_OH_NetConn_GetNetCapabilities(NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCapabilities);
int32_t __wrap_OH_NetConn_GetNetCapabilities(NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCapabilities);

extern std::function<int32_t (NetConn_NetHandleList *netHandleList)> g_mock_OH_NetConn_GetAllNets;
int32_t __real_OH_NetConn_GetAllNets(NetConn_NetHandleList *netHandleList);
int32_t __wrap_OH_NetConn_GetAllNets(NetConn_NetHandleList *netHandleList);

extern std::function<Input_DeviceInfo* ()> g_mock_OH_Input_CreateDeviceInfo;
Input_DeviceInfo* __real_OH_Input_CreateDeviceInfo();
Input_DeviceInfo* __wrap_OH_Input_CreateDeviceInfo();

extern std::function<Input_Result (int32_t *deviceIds, int32_t inSize, int32_t *outSize)> g_mock_OH_Input_GetDeviceIds;
Input_Result __real_OH_Input_GetDeviceIds(int32_t *deviceIds, int32_t inSize, int32_t *outSize);
Input_Result __wrap_OH_Input_GetDeviceIds(int32_t *deviceIds, int32_t inSize, int32_t *outSize);

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi 
#endif // MOCK_NDK_API_H

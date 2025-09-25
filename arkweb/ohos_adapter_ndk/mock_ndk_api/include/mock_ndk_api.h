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
#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <unordered_map>
#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <network/netmanager/net_connection_type.h>
#include <network/netmanager/net_connection.h>
#include <telephony/core_service/telephony_radio_type.h>
#include <multimodalinput/oh_input_manager.h>
#include <bundle/native_interface_bundle.h>
#include <multimedia/av_session/native_avmetadata.h>
#include <multimedia/av_session/native_avsession.h>
#include <multimedia/av_session/native_avsession_errors.h>
#include <sensors/oh_sensor.h>

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
class OhosInterfaceMock {
public:
    static OhosInterfaceMock& GetInstance() {
        static OhosInterfaceMock instance;
        return instance;
    }
    MOCK_METHOD(OH_NativeBundle_ElementName, OH_NativeBundle_GetMainElementName, (), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_Create, (OH_AVMetadataBuilder**), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetAssetId, (OH_AVMetadataBuilder*, const char*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_GenerateAVMetadata,
        (OH_AVMetadataBuilder*, OH_AVMetadata**), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadata_Destroy, (OH_AVMetadata*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_Destroy, (OH_AVMetadataBuilder*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_Create,
        (AVSession_Type, const char*, const char*, const char*, OH_AVSession**), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_Destroy, (OH_AVSession*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_Activate, (OH_AVSession*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_Deactivate, (OH_AVSession*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_RegisterCommandCallback,
        (OH_AVSession*, AVSession_ControlCommand, OH_AVSessionCallback_OnCommand*, void*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_RegisterSeekCallback,
        (OH_AVSession*, OH_AVSessionCallback_OnSeek*, void*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetTitle, (OH_AVMetadataBuilder*, const char*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetArtist, (OH_AVMetadataBuilder*, const char*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetAlbum, (OH_AVMetadataBuilder*, const char*), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetDuration, (OH_AVMetadataBuilder*, int64_t), (const));
    MOCK_METHOD(AVMetadata_Result, OH_AVMetadataBuilder_SetMediaImageUri,
        (OH_AVMetadataBuilder*, const char*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_SetPlaybackState, (OH_AVSession*, AVSession_PlaybackState), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_SetAVMetadata, (OH_AVSession*, OH_AVMetadata*), (const));
    MOCK_METHOD(AVSession_ErrCode, OH_AVSession_SetPlaybackPosition,
        (OH_AVSession*, AVSession_PlaybackPosition*), (const));

    static bool bNativeBundleGetMainElementName;
    static bool bAVMetadataBuilderCreate;
    static bool bAVMetadataBuilderSetAssetId;
    static bool bAVMetadataBuilderGenerateAVMetadata;
    static bool bAVMetadataDestroy;
    static bool bAVMetadataBuilderDestroy;
    static bool bAVSessionCreate;
    static bool bAVSessionDestroy;
    static bool bAVSessionActivate;
    static bool bAVSessionDeactivate;
    static bool bAVSessionRegisterCommandCallback;
    static bool bAVSessionRegisterSeekCallback;
    static bool bAVMetadataBuilderSetTitle;
    static bool bAVMetadataBuilderSetArtist;
    static bool bAVMetadataBuilderSetAlbum;
    static bool bAVMetadataBuilderSetDuration;
    static bool bAVMetadataBuilderSetMediaImageUri;
    static bool bAVSessionSetPlaybackState;
    static bool bAVSessionSetAVMetadata;
    static bool bAVSessionSetPlaybackPosition;
};

#ifdef __cplusplus
extern "C" {
#endif
    OH_NativeBundle_ElementName __real_OH_NativeBundle_GetMainElementName();
    AVMetadata_Result __real_OH_AVMetadataBuilder_Create(OH_AVMetadataBuilder**);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetAssetId(OH_AVMetadataBuilder *, const char *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_GenerateAVMetadata(OH_AVMetadataBuilder *, OH_AVMetadata **);
    AVMetadata_Result __real_OH_AVMetadata_Destroy(OH_AVMetadata *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_Destroy(OH_AVMetadataBuilder *);
    AVSession_ErrCode __real_OH_AVSession_Destroy(OH_AVSession *);
    AVSession_ErrCode __real_OH_AVSession_Create(AVSession_Type, const char*, const char*,
                                                 const char*, OH_AVSession**);
    AVSession_ErrCode __real_OH_AVSession_Activate(OH_AVSession *);
    AVSession_ErrCode __real_OH_AVSession_Deactivate(OH_AVSession *);
    AVSession_ErrCode __real_OH_AVSession_RegisterCommandCallback(OH_AVSession *, AVSession_ControlCommand,
                                                                  OH_AVSessionCallback_OnCommand *, void *);
    AVSession_ErrCode __real_OH_AVSession_RegisterSeekCallback(OH_AVSession *, OH_AVSessionCallback_OnSeek *, void *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetTitle(OH_AVMetadataBuilder *, const char *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetArtist(OH_AVMetadataBuilder *, const char *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetAlbum(OH_AVMetadataBuilder *, const char *);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetDuration(OH_AVMetadataBuilder *, int64_t);
    AVMetadata_Result __real_OH_AVMetadataBuilder_SetMediaImageUri(OH_AVMetadataBuilder *, const char *);
    AVSession_ErrCode __real_OH_AVSession_SetPlaybackState(OH_AVSession *, AVSession_PlaybackState);
    AVSession_ErrCode __real_OH_AVSession_SetAVMetadata(OH_AVSession *, OH_AVMetadata *);
    AVSession_ErrCode __real_OH_AVSession_SetPlaybackPosition(OH_AVSession *, AVSession_PlaybackPosition *);
#ifdef __cplusplus
}
#endif

class MockOhSensorSupport {
public:
    static bool enableGetInfos;
    static bool enableCreateInfos;
    static bool enableDestroyInfos;
    static bool enableInfoGetType;
    static bool enableInfoGetMinSamplingInterval;
    static bool enableInfoGetMaxSamplingInterval;
    static bool enableCreateSubscriber;
    static bool enableSubscriberSetCallback;
    static bool enableCreateSubscriptionId;
    static bool enableSubscriptionIdSetType;
    static bool enableCreateSubscriptionAttribute;
    static bool enableSubscriptionAttributeSetSamplingInterval;
    static bool enableSubscribe;
    static bool enableUnsubscribe;
    static bool enableDestroySubscriber;
    static bool enableDestroySubscriptionId;
    static bool enableDestroySubscriptionAttribute;
    static bool enableEventGetType;
    static bool enableEventGetData;
    static bool enableEventGetTimestamp;
    static void EnableAll(bool enable) {
        enableGetInfos = enable;
        enableCreateInfos = enable;
        enableDestroyInfos = enable;
        enableInfoGetType = enable;
        enableInfoGetMinSamplingInterval = enable;
        enableInfoGetMaxSamplingInterval = enable;
        enableCreateSubscriber = enable;
        enableSubscriberSetCallback = enable;
        enableCreateSubscriptionId = enable;
        enableSubscriptionIdSetType = enable;
        enableCreateSubscriptionAttribute = enable;
        enableSubscriptionAttributeSetSamplingInterval = enable;
        enableSubscribe = enable;
        enableUnsubscribe = enable;
        enableDestroySubscriber = enable;
        enableDestroySubscriptionId = enable;
        enableDestroySubscriptionAttribute = enable;
        enableEventGetType = enable;
        enableEventGetData = enable;
        enableEventGetTimestamp = enable;
    }

    static MockOhSensorSupport& getInstance() {
        static MockOhSensorSupport instance;
        return instance;
    }

    MOCK_METHOD(Sensor_Result, OH_Sensor_GetInfos, (Sensor_Info**, uint32_t*));
    MOCK_METHOD(Sensor_Info**, OH_Sensor_CreateInfos, (uint32_t));
    MOCK_METHOD(int32_t, OH_Sensor_DestroyInfos, (Sensor_Info**, uint32_t));
    MOCK_METHOD(int32_t, OH_SensorInfo_GetType, (Sensor_Info*, Sensor_Type*));
    MOCK_METHOD(int32_t, OH_SensorInfo_GetMinSamplingInterval, (Sensor_Info*, int64_t*));
    MOCK_METHOD(int32_t, OH_SensorInfo_GetMaxSamplingInterval, (Sensor_Info*, int64_t*));
    MOCK_METHOD(int32_t, OH_SensorEvent_GetType, (Sensor_Event*, Sensor_Type*));
    MOCK_METHOD(int32_t, OH_SensorEvent_GetData, (Sensor_Event*, float**, uint32_t*));
    MOCK_METHOD(int32_t, OH_SensorEvent_GetTimestamp, (Sensor_Event*, int64_t*));
    MOCK_METHOD(Sensor_Subscriber*, OH_Sensor_CreateSubscriber, ());
    MOCK_METHOD(int32_t, OH_SensorSubscriber_SetCallback, (Sensor_Subscriber*, const Sensor_EventCallback));
    MOCK_METHOD(Sensor_SubscriptionId*, OH_Sensor_CreateSubscriptionId, ());
    MOCK_METHOD(int32_t, OH_SensorSubscriptionId_SetType, (Sensor_SubscriptionId*, const Sensor_Type));
    MOCK_METHOD(Sensor_SubscriptionAttribute*, OH_Sensor_CreateSubscriptionAttribute, ());
    MOCK_METHOD(int32_t, OH_SensorSubscriptionAttribute_SetSamplingInterval, (Sensor_SubscriptionAttribute*, const int64_t));
    MOCK_METHOD(Sensor_Result, OH_Sensor_Subscribe, (const Sensor_SubscriptionId*, const Sensor_SubscriptionAttribute*, const Sensor_Subscriber*));
    MOCK_METHOD(Sensor_Result, OH_Sensor_Unsubscribe, (const Sensor_SubscriptionId*, const Sensor_Subscriber*));
    MOCK_METHOD(int32_t, OH_Sensor_DestroySubscriber, (Sensor_Subscriber*));
    MOCK_METHOD(int32_t, OH_Sensor_DestroySubscriptionId, (Sensor_SubscriptionId*));
    MOCK_METHOD(int32_t, OH_Sensor_DestroySubscriptionAttribute, (Sensor_SubscriptionAttribute*));
};

#ifdef __cplusplus
extern "C" {
#endif
extern Sensor_Result (*__real_OH_Sensor_GetInfos)(Sensor_Info**, uint32_t*);
extern Sensor_Info** (*__real_OH_Sensor_CreateInfos)(uint32_t);
extern int32_t (*__real_OH_Sensor_DestroyInfos)(Sensor_Info**, uint32_t);
extern int32_t (*__real_OH_SensorInfo_GetType)(Sensor_Info*, Sensor_Type*);
extern int32_t (*__real_OH_SensorInfo_GetMinSamplingInterval)(Sensor_Info*, int64_t*);
extern int32_t (*__real_OH_SensorInfo_GetMaxSamplingInterval)(Sensor_Info*, int64_t*);
extern Sensor_Subscriber* (*__real_OH_Sensor_CreateSubscriber)();
extern int32_t (*__real_OH_SensorSubscriber_SetCallback)(Sensor_Subscriber*, Sensor_EventCallback);
extern Sensor_SubscriptionId* (*__real_OH_Sensor_CreateSubscriptionId)();
extern int32_t (*__real_OH_SensorSubscriptionId_SetType)(Sensor_SubscriptionId*, Sensor_Type);
extern Sensor_SubscriptionAttribute* (*__real_OH_Sensor_CreateSubscriptionAttribute)();
extern int32_t (*__real_OH_SensorSubscriptionAttribute_SetSamplingInterval)(Sensor_SubscriptionAttribute*, int64_t);
extern Sensor_Result (*__real_OH_Sensor_Subscribe)(const Sensor_SubscriptionId*, const Sensor_SubscriptionAttribute*, const Sensor_Subscriber*);
extern Sensor_Result (*__real_OH_Sensor_Unsubscribe)(const Sensor_SubscriptionId*, const Sensor_Subscriber*);
extern int32_t (*__real_OH_Sensor_DestroySubscriber)(Sensor_Subscriber*);
extern int32_t (*__real_OH_Sensor_DestroySubscriptionId)(Sensor_SubscriptionId*);
extern int32_t (*__real_OH_Sensor_DestroySubscriptionAttribute)(Sensor_SubscriptionAttribute*);
extern int32_t (*__real_OH_SensorEvent_GetType)(Sensor_Event*, Sensor_Type*);
extern int32_t (*__real_OH_SensorEvent_GetData)(Sensor_Event*, float**, uint32_t*);
extern int32_t (*__real_OH_SensorEvent_GetTimestamp)(Sensor_Event*, int64_t*);

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi 
#endif // MOCK_NDK_API_H

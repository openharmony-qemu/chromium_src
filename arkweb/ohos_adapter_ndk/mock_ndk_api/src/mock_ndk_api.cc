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

bool OhosInterfaceMock::bNativeBundleGetMainElementName = false;
bool OhosInterfaceMock::bAVMetadataBuilderCreate = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetAssetId = false;
bool OhosInterfaceMock::bAVMetadataBuilderGenerateAVMetadata = false;
bool OhosInterfaceMock::bAVMetadataDestroy = false;
bool OhosInterfaceMock::bAVMetadataBuilderDestroy = false;
bool OhosInterfaceMock::bAVSessionCreate = false;
bool OhosInterfaceMock::bAVSessionDestroy = false;
bool OhosInterfaceMock::bAVSessionActivate = false;
bool OhosInterfaceMock::bAVSessionDeactivate = false;
bool OhosInterfaceMock::bAVSessionRegisterCommandCallback = false;
bool OhosInterfaceMock::bAVSessionRegisterSeekCallback = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetTitle = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetArtist = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetAlbum = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetDuration = false;
bool OhosInterfaceMock::bAVMetadataBuilderSetMediaImageUri = false;
bool OhosInterfaceMock::bAVSessionSetPlaybackState = false;
bool OhosInterfaceMock::bAVSessionSetAVMetadata = false;
bool OhosInterfaceMock::bAVSessionSetPlaybackPosition = false;

#ifdef __cplusplus
extern "C" {
#endif
OH_NativeBundle_ElementName __wrap_OH_NativeBundle_GetMainElementName() {
    if (OhosInterfaceMock::bNativeBundleGetMainElementName == true) {
        return OhosInterfaceMock::GetInstance().OH_NativeBundle_GetMainElementName();
    } else {
        return __real_OH_NativeBundle_GetMainElementName();
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_Create(OH_AVMetadataBuilder** builder) {
    if (OhosInterfaceMock::bAVMetadataBuilderCreate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_Create(builder);
    } else {
        return __real_OH_AVMetadataBuilder_Create(builder);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetAssetId(OH_AVMetadataBuilder* builder, const char* assetId) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetAssetId == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetAssetId(builder, assetId);
    } else {
        return __real_OH_AVMetadataBuilder_SetAssetId(builder, assetId);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_GenerateAVMetadata(OH_AVMetadataBuilder* builder,
                                                                 OH_AVMetadata** metadata) {
    if (OhosInterfaceMock::bAVMetadataBuilderGenerateAVMetadata == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_GenerateAVMetadata(builder, metadata);
    } else {
        return __real_OH_AVMetadataBuilder_GenerateAVMetadata(builder, metadata);
    }
}
AVMetadata_Result __wrap_OH_AVMetadata_Destroy(OH_AVMetadata* metadata) {
    if (OhosInterfaceMock::bAVMetadataDestroy == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadata_Destroy(metadata);
    } else {
        return __real_OH_AVMetadata_Destroy(metadata);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_Destroy(OH_AVMetadataBuilder* builder) {
    if (OhosInterfaceMock::bAVMetadataBuilderDestroy == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_Destroy(builder);
    } else {
        return __real_OH_AVMetadataBuilder_Destroy(builder);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_Create(AVSession_Type type,
                                             const char* sessionName,
                                             const char* bundleName,
                                             const char* abilityName,
                                             OH_AVSession** session) {
    if (OhosInterfaceMock::bAVSessionCreate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_Create(type, sessionName, bundleName,
                                                                    abilityName, session);
    } else {
        return __real_OH_AVSession_Create(type, sessionName, bundleName, abilityName, session);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_Destroy(OH_AVSession* session) {
    if (OhosInterfaceMock::bAVSessionDestroy == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_Destroy(session);
    } else {
        return __real_OH_AVSession_Destroy(session);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_Activate(OH_AVSession* session) {
    if (OhosInterfaceMock::bAVSessionActivate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_Activate(session);
    } else {
        return __real_OH_AVSession_Activate(session);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_Deactivate(OH_AVSession* session) {
    if (OhosInterfaceMock::bAVSessionDeactivate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_Deactivate(session);
    } else {
        return __real_OH_AVSession_Deactivate(session);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_RegisterCommandCallback(OH_AVSession* session,
                                                              AVSession_ControlCommand command,
                                                              OH_AVSessionCallback_OnCommand* callback,
                                                              void* userData) {
    if (OhosInterfaceMock::bAVSessionRegisterCommandCallback == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_RegisterCommandCallback(session, command,
                                                                                     callback, userData);
    } else {
        return __real_OH_AVSession_RegisterCommandCallback(session, command, callback, userData);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_RegisterSeekCallback(OH_AVSession* session,
                                                           OH_AVSessionCallback_OnSeek* callback,
                                                           void* userData) {
    if (OhosInterfaceMock::bAVSessionRegisterSeekCallback == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_RegisterSeekCallback(session, callback, userData);
    } else {
        return __real_OH_AVSession_RegisterSeekCallback(session, callback, userData);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetTitle(OH_AVMetadataBuilder* builder, const char* title) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetTitle == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetTitle(builder, title);
    } else {
        return __real_OH_AVMetadataBuilder_SetTitle(builder, title);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetArtist(OH_AVMetadataBuilder* builder, const char* artist) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetArtist == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetArtist(builder, artist);
    } else {
        return __real_OH_AVMetadataBuilder_SetArtist(builder, artist);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetAlbum(OH_AVMetadataBuilder* builder, const char* album) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetAlbum == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetAlbum(builder, album);
    } else {
        return __real_OH_AVMetadataBuilder_SetAlbum(builder, album);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetDuration(OH_AVMetadataBuilder* builder, int64_t duration) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetDuration == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetDuration(builder, duration);
    } else {
        return __real_OH_AVMetadataBuilder_SetDuration(builder, duration);
    }
}
AVMetadata_Result __wrap_OH_AVMetadataBuilder_SetMediaImageUri(OH_AVMetadataBuilder* builder, const char* uri) {
    if (OhosInterfaceMock::bAVMetadataBuilderSetMediaImageUri == true) {
        return OhosInterfaceMock::GetInstance().OH_AVMetadataBuilder_SetMediaImageUri(builder, uri);
    } else {
        return __real_OH_AVMetadataBuilder_SetMediaImageUri(builder, uri);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_SetPlaybackState(OH_AVSession* session, AVSession_PlaybackState state) {
    if (OhosInterfaceMock::bAVSessionSetPlaybackState == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_SetPlaybackState(session, state);
    } else {
        return __real_OH_AVSession_SetPlaybackState(session, state);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_SetAVMetadata(OH_AVSession* session, OH_AVMetadata* metadata) {
    if (OhosInterfaceMock::bAVSessionSetAVMetadata == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_SetAVMetadata(session, metadata);
    } else {
        return __real_OH_AVSession_SetAVMetadata(session, metadata);
    }
}
AVSession_ErrCode __wrap_OH_AVSession_SetPlaybackPosition(OH_AVSession* session,
                                                          AVSession_PlaybackPosition* position) {
    if (OhosInterfaceMock::bAVSessionSetPlaybackPosition == true) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_SetPlaybackPosition(session, position);
    } else {
        return __real_OH_AVSession_SetPlaybackPosition(session, position);
    }
}

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi 

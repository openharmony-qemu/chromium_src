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

std::function<int32_t (OH_NetConn_AppHttpProxyChange appHttpProxyChange,
    uint32_t *callbackId)> g_mock_OH_NetConn_RegisterAppHttpProxyCallback;
int32_t __wrap_OH_NetConn_RegisterAppHttpProxyCallback(OH_NetConn_AppHttpProxyChange appHttpProxyChange,
    uint32_t *callbackId) {
  return g_mock_OH_NetConn_RegisterAppHttpProxyCallback
      ?g_mock_OH_NetConn_RegisterAppHttpProxyCallback(appHttpProxyChange, callbackId)
      :__real_OH_NetConn_RegisterAppHttpProxyCallback(appHttpProxyChange, callbackId);
}


std::function<OH_NativeVSync* (const char* name, unsigned int length)> g_mock_OH_NativeVSync_Create;
OH_NativeVSync* __wrap_OH_NativeVSync_Create(const char* name, unsigned int length) {
  return g_mock_OH_NativeVSync_Create ? g_mock_OH_NativeVSync_Create(name, length)
                                      : __real_OH_NativeVSync_Create(name, length);
}

std::function<int (OH_NativeVSync* nativeVSync, long long* period)> g_mock_OH_NativeVSync_GetPeriod;
int __wrap_OH_NativeVSync_GetPeriod(OH_NativeVSync* nativeVSync, long long* period) {
  return g_mock_OH_NativeVSync_GetPeriod ? g_mock_OH_NativeVSync_GetPeriod(nativeVSync, period)
                                         : __real_OH_NativeVSync_GetPeriod(nativeVSync, period);
}

std::function<int (OH_NativeVSync* nativeVSync, OH_NativeVSync_FrameCallback callback,
    void* data)> g_mock_OH_NativeVSync_RequestFrame;
int __wrap_OH_NativeVSync_RequestFrame(OH_NativeVSync* nativeVSync, OH_NativeVSync_FrameCallback callback,
    void* data) {
  return g_mock_OH_NativeVSync_RequestFrame ? g_mock_OH_NativeVSync_RequestFrame(nativeVSync, callback, data)
                                            : __real_OH_NativeVSync_RequestFrame(nativeVSync, callback, data);
}

std::function<OHNativeWindow* (void* pSurface)> g_mock_OH_NativeWindow_CreateNativeWindow;
OHNativeWindow* __wrap_OH_NativeWindow_CreateNativeWindow(void* pSurface) {
  return g_mock_OH_NativeWindow_CreateNativeWindow ? g_mock_OH_NativeWindow_CreateNativeWindow(pSurface)
                                                   : __real_OH_NativeWindow_CreateNativeWindow(pSurface);
}

std::function<int32_t (void* obj)> g_mock_OH_NativeWindow_NativeObjectReference;
int32_t __wrap_OH_NativeWindow_NativeObjectReference(void* obj) {
  return g_mock_OH_NativeWindow_NativeObjectReference ? g_mock_OH_NativeWindow_NativeObjectReference(obj)
                                                      : __real_OH_NativeWindow_NativeObjectReference(obj);
}

std::function<int32_t (void* obj)> g_mock_OH_NativeWindow_NativeObjectUnreference;
int32_t __wrap_OH_NativeWindow_NativeObjectUnreference(void* obj) {
  return g_mock_OH_NativeWindow_NativeObjectUnreference ? g_mock_OH_NativeWindow_NativeObjectUnreference(obj)
                                                        : __real_OH_NativeWindow_NativeObjectUnreference(obj);
}

std::function<int32_t (OHNativeWindow *window, OHNativeWindowBuffer **buffer,
    int *fenceFd)> g_mock_OH_NativeWindow_NativeWindowRequestBuffer;
int32_t __wrap_OH_NativeWindow_NativeWindowRequestBuffer(OHNativeWindow *window, OHNativeWindowBuffer **buffer,
    int *fenceFd) {
  return g_mock_OH_NativeWindow_NativeWindowRequestBuffer ?
         g_mock_OH_NativeWindow_NativeWindowRequestBuffer(window, buffer, fenceFd) :
         __real_OH_NativeWindow_NativeWindowRequestBuffer(window, buffer, fenceFd);
}

std::function<int32_t (OHNativeWindow *window, OHNativeWindowBuffer *buffer)>
    g_mock_OH_NativeWindow_NativeWindowAbortBuffer;
int32_t __wrap_OH_NativeWindow_NativeWindowAbortBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer) {
  return g_mock_OH_NativeWindow_NativeWindowAbortBuffer ?
         g_mock_OH_NativeWindow_NativeWindowAbortBuffer(window, buffer) :
         __real_OH_NativeWindow_NativeWindowAbortBuffer(window, buffer);
}

std::function<TimeService_ErrCode (char *timeZone, uint32_t len)> g_mock_OH_TimeService_GetTimeZone;
TimeService_ErrCode __wrap_OH_TimeService_GetTimeZone(char *timeZone, uint32_t len) {
  return g_mock_OH_TimeService_GetTimeZone ?
         g_mock_OH_TimeService_GetTimeZone(timeZone, len) :
         __real_OH_TimeService_GetTimeZone(timeZone, len);
}

std::function<void (CommonEvent_SubscribeInfo* info)> g_mock_OH_CommonEvent_DestroySubscribeInfo;
void __wrap_OH_CommonEvent_DestroySubscribeInfo(CommonEvent_SubscribeInfo* info) {
  return g_mock_OH_CommonEvent_DestroySubscribeInfo ?
         g_mock_OH_CommonEvent_DestroySubscribeInfo(info) :
         __real_OH_CommonEvent_DestroySubscribeInfo(info);
}

std::function<void (CommonEvent_Subscriber* subscriber)> g_mock_OH_CommonEvent_DestroySubscriber;
void __wrap_OH_CommonEvent_DestroySubscriber(CommonEvent_Subscriber* subscriber) {
  return g_mock_OH_CommonEvent_DestroySubscriber ?
         g_mock_OH_CommonEvent_DestroySubscriber(subscriber) :
         __real_OH_CommonEvent_DestroySubscriber(subscriber);
}

std::function<int32_t (OH_NativeImage* image, OHNativeWindowBuffer** nativeWindowBuffer,
    int* fenceFd)> g_mock_OH_NativeImage_AcquireNativeWindowBuffer;
int32_t __wrap_OH_NativeImage_AcquireNativeWindowBuffer(OH_NativeImage* image,
    OHNativeWindowBuffer** nativeWindowBuffer, int* fenceFd) {
  return g_mock_OH_NativeImage_AcquireNativeWindowBuffer ?
         g_mock_OH_NativeImage_AcquireNativeWindowBuffer(image, nativeWindowBuffer, fenceFd) :
         __real_OH_NativeImage_AcquireNativeWindowBuffer(image, nativeWindowBuffer, fenceFd);
}

std::function<int32_t (OHNativeWindowBuffer* nativeWindowBuffer, OH_NativeBuffer **buffer)>
    g_mock_OH_NativeBuffer_FromNativeWindowBuffer;
int32_t __wrap_OH_NativeBuffer_FromNativeWindowBuffer(OHNativeWindowBuffer* nativeWindowBuffer,
    OH_NativeBuffer **buffer) {
  return g_mock_OH_NativeBuffer_FromNativeWindowBuffer ?
         g_mock_OH_NativeBuffer_FromNativeWindowBuffer(nativeWindowBuffer, buffer) :
         __real_OH_NativeBuffer_FromNativeWindowBuffer(nativeWindowBuffer, buffer);
}

std::function<BufferHandle* (OHNativeWindowBuffer *buffer)>
    g_mock_OH_NativeWindow_GetBufferHandleFromNative;
BufferHandle* __wrap_OH_NativeWindow_GetBufferHandleFromNative(OHNativeWindowBuffer *buffer) {
  return g_mock_OH_NativeWindow_GetBufferHandleFromNative ?
         g_mock_OH_NativeWindow_GetBufferHandleFromNative(buffer) :
         __real_OH_NativeWindow_GetBufferHandleFromNative(buffer);
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
bool OhosInterfaceMock::bAVCastControllerDestroy = false;
bool OhosInterfaceMock::bAVSessionRegisterOutputDeviceChangeCallback = false;
bool OhosInterfaceMock::bAVSessionUnregisterOutputDeviceChangeCallback = false;
bool OhosInterfaceMock::bDeviceInfoGetDeviceName = false;
bool OhosInterfaceMock::bAVSessionSetRemoteCastEnabled = false;
bool OhosInterfaceMock::bAVSessionStopCasting = false;
bool OhosInterfaceMock::bAVSessionCreateAVCastController = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderCreate = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderDestroy = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionDestroy = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetDuration = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaUri = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetStartPosition = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaType = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetTitle = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAssetId = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAlbumCoverUri = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderGenerateAVMediaDescription = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionGetMediaUri = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionGetStartPosition = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionGetDuration = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionGetTitle = false;
bool OhosInterfaceMock::bAVSessionAVMediaDescriptionGetAssetId = false;
bool OhosInterfaceMock::bAVCastControllerPrepare = false;
bool OhosInterfaceMock::bAVCastControllerStart = false;
bool OhosInterfaceMock::bAVCastControllerRegisterPlaybackStateChangedCallback = false;
bool OhosInterfaceMock::bAVCastControllerRegisterMediaItemChangedCallback = false;
bool OhosInterfaceMock::bAVCastControllerRegisterSeekDoneCallback = false;
bool OhosInterfaceMock::bAVCastControllerRegisterEndOfStreamCallback = false;
bool OhosInterfaceMock::bAVCastControllerRegisterErrorCallback = false;
bool OhosInterfaceMock::bAVCastControllerUnregisterPlaybackStateChangedCallback = false;
bool OhosInterfaceMock::bAVCastControllerUnregisterMediaItemChangedCallback = false;
bool OhosInterfaceMock::bAVCastControllerUnregisterSeekDoneCallback = false;
bool OhosInterfaceMock::bAVCastControllerUnregisterEndOfStreamCallback = false;
bool OhosInterfaceMock::bAVCastControllerUnregisterErrorCallback = false;
bool OhosInterfaceMock::bAVSessionGetPlaybackState = false;
bool OhosInterfaceMock::bAVSessionGetPlaybackPosition = false;
bool OhosInterfaceMock::bAVCastControllerSendCommonCommand = false;
bool OhosInterfaceMock::bAVCastControllerSendSeekCommand = false;
bool OhosInterfaceMock::bAVCastControllerGetPlaybackState = false;

bool MockAudioCommonEventSupport::start = false;
bool MockAudioCommonEventSupport::stop = false;
bool MockAudioCommonEventSupport::release = false;
bool MockAudioCommonEventSupport::getFrameSizeInCallback = false;
bool MockAudioCommonEventSupport::getTimestamp = false;
bool MockAudioCommonEventSupport::create = false;
bool MockAudioCommonEventSupport::setSamplingRate = false;
bool MockAudioCommonEventSupport::destroy = false;
bool MockAudioCommonEventSupport::setEncodingType = false;
bool MockAudioCommonEventSupport::setSampleFormat = false;
bool MockAudioCommonEventSupport::setChannelCount = false;
bool MockAudioCommonEventSupport::setCapturerInfo = false;
bool MockAudioCommonEventSupport::setBufferAttr = false;
bool MockAudioCommonEventSupport::setLatencyMode = false;
bool MockAudioCommonEventSupport::setCapturerCallback = false;
bool MockAudioCommonEventSupport::generateCapturer = false;

bool MockDatashareCommonEventSupport::bGetPath = false;
bool MockDatashareCommonEventSupport::bGetFileNameMock = false;

bool MockNetCommonEventSupport::bGetParameters = false;
bool MockNetCommonEventSupport::bHasKey = false;
bool MockNetCommonEventSupport::bGetInt = false;
bool MockNetCommonEventSupport::bGetCode = false;
bool MockNetCommonEventSupport::bGetCapacity = false;
bool MockNetCommonEventSupport::bGetPluggedType = false;

#ifdef __cplusplus
extern "C" {
#endif

const CommonEvent_Parameters * __wrap_OH_CommonEvent_GetParametersFromRcvData(const CommonEvent_RcvData *rcvData) {
    if (MockNetCommonEventSupport::bGetParameters == true) {
        return MockNetCommonEventSupport::GetInstance().OH_CommonEvent_GetParametersFromRcvData(rcvData);
    } else {
        return __real_OH_CommonEvent_GetParametersFromRcvData(rcvData);
    }
}

bool __wrap_OH_CommonEvent_HasKeyInParameters(const CommonEvent_Parameters *para, const char *key) {
    if (MockNetCommonEventSupport::bHasKey == true) {
        return MockNetCommonEventSupport::GetInstance().OH_CommonEvent_HasKeyInParameters(para, key);
    } else {
        return __real_OH_CommonEvent_HasKeyInParameters(para, key);
    }
}

int __wrap_OH_CommonEvent_GetIntFromParameters(const CommonEvent_Parameters *para, const char *key, int defaultValue) {
    if (MockNetCommonEventSupport::bGetInt == true) {
        return MockNetCommonEventSupport::GetInstance().OH_CommonEvent_GetIntFromParameters(para, key, defaultValue);
    } else {
        return __real_OH_CommonEvent_GetIntFromParameters(para, key, defaultValue);
    }
}

int32_t __wrap_OH_CommonEvent_GetCodeFromRcvData(const CommonEvent_RcvData *rcvData) {
    if (MockNetCommonEventSupport::bGetCode == true) {
        return MockNetCommonEventSupport::GetInstance().OH_CommonEvent_GetCodeFromRcvData(rcvData);
    } else {
        return __real_OH_CommonEvent_GetCodeFromRcvData(rcvData);
    }
}

int32_t __wrap_OH_BatteryInfo_GetCapacity() {
    if (MockNetCommonEventSupport::bGetCapacity == true) {
        return MockNetCommonEventSupport::GetInstance().OH_BatteryInfo_GetCapacity();
    } else {
        return __real_OH_BatteryInfo_GetCapacity();
    }
}

BatteryInfo_BatteryPluggedType __wrap_OH_BatteryInfo_GetPluggedType() {
    if (MockNetCommonEventSupport::bGetPluggedType == true) {
        return MockNetCommonEventSupport::GetInstance().OH_BatteryInfo_GetPluggedType();
    } else {
        return __real_OH_BatteryInfo_GetPluggedType();
    }
}

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

OH_AudioStream_Result __wrap_OH_AudioCapturer_Start(OH_AudioCapturer *capturer) {
    if (MockAudioCommonEventSupport::start) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioCapturer_Start(capturer);
    }
    return __real_OH_AudioCapturer_Start(capturer);
}

OH_AudioStream_Result __wrap_OH_AudioCapturer_Stop(OH_AudioCapturer *capturer) {
    if (MockAudioCommonEventSupport::stop) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioCapturer_Stop(capturer);
    }
    return __real_OH_AudioCapturer_Stop(capturer);
}

OH_AudioStream_Result __wrap_OH_AudioCapturer_Release(OH_AudioCapturer *capturer) {
    if (MockAudioCommonEventSupport::release) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioCapturer_Release(capturer);
    }
    return __real_OH_AudioCapturer_Release(capturer);
}

OH_AudioStream_Result __wrap_OH_AudioCapturer_GetFrameSizeInCallback(OH_AudioCapturer *capturer, int32_t *frameSize) {
    if (MockAudioCommonEventSupport::getFrameSizeInCallback) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioCapturer_GetFrameSizeInCallback(capturer, frameSize);
    }
    return __real_OH_AudioCapturer_GetFrameSizeInCallback(capturer, frameSize);
}

OH_AudioStream_Result __wrap_OH_AudioCapturer_GetTimestamp(OH_AudioCapturer *capturer, clockid_t clockId, 
    int64_t *framePosition, int64_t *timestamp) {
    if (MockAudioCommonEventSupport::getTimestamp) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioCapturer_GetTimestamp(
            capturer, clockId, framePosition, timestamp);
    }
    return __real_OH_AudioCapturer_GetTimestamp(capturer, clockId, framePosition, timestamp);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_Create(OH_AudioStreamBuilder **builder, OH_AudioStream_Type type) {
    if (MockAudioCommonEventSupport::create) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_Create(builder, type);
    }
    return __real_OH_AudioStreamBuilder_Create(builder, type);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetSamplingRate(OH_AudioStreamBuilder *builder, int32_t rate) {
    if (MockAudioCommonEventSupport::setSamplingRate) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetSamplingRate(builder, rate);
    }
    return __real_OH_AudioStreamBuilder_SetSamplingRate(builder, rate);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_Destroy(OH_AudioStreamBuilder *builder) {
    if (MockAudioCommonEventSupport::destroy) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_Destroy(builder);
    }
    return __real_OH_AudioStreamBuilder_Destroy(builder);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetEncodingType(
    OH_AudioStreamBuilder *builder, OH_AudioStream_EncodingType encodingType) {
    if (MockAudioCommonEventSupport::setEncodingType) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetEncodingType(builder, encodingType);
    }
    return __real_OH_AudioStreamBuilder_SetEncodingType(builder, encodingType);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetSampleFormat(
    OH_AudioStreamBuilder *builder, OH_AudioStream_SampleFormat format) {
    if (MockAudioCommonEventSupport::setSampleFormat) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetSampleFormat(builder, format);
    }
    return __real_OH_AudioStreamBuilder_SetSampleFormat(builder, format);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetChannelCount(OH_AudioStreamBuilder *builder, int32_t channelCount) {
    if (MockAudioCommonEventSupport::setChannelCount) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetChannelCount(builder, channelCount);
    }
    return __real_OH_AudioStreamBuilder_SetChannelCount(builder, channelCount);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetCapturerInfo(
    OH_AudioStreamBuilder *builder, OH_AudioStream_SourceType sourceType) {
    if (MockAudioCommonEventSupport::setCapturerInfo) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetCapturerInfo(builder, sourceType);
    }
    return __real_OH_AudioStreamBuilder_SetCapturerInfo(builder, sourceType);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetLatencyMode(
    OH_AudioStreamBuilder *builder, OH_AudioStream_LatencyMode latencyMode) {
    if (MockAudioCommonEventSupport::setLatencyMode) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetLatencyMode(builder, latencyMode);
    }
    return __real_OH_AudioStreamBuilder_SetLatencyMode(builder, latencyMode);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_SetCapturerCallback(
    OH_AudioStreamBuilder *builder, OH_AudioCapturer_Callbacks callbacks, void *userData) {
    if (MockAudioCommonEventSupport::setCapturerCallback) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_SetCapturerCallback(
            builder, callbacks, userData);
    }
    return __real_OH_AudioStreamBuilder_SetCapturerCallback(builder, callbacks, userData);
}

OH_AudioStream_Result __wrap_OH_AudioStreamBuilder_GenerateCapturer(
    OH_AudioStreamBuilder *builder, OH_AudioCapturer **audioCapturer) {
    if (MockAudioCommonEventSupport::generateCapturer) {
        return MockAudioCommonEventSupport::GetInstance().OH_AudioStreamBuilder_GenerateCapturer(builder, audioCapturer);
    }
    return __real_OH_AudioStreamBuilder_GenerateCapturer(builder, audioCapturer);
}

FileManagement_ErrCode __wrap_OH_FileUri_GetPathFromUri(const char* uri,
                                                        unsigned int length,
                                                        char** uriResult) {
  if (MockDatashareCommonEventSupport::bGetPath) {
    return MockDatashareCommonEventSupport::GetInstance().GetPathFromUriMock(
        uri, length, uriResult);
  }
  return __real_OH_FileUri_GetPathFromUri(uri, length, uriResult);
}

FileManagement_ErrCode __wrap_OH_FileUri_GetFileName(const char* uri,
                                                     unsigned int length,
                                                     char** fileName) {
  if (MockDatashareCommonEventSupport::bGetFileNameMock) {
    return MockDatashareCommonEventSupport::GetInstance().GetFileNameMock(
        uri, length, fileName);
  }
  return __real_OH_FileUri_GetFileName(uri, length, fileName);
}

#ifdef __cplusplus
}
#endif

bool MockOhSensorSupport::enableGetInfos = false;
bool MockOhSensorSupport::enableCreateInfos = false;
bool MockOhSensorSupport::enableDestroyInfos = false;
bool MockOhSensorSupport::enableInfoGetType = false;
bool MockOhSensorSupport::enableInfoGetMinSamplingInterval = false;
bool MockOhSensorSupport::enableInfoGetMaxSamplingInterval = false;
bool MockOhSensorSupport::enableCreateSubscriber = false;
bool MockOhSensorSupport::enableSubscriberSetCallback = false;
bool MockOhSensorSupport::enableCreateSubscriptionId = false;
bool MockOhSensorSupport::enableSubscriptionIdSetType = false;
bool MockOhSensorSupport::enableCreateSubscriptionAttribute = false;
bool MockOhSensorSupport::enableSubscriptionAttributeSetSamplingInterval = false;
bool MockOhSensorSupport::enableSubscribe = false;
bool MockOhSensorSupport::enableUnsubscribe = false;
bool MockOhSensorSupport::enableDestroySubscriber = false;
bool MockOhSensorSupport::enableDestroySubscriptionId = false;
bool MockOhSensorSupport::enableDestroySubscriptionAttribute = false;
bool MockOhSensorSupport::enableEventGetType = false;
bool MockOhSensorSupport::enableEventGetData = false;
bool MockOhSensorSupport::enableEventGetTimestamp = false;

#ifdef __cplusplus
extern "C" {
#endif

Sensor_Result __wrap_OH_Sensor_GetInfos(Sensor_Info** infos, uint32_t* count) {
    if (MockOhSensorSupport::enableGetInfos) {
        return MockOhSensorSupport::getInstance().OH_Sensor_GetInfos(infos, count);
    } else {
        return __real_OH_Sensor_GetInfos(infos, count);
    }
}

Sensor_Info** __wrap_OH_Sensor_CreateInfos(uint32_t count) {
    if (MockOhSensorSupport::enableCreateInfos) {
        return MockOhSensorSupport::getInstance().OH_Sensor_CreateInfos(count);
    } else {
        return __real_OH_Sensor_CreateInfos(count);
    }
}

int32_t __wrap_OH_Sensor_DestroyInfos(Sensor_Info** infos, uint32_t count) {
    if (MockOhSensorSupport::enableDestroyInfos) {
        return MockOhSensorSupport::getInstance().OH_Sensor_DestroyInfos(infos, count);
    } else {
        return __real_OH_Sensor_DestroyInfos(infos, count);
    }
}

int32_t __wrap_OH_SensorInfo_GetType(Sensor_Info* info, Sensor_Type* type) {
    if (MockOhSensorSupport::enableInfoGetType) {
        return MockOhSensorSupport::getInstance().OH_SensorInfo_GetType(info, type);
    } else {
        return __real_OH_SensorInfo_GetType(info, type);
    }
}

int32_t __wrap_OH_SensorInfo_GetMinSamplingInterval(Sensor_Info* info, int64_t* interval) {
    if (MockOhSensorSupport::enableInfoGetMinSamplingInterval) {
        return MockOhSensorSupport::getInstance().OH_SensorInfo_GetMinSamplingInterval(info, interval);
    } else {
        return __real_OH_SensorInfo_GetMinSamplingInterval(info, interval);
    }
}

int32_t __wrap_OH_SensorInfo_GetMaxSamplingInterval(Sensor_Info* info, int64_t* interval) {
    if (MockOhSensorSupport::enableInfoGetMaxSamplingInterval) {
        return MockOhSensorSupport::getInstance().OH_SensorInfo_GetMaxSamplingInterval(info, interval);
    } else {
        return __real_OH_SensorInfo_GetMaxSamplingInterval(info, interval);
    }
}

Sensor_Subscriber* __wrap_OH_Sensor_CreateSubscriber() {
    if (MockOhSensorSupport::enableCreateSubscriber) {
        return MockOhSensorSupport::getInstance().OH_Sensor_CreateSubscriber();
    } else {
        return __real_OH_Sensor_CreateSubscriber();
    }
}

int32_t __wrap_OH_SensorSubscriber_SetCallback(Sensor_Subscriber* subscriber, Sensor_EventCallback callback) {
    if (MockOhSensorSupport::enableSubscriberSetCallback) {
        return MockOhSensorSupport::getInstance().OH_SensorSubscriber_SetCallback(subscriber, callback);
    } else {
        return __real_OH_SensorSubscriber_SetCallback(subscriber, callback);
    }
}

Sensor_SubscriptionId* __wrap_OH_Sensor_CreateSubscriptionId() {
    if (MockOhSensorSupport::enableCreateSubscriptionId) {
        return MockOhSensorSupport::getInstance().OH_Sensor_CreateSubscriptionId();
    } else {
        return __real_OH_Sensor_CreateSubscriptionId();
    }
}

int32_t __wrap_OH_SensorSubscriptionId_SetType(Sensor_SubscriptionId* id, Sensor_Type type) {
    if (MockOhSensorSupport::enableSubscriptionIdSetType) {
        return MockOhSensorSupport::getInstance().OH_SensorSubscriptionId_SetType(id, type);
    } else {
        return __real_OH_SensorSubscriptionId_SetType(id, type);
    }
}

Sensor_SubscriptionAttribute* __wrap_OH_Sensor_CreateSubscriptionAttribute() {
    if (MockOhSensorSupport::enableCreateSubscriptionAttribute) {
        return MockOhSensorSupport::getInstance().OH_Sensor_CreateSubscriptionAttribute();
    } else {
        return __real_OH_Sensor_CreateSubscriptionAttribute();
    }
}

int32_t __wrap_OH_SensorSubscriptionAttribute_SetSamplingInterval(Sensor_SubscriptionAttribute* attr, int64_t samplingInterval) {
    if (MockOhSensorSupport::enableSubscriptionAttributeSetSamplingInterval) {
        return MockOhSensorSupport::getInstance().OH_SensorSubscriptionAttribute_SetSamplingInterval(attr, samplingInterval);
    } else {
        return __real_OH_SensorSubscriptionAttribute_SetSamplingInterval(attr, samplingInterval);
    }
}

Sensor_Result __wrap_OH_Sensor_Subscribe(const Sensor_SubscriptionId* id, const Sensor_SubscriptionAttribute* attr, const Sensor_Subscriber* subscriber) {
    if (MockOhSensorSupport::enableSubscribe) {
        return MockOhSensorSupport::getInstance().OH_Sensor_Subscribe(id, attr, subscriber);
    } else {
        return __real_OH_Sensor_Subscribe(id, attr, subscriber);
    }
}

Sensor_Result __wrap_OH_Sensor_Unsubscribe(const Sensor_SubscriptionId* id, const Sensor_Subscriber* subscriber) {
    if (MockOhSensorSupport::enableUnsubscribe) {
        return MockOhSensorSupport::getInstance().OH_Sensor_Unsubscribe(id, subscriber);
    } else {
        return __real_OH_Sensor_Unsubscribe(id, subscriber);
    }
}

int32_t __wrap_OH_Sensor_DestroySubscriber(Sensor_Subscriber* subscriber) {
    if (MockOhSensorSupport::enableDestroySubscriber) {
        return MockOhSensorSupport::getInstance().OH_Sensor_DestroySubscriber(subscriber);
    } else {
        return __real_OH_Sensor_DestroySubscriber(subscriber);
    }
}

int32_t __wrap_OH_Sensor_DestroySubscriptionId(Sensor_SubscriptionId* id) {
    if (MockOhSensorSupport::enableDestroySubscriptionId) {
        return MockOhSensorSupport::getInstance().OH_Sensor_DestroySubscriptionId(id);
    } else {
        return __real_OH_Sensor_DestroySubscriptionId(id);
    }
}

int32_t __wrap_OH_Sensor_DestroySubscriptionAttribute(Sensor_SubscriptionAttribute* attr) {
    if (MockOhSensorSupport::enableDestroySubscriptionAttribute) {
        return MockOhSensorSupport::getInstance().OH_Sensor_DestroySubscriptionAttribute(attr);
    } else {
        return __real_OH_Sensor_DestroySubscriptionAttribute(attr);
    }
}

int32_t __wrap_OH_SensorEvent_GetType(Sensor_Event* event, Sensor_Type* type) {
    if (MockOhSensorSupport::enableEventGetType) {
        return MockOhSensorSupport::getInstance().OH_SensorEvent_GetType(event, type);
    } else {
        return __real_OH_SensorEvent_GetType(event, type);
    }
}

int32_t __wrap_OH_SensorEvent_GetData(Sensor_Event* event, float** data, uint32_t* length) {
    if (MockOhSensorSupport::enableEventGetData) {
        return MockOhSensorSupport::getInstance().OH_SensorEvent_GetData(event, data, length);
    } else {
        return __real_OH_SensorEvent_GetData(event, data, length);
    }
}

int32_t __wrap_OH_SensorEvent_GetTimestamp(Sensor_Event* event, int64_t* timestamp) {
    if (MockOhSensorSupport::enableEventGetTimestamp) {
        return MockOhSensorSupport::getInstance().OH_SensorEvent_GetTimestamp(event, timestamp);
    } else {
        return __real_OH_SensorEvent_GetTimestamp(event, timestamp);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_Destroy(OH_AVCastController* controller) {
    if (OhosInterfaceMock::bAVCastControllerDestroy) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_Destroy(controller);
    } else {
        return __real_OH_AVCastController_Destroy(controller);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_RegisterOutputDeviceChangeCallback(OH_AVSession* session,
    OH_AVSessionCallback_OutputDeviceChange callback) {
    if (OhosInterfaceMock::bAVSessionRegisterOutputDeviceChangeCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_RegisterOutputDeviceChangeCallback(session, callback);
    } else {
        return __real_OH_AVSession_RegisterOutputDeviceChangeCallback(session, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_UnregisterOutputDeviceChangeCallback(OH_AVSession* session,
    OH_AVSessionCallback_OutputDeviceChange callback) {
    if (OhosInterfaceMock::bAVSessionUnregisterOutputDeviceChangeCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_UnregisterOutputDeviceChangeCallback(session, callback);
    } else {
        return __real_OH_AVSession_UnregisterOutputDeviceChangeCallback(session, callback);
    }
}

AVSession_ErrCode __wrap_OH_DeviceInfo_GetDeviceName(AVSession_DeviceInfo* deviceInfo, char** deviceName) {
    if (OhosInterfaceMock::bDeviceInfoGetDeviceName) {
        return OhosInterfaceMock::GetInstance().OH_DeviceInfo_GetDeviceName(deviceInfo, deviceName);
    } else {
        return __real_OH_DeviceInfo_GetDeviceName(deviceInfo, deviceName);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_SetRemoteCastEnabled(OH_AVSession* session, bool enabled) {
    if (OhosInterfaceMock::bAVSessionSetRemoteCastEnabled) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_SetRemoteCastEnabled(session, enabled);
    } else {
        return __real_OH_AVSession_SetRemoteCastEnabled(session, enabled);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_StopCasting(OH_AVSession* session) {
    if (OhosInterfaceMock::bAVSessionStopCasting) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_StopCasting(session);
    } else {
        return __real_OH_AVSession_StopCasting(session);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_CreateAVCastController(OH_AVSession* session, OH_AVCastController** controller) {
    if (OhosInterfaceMock::bAVSessionCreateAVCastController) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_CreateAVCastController(session, controller);
    } else {
        return __real_OH_AVSession_CreateAVCastController(session, controller);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_Create(OH_AVSession_AVMediaDescriptionBuilder** builder) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderCreate) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_Create(builder);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_Create(builder);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_Destroy(OH_AVSession_AVMediaDescriptionBuilder* builder) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderDestroy) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_Destroy(builder);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_Destroy(builder);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_Destroy(OH_AVSession_AVMediaDescription* description) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionDestroy) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_Destroy(description);
    } else {
        return __real_OH_AVSession_AVMediaDescription_Destroy(description);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetDuration(OH_AVSession_AVMediaDescriptionBuilder* builder, const int32_t duration) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetDuration) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetDuration(builder, duration);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetDuration(builder, duration);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(OH_AVSession_AVMediaDescriptionBuilder* builder, const char* uri) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaUri) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(builder, uri);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(builder, uri);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(OH_AVSession_AVMediaDescriptionBuilder* builder, const int32_t position) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetStartPosition) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(builder, position);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(builder, position);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(OH_AVSession_AVMediaDescriptionBuilder* builder, const char* type) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaType) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(builder, type);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(builder, type);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetTitle(OH_AVSession_AVMediaDescriptionBuilder* builder, const char* title) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetTitle) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetTitle(builder, title);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetTitle(builder, title);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(OH_AVSession_AVMediaDescriptionBuilder* builder, const char* assetId) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAssetId) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(builder, assetId);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(builder, assetId);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri(OH_AVSession_AVMediaDescriptionBuilder* builder, const char* uri) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAlbumCoverUri) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri(builder, uri);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri(builder, uri);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(
    OH_AVSession_AVMediaDescriptionBuilder* builder, OH_AVSession_AVMediaDescription** description) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderGenerateAVMediaDescription) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(builder, description);
    } else {
        return __real_OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(builder, description);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_GetMediaUri(OH_AVSession_AVMediaDescription* description, char** uri) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionGetMediaUri) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_GetMediaUri(description, uri);
    } else {
        return __real_OH_AVSession_AVMediaDescription_GetMediaUri(description, uri);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_GetStartPosition(OH_AVSession_AVMediaDescription* description, int32_t* position) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionGetStartPosition) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_GetStartPosition(description, position);
    } else {
        return __real_OH_AVSession_AVMediaDescription_GetStartPosition(description, position);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_GetDuration(OH_AVSession_AVMediaDescription* description, int32_t* duration) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionGetDuration) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_GetDuration(description, duration);
    } else {
        return __real_OH_AVSession_AVMediaDescription_GetDuration(description, duration);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_GetTitle(OH_AVSession_AVMediaDescription* description, char** title) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionGetTitle) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_GetTitle(description, title);
    } else {
        return __real_OH_AVSession_AVMediaDescription_GetTitle(description, title);
    }
}

AVQueueItem_Result __wrap_OH_AVSession_AVMediaDescription_GetAssetId(OH_AVSession_AVMediaDescription* description, char** assetId) {
    if (OhosInterfaceMock::bAVSessionAVMediaDescriptionGetAssetId) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_AVMediaDescription_GetAssetId(description, assetId);
    } else {
        return __real_OH_AVSession_AVMediaDescription_GetAssetId(description, assetId);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_Prepare(OH_AVCastController* controller, OH_AVSession_AVQueueItem* item) {
    if (OhosInterfaceMock::bAVCastControllerPrepare) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_Prepare(controller, item);
    } else {
        return __real_OH_AVCastController_Prepare(controller, item);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_Start(OH_AVCastController* controller, OH_AVSession_AVQueueItem* item) {
    if (OhosInterfaceMock::bAVCastControllerStart) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_Start(controller, item);
    } else {
        return __real_OH_AVCastController_Start(controller, item);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_RegisterPlaybackStateChangedCallback(OH_AVCastController* controller, int32_t filter,
    OH_AVCastControllerCallback_PlaybackStateChanged callback, void* userData) {
    if (OhosInterfaceMock::bAVCastControllerRegisterPlaybackStateChangedCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_RegisterPlaybackStateChangedCallback(controller, filter, callback, userData);
    } else {
        return __real_OH_AVCastController_RegisterPlaybackStateChangedCallback(controller, filter, callback, userData);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_RegisterMediaItemChangedCallback(OH_AVCastController* controller, int32_t filter,
    OH_AVCastControllerCallback_MediaItemChange callback, void* userData) {
    if (OhosInterfaceMock::bAVCastControllerRegisterMediaItemChangedCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_RegisterMediaItemChangedCallback(controller, filter, callback, userData);
    } else {
        return __real_OH_AVCastController_RegisterMediaItemChangedCallback(controller, filter, callback, userData);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_RegisterSeekDoneCallback(OH_AVCastController* controller, int32_t filter,
    OH_AVCastControllerCallback_SeekDone callback, void* userData) {
    if (OhosInterfaceMock::bAVCastControllerRegisterSeekDoneCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_RegisterSeekDoneCallback(controller, filter, callback, userData);
    } else {
        return __real_OH_AVCastController_RegisterSeekDoneCallback(controller, filter, callback, userData);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_RegisterEndOfStreamCallback(OH_AVCastController* controller, int32_t filter,
    OH_AVCastControllerCallback_EndOfStream callback, void* userData) {
    if (OhosInterfaceMock::bAVCastControllerRegisterEndOfStreamCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_RegisterEndOfStreamCallback(controller, filter, callback, userData);
    } else {
        return __real_OH_AVCastController_RegisterEndOfStreamCallback(controller, filter, callback, userData);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_RegisterErrorCallback(OH_AVCastController* controller, int32_t filter,
    OH_AVCastControllerCallback_Error callback, void* userData) {
    if (OhosInterfaceMock::bAVCastControllerRegisterErrorCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_RegisterErrorCallback(controller, filter, callback, userData);
    } else {
        return __real_OH_AVCastController_RegisterErrorCallback(controller, filter, callback, userData);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_UnregisterPlaybackStateChangedCallback(OH_AVCastController* controller,
    OH_AVCastControllerCallback_PlaybackStateChanged callback) {
    if (OhosInterfaceMock::bAVCastControllerUnregisterPlaybackStateChangedCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_UnregisterPlaybackStateChangedCallback(controller, callback);
    } else {
        return __real_OH_AVCastController_UnregisterPlaybackStateChangedCallback(controller, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_UnregisterMediaItemChangedCallback(OH_AVCastController* controller,
    OH_AVCastControllerCallback_MediaItemChange callback) {
    if (OhosInterfaceMock::bAVCastControllerUnregisterMediaItemChangedCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_UnregisterMediaItemChangedCallback(controller, callback);
    } else {
        return __real_OH_AVCastController_UnregisterMediaItemChangedCallback(controller, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_UnregisterSeekDoneCallback(OH_AVCastController* controller,
    OH_AVCastControllerCallback_SeekDone callback) {
    if (OhosInterfaceMock::bAVCastControllerUnregisterSeekDoneCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_UnregisterSeekDoneCallback(controller, callback);
    } else {
        return __real_OH_AVCastController_UnregisterSeekDoneCallback(controller, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_UnregisterEndOfStreamCallback(OH_AVCastController* controller,
    OH_AVCastControllerCallback_EndOfStream callback) {
    if (OhosInterfaceMock::bAVCastControllerUnregisterEndOfStreamCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_UnregisterEndOfStreamCallback(controller, callback);
    } else {
        return __real_OH_AVCastController_UnregisterEndOfStreamCallback(controller, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_UnregisterErrorCallback(OH_AVCastController* controller,
    OH_AVCastControllerCallback_Error callback) {
    if (OhosInterfaceMock::bAVCastControllerUnregisterErrorCallback) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_UnregisterErrorCallback(controller, callback);
    } else {
        return __real_OH_AVCastController_UnregisterErrorCallback(controller, callback);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_GetPlaybackState(OH_AVSession_AVPlaybackState* playbackState,
    AVSession_PlaybackState* state) {
    if (OhosInterfaceMock::bAVSessionGetPlaybackState) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_GetPlaybackState(playbackState, state);
    } else {
        return __real_OH_AVSession_GetPlaybackState(playbackState, state);
    }
}

AVSession_ErrCode __wrap_OH_AVSession_GetPlaybackPosition(OH_AVSession_AVPlaybackState* playbackState,
    AVSession_PlaybackPosition* position) {
    if (OhosInterfaceMock::bAVSessionGetPlaybackPosition) {
        return OhosInterfaceMock::GetInstance().OH_AVSession_GetPlaybackPosition(playbackState, position);
    } else {
        return __real_OH_AVSession_GetPlaybackPosition(playbackState, position);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_SendCommonCommand(OH_AVCastController* controller,
    AVSession_AVCastControlCommandType* cmdType) {
    if (OhosInterfaceMock::bAVCastControllerSendCommonCommand) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_SendCommonCommand(controller, cmdType);
    } else {
        return __real_OH_AVCastController_SendCommonCommand(controller, cmdType);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_SendSeekCommand(OH_AVCastController* controller, int32_t position) {
    if (OhosInterfaceMock::bAVCastControllerSendSeekCommand) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_SendSeekCommand(controller, position);
    } else {
        return __real_OH_AVCastController_SendSeekCommand(controller, position);
    }
}

AVSession_ErrCode __wrap_OH_AVCastController_GetPlaybackState(OH_AVCastController*controller,
    OH_AVSession_AVPlaybackState* playbackState) {
    if (OhosInterfaceMock::bAVCastControllerGetPlaybackState) {
        return OhosInterfaceMock::GetInstance().OH_AVCastController_GetPlaybackState(controller, playbackState);
    } else {
        return __real_OH_AVCastController_GetPlaybackState(controller, playbackState);
    }
}

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi 

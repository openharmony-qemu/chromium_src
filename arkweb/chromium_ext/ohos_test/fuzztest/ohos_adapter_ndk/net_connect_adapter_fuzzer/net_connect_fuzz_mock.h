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
#ifndef NET_CONNECT_FUZZ_MOCK_H
#define NET_CONNECT_FUZZ_MOCK_H

#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>
#include <BasicServicesKit/oh_commonevent.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/core_service/telephony_radio.h>
#include <telephony/core_service/telephony_radio_type.h>

#define FUZZ_MOCK_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif

struct CArrParameters {
  int32_t netType;
};

typedef struct CommonEvent_RcvData {
  std::string event;
  std::string bundleName;
  int32_t code;
  std::string data;
  CArrParameters* parameters = nullptr;
} CommonEvent_RcvData;

FUZZ_MOCK_EXPORT const char* OH_CommonEvent_GetEventFromRcvData(
    const CommonEvent_RcvData* rcvData);

FUZZ_MOCK_EXPORT const CommonEvent_Parameters*
OH_CommonEvent_GetParametersFromRcvData(const CommonEvent_RcvData* rcvData);

FUZZ_MOCK_EXPORT int32_t
OH_CommonEvent_GetCodeFromRcvData(const CommonEvent_RcvData* rcvData);

FUZZ_MOCK_EXPORT bool OH_CommonEvent_HasKeyInParameters(
    const CommonEvent_Parameters* para,
    const char* key);

FUZZ_MOCK_EXPORT int OH_CommonEvent_GetIntFromParameters(
    const CommonEvent_Parameters* para,
    const char* key,
    const int defaultValue);

FUZZ_MOCK_EXPORT int32_t OH_NetConn_RegisterDefaultNetConnCallback(
    NetConn_NetConnCallback* netConnCallback,
    uint32_t* callbackId);

FUZZ_MOCK_EXPORT int32_t
OH_NetConn_UnregisterNetConnCallback(uint32_t callBackId);

FUZZ_MOCK_EXPORT int32_t OH_NetConn_GetDefaultNet(NetConn_NetHandle* netHandle);

FUZZ_MOCK_EXPORT int32_t
OH_NetConn_GetNetCapabilities(NetConn_NetHandle* netHandle,
                              NetConn_NetCapabilities* netCapacities);

FUZZ_MOCK_EXPORT int32_t OH_Telephony_GetDefaultCellularDataSlotId(void);

FUZZ_MOCK_EXPORT Telephony_RadioResult
OH_Telephony_GetNetworkStateForSlot(int32_t slotId,
                                    Telephony_NetworkState* state);

FUZZ_MOCK_EXPORT int32_t
OH_NetConn_GetAllNets(NetConn_NetHandleList* netHandleList);

FUZZ_MOCK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetNativeBufferPropertiesOHOS(VkDevice device,
                                const struct OH_NativeBuffer* buffer,
                                VkNativeBufferPropertiesOHOS* pProperties);

FUZZ_MOCK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSurfaceOHOS(VkInstance instance,
                    const VkSurfaceCreateInfoOHOS* pCreateInfo,
                    const VkAllocationCallbacks* pAllocator,
                    VkSurfaceKHR* pSurface);

#ifdef __cplusplus
}
#endif

#endif  // NET_CONNECT_FUZZ_MOCK_H
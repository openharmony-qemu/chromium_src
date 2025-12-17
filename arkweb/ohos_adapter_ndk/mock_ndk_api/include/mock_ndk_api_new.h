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

#ifndef MOCK_NDK_API_NEW_H
#define MOCK_NDK_API_NEW_H

#include <functional>
#include <AbilityKit/ability_runtime/ability_runtime_common.h>
#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <database/pasteboard/oh_pasteboard.h>
#include <database/pasteboard/oh_pasteboard_err_code.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimodalinput/oh_input_manager.h>
#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/core_service/telephony_radio_type.h>

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_api_define.h"

namespace MockNdkApi {

class MockOHOSFunction {
 public:
  static MockOHOSFunction& GetInstance() {
    static MockOHOSFunction instance;
    return instance;
  }

  MOCK_METHOD(Pasteboard_GetDataParams*,
              OH_Pasteboard_GetDataParams_Create,
              ());
  MOCK_METHOD(AbilityRuntime_ErrorCode,
              OH_AbilityRuntime_ApplicationContextGetCacheDir,
              (char* buffer, int32_t bufferSize, int32_t* writeLength));
  MOCK_METHOD(void,
              OH_Pasteboard_GetDataParams_SetProgressIndicator,
              (Pasteboard_GetDataParams * params,
               Pasteboard_ProgressIndicator progressIndicator));
  MOCK_METHOD(void,
              OH_Pasteboard_GetDataParams_SetDestUri,
              (Pasteboard_GetDataParams * params,
               const char* destUri,
               uint32_t destUriLen));
  MOCK_METHOD(void,
              OH_Pasteboard_GetDataParams_SetFileConflictOptions,
              (Pasteboard_GetDataParams * params,
               Pasteboard_FileConflictOptions option));
  MOCK_METHOD(OH_UdmfData*,
              OH_Pasteboard_GetDataWithProgress,
              (OH_Pasteboard * pasteboard,
               Pasteboard_GetDataParams* params,
               int* status));
  MOCK_METHOD(bool, OH_Pasteboard_HasData, (OH_Pasteboard * pasteboard));
  MOCK_METHOD(void,
              OH_Pasteboard_GetDataParams_Destroy,
              (Pasteboard_GetDataParams * params));
  MOCK_METHOD(OH_UdmfRecord**,
              OH_UdmfData_GetRecords,
              (OH_UdmfData * pThis, unsigned int* count));
  MOCK_METHOD(bool, OH_UdmfData_IsLocal, (OH_UdmfData * data));

  static bool pasteboard_GetDataParams_Create;
  static bool abilityRuntime_ApplicationContextGetCacheDir;
  static bool pasteboard_GetDataParams_SetProgressIndicator;
  static bool pasteboard_GetDataParams_SetDestUri;
  static bool pasteboard_GetDataParams_SetFileConflictOptions;
  static bool pasteboard_GetDataWithProgress;
  static bool pasteboard_HasData;
  static bool pasteboard_GetDataParams_Destroy;
  static bool udmfData_GetRecords;
  static bool udmfData_IsLocal;
};

#ifdef __cplusplus
extern "C" {
#endif
DECLARE_REAL_FUNC(OH_AVErrCode, OH_VideoDecoder_Destroy, (OH_AVCodec * codec));
Pasteboard_GetDataParams* __real_OH_Pasteboard_GetDataParams_Create();
AbilityRuntime_ErrorCode __real_OH_AbilityRuntime_ApplicationContextGetCacheDir(
    char* buffer,
    int32_t bufferSize,
    int32_t* writeLength);
void __real_OH_Pasteboard_GetDataParams_SetProgressIndicator(
    Pasteboard_GetDataParams* params,
    Pasteboard_ProgressIndicator progressIndicator);
void __real_OH_Pasteboard_GetDataParams_SetDestUri(
    Pasteboard_GetDataParams* params,
    const char* destUri,
    uint32_t destUriLen);
void __real_OH_Pasteboard_GetDataParams_SetFileConflictOptions(
    Pasteboard_GetDataParams* params,
    Pasteboard_FileConflictOptions option);
OH_UdmfData* __real_OH_Pasteboard_GetDataWithProgress(
    OH_Pasteboard* pasteboard,
    Pasteboard_GetDataParams* params,
    int* status);
bool __real_OH_Pasteboard_HasData(OH_Pasteboard* pasteboard);
void __real_OH_Pasteboard_GetDataParams_Destroy(
    Pasteboard_GetDataParams* params);
OH_UdmfRecord** __real_OH_UdmfData_GetRecords(OH_UdmfData* pThis,
                                              unsigned int* count);
bool __real_OH_UdmfData_IsLocal(OH_UdmfData* data);

#ifdef __cplusplus
}
#endif
}  // namespace MockNdkApi
#endif  // MOCK_NDK_API_H

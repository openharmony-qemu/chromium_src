/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/ctocpp/ark_hi_trace_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::StartTrace(const ArkWebString &value,
                                         float limit) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_trace, );

  // Execute
  _struct->start_trace(_struct, &value, limit);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::FinishTrace() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, finish_trace, );

  // Execute
  _struct->finish_trace(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::StartAsyncTrace(const ArkWebString &value,
                                              int32_t taskId, float limit) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_async_trace, );

  // Execute
  _struct->start_async_trace(_struct, &value, taskId, limit);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::FinishAsyncTrace(const ArkWebString &value,
                                               int32_t taskId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, finish_async_trace, );

  // Execute
  _struct->finish_async_trace(_struct, &value, taskId);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::CountTrace(const ArkWebString &name,
                                         int64_t count) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, count_trace, );

  // Execute
  _struct->count_trace(_struct, &name, count);
}

ARK_WEB_NO_SANITIZE
bool ArkHiTraceAdapterCToCpp::IsHiTraceEnable() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_hi_trace_enable, false);

  // Execute
  return _struct->is_hi_trace_enable(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::StartOHOSTrace(const ArkWebString &value,
                                         float limit) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_trace, );

  // Execute
  _struct->start_ohos_trace(_struct, &value, limit);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::FinishOHOSTrace() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, finish_trace, );

  // Execute
  _struct->finish_ohos_trace(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkHiTraceAdapterCToCpp::CountOHOSTrace(const ArkWebString &name,
                                         int64_t count) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_hi_trace_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, count_trace, );

  // Execute
  _struct->count_ohos_trace(_struct, &name, count);
}

ArkHiTraceAdapterCToCpp::ArkHiTraceAdapterCToCpp() {
}

ArkHiTraceAdapterCToCpp::~ArkHiTraceAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkHiTraceAdapterCToCpp, ArkHiTraceAdapter,
                           ark_hi_trace_adapter_t>::kBridgeType =
        ARK_HI_TRACE_ADAPTER;

} // namespace OHOS::ArkWeb

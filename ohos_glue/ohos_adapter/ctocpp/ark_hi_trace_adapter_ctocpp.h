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

#ifndef ARK_HI_TRACE_ADAPTER_CTOCPP_H_
#define ARK_HI_TRACE_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_hitrace_adapter_capi.h"
#include "ohos_adapter/include/ark_hitrace_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkHiTraceAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkHiTraceAdapterCToCpp, ArkHiTraceAdapter,
                                    ark_hi_trace_adapter_t> {
public:
  ArkHiTraceAdapterCToCpp();
  virtual ~ArkHiTraceAdapterCToCpp();

  // ArkHiTraceAdapter methods.
  void StartTrace(const ArkWebString &value, float limit) override;

  void FinishTrace() override;

  void StartAsyncTrace(const ArkWebString &value, int32_t taskId,
                       float limit) override;

  void FinishAsyncTrace(const ArkWebString &value, int32_t taskId) override;

  void CountTrace(const ArkWebString &name, int64_t count) override;

  bool IsHiTraceEnable() override;

  void StartOHOSTrace(const ArkWebString &value, float limit) override;

  void FinishOHOSTrace() override;

  void CountOHOSTrace(const ArkWebString &name, int64_t count) override;

};

} // namespace OHOS::ArkWeb

#endif // ARK_HI_TRACE_ADAPTER_CTOCPP_H_

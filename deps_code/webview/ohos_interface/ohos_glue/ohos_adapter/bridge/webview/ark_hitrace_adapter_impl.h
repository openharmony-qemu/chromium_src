/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_HITRACE_ADAPTER_IMPL_H
#define ARK_HITRACE_ADAPTER_IMPL_H
#pragma once

#include "hitrace_adapter.h"
#include "ohos_adapter/include/ark_hitrace_adapter.h"

namespace OHOS::ArkWeb {

class ArkHiTraceAdapterImpl : public ArkHiTraceAdapter {
public:
    explicit ArkHiTraceAdapterImpl(NWeb::HiTraceAdapter&);

    void StartTrace(const ArkWebString& value, float limit) override;

    void FinishTrace() override;

    void StartAsyncTrace(const ArkWebString& value, int32_t taskId, float limit) override;

    void FinishAsyncTrace(const ArkWebString& value, int32_t taskId) override;

    void CountTrace(const ArkWebString& name, int64_t count) override;

    bool IsHiTraceEnable() override;

    void StartOHOSTrace(const ArkWebString& value, float limit) override;

    void FinishOHOSTrace() override;

    void CountOHOSTrace(const ArkWebString& name, int64_t count) override;

    bool IsACETraceEnable() override;

private:
    NWeb::HiTraceAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkHiTraceAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_HITRACE_ADAPTER_IMPL_H

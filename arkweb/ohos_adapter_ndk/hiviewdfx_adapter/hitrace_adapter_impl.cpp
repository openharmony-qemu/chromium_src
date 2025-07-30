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

#include "hitrace_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"

#include <hitrace/trace.h>

namespace OHOS::NWeb {
HiTraceAdapterImpl& HiTraceAdapterImpl::GetInstance()
{
    static HiTraceAdapterImpl instance;
    return instance;
}

int ConvertToInt(const char *originValue, int defaultValue)
{
    return originValue == nullptr ? defaultValue : std::atoi(originValue);
}

uint64_t HiTraceAdapterImpl::ConvertToInt64(const char *originValue, uint64_t defaultValue)
{
    return originValue == nullptr ? defaultValue : std::strtoull(originValue, nullptr, decimal_numeral_system_);
}

void HiTraceAdapterImpl::StartTrace(const std::string& value, float limit)
{
    OH_HiTrace_StartTrace(value.c_str());
}

void HiTraceAdapterImpl::FinishTrace()
{
    OH_HiTrace_FinishTrace();
}

void HiTraceAdapterImpl::StartAsyncTrace(const std::string& value, int32_t taskId, float limit)
{
    OH_HiTrace_StartAsyncTrace(value.c_str(), taskId);
}

void HiTraceAdapterImpl::FinishAsyncTrace(const std::string& value, int32_t taskId)
{
    OH_HiTrace_FinishAsyncTrace(value.c_str(), taskId);
}

void HiTraceAdapterImpl::CountTrace(const std::string& name, int64_t count)
{
    OH_HiTrace_CountTrace(name.c_str(), count);
}

bool HiTraceAdapterImpl::IsHiTraceEnable()
{
    std::string enable = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                            .GetStringParameter("debug.hitrace.tags.enableflags", "0");
    uint64_t tags = ConvertToInt64(enable.c_str(), 0);
    return (tags & arkweb_hitrace_enable_);
}

void HiTraceAdapterImpl::StartOHOSTrace(const std::string& value, float limit)
{
    OH_HiTrace_StartTrace(value.c_str());
}

void HiTraceAdapterImpl::FinishOHOSTrace()
{
    OH_HiTrace_FinishTrace();
}

void HiTraceAdapterImpl::CountOHOSTrace(const std::string& name, int64_t count)
{
    OH_HiTrace_CountTrace(name.c_str(), count);
}

bool HiTraceAdapterImpl::IsACETraceEnable()
{
    return false;
}
} // namespace OHOS::NWeb

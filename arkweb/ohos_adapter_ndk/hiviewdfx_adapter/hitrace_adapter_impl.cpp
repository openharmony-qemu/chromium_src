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
#include "arkweb/ohos_adapter_ndk/utils/include/sys_param.h"

#include <cstdlib>
#include <sstream>
#include <hitrace/trace.h>

namespace OHOS::NWeb {
constexpr static int DECIMAL_NUMERAL_SYSTEM = 10;
extern "C" __attribute__((weak)) void *CachedParameterCreate(const char *name, const char *defValue);

HiTraceAdapterImpl& HiTraceAdapterImpl::GetInstance()
{
    static HiTraceAdapterImpl instance;
    return instance;
}

int ConvertToInt(const char *originValue, int defaultValue)
{
    if (originValue == nullptr) {
        return defaultValue;
    }
    std::istringstream origin_value_stream(originValue);
    int origin_value_int = 0;
    origin_value_stream >> origin_value_int;
    return origin_value_int;
}

uint64_t HiTraceAdapterImpl::ConvertToUint64(const char *originValue, uint64_t defaultValue)
{
    if (originValue == nullptr) {
        return defaultValue;
    }
    char *endptr = nullptr;
    uint64_t res = std::strtoull(originValue, &endptr, DECIMAL_NUMERAL_SYSTEM);
    if (endptr == originValue) {
        WVLOG_E("failed to strtoull, return default value");
        return defaultValue;
    }
    return res;
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
#if defined(OS_OHOS)
    static void* g_handle = CachedParameterCreate("debug.hitrace.tags.enableflags", "0");
    if (!g_handle) {
        return false;
    }
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_handle, &changed);
    uint64_t tags = ConvertToUint64(enable, 0);
    return (tags & ARKWEB_HITRACE_ENABLE) != 0;
#else
    return true;
#endif
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

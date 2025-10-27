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

#include "ohos_adapter/bridge/ark_hitrace_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkHiTraceAdapterWrapper::ArkHiTraceAdapterWrapper(ArkWebRefPtr<ArkHiTraceAdapter> ref) : ctocpp_(ref) {}

void ArkHiTraceAdapterWrapper::StartTrace(const std::string& value, float limit)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(value);
    ctocpp_->StartTrace(str, limit);

    ArkWebStringStructRelease(str);
}

void ArkHiTraceAdapterWrapper::FinishTrace()
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->FinishTrace();
}

void ArkHiTraceAdapterWrapper::StartAsyncTrace(const std::string& value, int32_t taskId, float limit)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(value);
    ctocpp_->StartAsyncTrace(str, taskId, limit);

    ArkWebStringStructRelease(str);
}

void ArkHiTraceAdapterWrapper::FinishAsyncTrace(const std::string& value, int32_t taskId)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(value);
    ctocpp_->FinishAsyncTrace(str, taskId);

    ArkWebStringStructRelease(str);
}

void ArkHiTraceAdapterWrapper::CountTrace(const std::string& name, int64_t count)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(name);
    ctocpp_->CountTrace(str, count);

    ArkWebStringStructRelease(str);
}

bool ArkHiTraceAdapterWrapper::IsHiTraceEnable()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsHiTraceEnable();
}

void ArkHiTraceAdapterWrapper::StartOHOSTrace(const std::string& value, float limit)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(value);
    ctocpp_->StartOHOSTrace(str, limit);

    ArkWebStringStructRelease(str);
}

void ArkHiTraceAdapterWrapper::FinishOHOSTrace()
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->FinishOHOSTrace();
}

void ArkHiTraceAdapterWrapper::CountOHOSTrace(const std::string& name, int64_t count)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString str = ArkWebStringClassToStruct(name);
    ctocpp_->CountOHOSTrace(str, count);

    ArkWebStringStructRelease(str);
}

bool ArkHiTraceAdapterWrapper::IsACETraceEnable()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsACETraceEnable();
}
} // namespace OHOS::ArkWeb

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

#include "ohos_adapter/bridge/ark_date_time_format_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_timezone_event_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDateTimeFormatAdapterWrapper::ArkDateTimeFormatAdapterWrapper(ArkWebRefPtr<ArkDateTimeFormatAdapter> ref)
    : ctocpp_(ref)
{}

void ArkDateTimeFormatAdapterWrapper::RegTimezoneEvent(
    std::shared_ptr<NWeb::TimezoneEventCallbackAdapter> eventCallback)
{
    if (CHECK_SHARED_PTR_IS_NULL(eventCallback)) {
        return ctocpp_->RegTimezoneEvent(nullptr);
    }

    ctocpp_->RegTimezoneEvent(new ArkTimezoneEventCallbackAdapterImpl(eventCallback));
}

bool ArkDateTimeFormatAdapterWrapper::StartListen()
{
    return ctocpp_->StartListen();
}

void ArkDateTimeFormatAdapterWrapper::StopListen()
{
    return ctocpp_->StopListen();
}

std::string ArkDateTimeFormatAdapterWrapper::GetTimezone()
{
    ArkWebString str = ctocpp_->GetTimezone();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

} // namespace OHOS::ArkWeb

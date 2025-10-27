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

#include "ohos_adapter/bridge/ark_date_time_format_adapter_impl.h"

#include "ohos_adapter/bridge/ark_timezone_event_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDateTimeFormatAdapterImpl::ArkDateTimeFormatAdapterImpl(std::shared_ptr<OHOS::NWeb::DateTimeFormatAdapter> ref)
    : real_(ref)
{}

void ArkDateTimeFormatAdapterImpl::RegTimezoneEvent(ArkWebRefPtr<ArkTimezoneEventCallbackAdapter> eventCallback)
{
    if (CHECK_REF_PTR_IS_NULL(eventCallback)) {
        return real_->RegTimezoneEvent(nullptr);
    }

    real_->RegTimezoneEvent(std::make_shared<ArkTimezoneEventCallbackAdapterWrapper>(eventCallback));
}

bool ArkDateTimeFormatAdapterImpl::StartListen()
{
    return real_->StartListen();
}

void ArkDateTimeFormatAdapterImpl::StopListen()
{
    return real_->StopListen();
}

ArkWebString ArkDateTimeFormatAdapterImpl::GetTimezone()
{
    std::string str = real_->GetTimezone();
    return ArkWebStringClassToStruct(str);
}

} // namespace OHOS::ArkWeb

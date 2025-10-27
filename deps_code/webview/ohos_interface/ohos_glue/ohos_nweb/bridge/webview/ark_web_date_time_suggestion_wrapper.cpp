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

#include "ohos_nweb/bridge/ark_web_date_time_suggestion_wrapper.h"

#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTimeSuggestionWrapper::ArkWebDateTimeSuggestionWrapper(
    ArkWebRefPtr<ArkWebDateTimeSuggestion> ark_web_date_time_suggestion)
    : ark_web_date_time_suggestion_(ark_web_date_time_suggestion)
{}

std::string ArkWebDateTimeSuggestionWrapper::GetLabel()
{
    ArkWebString stLabel = ark_web_date_time_suggestion_->GetLabel();

    std::string objLabel = ArkWebStringStructToClass(stLabel);
    ArkWebStringStructRelease(stLabel);
    return objLabel;
}

OHOS::NWeb::DateTime ArkWebDateTimeSuggestionWrapper::GetValue()
{
    ArkWebDateTime ark_web_date_time = ark_web_date_time_suggestion_->GetValue();
    return ArkWebDateTimeStructToClass(ark_web_date_time);
}

std::string ArkWebDateTimeSuggestionWrapper::GetLocalizedValue()
{
    ArkWebString stLocalizedValue = ark_web_date_time_suggestion_->GetLocalizedValue();

    std::string objLocalizedValue = ArkWebStringStructToClass(stLocalizedValue);
    ArkWebStringStructRelease(stLocalizedValue);
    return objLocalizedValue;
}

} // namespace OHOS::ArkWeb

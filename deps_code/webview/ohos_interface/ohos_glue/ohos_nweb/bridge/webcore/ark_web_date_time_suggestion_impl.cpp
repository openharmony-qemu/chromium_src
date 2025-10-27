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

#include "ohos_nweb/bridge/ark_web_date_time_suggestion_impl.h"

#include "ohos_nweb/bridge/ark_web_core_struct_utils.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTimeSuggestionImpl::ArkWebDateTimeSuggestionImpl(
    std::shared_ptr<OHOS::NWeb::NWebDateTimeSuggestion> nweb_date_time_suggestion)
    : nweb_date_time_suggestion_(nweb_date_time_suggestion)
{}

ArkWebString ArkWebDateTimeSuggestionImpl::GetLabel()
{
    return ArkWebStringClassToStruct(nweb_date_time_suggestion_->GetLabel());
}

ArkWebDateTime ArkWebDateTimeSuggestionImpl::GetValue()
{
    OHOS::NWeb::DateTime nweb_date_time = nweb_date_time_suggestion_->GetValue();
    return ArkWebDateTimeClassToStruct(nweb_date_time);
}

ArkWebString ArkWebDateTimeSuggestionImpl::GetLocalizedValue()
{
    return ArkWebStringClassToStruct(nweb_date_time_suggestion_->GetLocalizedValue());
}

} // namespace OHOS::ArkWeb

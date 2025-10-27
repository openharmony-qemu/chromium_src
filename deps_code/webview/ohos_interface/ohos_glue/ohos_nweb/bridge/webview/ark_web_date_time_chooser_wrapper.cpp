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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_date_time_chooser_wrapper.h"

#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTimeChooserWrapper::ArkWebDateTimeChooserWrapper(
    ArkWebRefPtr<ArkWebDateTimeChooser> ark_web_date_time_chooser)
    : ark_web_date_time_chooser_(ark_web_date_time_chooser)
{}

ArkWebDateTimeChooserType ArkWebDateTimeChooserWrapper::GetType()
{
    return static_cast<ArkWebDateTimeChooserType>(ark_web_date_time_chooser_->GetType());
}

double ArkWebDateTimeChooserWrapper::GetStep()
{
    return ark_web_date_time_chooser_->GetStep();
}

OHOS::NWeb::DateTime ArkWebDateTimeChooserWrapper::GetMinimum()
{
    return ArkWebDateTimeStructToClass(ark_web_date_time_chooser_->GetMinimum());
}

OHOS::NWeb::DateTime ArkWebDateTimeChooserWrapper::GetMaximum()
{
    return ArkWebDateTimeStructToClass(ark_web_date_time_chooser_->GetMaximum());
}

OHOS::NWeb::DateTime ArkWebDateTimeChooserWrapper::GetDialogValue()
{
    return ArkWebDateTimeStructToClass(ark_web_date_time_chooser_->GetDialogValue());
}

bool ArkWebDateTimeChooserWrapper::GetHasSelected()
{
    return ark_web_date_time_chooser_->GetHasSelected();
}

size_t ArkWebDateTimeChooserWrapper::GetSuggestionIndex()
{
    return ark_web_date_time_chooser_->GetSuggestionIndex();
}

} // namespace OHOS::ArkWeb

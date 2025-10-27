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

#include "ohos_nweb/bridge/ark_web_date_time_chooser_impl.h"

#include "ohos_nweb/bridge/ark_web_core_struct_utils.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTimeChooserImpl::ArkWebDateTimeChooserImpl(
    std::shared_ptr<OHOS::NWeb::NWebDateTimeChooser> nweb_date_time_chooser)
    : nweb_date_time_chooser_(nweb_date_time_chooser)
{}

int ArkWebDateTimeChooserImpl::GetType()
{
    return static_cast<int>(nweb_date_time_chooser_->GetType());
}

double ArkWebDateTimeChooserImpl::GetStep()
{
    return nweb_date_time_chooser_->GetStep();
}

ArkWebDateTime ArkWebDateTimeChooserImpl::GetMinimum()
{
    return ArkWebDateTimeClassToStruct(nweb_date_time_chooser_->GetMinimum());
}

ArkWebDateTime ArkWebDateTimeChooserImpl::GetMaximum()
{
    return ArkWebDateTimeClassToStruct(nweb_date_time_chooser_->GetMaximum());
}

ArkWebDateTime ArkWebDateTimeChooserImpl::GetDialogValue()
{
    return ArkWebDateTimeClassToStruct(nweb_date_time_chooser_->GetDialogValue());
}

bool ArkWebDateTimeChooserImpl::GetHasSelected()
{
    return nweb_date_time_chooser_->GetHasSelected();
}

size_t ArkWebDateTimeChooserImpl::GetSuggestionIndex()
{
    return nweb_date_time_chooser_->GetSuggestionIndex();
}

} // namespace OHOS::ArkWeb

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

#include "nweb_date_time_chooser_impl.h"

namespace OHOS::NWeb {

NWebDateTimeChooserImpl::NWebDateTimeChooserImpl(double step, DateTime minimum,
                                                 DateTime maximum,
                                                 DateTime dialogValue,
                                                 DateTimeChooserType type)
    : step_(step), minimum_(minimum), maximum_(maximum),
      dialogValue_(dialogValue), type_(type) {
}

DateTimeChooserType NWebDateTimeChooserImpl::GetType() {
  return type_;
}

double NWebDateTimeChooserImpl::GetStep() {
  return step_;
}

DateTime NWebDateTimeChooserImpl::GetMinimum() {
  return minimum_;
}

DateTime NWebDateTimeChooserImpl::GetMaximum() {
  return maximum_;
}

DateTime NWebDateTimeChooserImpl::GetDialogValue() {
  return dialogValue_;
}

bool NWebDateTimeChooserImpl::GetHasSelected() {
  return hasSelected_;
}

void NWebDateTimeChooserImpl::SetHasSelected(bool hasSelected) {
  hasSelected_ = hasSelected;
}

size_t NWebDateTimeChooserImpl::GetSuggestionIndex() {
  return suggestionIndex_;
}

void NWebDateTimeChooserImpl::SetSuggestionIndex(size_t suggestionIndex) {
  suggestionIndex_ = suggestionIndex;
}

} // namespace OHOS::NWeb

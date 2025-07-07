/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_date_time_chooser_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void NWebDateTimeChooserImplFuzzTest(FuzzedDataProvider* fdp) {
    double step = fdp->ConsumeFloatingPoint<double>();
    DateTime minimum;
    minimum.year = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    minimum.month = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    minimum.day = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    minimum.hour = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    minimum.minute = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    minimum.second = fdp->ConsumeIntegralInRange<int32_t>(0, 3000);
    DateTime maximum(minimum.year,minimum.month,minimum.day,minimum.hour,minimum.minute,minimum.second);
    DateTime dialogValue(minimum.year,minimum.month,minimum.day,minimum.hour,minimum.minute,minimum.second);
    NWebDateTimeChooserImpl impl(step, minimum, maximum, dialogValue, OHOS::NWeb::DTC_DATE);

    bool hasSelected = fdp->ConsumeBool();
    impl.SetHasSelected(hasSelected);

    size_t suggestionIndex = fdp->ConsumeIntegralInRange<size_t>(0, 256);
    impl.SetSuggestionIndex(suggestionIndex);

    impl.GetType();

    impl.GetStep();

    impl.GetMinimum();

    impl.GetMaximum();

    impl.GetDialogValue();

    impl.GetHasSelected();

    impl.GetSuggestionIndex();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebDateTimeChooserImplFuzzTest(&fdp);
  
  return 0;
}

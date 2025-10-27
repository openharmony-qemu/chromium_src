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

#ifndef ARK_WEB_DATE_TIME_SUGGESTION_WRAPPER_H_
#define ARK_WEB_DATE_TIME_SUGGESTION_WRAPPER_H_
#pragma once

#include "include/nweb_date_time_chooser.h"
#include "ohos_nweb/include/ark_web_date_time_suggestion.h"

namespace OHOS::ArkWeb {

class ArkWebDateTimeSuggestionWrapper : public OHOS::NWeb::NWebDateTimeSuggestion {
public:
    ArkWebDateTimeSuggestionWrapper(ArkWebRefPtr<ArkWebDateTimeSuggestion> ark_web_date_time_suggestion);
    ~ArkWebDateTimeSuggestionWrapper() = default;

    std::string GetLabel() override;

    OHOS::NWeb::DateTime GetValue() override;

    std::string GetLocalizedValue() override;

private:
    ArkWebRefPtr<ArkWebDateTimeSuggestion> ark_web_date_time_suggestion_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DATE_TIME_SUGGESTION_WRAPPER_H_

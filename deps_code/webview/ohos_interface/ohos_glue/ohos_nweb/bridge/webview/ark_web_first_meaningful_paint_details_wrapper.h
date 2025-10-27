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

#ifndef ARK_WEB_FIRST_MEANINGFUL_PAINT_DETAILS_WRAPPER_H_
#define ARK_WEB_FIRST_MEANINGFUL_PAINT_DETAILS_WRAPPER_H_
#pragma once

#include "include/nweb_first_meaningful_paint_details.h"
#include "ohos_nweb/include/ark_web_first_meaningful_paint_details.h"

namespace OHOS::ArkWeb {

class ArkWebFirstMeaningfulPaintDetailsWrapper : public OHOS::NWeb::NWebFirstMeaningfulPaintDetails {
public:
    explicit ArkWebFirstMeaningfulPaintDetailsWrapper(
        ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> nweb_first_meaningful_paint_details);
    ~ArkWebFirstMeaningfulPaintDetailsWrapper() = default;

    /**
     * @brief Get start time of navigation.
     *
     * @return Start time of navigation.
     */
    int64_t GetNavigationStartTime() override;

    /**
     * @brief Get paint time of first meaningful content.
     *
     * @return Paint time of first meaningful content.
     */
    int64_t GetFirstMeaningfulPaintTime() override;

private:
    ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> ark_web_first_meaningful_paint_details_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H_

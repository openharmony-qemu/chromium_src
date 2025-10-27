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

#ifndef ARK_WEB_LOAD_COMMITTED_DETAILS_WRAPPER_H_
#define ARK_WEB_LOAD_COMMITTED_DETAILS_WRAPPER_H_
#pragma once

#include "include/nweb_load_committed_details.h"
#include "ohos_nweb/include/ark_web_load_committed_details.h"

namespace OHOS::ArkWeb {

using ArkWebNavigationType = OHOS::NWeb::NWebLoadCommittedDetails::NavigationType;

class ArkWebLoadCommittedDetailsWrapper : public OHOS::NWeb::NWebLoadCommittedDetails {
public:
    ArkWebLoadCommittedDetailsWrapper(ArkWebRefPtr<ArkWebLoadCommittedDetails> ark_web_load_committed_details);
    ~ArkWebLoadCommittedDetailsWrapper() = default;

    /**
     * @brief Check whether the request is for getting the main frame.
     *
     * @return Is main frame.
     */
    bool IsMainFrame() override;

    /**
     * @brief Check whether document and other documents have the same
     * properties.
     *
     * @return Is the same document.
     */
    bool IsSameDocument() override;

    /**
     * @brief Check whether the entry is replaced.
     *
     * @return The entry is replaced.
     */
    bool DidReplaceEntry() override;

    /**
     * @brief Get the value of the navigation type.
     *
     * @return The value of the navigation type.
     */
    ArkWebNavigationType GetNavigationType() override;

    /**
     * @brief Gets the url of the current navigation.
     *
     * @return The url of the current navigation.
     */
    std::string GetURL() override;

private:
    ArkWebRefPtr<ArkWebLoadCommittedDetails> ark_web_load_committed_details_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_LOAD_COMMITTED_DETAILS_WRAPPER_H_

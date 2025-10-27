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

#ifndef ARK_WEB_LOAD_COMMITTED_DETAILS_H_
#define ARK_WEB_LOAD_COMMITTED_DETAILS_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebLoadCommittedDetails : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Check whether the request is for getting the main frame.
     *
     * @return Is main frame.
     */
    /*--ark web()--*/
    virtual bool IsMainFrame() = 0;

    /**
     * @brief Check whether document and other documents have the same
     * properties.
     *
     * @return Is the same document.
     */
    /*--ark web()--*/
    virtual bool IsSameDocument() = 0;

    /**
     * @brief Check whether the entry is replaced.
     *
     * @return The entry is replaced.
     */
    /*--ark web()--*/
    virtual bool DidReplaceEntry() = 0;

    /**
     * @brief Get the value of the navigation type.
     *
     * @return The value of the navigation type.
     */
    /*--ark web()--*/
    virtual int GetNavigationType() = 0;

    /**
     * @brief Gets the url of the current navigation.
     *
     * @return The url of the current navigation.
     */
    /*--ark web()--*/
    virtual ArkWebString GetURL() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_LOAD_COMMITTED_DETAILS_H_

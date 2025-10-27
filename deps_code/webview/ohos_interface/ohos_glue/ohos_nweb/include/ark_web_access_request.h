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

#ifndef ARK_WEB_ACCESS_REQUEST_H_
#define ARK_WEB_ACCESS_REQUEST_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebAccessRequest : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Agree the origin to access the given resources. The granted access
     *        is only valid for this WebView.
     *
     * @param resource_id id of the resource agreed to be accessed by origin. It
     *        must be equal to requested resource id returned by
     *        {@link#GetResourceAccessId()}.
     */
    /*--ark web()--*/
    virtual void Agree(int resource_id) = 0;

    /**
     * @brief Get the origin of the web page which is trying to access the
     *        resource.
     *
     * @return the origin of the web page which is trying to access the resource.
     */
    /*--ark web()--*/
    virtual ArkWebString Origin() = 0;

    /**
     * @brief Refuse the request.
     */
    /*--ark web()--*/
    virtual void Refuse() = 0;

    /**
     * @brief Get the resource id of the web page which is trying to access.
     *
     * @return the resource id of the web page which is trying to access.
     */
    /*--ark web()--*/
    virtual int ResourceAccessId() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ACCESS_REQUEST_H_

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

#ifndef ARK_WEB_URL_RESOURCE_REQUEST_H_
#define ARK_WEB_URL_RESOURCE_REQUEST_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebUrlResourceRequest : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Gets the URL for which the resource request was made.
     *
     * @return URL url string
     */
    /*--ark web()--*/
    virtual ArkWebString Url() = 0;

    /**
     * @brief Gets the method associated with the request, for example "GET".
     *
     * @return method the method associated with the request.
     */
    /*--ark web()--*/
    virtual ArkWebString Method() = 0;

    /**
     * @brief Gets whether a gesture (such as a click) was associated with the
     *        request.
     *
     * @return gesture whether was associated with the request.
     */
    /*--ark web()--*/
    virtual bool FromGesture() = 0;

    /**
     * @brief Gets the headers associated with the request.
     *
     * @return the headers associated with the request.
     */
    /*--ark web()--*/
    virtual ArkWebStringMap RequestHeaders() = 0;

    /**
     * @brief Gets whether the request was made in order to fetch the main frame's
     *        document.
     *
     * @return Is main frame
     */
    /*--ark web()--*/
    virtual bool IsAboutMainFrame() = 0;

    /**
     * @brief Gets whether the request was a result of a server-side redirect.
     *
     * @return is redirect
     */
    /*--ark web()--*/
    virtual bool IsRequestRedirect() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_REQUEST_H_

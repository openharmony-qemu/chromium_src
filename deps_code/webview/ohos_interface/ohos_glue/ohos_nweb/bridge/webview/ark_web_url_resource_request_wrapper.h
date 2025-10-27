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

#ifndef ARK_WEB_URL_RESOURCE_REQUEST_WRAPPER_H_
#define ARK_WEB_URL_RESOURCE_REQUEST_WRAPPER_H_
#pragma once

#include "include/nweb_url_resource_request.h"
#include "ohos_nweb/include/ark_web_url_resource_request.h"

namespace OHOS::ArkWeb {

class ArkWebUrlResourceRequestWrapper : public OHOS::NWeb::NWebUrlResourceRequest {
public:
    ArkWebUrlResourceRequestWrapper(ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_url_resource_request);
    ~ArkWebUrlResourceRequestWrapper() = default;

    /**
     * @brief Gets the URL for which the resource request was made.
     *
     * @return URL url string
     */
    std::string Url() override;

    /**
     * @brief Gets the method associated with the request, for example "GET".
     *
     * @return method the method associated with the request.
     */
    std::string Method() override;

    /**
     * @brief Gets whether a gesture (such as a click) was associated with the
     *        request.
     *
     * @return gesture whether was associated with the request.
     */
    bool FromGesture() override;

    /**
     * @brief Gets the headers associated with the request.
     *
     * @return the headers associated with the request.
     */
    std::map<std::string, std::string> RequestHeaders() override;

    /**
     * @brief Gets whether the request was made in order to fetch the main frame's
     *        document.
     *
     * @return Is main frame
     */
    bool IsAboutMainFrame() override;

    /**
     * @brief Gets whether the request was a result of a server-side redirect.
     *
     * @return is redirect
     */
    bool IsRequestRedirect() override;

private:
    ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_url_resource_request_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_REQUEST_WRAPPER_H_

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

#ifndef ARK_WEB_ACCESS_REQUEST_WRAPPER_H_
#define ARK_WEB_ACCESS_REQUEST_WRAPPER_H_
#pragma once

#include "include/nweb_access_request.h"
#include "ohos_nweb/include/ark_web_access_request.h"

namespace OHOS::ArkWeb {

class ArkWebAccessRequestWrapper : public OHOS::NWeb::NWebAccessRequest {
public:
    ArkWebAccessRequestWrapper(ArkWebRefPtr<ArkWebAccessRequest> ark_web_access_request);
    ~ArkWebAccessRequestWrapper() = default;

    /**
     * @brief Agree the origin to access the given resources. The granted access
     *        is only valid for this WebView.
     *
     * @param resource_id id of the resource agreed to be accessed by origin. It
     *        must be equal to requested resource id returned by
     *        {@link#GetResourceAccessId()}.
     */
    void Agree(int resource_id) override;

    /**
     * @brief Get the origin of the web page which is trying to access the
     *        resource.
     *
     * @return the origin of the web page which is trying to access the resource.
     */
    std::string Origin() override;

    /**
     * @brief Refuse the request.
     */
    void Refuse() override;

    /**
     * @brief Get the resource id of the web page which is trying to access.
     *
     * @return the resource id of the web page which is trying to access.
     */
    int ResourceAcessId() override;

private:
    ArkWebRefPtr<ArkWebAccessRequest> ark_web_access_request_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ACCESS_REQUEST_WRAPPER_H_

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

#ifndef ARK_WEB_JS_HTTP_AUTH_RESULT_WRAPPER_H_
#define ARK_WEB_JS_HTTP_AUTH_RESULT_WRAPPER_H_
#pragma once

#include "include/nweb_js_http_auth_result.h"
#include "ohos_nweb/include/ark_web_js_http_auth_result.h"

namespace OHOS::ArkWeb {

class ArkWebJsHttpAuthResultWrapper : public OHOS::NWeb::NWebJSHttpAuthResult {
public:
    ArkWebJsHttpAuthResultWrapper(ArkWebRefPtr<ArkWebJsHttpAuthResult> ark_web_js_http_auth_result);
    ~ArkWebJsHttpAuthResultWrapper() = default;

    /**
     * @brief Handle the result if the user cancelled the dialog.
     */
    void Cancel() override;

    /**
     * @brief Handle a confirmation response from the user.
     */
    bool Confirm(const std::string& user_name, const std::string& pwd) override;

    /**
     * @brief Handle a IsHttpAuthInfoSaved response from the user.
     */
    bool IsHttpAuthInfoSaved() override;

private:
    ArkWebRefPtr<ArkWebJsHttpAuthResult> ark_web_js_http_auth_result_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_HTTP_AUTH_RESULT_WRAPPER_H_

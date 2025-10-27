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

#ifndef ARK_WEB_JS_DIALOG_RESULT_WRAPPER_H_
#define ARK_WEB_JS_DIALOG_RESULT_WRAPPER_H_
#pragma once

#include "include/nweb_js_dialog_result.h"
#include "ohos_nweb/include/ark_web_js_dialog_result.h"

namespace OHOS::ArkWeb {

class ArkWebJsDialogResultWrapper : public OHOS::NWeb::NWebJSDialogResult {
public:
    ArkWebJsDialogResultWrapper(ArkWebRefPtr<ArkWebJsDialogResult> ark_web_js_dialog_result);
    ~ArkWebJsDialogResultWrapper() = default;

    /**
     * @brief Handle the result if the user cancelled the dialog.
     */
    void Cancel() override;

    /**
     * @brief Handle a confirmation response from the user.
     */
    void Confirm() override;

    /**
     * @brief Handle a confirmation response from the user with input message.
     *
     * @param message confirm message.
     */
    void Confirm(const std::string& message) override;

private:
    ArkWebRefPtr<ArkWebJsDialogResult> ark_web_js_dialog_result_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_DIALOG_RESULT_WRAPPER_H_

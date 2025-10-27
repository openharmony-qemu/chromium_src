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

#ifndef ARK_WEB_SELECT_POPUP_MENU_CALLBACK_WRAPPER_H_
#define ARK_WEB_SELECT_POPUP_MENU_CALLBACK_WRAPPER_H_
#pragma once

#include "include/nweb_select_popup_menu.h"
#include "ohos_nweb/include/ark_web_select_popup_menu_callback.h"

namespace OHOS::ArkWeb {

class ArkWebSelectPopupMenuCallbackWrapper : public OHOS::NWeb::NWebSelectPopupMenuCallback {
public:
    ArkWebSelectPopupMenuCallbackWrapper(
        ArkWebRefPtr<ArkWebSelectPopupMenuCallback> ark_web_select_popup_menu_callback);
    ~ArkWebSelectPopupMenuCallbackWrapper() = default;

    void Cancel() override;

    void Continue(const std::vector<int32_t>& indices) override;

private:
    ArkWebRefPtr<ArkWebSelectPopupMenuCallback> ark_web_select_popup_menu_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_SELECT_POPUP_MENU_CALLBACK_WRAPPER_H_

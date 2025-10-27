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

#ifndef ARK_WEB_MOUSE_EVENT_H_
#define ARK_WEB_MOUSE_EVENT_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebMouseEvent : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @Description: Get mouse x coordinate.
     * @Return: return x coordinate.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int32_t GetX() = 0;

    /**
     * @Description: Get mouse y coordinate.
     * @Return: return y coordinate.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int32_t GetY() = 0;

    /**
     * @Description: Get mouse button type.
     * @Return: return button type.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int32_t GetButton() = 0;

    /**
     * @Description: Get mouse action.
     * @Return: return action type.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int32_t GetAction() = 0;

    /**
     * @Description: Get mouse click num.
     * @Return: return click num.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int32_t GetClickNum() = 0;

    /**
     * @Description: Get current pressed keyCode.
     * @Return: return press key codes.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual ArkWebInt32Vector GetPressKeyCodes() = 0;

    /**
     * @Description: Get raw mouse x coordinate.
     * @Return: return raw x coordinate.
     */
    /*--ark web()--*/
    virtual int32_t GetRawX() = 0;

    /**
     * @Description: Get raw mouse y coordinate.
     * @Return: return raw y coordinate.
     */
    /*--ark web()--*/
    virtual int32_t GetRawY() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_MOUSE_EVENT_H_

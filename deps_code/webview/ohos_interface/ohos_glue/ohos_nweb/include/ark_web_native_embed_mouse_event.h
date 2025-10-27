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

#ifndef ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_H_
#define ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_H_
#pragma once

#include "ohos_nweb/include/ark_web_mouse_event_result.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebNativeEmbedMouseEvent : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual float GetX() = 0;

    /*--ark web()--*/
    virtual float GetY() = 0;

    /*--ark web()--*/
    virtual bool IsHitNativeArea() = 0;

    /*--ark web()--*/
    virtual size_t GetType() = 0;
    
    /*--ark web()--*/
    virtual size_t GetButton() = 0;

    /*--ark web()--*/
    virtual float GetOffsetX() = 0;

    /*--ark web()--*/
    virtual float GetOffsetY() = 0;

    /*--ark web()--*/
    virtual float GetScreenX() = 0;

    /*--ark web()--*/
    virtual float GetScreenY() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetEmbedId() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebMouseEventResult> GetResult() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_H_

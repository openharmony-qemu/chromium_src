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

#ifndef ARK_WEB_JS_RESULT_CALLBACK_H_
#define ARK_WEB_JS_RESULT_CALLBACK_H_
#pragma once

#include "ohos_nweb/include/ark_web_hap_value.h"
#include "ohos_nweb/include/ark_web_value.h"
#include "ohos_nweb/include/ark_web_value_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebJsResultCallback : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebValue GetJavaScriptResult(ArkWebValueVector args, const ArkWebString& method,
        const ArkWebString& object_name, int32_t routing_id, int32_t object_id) = 0;

    /**
     * @brief GetJavaScriptObjectMethods
     *
     * @param object_id: means the JavaScript object id
     * @param object_id: means the method name
     */
    /*--ark web()--*/
    virtual bool HasJavaScriptObjectMethods(int32_t object_id, const ArkWebString& method_name) = 0;

    /**
     * @brief GetJavaScriptObjectMethods
     *
     * @param object_id: means the JavaScript object id
     */
    /*--ark web()--*/
    virtual ArkWebValue GetJavaScriptObjectMethods(int32_t object_id) = 0;

    /**
     * @brief RemoveJavaScriptObjectHolder
     *
     * @param holder: means the JavaScript object is holded by
     * it(routing_id)
     * @param object_id: means the JavaScript object id
     */
    /*--ark web()--*/
    virtual void RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id) = 0;

    /**
     * @brief Remove Transient JavaScript Object
     */
    /*--ark web()--*/
    virtual void RemoveTransientJavaScriptObject() = 0;

    /*--ark web()--*/
    virtual ArkWebValue GetJavaScriptResultFlowbuf(ArkWebValueVector args, const ArkWebString& method,
        const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id) = 0;

    /*--ark web()--*/
    virtual void GetJavaScriptResultV2(const ArkWebHapValueVector& args, const ArkWebString& method,
        const ArkWebString& object_name, int32_t routing_id, int32_t object_id,
        ArkWebRefPtr<ArkWebHapValue> result) = 0;

    /*--ark web()--*/
    virtual void GetJavaScriptResultFlowbufV2(const ArkWebHapValueVector& args, const ArkWebString& method,
        const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id,
        ArkWebRefPtr<ArkWebHapValue> result) = 0;

    /*--ark web()--*/
    virtual void GetJavaScriptObjectMethodsV2(int32_t object_id, ArkWebRefPtr<ArkWebHapValue> result) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_RESULT_CALLBACK_H_

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

#ifndef ARK_WEB_JS_RESULT_CALLBACK_IMPL_H_
#define ARK_WEB_JS_RESULT_CALLBACK_IMPL_H_
#pragma once

#include "include/nweb_javascript_result_callback.h"
#include "ohos_nweb/include/ark_web_js_result_callback.h"

namespace OHOS::ArkWeb {

class ArkWebJsResultCallbackImpl : public ArkWebJsResultCallback {
    IMPLEMENT_REFCOUNTING(ArkWebJsResultCallbackImpl);

public:
    ArkWebJsResultCallbackImpl(std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> nweb_js_result_callback);
    ~ArkWebJsResultCallbackImpl() = default;

    ArkWebValue GetJavaScriptResult(ArkWebValueVector args, const ArkWebString& method, const ArkWebString& object_name,
        int32_t routing_id, int32_t object_id) override;

    ArkWebValue GetJavaScriptResultFlowbuf(ArkWebValueVector args, const ArkWebString& method,
        const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id) override;

    /**
     * @brief GetJavaScriptObjectMethods
     *
     * @param object_id: means the JavaScript object id
     * @param object_id: means the method name
     */
    bool HasJavaScriptObjectMethods(int32_t object_id, const ArkWebString& method_name) override;

    /**
     * @brief GetJavaScriptObjectMethods
     *
     * @param object_id: means the JavaScript object id
     */
    ArkWebValue GetJavaScriptObjectMethods(int32_t object_id) override;

    /**
     * @brief RemoveJavaScriptObjectHolder
     *
     * @param holder: means the JavaScript object is holded by
     * it(routing_id)
     * @param object_id: means the JavaScript object id
     */
    void RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id) override;

    /**
     * @brief Remove Transient JavaScript Object
     */
    void RemoveTransientJavaScriptObject() override;

    void GetJavaScriptResultV2(const ArkWebHapValueVector& args, const ArkWebString& method,
        const ArkWebString& object_name, int32_t routing_id, int32_t object_id,
        ArkWebRefPtr<ArkWebHapValue> result) override;

    void GetJavaScriptResultFlowbufV2(const ArkWebHapValueVector& args, const ArkWebString& method,
        const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id,
        ArkWebRefPtr<ArkWebHapValue> result) override;

    void GetJavaScriptObjectMethodsV2(int32_t object_id, ArkWebRefPtr<ArkWebHapValue> result) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> nweb_js_result_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_RESULT_CALLBACK_IMPL_H_

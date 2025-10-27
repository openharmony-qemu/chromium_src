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

#include "ohos_nweb/bridge/ark_web_js_result_callback_impl.h"

#include "ohos_nweb/bridge/ark_web_hap_value_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_hap_value_vector_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_value_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsResultCallbackImpl::ArkWebJsResultCallbackImpl(
    std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> nweb_js_result_callback)
    : nweb_js_result_callback_(nweb_js_result_callback)
{}

ArkWebValue ArkWebJsResultCallbackImpl::GetJavaScriptResult(ArkWebValueVector args, const ArkWebString& method,
    const ArkWebString& object_name, int32_t routing_id, int32_t object_id)
{
    ArkWebValue ark_web_value;
    ark_web_value.nweb_value = nweb_js_result_callback_->GetJavaScriptResult(ArkWebValueVectorStructToClass(args),
        ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), routing_id, object_id);
    return ark_web_value;
}

ArkWebValue ArkWebJsResultCallbackImpl::GetJavaScriptResultFlowbuf(ArkWebValueVector args, const ArkWebString& method,
    const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id)
{
    ArkWebValue ark_web_value;
    ark_web_value.nweb_value =
        nweb_js_result_callback_->GetJavaScriptResultFlowbuf(ArkWebValueVectorStructToClass(args),
            ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), fd, routing_id, object_id);
    return ark_web_value;
}

bool ArkWebJsResultCallbackImpl::HasJavaScriptObjectMethods(int32_t object_id, const ArkWebString& method_name)
{
    return nweb_js_result_callback_->HasJavaScriptObjectMethods(object_id, ArkWebStringStructToClass(method_name));
}

ArkWebValue ArkWebJsResultCallbackImpl::GetJavaScriptObjectMethods(int32_t object_id)
{
    ArkWebValue ark_web_value;
    ark_web_value.nweb_value = nweb_js_result_callback_->GetJavaScriptObjectMethods(object_id);
    return ark_web_value;
}

void ArkWebJsResultCallbackImpl::RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id)
{
    nweb_js_result_callback_->RemoveJavaScriptObjectHolder(holder, object_id);
}

void ArkWebJsResultCallbackImpl::RemoveTransientJavaScriptObject()
{
    nweb_js_result_callback_->RemoveTransientJavaScriptObject();
}

void ArkWebJsResultCallbackImpl::GetJavaScriptResultV2(const ArkWebHapValueVector& args, const ArkWebString& method,
    const ArkWebString& object_name, int32_t routing_id, int32_t object_id, ArkWebRefPtr<ArkWebHapValue> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        nweb_js_result_callback_->GetJavaScriptResultV2(ArkWebHapValueVectorStructToClass(args),
            ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), routing_id, object_id, nullptr);
    } else {
        nweb_js_result_callback_->GetJavaScriptResultV2(ArkWebHapValueVectorStructToClass(args),
            ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), routing_id, object_id,
            std::make_shared<ArkWebHapValueWrapper>(result));
    }
}

void ArkWebJsResultCallbackImpl::GetJavaScriptResultFlowbufV2(const ArkWebHapValueVector& args,
    const ArkWebString& method, const ArkWebString& object_name, int fd, int32_t routing_id, int32_t object_id,
    ArkWebRefPtr<ArkWebHapValue> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        nweb_js_result_callback_->GetJavaScriptResultFlowbufV2(ArkWebHapValueVectorStructToClass(args),
            ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), fd, routing_id, object_id,
            nullptr);
    } else {
        nweb_js_result_callback_->GetJavaScriptResultFlowbufV2(ArkWebHapValueVectorStructToClass(args),
            ArkWebStringStructToClass(method), ArkWebStringStructToClass(object_name), fd, routing_id, object_id,
            std::make_shared<ArkWebHapValueWrapper>(result));
    }
}

void ArkWebJsResultCallbackImpl::GetJavaScriptObjectMethodsV2(int32_t object_id, ArkWebRefPtr<ArkWebHapValue> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        nweb_js_result_callback_->GetJavaScriptObjectMethodsV2(object_id, nullptr);
    } else {
        nweb_js_result_callback_->GetJavaScriptObjectMethodsV2(
            object_id, std::make_shared<ArkWebHapValueWrapper>(result));
    }
}

} // namespace OHOS::ArkWeb

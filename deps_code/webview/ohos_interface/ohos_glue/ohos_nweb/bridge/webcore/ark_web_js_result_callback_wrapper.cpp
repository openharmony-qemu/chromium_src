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

#include "ohos_nweb/bridge/ark_web_hap_value_impl.h"
#include "ohos_nweb/bridge/ark_web_js_result_callback_wrapper.h"
#include "ohos_nweb/cpptoc/ark_web_hap_value_vector_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_value_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsResultCallbackWrapper::ArkWebJsResultCallbackWrapper(
    ArkWebRefPtr<ArkWebJsResultCallback> ark_web_js_result_callback)
    : ark_web_js_result_callback_(ark_web_js_result_callback)
{}

std::shared_ptr<OHOS::NWeb::NWebValue> ArkWebJsResultCallbackWrapper::GetJavaScriptResult(
    std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> args, const std::string& method, const std::string& object_name,
    int32_t routing_id, int32_t object_id)
{
    ArkWebString stMethod = ArkWebStringClassToStruct(method);
    ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);
    ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);

    ArkWebValue ark_web_value =
        ark_web_js_result_callback_->GetJavaScriptResult(stArgs, stMethod, stObjectName, routing_id, object_id);

    ArkWebStringStructRelease(stMethod);
    ArkWebValueVectorStructRelease(stArgs);
    ArkWebStringStructRelease(stObjectName);
    return ark_web_value.nweb_value;
}

std::shared_ptr<OHOS::NWeb::NWebValue> ArkWebJsResultCallbackWrapper::GetJavaScriptResultFlowbuf(
    std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> args, const std::string& method, const std::string& object_name,
    int fd, int32_t routing_id, int32_t object_id)
{
    ArkWebString stMethod = ArkWebStringClassToStruct(method);
    ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);
    ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);

    ArkWebValue ark_web_value = ark_web_js_result_callback_->GetJavaScriptResultFlowbuf(
        stArgs, stMethod, stObjectName, fd, routing_id, object_id);

    ArkWebStringStructRelease(stMethod);
    ArkWebValueVectorStructRelease(stArgs);
    ArkWebStringStructRelease(stObjectName);
    return ark_web_value.nweb_value;
}

bool ArkWebJsResultCallbackWrapper::HasJavaScriptObjectMethods(int32_t object_id, const std::string& method_name)
{
    ArkWebString stMethodName = ArkWebStringClassToStruct(method_name);

    bool flag = ark_web_js_result_callback_->HasJavaScriptObjectMethods(object_id, stMethodName);

    ArkWebStringStructRelease(stMethodName);
    return flag;
}

std::shared_ptr<OHOS::NWeb::NWebValue> ArkWebJsResultCallbackWrapper::GetJavaScriptObjectMethods(int32_t object_id)
{
    ArkWebValue ark_web_value = ark_web_js_result_callback_->GetJavaScriptObjectMethods(object_id);
    return ark_web_value.nweb_value;
}

void ArkWebJsResultCallbackWrapper::RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id)
{
    ark_web_js_result_callback_->RemoveJavaScriptObjectHolder(holder, object_id);
}

void ArkWebJsResultCallbackWrapper::RemoveTransientJavaScriptObject()
{
    ark_web_js_result_callback_->RemoveTransientJavaScriptObject();
}

void ArkWebJsResultCallbackWrapper::GetJavaScriptResultV2(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>>& args, const std::string& method,
    const std::string& object_name, int32_t routing_id, int32_t object_id,
    std::shared_ptr<OHOS::NWeb::NWebHapValue> result)
{
    ArkWebString stMethod = ArkWebStringClassToStruct(method);
    ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);
    ArkWebHapValueVector stArgs = ArkWebHapValueVectorClassToStruct(args);
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        ark_web_js_result_callback_->GetJavaScriptResultV2(
            stArgs, stMethod, stObjectName, routing_id, object_id, nullptr);
    } else {
        ark_web_js_result_callback_->GetJavaScriptResultV2(
            stArgs, stMethod, stObjectName, routing_id, object_id, new ArkWebHapValueImpl(result));
    }
    ArkWebStringStructRelease(stMethod);
    ArkWebStringStructRelease(stObjectName);
    ArkWebHapValueVectorStructRelease(stArgs);
}

void ArkWebJsResultCallbackWrapper::GetJavaScriptResultFlowbufV2(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>>& args, const std::string& method,
    const std::string& object_name, int fd, int32_t routing_id, int32_t object_id,
    std::shared_ptr<OHOS::NWeb::NWebHapValue> result)
{
    ArkWebString stMethod = ArkWebStringClassToStruct(method);
    ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);
    ArkWebHapValueVector stArgs = ArkWebHapValueVectorClassToStruct(args);
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        ark_web_js_result_callback_->GetJavaScriptResultFlowbufV2(
            stArgs, stMethod, stObjectName, fd, routing_id, object_id, nullptr);
    } else {
        ark_web_js_result_callback_->GetJavaScriptResultFlowbufV2(
            stArgs, stMethod, stObjectName, fd, routing_id, object_id, new ArkWebHapValueImpl(result));
    }
    ArkWebStringStructRelease(stMethod);
    ArkWebStringStructRelease(stObjectName);
    ArkWebHapValueVectorStructRelease(stArgs);
}

void ArkWebJsResultCallbackWrapper::GetJavaScriptObjectMethodsV2(
    int32_t object_id, std::shared_ptr<OHOS::NWeb::NWebHapValue> result)
{
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        ark_web_js_result_callback_->GetJavaScriptObjectMethodsV2(object_id, nullptr);
    } else {
        ark_web_js_result_callback_->GetJavaScriptObjectMethodsV2(object_id, new ArkWebHapValueImpl(result));
    }
}

} // namespace OHOS::ArkWeb

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

#include "ohos_nweb/bridge/ark_web_js_result_callback_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_nweb/cpptoc/ark_web_value_vector_cpptoc.h"

namespace OHOS::ArkWeb {

ArkWebJsResultCallbackWrapper::ArkWebJsResultCallbackWrapper(
    ArkWebRefPtr<ArkWebJsResultCallback> ark_web_js_result_callback)
    : ark_web_js_result_callback_(ark_web_js_result_callback) {
}

std::shared_ptr<OHOS::NWeb::NWebValue>
ArkWebJsResultCallbackWrapper::GetJavaScriptResult(
    std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> args,
    const std::string &method, const std::string &object_name,
    int32_t routing_id, int32_t object_id) {
  ArkWebString stMethod = ArkWebStringClassToStruct(method);
  ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);
  ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);

  ArkWebValue ark_web_value = ark_web_js_result_callback_->GetJavaScriptResult(
      stArgs, stMethod, stObjectName, routing_id, object_id);

  ArkWebStringStructRelease(stMethod);
  ArkWebValueVectorStructRelease(stArgs);
  ArkWebStringStructRelease(stObjectName);
  return ark_web_value.nweb_value;
}

std::shared_ptr<OHOS::NWeb::NWebValue>
ArkWebJsResultCallbackWrapper::GetJavaScriptResultFlowbuf(
    std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> args,
    const std::string &method, const std::string &object_name, int fd,
    int32_t routing_id, int32_t object_id) {
  ArkWebString stMethod = ArkWebStringClassToStruct(method);
  ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);
  ArkWebString stObjectName = ArkWebStringClassToStruct(object_name);

  ArkWebValue ark_web_value =
      ark_web_js_result_callback_->GetJavaScriptResultFlowbuf(
          stArgs, stMethod, stObjectName, fd, routing_id, object_id);

  ArkWebStringStructRelease(stMethod);
  ArkWebValueVectorStructRelease(stArgs);
  ArkWebStringStructRelease(stObjectName);
  return ark_web_value.nweb_value;
}

bool ArkWebJsResultCallbackWrapper::HasJavaScriptObjectMethods(
    int32_t object_id, const std::string &method_name) {
  ArkWebString stMethodName = ArkWebStringClassToStruct(method_name);

  bool flag = ark_web_js_result_callback_->HasJavaScriptObjectMethods(
      object_id, stMethodName);

  ArkWebStringStructRelease(stMethodName);
  return flag;
}

std::shared_ptr<OHOS::NWeb::NWebValue>
ArkWebJsResultCallbackWrapper::GetJavaScriptObjectMethods(int32_t object_id) {
  ArkWebValue ark_web_value =
      ark_web_js_result_callback_->GetJavaScriptObjectMethods(object_id);
  return ark_web_value.nweb_value;
}

void ArkWebJsResultCallbackWrapper::RemoveJavaScriptObjectHolder(
    int32_t holder, int32_t object_id) {
  ark_web_js_result_callback_->RemoveJavaScriptObjectHolder(holder, object_id);
}

void ArkWebJsResultCallbackWrapper::RemoveTransientJavaScriptObject() {
  ark_web_js_result_callback_->RemoveTransientJavaScriptObject();
}

} // namespace OHOS::ArkWeb

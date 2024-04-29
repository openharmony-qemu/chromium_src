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

#include "ohos_nweb/ctocpp/ark_web_js_result_callback_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebValue ArkWebJsResultCallbackCToCpp::GetJavaScriptResult(
    ArkWebValueVector args, const ArkWebString &method,
    const ArkWebString &object_name, int32_t routing_id, int32_t object_id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_value_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_java_script_result,
                                   ark_web_value_default);

  // Execute
  return _struct->get_java_script_result(_struct, args, &method, &object_name,
                                         routing_id, object_id);
}

ARK_WEB_NO_SANITIZE
bool ArkWebJsResultCallbackCToCpp::HasJavaScriptObjectMethods(
    int32_t object_id, const ArkWebString &method_name) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, has_java_script_object_methods,
                                   false);

  // Execute
  return _struct->has_java_script_object_methods(_struct, object_id,
                                                 &method_name);
}

ARK_WEB_NO_SANITIZE
ArkWebValue
ArkWebJsResultCallbackCToCpp::GetJavaScriptObjectMethods(int32_t object_id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_value_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_java_script_object_methods,
                                   ark_web_value_default);

  // Execute
  return _struct->get_java_script_object_methods(_struct, object_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebJsResultCallbackCToCpp::RemoveJavaScriptObjectHolder(
    int32_t holder, int32_t object_id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, remove_java_script_object_holder, );

  // Execute
  _struct->remove_java_script_object_holder(_struct, holder, object_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebJsResultCallbackCToCpp::RemoveTransientJavaScriptObject() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   remove_transient_java_script_object, );

  // Execute
  _struct->remove_transient_java_script_object(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebValue ArkWebJsResultCallbackCToCpp::GetJavaScriptResultFlowbuf(
    ArkWebValueVector args, const ArkWebString &method,
    const ArkWebString &object_name, int fd, int32_t routing_id,
    int32_t object_id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_js_result_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_value_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_java_script_result_flowbuf,
                                   ark_web_value_default);

  // Execute
  return _struct->get_java_script_result_flowbuf(
      _struct, args, &method, &object_name, fd, routing_id, object_id);
}

ArkWebJsResultCallbackCToCpp::ArkWebJsResultCallbackCToCpp() {
}

ArkWebJsResultCallbackCToCpp::~ArkWebJsResultCallbackCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkWebJsResultCallbackCToCpp, ArkWebJsResultCallback,
                           ark_web_js_result_callback_t>::kBridgeType =
        ARK_WEB_JS_RESULT_CALLBACK;

} // namespace OHOS::ArkWeb

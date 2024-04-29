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

#ifndef ARK_WEB_JS_RESULT_CALLBACK_CTOCPP_H_
#define ARK_WEB_JS_RESULT_CALLBACK_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_nweb/capi/ark_web_js_result_callback_capi.h"
#include "ohos_nweb/include/ark_web_js_result_callback.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkWebJsResultCallbackCToCpp
    : public ArkWebCToCppRefCounted<ArkWebJsResultCallbackCToCpp,
                                    ArkWebJsResultCallback,
                                    ark_web_js_result_callback_t> {
public:
  ArkWebJsResultCallbackCToCpp();
  virtual ~ArkWebJsResultCallbackCToCpp();

  // ArkWebJsResultCallback methods.
  ArkWebValue GetJavaScriptResult(ArkWebValueVector args,
                                  const ArkWebString &method,
                                  const ArkWebString &object_name,
                                  int32_t routing_id,
                                  int32_t object_id) override;

  bool HasJavaScriptObjectMethods(int32_t object_id,
                                  const ArkWebString &method_name) override;

  ArkWebValue GetJavaScriptObjectMethods(int32_t object_id) override;

  void RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id) override;

  void RemoveTransientJavaScriptObject() override;

  ArkWebValue GetJavaScriptResultFlowbuf(ArkWebValueVector args,
                                         const ArkWebString &method,
                                         const ArkWebString &object_name,
                                         int fd, int32_t routing_id,
                                         int32_t object_id) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_RESULT_CALLBACK_CTOCPP_H_

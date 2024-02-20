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

#ifndef ARK_WEB_STRING_VECTOR_VALUE_CALLBACK_CPPTOC_H_
#define ARK_WEB_STRING_VECTOR_VALUE_CALLBACK_CPPTOC_H_
#pragma once

#include "base/cpptoc/ark_web_cpptoc_ref_counted.h"
#include "ohos_nweb/capi/ark_web_string_vector_value_callback_capi.h"
#include "ohos_nweb/include/ark_web_string_vector_value_callback.h"

namespace OHOS::ArkWeb {

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class ArkWebStringVectorValueCallbackCppToC
    : public ArkWebCppToCRefCounted<ArkWebStringVectorValueCallbackCppToC,
                                    ArkWebStringVectorValueCallback,
                                    ark_web_string_vector_value_callback_t> {
public:
  ArkWebStringVectorValueCallbackCppToC();
  virtual ~ArkWebStringVectorValueCallbackCppToC();
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_STRING_VECTOR_VALUE_CALLBACK_CPPTOC_H_

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

#ifndef ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_CPPTOC_H_
#define ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_CPPTOC_H_
#pragma once

#include "base/cpptoc/ark_web_cpptoc_ref_counted.h"
#include "ohos_nweb/capi/ark_web_screen_capture_access_request_capi.h"
#include "ohos_nweb/include/ark_web_screen_capture_access_request.h"

namespace OHOS::ArkWeb {

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class ArkWebScreenCaptureAccessRequestCppToC
    : public ArkWebCppToCRefCounted<ArkWebScreenCaptureAccessRequestCppToC,
                                    ArkWebScreenCaptureAccessRequest,
                                    ark_web_screen_capture_access_request_t> {
public:
  ArkWebScreenCaptureAccessRequestCppToC();
  virtual ~ArkWebScreenCaptureAccessRequestCppToC();
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_CPPTOC_H_

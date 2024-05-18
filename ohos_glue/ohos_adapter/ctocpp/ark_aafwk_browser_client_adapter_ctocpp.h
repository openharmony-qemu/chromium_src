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

#ifndef ARK_AAFWK_BROWSER_CLIENT_ADAPTER_CTOCPP_H_
#define ARK_AAFWK_BROWSER_CLIENT_ADAPTER_CTOCPP_H_
#pragma once

#include "ohos_adapter/capi/ark_aafwk_browser_client_adapter_capi.h"
#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/include/ark_aafwk_browser_client_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkAafwkBrowserClientAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkAafwkBrowserClientAdapterCToCpp,
                                    ArkAafwkBrowserClientAdapter,
                                    ark_aafwk_browser_client_adapter_t> {
public:
  ArkAafwkBrowserClientAdapterCToCpp();
  virtual ~ArkAafwkBrowserClientAdapterCToCpp();

  void* QueryRenderSurface(int32_t surface_id) override;

  void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role) override;

  void PassSurface(int64_t surface_id) override;

  void DestroyRenderSurface(int32_t surface_id) override;
};

} // namespace OHOS::ArkWeb
#endif // ARK_AAFWK_BROWSER_CLIENT_ADAPTER_CTOCPP_H_
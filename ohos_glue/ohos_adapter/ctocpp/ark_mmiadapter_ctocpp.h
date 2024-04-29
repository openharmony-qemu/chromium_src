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

#ifndef ARK_MMIADAPTER_CTOCPP_H_
#define ARK_MMIADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_mmi_adapter_capi.h"
#include "ohos_adapter/include/ark_mmi_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkMMIAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkMMIAdapterCToCpp, ArkMMIAdapter,
                                    ark_mmiadapter_t> {
public:
  ArkMMIAdapterCToCpp();
  virtual ~ArkMMIAdapterCToCpp();

  // ArkMMIAdapter methods.
  char *KeyCodeToString(int32_t keyCode) override;

  int32_t RegisterMMIInputListener(
      ArkWebRefPtr<ArkMMIInputListenerAdapter> eventCallback) override;

  void UnregisterMMIInputListener(int32_t monitorId) override;

  int32_t
  RegisterDevListener(ArkWebString type,
                      ArkWebRefPtr<ArkMMIListenerAdapter> listener) override;

  int32_t UnregisterDevListener(ArkWebString type) override;

  int32_t GetKeyboardType(int32_t deviceId, int32_t &type) override;

  int32_t GetDeviceIds(ArkWebInt32Vector &ids) override;

  int32_t GetDeviceInfo(int32_t deviceId,
                        ArkWebRefPtr<ArkMMIDeviceInfoAdapter> info) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MMIADAPTER_CTOCPP_H_

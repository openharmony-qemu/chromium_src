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

#ifndef ARK_OHOS_RESOURCE_ADAPTER_CTOCPP_H_
#define ARK_OHOS_RESOURCE_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_ohos_resource_adapter_capi.h"
#include "ohos_adapter/include/ark_ohos_resource_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkOhosResourceAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkOhosResourceAdapterCToCpp,
                                    ArkOhosResourceAdapter,
                                    ark_ohos_resource_adapter_t> {
public:
  ArkOhosResourceAdapterCToCpp();
  virtual ~ArkOhosResourceAdapterCToCpp();

  // ArkOhosResourceAdapter methods.
  bool GetRawFileData(const ArkWebString &rawFile, size_t &len, uint8_t **dest,
                      bool isSys) override;

  ArkWebRefPtr<ArkOhosFileMapper> GetRawFileMapper(const ArkWebString &rawFile,
                                                   bool isSys) override;

  bool IsRawFileExist(const ArkWebString &rawFile, bool isSys) override;

  bool GetRawFileLastModTime(const ArkWebString &rawFile, uint16_t &date,
                             uint16_t &time, bool isSys) override;

  bool GetRawFileLastModTime(const ArkWebString &rawFile, time_t &time,
                             bool isSys) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_RESOURCE_ADAPTER_CTOCPP_H_

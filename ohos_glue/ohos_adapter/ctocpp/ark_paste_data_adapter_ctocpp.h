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

#ifndef ARK_PASTE_DATA_ADAPTER_CTOCPP_H_
#define ARK_PASTE_DATA_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_pasteboard_client_adapter_capi.h"
#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkPasteDataAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkPasteDataAdapterCToCpp,
                                    ArkPasteDataAdapter,
                                    ark_paste_data_adapter_t> {
public:
  ArkPasteDataAdapterCToCpp();
  virtual ~ArkPasteDataAdapterCToCpp();

  // ArkPasteDataAdapter methods.
  void AddHtmlRecord(const ArkWebString &html) override;

  void AddTextRecord(const ArkWebString &text) override;

  ArkWebStringVector GetMimeTypes() override;

  void GetPrimaryHtml(void *data) override;

  void GetPrimaryText(void *data) override;

  void GetPrimaryMimeType(void *data) override;

  ArkWebRefPtr<ArkPasteDataRecordAdapter> GetRecordAt(size_t index) override;

  size_t GetRecordCount() override;

  ArkPasteRecordVector AllRecords() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_DATA_ADAPTER_CTOCPP_H_

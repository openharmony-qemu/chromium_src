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

#ifndef ARK_PASTE_DATA_RECORD_ADAPTER_CTOCPP_H_
#define ARK_PASTE_DATA_RECORD_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_pasteboard_client_adapter_capi.h"
#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkPasteDataRecordAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkPasteDataRecordAdapterCToCpp,
                                    ArkPasteDataRecordAdapter,
                                    ark_paste_data_record_adapter_t> {
public:
  ArkPasteDataRecordAdapterCToCpp();
  virtual ~ArkPasteDataRecordAdapterCToCpp();

  // ArkPasteDataRecordAdapter methods.
  bool SetHtmlText(void *htmlText) override;

  bool SetPlainText(void *plainText) override;

  bool
  SetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) override;

  ArkWebString GetMimeType() override;

  void GetHtmlText(void *data) override;

  void GetPlainText(void *data) override;

  bool
  GetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) override;

  bool SetUri(const ArkWebString &uriString) override;

  bool SetCustomData(void *data) override;

  void GetUri(void *data) override;

  void GetCustomData(void *data) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_DATA_RECORD_ADAPTER_CTOCPP_H_

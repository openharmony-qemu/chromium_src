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

#ifndef ARK_PASTE_BOARD_CLIENT_ADAPTER_CTOCPP_H_
#define ARK_PASTE_BOARD_CLIENT_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_pasteboard_client_adapter_capi.h"
#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkPasteBoardClientAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkPasteBoardClientAdapterCToCpp,
                                    ArkPasteBoardClientAdapter,
                                    ark_paste_board_client_adapter_t> {
public:
  ArkPasteBoardClientAdapterCToCpp();
  virtual ~ArkPasteBoardClientAdapterCToCpp();

  // ArkPasteBoardClientAdapter methods.
  bool GetPasteData(ArkPasteRecordVector &data) override;

  void SetPasteData(ArkPasteRecordVector &data, int32_t copyOption) override;

  bool HasPasteData() override;

  void Clear() override;

  int32_t OpenRemoteUri(const ArkWebString &path) override;

  bool IsLocalPaste() override;

  uint32_t GetTokenId() override;

  int32_t AddPasteboardChangedObserver(
      ArkWebRefPtr<ArkPasteBoardObserverAdapter> callback) override;

  void RemovePasteboardChangedObserver(int32_t callbackId) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_BOARD_CLIENT_ADAPTER_CTOCPP_H_

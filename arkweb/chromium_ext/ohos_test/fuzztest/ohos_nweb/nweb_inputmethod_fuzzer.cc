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

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"
#include "ohos_nweb/src/nweb_inputmethod_handler.h"

using namespace OHOS::NWeb;

namespace OHOS {
void NWebInputMethodHandlerFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return false;
  }
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_EMAIL;
  int32_t enterKeyType = -1;
  NWebInputMethodHandler::Attach(nullptr, inputInfo, is_need_reset_listener,
                                 enterKeyType);
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;
  NWebInputMethodHandler::Reattach(nwebId, type);
  NWebInputMethodHandler::OnSelectionChanged(nullptr, CefString(""),
                                             CefRange(0, 1));
}
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  OHOS::NWebInputMethodHandlerFuzzTest(data, size);
  return 0;
}
 
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "base/logging.h"
#include "cef/include/cef_browser.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "nweb_devtools_message_handler_impl.h"
#include "cef_devtools_message_handler_delegate.h"
#include "nweb_devtools_message_handler.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
bool LLVMFuzzerTestSetup() {
  return true;
}

void FuzzApi(const uint8_t* data, size_t size) {
  std::unique_ptr<NWebDevtoolsMessageHandler> handler = std::make_unique<NWebDevtoolsMessageHandler>();
  NWebDevToolsMessageHandlerImpl handleImp(std::move(handler));
  FuzzedDataProvider fdp(data, size);

  std::string cef_message_std = fdp.ConsumeRandomLengthString();
  CefString cef_message = CefString(cef_message_std);
  std::string cef_path_std = fdp.ConsumeRandomLengthString();
  CefString cef_path = CefString(cef_path_std);

  handleImp.ShowInfoBar(cef_message, cef_path, nullptr);
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::FuzzApi(data, size);
  }
  return 0;
}

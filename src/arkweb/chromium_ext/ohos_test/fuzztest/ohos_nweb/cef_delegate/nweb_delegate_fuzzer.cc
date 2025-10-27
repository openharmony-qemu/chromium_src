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

#include "ohos_nweb/src/cef_delegate/nweb_delegate.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
void NwebDelegateFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return false;
  }
  int argc;
  const char* argv[5];
  std::shared_ptr<NWebDelegate> nweb_delegate_ = std::make_shared<NWebDelegate>(argc, argv);
  uint32_t width = 100;
  uint32_t height = 100;
  bool isKeyboard = false;
  nweb_delegate_->Resize(width, height, isKeyboard);
  const FocusReason focusReason = FocusReason::FOCUS_DEFAULT;
  nweb_delegate_->OnFocus(focusReason);
  nweb_delegate_->OnBlur();
  int32_t id = 1;
  double x = 100.0;
  double y = 100.0;
  bool from_overlay = false;
  nweb_delegate_->OnTouchPress(id, x, y, from_overlay);
}
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  OHOS::NwebDelegateFuzzTest(data, size);
  return 0;
}
 
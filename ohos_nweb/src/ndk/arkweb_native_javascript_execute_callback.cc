/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "arkweb_native_javascript_execute_callback.h"

#include <vector>

#include "base/logging.h"

namespace OHOS::NWeb {
void ArkWebNativeJavaScriptExecuteCallback::OnReceiveValue(
    std::shared_ptr<NWebMessage> result) {
  if (!result) {
    LOG(ERROR) << "NativeArkWeb RunJavaScript callback result is null";
    return;
  }
  if (!callback_) {
    LOG(ERROR) << "NativeArkWeb RunJavaScript callback is null";
    return;
  }

  if (result->IsString()) {
    auto resultString = result->GetString();
    std::vector<uint8_t> data(resultString.begin(), resultString.end());
    callback_(data);
  } else if (result->IsBinary()) {
    auto resultBin = result->GetBinary();
    callback_(resultBin);
  } else {
    LOG(ERROR) << "NativeArkWeb RunJavaScript result is not string";
    callback_(std::vector<uint8_t>());
  }
}
}  // namespace OHOS::NWeb
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

#include "arkweb_native_web_message_callback.h"

#include <vector>

#include "base/logging.h"

namespace OHOS::NWeb {
void ArkWebNativeWebMessageCallback::OnReceiveValue(
    std::shared_ptr<NWebMessage> result) {
  if (!result) {
    LOG(ERROR) << "NativeArkWeb Web Message callback result is null";
    return;
  }
  if (!callback_) {
    LOG(ERROR) << "NativeArkWeb Web Message callback is null";
    return;
  }

  if (result->IsString()) {
    LOG(DEBUG) << "NativeArkWeb Web Message, OnReceiveValue string type";
    auto resultString = result->GetString();
    ArkWeb_WebMessage message;
    message.webMessageType = ArkWeb_WebMessageType::ARKWEB_STRING;
    message.data = (void*)(resultString.c_str());
    message.dataLength = resultString.length();
    callback_(&message);
  } else if (result->IsBinary()) {
    LOG(DEBUG) << "NativeArkWeb Web Message, OnReceiveValue buffer type";
    auto resultBin = result->GetBinary();
    ArkWeb_WebMessage message;
    message.webMessageType = ArkWeb_WebMessageType::ARKWEB_BUFFER;
    message.data = (void*)(&resultBin);
    message.dataLength = resultBin.size();
    callback_(&message);
  } else {
    LOG(ERROR) << "NativeArkWeb Web Message, type not support";
  }
}
}  // namespace OHOS::NWeb
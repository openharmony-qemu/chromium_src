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
#ifndef ARKWEB_NATIVE_WEB_MESSAGE_CALLBACK_H
#define ARKWEB_NATIVE_WEB_MESSAGE_CALLBACK_H

#include <functional>
#include <memory>

#include "nweb_value_callback.h"
#include "nweb_web_message.h"
#include "ohos_nweb/src/capi/arkweb_model.h"
#include "ohos_nweb/src/capi/arkweb_type.h"

namespace OHOS::NWeb {
class ArkWebNativeWebMessageCallback : public NWebMessageValueCallback {
 public:
  using WebMessageFunc =
      std::function<void(const ArkWeb_WebMessagePtr message)>;
  ArkWebNativeWebMessageCallback() = default;
  explicit ArkWebNativeWebMessageCallback(WebMessageFunc&& callback)
      : callback_(callback) {}
  ~ArkWebNativeWebMessageCallback() override = default;
  void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;

 private:
  WebMessageFunc callback_ = nullptr;
};

}  // namespace OHOS::NWeb
#endif  // ARKWEB_NATIVE_WEB_MESSAGE_CALLBACK_H

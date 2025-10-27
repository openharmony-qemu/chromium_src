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
#ifndef NWEB_WEBVIEW_NATIVE_JAVA_SCRIPT_EXECUTE_CALLBACK_H
#define NWEB_WEBVIEW_NATIVE_JAVA_SCRIPT_EXECUTE_CALLBACK_H

#include <functional>
#include <memory>

#include "nweb_value_callback.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
class NativeJavaScriptExecuteCallback : public OHOS::NWeb::NWebMessageValueCallback {
public:
    explicit NativeJavaScriptExecuteCallback(std::function<void(const char*)> callback) : callbackNative_(callback) {}
    ~NativeJavaScriptExecuteCallback() = default;
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<NWebHapValue> value) override;

private:
    std::function<void(const char*)> callbackNative_ = nullptr;
};

} // namespace OHOS::NWeb
#endif // NWEB_WEBVIEW_NATIVE_JAVA_SCRIPT_EXECUTE_CALLBACK_H
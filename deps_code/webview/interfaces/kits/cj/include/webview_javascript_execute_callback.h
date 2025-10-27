/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WEBVIEW_JAVA_SCRIPT_EXECUTE_CALLBACK_H
#define WEBVIEW_JAVA_SCRIPT_EXECUTE_CALLBACK_H

#include <string>

#include "webview_ffi.h"
#include "webview_controller_impl.h"
#include "nweb_value_callback.h"
#include "nweb_web_message.h"
#include "nweb_message_ext.h"

namespace OHOS::Webview {

enum class JsMessageType : int {
    NOTSUPPORT = 0,
    STRING,
    NUMBER,
    BOOLEAN,
    ARRAYBUFFER,
    ARRAY
};

class WebviewJavaScriptExecuteCallback :
    public OHOS::NWeb::NWebMessageValueCallback {
public:
    explicit WebviewJavaScriptExecuteCallback(std::function<void(RetDataCString)> callbackRef)
        : callbackRef_(callbackRef)
    {}
    ~WebviewJavaScriptExecuteCallback() = default;
    void OnReceiveValue(std::shared_ptr<OHOS::NWeb::NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<OHOS::NWeb::NWebHapValue> value) override;

private:
    std::function<void(RetDataCString)> callbackRef_ = nullptr;
};

class WebJsMessageExtImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebJsMessageExtImpl, OHOS::FFI::FFIData)
public:
    explicit WebJsMessageExtImpl(std::shared_ptr<NWeb::NWebMessage> value) : value_(value) {}
    explicit WebJsMessageExtImpl(std::shared_ptr<NWeb::NWebHapValue> value) 
    {
        value_ = NWeb::ConvertNwebHap2NwebMessage(value);
    }
    ~WebJsMessageExtImpl() = default;

    int32_t ConvertToJsType(NWeb::NWebValue::Type type);
    int32_t GetType();
    std::string GetString();
    double GetNumber();
    bool GetBoolean();
    std::shared_ptr<NWeb::NWebMessage> GetJsMsgResult()
    {
        return value_;
    }

private:
    std::shared_ptr<NWeb::NWebMessage> value_ = nullptr;
};

class WebviewJavaScriptExtExecuteCallback :
    public OHOS::NWeb::NWebMessageValueCallback {
public:
    explicit WebviewJavaScriptExtExecuteCallback(std::function<void(RetDataI64)> callbackRef)
        : callbackRef_(callbackRef)
    {}
    ~WebviewJavaScriptExtExecuteCallback() = default;
    void OnReceiveValue(std::shared_ptr<OHOS::NWeb::NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<OHOS::NWeb::NWebHapValue> value) override;

private:
    std::function<void(RetDataI64)> callbackRef_ = nullptr;
};

} // namespace OHOS::Webview
#endif

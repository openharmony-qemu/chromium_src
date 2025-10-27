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

#include "webview_javascript_execute_callback.h"
#include "webview_log.h"
#include "nweb_message_ext.h"

using namespace OHOS::NWeb;

namespace OHOS::Webview {

int32_t WebJsMessageExtImpl::ConvertToJsType(NWebValue::Type type)
{
    JsMessageType jsMessageType = JsMessageType::NOTSUPPORT;
    switch (type) {
        case NWebValue::Type::STRING:
            jsMessageType = JsMessageType::STRING;
            break;
        case NWebValue::Type::INTEGER:
        case NWebValue::Type::DOUBLE:
            jsMessageType = JsMessageType::NUMBER;
            break;
        case NWebValue::Type::BOOLEAN:
            jsMessageType = JsMessageType::BOOLEAN;
            break;
        case NWebValue::Type::BINARY:
            jsMessageType = JsMessageType::ARRAYBUFFER;
            break;
        case NWebValue::Type::STRINGARRAY:
        case NWebValue::Type::BOOLEANARRAY:
        case NWebValue::Type::DOUBLEARRAY:
        case NWebValue::Type::INT64ARRAY:
            jsMessageType = JsMessageType::ARRAY;
            break;
        default:
            jsMessageType = JsMessageType::NOTSUPPORT;
            break;
    }
    return static_cast<int32_t>(jsMessageType);
}

int32_t WebJsMessageExtImpl::GetType()
{
    if (value_) {
        return ConvertToJsType(value_->GetType());
    }
    return static_cast<int32_t>(JsMessageType::NOTSUPPORT);
}

std::string WebJsMessageExtImpl::GetString()
{
    if (value_) {
        return value_->GetString();
    }
    return "";
}

double WebJsMessageExtImpl::GetNumber()
{
    if (value_) {
        return value_->GetDouble();
    }
    return 0;
}

bool WebJsMessageExtImpl::GetBoolean()
{
    if (value_) {
        return value_->GetBoolean();
    }
    return false;
}

void WebviewJavaScriptExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WEBVIEWLOGI("WebviewJavaScriptExecuteCallback::OnReceiveValue start");
    RetDataCString ret = { .code = NWebError::INVALID_RESOURCE, .data = nullptr };
    if (result == nullptr) {
        callbackRef_(ret);
        return;
    }
    if (result->GetType() == NWebValue::Type::STRING && result->GetString().empty()) {
        callbackRef_(ret);
        return;
    }
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(result->GetString());
    if (ret.data == nullptr) {
        ret.code = NWebError::NEW_OOM;
    }
    callbackRef_(ret);
}

void WebviewJavaScriptExecuteCallback::OnReceiveValueV2(std::shared_ptr<NWebHapValue> value)
{
    WEBVIEWLOGI("WebviewJavaScriptExecuteCallback::OnReceiveValue2 start");
    RetDataCString ret = { .code = NWebError::INVALID_RESOURCE, .data = nullptr };
    if (value == nullptr) {
        callbackRef_(ret);
        return;
    }
    if (value->GetType() == NWebHapValue::Type::STRING && value->GetString().empty()) {
        callbackRef_(ret);
        return;
    }
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(value->GetString());
    if (ret.data == nullptr) {
        ret.code = NWebError::NEW_OOM;
    }
    callbackRef_(ret);
}

void WebviewJavaScriptExtExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WEBVIEWLOGI("WebviewJavaScriptExtExecuteCallback::OnReceiveValue start");
    RetDataI64 ret = { .code = NWebError::INVALID_RESOURCE, .data = 0 };
    if (result == nullptr) {
        callbackRef_(ret);
        return;
    }
    WebJsMessageExtImpl *webJsMessageExtImpl = OHOS::FFI::FFIData::Create<WebJsMessageExtImpl>(result);
    if (webJsMessageExtImpl == nullptr) {
        WEBVIEWLOGE("new WebJsMessageExtImpl failed.");
        callbackRef_(ret);
        return;
    }
    ret.code = NWebError::NO_ERROR;
    ret.data = webJsMessageExtImpl->GetID();
    callbackRef_(ret);
}

void WebviewJavaScriptExtExecuteCallback::OnReceiveValueV2(std::shared_ptr<NWebHapValue> value)
{
    WEBVIEWLOGI("WebviewJavaScriptExtExecuteCallback::OnReceiveValue start");
    RetDataI64 ret = { .code = NWebError::INVALID_RESOURCE, .data = 0 };
    if (value == nullptr) {
        callbackRef_(ret);
        return;
    }
    WebJsMessageExtImpl *webJsMessageExtImpl = OHOS::FFI::FFIData::Create<WebJsMessageExtImpl>(value);
    if (webJsMessageExtImpl == nullptr) {
        WEBVIEWLOGE("new WebJsMessageExtImpl failed.");
        callbackRef_(ret);
        return;
    }
    ret.code = NWebError::NO_ERROR;
    ret.data = webJsMessageExtImpl->GetID();
    callbackRef_(ret);
}

} // namespace OHOS::Webview

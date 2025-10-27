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

#include "webview_value.h"

#include "nweb_log.h"

namespace OHOS::NWeb {

WebViewValue::WebViewValue(NWebRomValue::Type type)
{
    type_ = type;
}

NWebRomValue::Type WebViewValue::GetType()
{
    return type_;
}

void WebViewValue::SetType(NWebRomValue::Type type)
{
    CheckType(NWebRomValue::Type::NONE);
    type_ = type;
}

int WebViewValue::GetInt()
{
    CheckType(NWebRomValue::Type::INTEGER);
    return data_.n;
}

void WebViewValue::SetInt(int value)
{
    CheckType(NWebRomValue::Type::INTEGER);
    data_.n = value;
}

bool WebViewValue::GetBool()
{
    CheckType(NWebRomValue::Type::BOOLEAN);
    return data_.b;
}

void WebViewValue::SetBool(bool value)
{
    CheckType(NWebRomValue::Type::BOOLEAN);
    data_.b = value;
}

double WebViewValue::GetDouble()
{
    CheckType(NWebRomValue::Type::DOUBLE);
    return data_.f;
}

void WebViewValue::SetDouble(double value)
{
    CheckType(NWebRomValue::Type::DOUBLE);
    data_.f = value;
}

std::string WebViewValue::GetString()
{
    CheckType(NWebRomValue::Type::STRING);
    return str_;
}

void WebViewValue::SetString(const std::string& value)
{
    CheckType(NWebRomValue::Type::STRING);
    str_ = value;
}

const char* WebViewValue::GetBinary(int& length)
{
    CheckType(NWebRomValue::Type::BINARY);
    length = static_cast<int>(str_.size());
    return str_.c_str();
}

void WebViewValue::SetBinary(int length, const char* value)
{
    CheckType(NWebRomValue::Type::BINARY);
    str_.assign(value, length);
}

std::map<std::string, std::shared_ptr<NWebRomValue>> WebViewValue::GetDictValue()
{
    CheckType(NWebRomValue::Type::DICTIONARY);
    return dict_value_;
}

std::vector<std::shared_ptr<NWebRomValue>> WebViewValue::GetListValue()
{
    CheckType(NWebRomValue::Type::LIST);
    return list_value_;
}

std::shared_ptr<NWebRomValue> WebViewValue::NewChildValue()
{
    if (child_node_) {
        WVLOG_E("child node is not nullptr");
    }

    child_node_ = std::make_shared<WebViewValue>();
    return child_node_;
}

void WebViewValue::SaveDictChildValue(const std::string& key)
{
    if (!child_node_) {
        WVLOG_E("child node is nullptr");
        return;
    }

    CheckType(NWebRomValue::Type::DICTIONARY);
    dict_value_[key] = child_node_;
    child_node_ = nullptr;
}

void WebViewValue::SaveListChildValue()
{
    if (!child_node_) {
        WVLOG_E("child node is nullptr");
        return;
    }

    CheckType(NWebRomValue::Type::LIST);
    list_value_.push_back(child_node_);
    child_node_ = nullptr;
}

void WebViewValue::CheckType(NWebRomValue::Type type)
{
    if ((type != NWebRomValue::Type::NONE) && (type_ != type)) {
        WVLOG_E("type %{public}hhu - %{public}hhu is invalid", type_, type);
    }
}

} // namespace OHOS::NWeb

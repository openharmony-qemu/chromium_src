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

#include "nweb_core_value.h"

#include "nweb_hilog.h"

namespace OHOS::NWeb {

NWebCoreValue::NWebCoreValue(NWebHapValue::Type type) {
  type_ = type;
}

NWebHapValue::Type NWebCoreValue::GetType() {
  return type_;
}

void NWebCoreValue::SetType(NWebHapValue::Type type) {
  CheckType(NWebHapValue::Type::NONE);
  type_ = type;
}

int NWebCoreValue::GetInt() {
  CheckType(NWebHapValue::Type::INTEGER);
  return data_.n;
}

void NWebCoreValue::SetInt(int value) {
  CheckType(NWebHapValue::Type::INTEGER);
  data_.n = value;
}

bool NWebCoreValue::GetBool() {
  CheckType(NWebHapValue::Type::BOOLEAN);
  return data_.b;
}

void NWebCoreValue::SetBool(bool value) {
  CheckType(NWebHapValue::Type::BOOLEAN);
  data_.b = value;
}

double NWebCoreValue::GetDouble() {
  CheckType(NWebHapValue::Type::DOUBLE);
  return data_.f;
}

void NWebCoreValue::SetDouble(double value) {
  CheckType(NWebHapValue::Type::DOUBLE);
  data_.f = value;
}

std::string NWebCoreValue::GetString() {
  CheckType(NWebHapValue::Type::STRING);
  return str_;
}

void NWebCoreValue::SetString(const std::string& value) {
  CheckType(NWebHapValue::Type::STRING);
  str_ = value;
}

const char* NWebCoreValue::GetBinary(int& length) {
  CheckType(NWebHapValue::Type::BINARY);
  length = str_.size();
  return str_.c_str();
}

void NWebCoreValue::SetBinary(int length, const char* value) {
  CheckType(NWebHapValue::Type::BINARY);
  str_.assign(value, length);
}

std::map<std::string, std::shared_ptr<NWebHapValue>> NWebCoreValue::GetDictValue() {
  CheckType(NWebHapValue::Type::DICTIONARY);
  return dict_value_;
}

std::vector<std::shared_ptr<NWebHapValue>> NWebCoreValue::GetListValue() {
  CheckType(NWebHapValue::Type::LIST);
  return list_value_;
}

std::shared_ptr<NWebHapValue> NWebCoreValue::NewChildValue() {
  if (child_node_) {
    WVLOG_E("child node is not nullptr");
  }

  child_node_ = std::make_shared<NWebCoreValue>();
  return child_node_;
}

void NWebCoreValue::SaveDictChildValue(const std::string& key) {
  if (!child_node_) {
    WVLOG_E("child node is nullptr");
    return;
  }

  CheckType(NWebHapValue::Type::DICTIONARY);
  dict_value_[key] = child_node_;
  child_node_ = nullptr;
}

void NWebCoreValue::SaveListChildValue() {
  if (!child_node_) {
    WVLOG_E("child node is nullptr");
    return;
  }

  CheckType(NWebHapValue::Type::LIST);
  list_value_.push_back(child_node_);
  child_node_ = nullptr;
}

void NWebCoreValue::CheckType(NWebHapValue::Type type) {
  if ((type != NWebHapValue::Type::NONE) && (type_ != type)) {
    WVLOG_E("type %{public}hhu - %{public}hhu is invalid", type_, type);
  }
}

}  // namespace OHOS::NWeb

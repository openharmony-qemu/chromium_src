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

#include "nweb_value_convert.h"

#include "base/logging.h"
#include "cef/include/cef_task.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
std::shared_ptr<NWebValue> AddNWebValueCef(CefRefPtr<CefValue> argument) {
  if (!argument) {
    LOG(ERROR) << "AddNWebValueCef: argument is null";
    return std::make_shared<NWebValue>();
  }

  switch (argument->GetType()) {
    case CefValueType::VTYPE_INT: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_INT int = " << argument->GetInt();
      return std::make_shared<NWebValue>(argument->GetInt());
    }
    case CefValueType::VTYPE_DOUBLE: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_DOUBLE double = "
                 << argument->GetDouble();
      return std::make_shared<NWebValue>(argument->GetDouble());
    }
    case CefValueType::VTYPE_BOOL: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_BOOL bool = "
                 << argument->GetBool();
      return std::make_shared<NWebValue>(argument->GetBool());
    }
    case CefValueType::VTYPE_STRING: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_STRING string = "
                 << argument->GetStdString();
      return std::make_shared<NWebValue>(argument->GetStdString());
    }
    case CefValueType::VTYPE_LIST: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_LIST";
      size_t length = argument->GetList()->GetSize();
      std::vector<NWebValue> vec;
      for (size_t i = 0; i < length; ++i) {
        vec.push_back(*AddNWebValueCef(argument->GetList()->GetValue(i)));
      }
      return std::make_shared<NWebValue>(vec);
    }
    case CefValueType::VTYPE_DICTIONARY: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_DICTIONARY";
      std::map<std::string, NWebValue> map;
      auto dict = argument->GetDictionary();
      CefDictionaryValue::KeyList keys;
      dict->GetKeys(keys);
      for (auto& key : keys) {
        auto val = dict->GetValue(key);
        map[key.ToString()] = *AddNWebValueCef(val);
      }
      return std::make_shared<NWebValue>(map);
    }
    case CefValueType::VTYPE_BINARY: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_BINARY";
      auto size = argument->GetBinary()->GetSize();
      auto buff = std::make_unique<char[]>(size);
      argument->GetBinary()->GetData(buff.get(), size, 0);
      return std::make_shared<NWebValue>(buff.get(), size);
    }
    case CefValueType::VTYPE_INVALID: {
      LOG(DEBUG) << "AddNWebValueCef: VTYPE_INVALID";
      return std::make_shared<NWebValue>();
    }
    default: {
      LOG(INFO) << "AddNWebValueCef: not support value";
      break;
    }
  }
  return std::make_shared<NWebValue>();
}

std::vector<std::shared_ptr<NWebValue>> ParseCefValueTONWebValue(
    CefRefPtr<CefListValue> args,
    int size) {
  std::vector<std::shared_ptr<NWebValue>> value_vector;
  for (int i = 0; i < size; i++) {
    CefRefPtr<CefValue> argument = args->GetValue(i);
    value_vector.push_back(AddNWebValueCef(argument));
  }
  return value_vector;
}

CefValueType TranslateCefType(NWebValue::Type type) {
  switch (type) {
    case NWebValue::Type::INTEGER:
      return CefValueType::VTYPE_INT;
    case NWebValue::Type::DOUBLE: {
      return CefValueType::VTYPE_DOUBLE;
    }
    case NWebValue::Type::BOOLEAN:
      return CefValueType::VTYPE_BOOL;
    case NWebValue::Type::STRING:
      return CefValueType::VTYPE_STRING;
    case NWebValue::Type::DICTIONARY:
      return CefValueType::VTYPE_DICTIONARY;
    case NWebValue::Type::LIST:
      return CefValueType::VTYPE_LIST;
    case NWebValue::Type::NONE:
      return CefValueType::VTYPE_INVALID;
    case NWebValue::Type::BINARY:
      return CefValueType::VTYPE_BINARY;
    default:
      return CefValueType::VTYPE_INVALID;
  }
}

CefRefPtr<CefListValue> ParseNWebValueToValue(std::shared_ptr<NWebValue> value,
                                              CefRefPtr<CefListValue> result) {
  result->SetValue(0, ParseNWebValueToValueHelper(value));
  return result;
}

CefRefPtr<CefValue> ParseNWebValueToValueHelper(
    std::shared_ptr<NWebValue> value) {
  if (!value) {
    LOG(ERROR) << "ParseNWebValueToValueHelper: value is null";
    return CefValue::Create();
  }
  CefRefPtr<CefValue> cefValue = CefValue::Create();
  NWebValue::Type type = value->GetType();
  switch (type) {
    case NWebValue::Type::INTEGER: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: INTEGER = "
                 << value->GetInt();
      cefValue->SetInt(value->GetInt());
      return cefValue;
    }
    case NWebValue::Type::DOUBLE: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: DOUBLE = "
                 << value->GetDouble();
      cefValue->SetDouble(value->GetDouble());
      return cefValue;
    }
    case NWebValue::Type::BOOLEAN: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: BOOLEAN = "
                 << value->GetBoolean();
      cefValue->SetBool(value->GetBoolean());
      return cefValue;
    }
    case NWebValue::Type::STRING: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: STRING";
      cefValue->SetString(value->GetString());
      return cefValue;
    }
    case NWebValue::Type::LIST: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: LIST";
      size_t length = value->GetListValueSize();
      auto cefList = CefListValue::Create();
      for (size_t i = 0; i < length; i++) {
        auto nPtr = std::make_shared<NWebValue>(value->GetListValue(i));
        auto cefVal = ParseNWebValueToValueHelper(nPtr);
        cefList->SetValue(i, cefVal);
      }
      cefValue->SetList(cefList);
      return cefValue;
    }
    case NWebValue::Type::DICTIONARY: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: DICTIONARY";
      auto dict = value->GetDictionaryValue();
      auto cefDict = CefDictionaryValue::Create();
      for (auto& item : dict) {
        auto nPtr = std::make_shared<NWebValue>(item.second);
        auto cefVal = ParseNWebValueToValueHelper(nPtr);
        cefDict->SetValue(CefString(item.first), cefVal.get());
      }
      cefValue->SetDictionary(cefDict);
      return cefValue;
    }
    case NWebValue::Type::BINARY: {
      auto size = value->GetBinaryValueSize() + 1;
      auto buff = value->GetBinaryValue();
      LOG(DEBUG) << "ParseNWebValueToValueHelper: BINARY str=" << buff << ", size=" << size;
      auto cefDict = CefBinaryValue::Create(buff, size);
      cefValue->SetBinary(cefDict);
      return cefValue;
    }
    case NWebValue::Type::NONE: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: NONE";
      break;
    }
    default: {
      LOG(ERROR) << "ParseNWebValueToValueHelper: not support value type";
      break;
    }
  }
  return cefValue;
}
}  // namespace OHOS::NWeb

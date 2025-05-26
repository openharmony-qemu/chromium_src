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
#include "nweb_core_value.h"
#include "ohos_adapter_helper.h"
#include "nweb_core_value.h"
#include "securec.h"

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
      return std::make_shared<NWebValue>(argument->GetString().ToString());
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
      LOG(DEBUG) << "ParseNWebValueToValueHelper: BINARY str=" << buff
                 << ", size=" << size;
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

void AddNWebValueCefV2(CefRefPtr<CefValue> argument,
                       std::shared_ptr<NWebHapValue> value) {
  if (!argument) {
    LOG(ERROR) << "AddNWebValueCef: argument is null";
    return;
  }

  switch (argument->GetType()) {
    case CefValueType::VTYPE_INT: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_INT int = " << argument->GetInt();
      value->SetType(NWebHapValue::Type::INTEGER);
      value->SetInt(argument->GetInt());
      break;
    }
    case CefValueType::VTYPE_DOUBLE: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_DOUBLE double = "
                 << argument->GetDouble();
      value->SetType(NWebHapValue::Type::DOUBLE);
      value->SetDouble(argument->GetDouble());
      break;
    }
    case CefValueType::VTYPE_BOOL: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_BOOL bool = "
                 << argument->GetBool();
      value->SetType(NWebHapValue::Type::BOOLEAN);
      value->SetBool(argument->GetBool());
      break;
    }
    case CefValueType::VTYPE_STRING: {
      value->SetType(NWebHapValue::Type::STRING);
      value->SetString(argument->GetStdString());
      break;
    }
    case CefValueType::VTYPE_LIST: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_LIST";
      value->SetType(NWebHapValue::Type::LIST);
      size_t length = argument->GetList()->GetSize();
      for (size_t i = 0; i < length; ++i) {
        auto child = value->NewChildValue();
        AddNWebValueCefV2(argument->GetList()->GetValue(i), child);
        value->SaveListChildValue();
      }
      break;
    }
    case CefValueType::VTYPE_DICTIONARY: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_DICTIONARY";
      value->SetType(NWebHapValue::Type::DICTIONARY);
      std::map<std::string, NWebValue> map;
      auto dict = argument->GetDictionary();
      CefDictionaryValue::KeyList keys;
      dict->GetKeys(keys);
      for (auto& key : keys) {
        auto child = value->NewChildValue();
        AddNWebValueCefV2(dict->GetValue(key), child);
        value->SaveDictChildValue(key.ToString());
      }
      break;
    }
    case CefValueType::VTYPE_BINARY: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_BINARY";
      value->SetType(NWebHapValue::Type::BINARY);
      auto size = argument->GetBinary()->GetSize();
      auto buff = std::make_unique<char[]>(size);
      argument->GetBinary()->GetData(buff.get(), size, 0);
      value->SetBinary(size, buff.get());
      break;
    }
    case CefValueType::VTYPE_INVALID: {
      LOG(ERROR) << "AddNWebValueCef: VTYPE_INVALID";
      break;
    }
    default: {
      LOG(ERROR) << "AddNWebValueCef: not support value";
      break;
    }
  }
}

std::vector<std::shared_ptr<NWebHapValue>> ParseCefValueToHapValue(
    CefRefPtr<CefListValue> args,
    int size) {
  std::vector<std::shared_ptr<NWebHapValue>> value_vector;
  for (int i = 0; i < size; i++) {
    CefRefPtr<CefValue> argument = args->GetValue(i);
    std::shared_ptr<NWebHapValue> value = std::make_shared<NWebCoreValue>();
    AddNWebValueCefV2(argument, value);
    value_vector.push_back(value);
  }
  return value_vector;
}

CefRefPtr<CefListValue> ParseNWebValueToHapValue(
    std::shared_ptr<NWebHapValue> value,
    CefRefPtr<CefListValue> result) {
  result->SetValue(0, ParseHapValueToValueHelper(value));
  return result;
}

template <typename T>
CefRefPtr<CefValue> ParseNWebValueToValueHelperV2(std::shared_ptr<T> value) {
  if (!value) {
    LOG(ERROR) << "ParseNWebValueToValueHelper: value is null";
    return CefValue::Create();
  }
  CefRefPtr<CefValue> cefValue = CefValue::Create();
  auto type = value->GetType();
  switch (type) {
    case T::Type::INTEGER: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: INTEGER = "
                 << value->GetInt();
      cefValue->SetInt(value->GetInt());
      return cefValue;
    }
    case T::Type::DOUBLE: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: DOUBLE = "
                 << value->GetDouble();
      cefValue->SetDouble(value->GetDouble());
      return cefValue;
    }
    case T::Type::BOOLEAN: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: BOOLEAN = "
                 << value->GetBool();
      cefValue->SetBool(value->GetBool());
      return cefValue;
    }
    case T::Type::STRING: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: STRING";
      cefValue->SetString(value->GetString());
      return cefValue;
    }
    case T::Type::LIST: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: LIST";
      auto list = value->GetListValue();
      auto cefList = CefListValue::Create();
      int i = 0;
      for (auto& item : list) {
        auto cefVal = ParseNWebValueToValueHelperV2(item);
        cefList->SetValue(i++, cefVal);
      }
      cefValue->SetList(cefList);
      return cefValue;
    }
    case T::Type::DICTIONARY: {
      LOG(DEBUG) << "ParseNWebValueToValueHelper: DICTIONARY";
      auto dict = value->GetDictValue();
      auto cefDict = CefDictionaryValue::Create();
      for (auto& item : dict) {
        auto cefVal = ParseNWebValueToValueHelperV2(item.second);
        cefDict->SetValue(CefString(item.first), cefVal.get());
      }
      cefValue->SetDictionary(cefDict);
      return cefValue;
    }
    case T::Type::BINARY: {
      auto length = 0;
      auto buff = value->GetBinary(length);
      LOG(DEBUG) << "ParseNWebValueToValueHelper: BINARY str=" << buff
                 << ", size=" << length;
      auto cefDict = CefBinaryValue::Create(buff, length);
      cefValue->SetBinary(cefDict);
      return cefValue;
    }
    case T::Type::NONE: {
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

CefRefPtr<CefValue> ParseHapValueToValueHelper(
    std::shared_ptr<NWebHapValue> value) {
  return ParseNWebValueToValueHelperV2(value);
}

CefRefPtr<CefValue> ParseRomValueToValueHelper(
    std::shared_ptr<NWebRomValue> value) {
  return ParseNWebValueToValueHelperV2(value);
}

std::shared_ptr<NWebMessage> ConvertNwebHap2NwebMessage(std::shared_ptr<NWebHapValue> hap)
{
    std::shared_ptr<NWebMessage> message = std::make_shared<NWebMessage>();
    if (!hap) {
        return message;
    }
    auto type = hap->GetType();
    switch (type) {
        case NWebHapValue::Type::INTEGER: {
            message->SetType(NWebValue::Type::INTEGER);
            message->SetInt64(hap->GetInt());
            break;
        }
        case NWebHapValue::Type::DOUBLE: {
            message->SetType(NWebValue::Type::DOUBLE);
            message->SetDouble(hap->GetDouble());
            break;
        }
        case NWebHapValue::Type::BOOLEAN: {
            message->SetType(NWebValue::Type::BOOLEAN);
            message->SetBoolean(hap->GetBool());
            break;
        }
        case NWebHapValue::Type::STRING: {
            message->SetType(NWebValue::Type::STRING);
            message->SetString(hap->GetString());
            break;
        }
        case NWebHapValue::Type::LIST: {
            auto list = hap->GetListValue();
            size_t len = list.size();
            std::vector<std::string> string_arr;
            std::vector<bool> bool_arr;
            std::vector<double> double_arr;
            std::vector<int64_t> int64_arr;
            NWebValue::Type elem_type;
            for (size_t i = 0; i < len; i++) {
                if (!list[i]) {
                    continue;
                }
                if (list[i]->GetType() == NWebHapValue::Type::STRING) {
                    elem_type = NWebValue::Type::STRING;
                    string_arr.push_back(list[i]->GetString());
                } else if (list[i]->GetType() == NWebHapValue::Type::BOOLEAN) {
                    elem_type = NWebValue::Type::BOOLEAN;
                    bool_arr.push_back(list[i]->GetBool());
                } else if (list[i]->GetType() == NWebHapValue::Type::DOUBLE) {
                    elem_type = NWebValue::Type::DOUBLE;
                    double_arr.push_back(list[i]->GetDouble());
                } else if (list[i]->GetType() == NWebHapValue::Type::INTEGER) {
                    elem_type = NWebValue::Type::INTEGER;
                    int64_arr.push_back(list[i]->GetInt());
                }
            }
            if (elem_type == NWebValue::Type::STRING) {
                message->SetType(NWebValue::Type::STRINGARRAY);
                message->SetStringArray(string_arr);
            } else if (elem_type == NWebValue::Type::BOOLEAN) {
                message->SetType(NWebValue::Type::BOOLEANARRAY);
                message->SetBooleanArray(bool_arr);
            } else if (elem_type == NWebValue::Type::DOUBLE) {
                message->SetType(NWebValue::Type::DOUBLEARRAY);
                message->SetDoubleArray(double_arr);
            } else if (elem_type == NWebValue::Type::INTEGER) {
                message->SetType(NWebValue::Type::INT64ARRAY);
                message->SetInt64Array(int64_arr);
            }
            break;
        }
        case NWebHapValue::Type::DICTIONARY: {
            std::map<std::string, NWebValue> map;
            auto dict = hap->GetDictValue();
            message->SetType(NWebValue::Type::ERROR);
            std::string strErrName;
            std::string strErrMsg;
            if (dict.find("Error.name") != dict.end() && dict["Error.name"]) {
                strErrName = dict["Error.name"]->GetString();
            }
            if (dict.find("Error.message") != dict.end() && dict["Error.message"]) {
                strErrMsg = dict["Error.message"]->GetString();
            }
            message->SetErrName(strErrName);
            message->SetErrMsg(strErrMsg);
            break;
        }
        case NWebHapValue::Type::BINARY: {
            auto length = 0;
            auto buff = hap->GetBinary(length);
            std::vector<uint8_t> arr(length);
            memcpy_s(&arr[0], length, buff, length);
            message->SetType(NWebValue::Type::BINARY);
            message->SetBinary(arr);
            break;
        }
        default: {
            break;
        }
    }
    return message;
}
}  // namespace OHOS::NWeb

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

#ifndef NWEB_CORE_VALUE_H
#define NWEB_CORE_VALUE_H

#include "nweb_hap_value.h"

namespace OHOS::NWeb {

class NWebCoreValue : public NWebHapValue {
 public:
  union data_union {
    int n;
    bool b;
    double f;
  };

  NWebCoreValue() = default;
  virtual ~NWebCoreValue() = default;
  explicit NWebCoreValue(NWebHapValue::Type type);

  NWebHapValue::Type GetType() override;

  void SetType(NWebHapValue::Type type) override;

  int GetInt() override;

  void SetInt(int value) override;

  bool GetBool() override;

  void SetBool(bool value) override;

  double GetDouble() override;

  void SetDouble(double value) override;

  std::string GetString() override;

  void SetString(const std::string& value) override;

  const char* GetBinary(int& length) override;

  void SetBinary(int length, const char* value) override;

  std::map<std::string, std::shared_ptr<NWebHapValue>> GetDictValue() override;

  std::vector<std::shared_ptr<NWebHapValue>> GetListValue() override;

  std::shared_ptr<NWebHapValue> NewChildValue() override;

  void SaveDictChildValue(const std::string& key) override;

  void SaveListChildValue() override;

 private:
  void CheckType(NWebHapValue::Type type);

 private:
  NWebHapValue::Type type_ = NWebHapValue::Type::NONE;

  data_union data_;
  std::string str_;
  std::shared_ptr<NWebHapValue> child_node_ = nullptr;
  std::vector<std::shared_ptr<NWebHapValue>> list_value_;
  std::map<std::string, std::shared_ptr<NWebHapValue>> dict_value_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_CORE_VALUE_H

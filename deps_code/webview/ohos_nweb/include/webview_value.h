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

#ifndef WEBVIEW_VALUE_H
#define WEBVIEW_VALUE_H

#include "nweb_rom_value.h"

namespace OHOS::NWeb {

class WebViewValue : public NWebRomValue {
public:
    union data_union {
        int n;
        bool b;
        double f;
    };

    WebViewValue() = default;
    virtual ~WebViewValue() = default;
    explicit WebViewValue(NWebRomValue::Type type);

    NWebRomValue::Type GetType() override;

    void SetType(NWebRomValue::Type type) override;

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

    std::map<std::string, std::shared_ptr<NWebRomValue>> GetDictValue() override;

    std::vector<std::shared_ptr<NWebRomValue>> GetListValue() override;

    std::shared_ptr<NWebRomValue> NewChildValue() override;

    void SaveDictChildValue(const std::string& key) override;

    void SaveListChildValue() override;

    int64_t GetInt64() override
    {
        return value_;
    }

    void SetInt64(int64_t value) override
    {
        value_ = value;
    }

    void SetBinary(const std::vector<uint8_t>& binary_data) override
    {
        binary_data_.reserve(binary_data.size());
        binary_data_ = binary_data;
    }

    std::vector<uint8_t> GetBinary() override
    {
        return binary_data_;
    }

    std::string GetErrName() override
    {
        return err_name_;
    }

    void SetErrName(const std::string& name) override
    {
        err_name_ = name;
    }

    std::string GetErrMsg() override
    {
        return err_msg_;
    }

    void SetErrMsg(const std::string& msg) override
    {
        err_msg_ = msg;
    }

    std::vector<std::string> GetStringArray() override
    {
        return string_arr_;
    }

    void SetStringArray(const std::vector<std::string>& string_arr) override
    {
        string_arr_ = string_arr;
    }

    std::vector<bool> GetBoolArray() override
    {
        return bool_arr_;
    }

    void SetBoolArray(const std::vector<bool>& bool_arr) override
    {
        bool_arr_ = bool_arr;
    }

    std::vector<double> GetDoubleArray() override
    {
        return double_arr_;
    }

    void SetDoubleArray(const std::vector<double>& double_arr) override
    {
        double_arr_ = double_arr;
    }

    std::vector<int64_t> GetInt64Array() override
    {
        return int64_arr_;
    }

    void SetInt64Array(const std::vector<int64_t>& int64_arr) override
    {
        int64_arr_ = int64_arr;
    }

private:
    void CheckType(NWebRomValue::Type type);

private:
    NWebRomValue::Type type_ = NWebRomValue::Type::NONE;

    data_union data_;
    std::string str_;
    std::shared_ptr<NWebRomValue> child_node_ = nullptr;
    std::vector<std::shared_ptr<NWebRomValue>> list_value_;
    std::map<std::string, std::shared_ptr<NWebRomValue>> dict_value_;

    std::vector<uint8_t> binary_data_;
    std::string err_name_;
    std::string err_msg_;
    int64_t value_ = -1;
    std::vector<std::string> string_arr_;
    std::vector<bool> bool_arr_;
    std::vector<double> double_arr_;
    std::vector<int64_t> int64_arr_;
};

} // namespace OHOS::NWeb

#endif // WEBVIEW_VALUE_H

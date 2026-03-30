/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <optional>
#include "napi/native_api.h"
#include "aki/jsbind.h"

namespace {
std::optional<int> ReturnOptionalInt()
{
    constexpr int testNum = 32;
    return std::optional<int>(testNum);
}

std::optional<int> ReturnOptionalIntEmpty()
{
    return {};
}

std::optional<std::string> ReturnOptionalString(bool isEmpty)
{
    if (isEmpty) {
        return {};
    }
    
    return std::optional<std::string>("hello js from c++");
}

void GetOptionalString(std::optional<std::string> opt)
{
    if (opt.has_value()) {
        AKI_LOG(INFO) << "GetOptionalString has value: " << opt.value();
    } else {
        AKI_LOG(INFO) << "GetOptionalString get empty optional value";
    }
}

struct Human {
public:
    Human(std::string name) : name_(name) {}

    std::string GetName() const { return name_; }
private:
    std::string name_;
};


void GetOptionalObject(std::optional<Human> opt)
{
    if (opt.has_value()) {
        AKI_LOG(INFO) << "GetOptionalObject has value:: " << opt.value().GetName();
    } else {
        AKI_LOG(INFO) << "GetOptionalObject get empty optional value";
    }
}

std::optional<Human> ReturnOptionalObject(bool isEmpty)
{
    if (isEmpty) {
        return {};
    }
    
    return std::optional<Human>(Human("xiao zhang"));
}

std::optional<aki::Value> ReturnOtionalAkiValue(bool isEmpty)
{
    if (isEmpty) {
        return {};
    }
    
    return std::optional<aki::Value>(aki::Value("aki value string"));
}

void GetOptionalAkiValue(std::optional<aki::Value> opt)
{
    if (opt.has_value()) {
        AKI_LOG(INFO) << "GetOptionalAkiValue has aki value : " <<opt.value().As<std::string>();
    } else {
        AKI_LOG(INFO) << "GetOptionalAkiValue is empty";
    }
}

JSBIND_CLASS(Human) {
    JSBIND_CONSTRUCTOR<std::string>();
    JSBIND_METHOD(GetName);
}


JSBIND_GLOBAL() {
    JSBIND_FUNCTION(ReturnOptionalInt);
    JSBIND_FUNCTION(ReturnOptionalIntEmpty);
    JSBIND_FUNCTION(GetOptionalString);
    JSBIND_FUNCTION(ReturnOptionalString);
    
    JSBIND_FUNCTION(GetOptionalObject);
    JSBIND_FUNCTION(ReturnOptionalObject);
    
    JSBIND_FUNCTION(ReturnOtionalAkiValue);
    JSBIND_FUNCTION(GetOptionalAkiValue);
}

JSBIND_ADDON(hello)
} // end of namespace
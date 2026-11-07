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
#include <bits/alltypes.h>
#include <map>
#include <aki/jsbind.h>

namespace {
class Student {
public:
    Student(std::string name, int age)
    {
        name_ = name;
        age_ = age;
        job_ = "student";
    }

    std::string GetName() { return name_; }
    std::string GetJob() { return job_; }
    char* Getscore() { return score; }

public:
    std::string name_;
    char* job_ = nullptr;
    char score[8] = {'1', '2', '3', '\0'};
    int age_;
};

JSBIND_CLASS(Student) {
    JSBIND_CONSTRUCTOR<std::string, int>();
    JSBIND_METHOD(GetName);
    JSBIND_METHOD(GetJob);
    JSBIND_METHOD(Getscore);
}

class JsonObjectHandle {
public:
    JsonObjectHandle() = default;

    std::map<std::string, aki::Value> PassingAkiReturnMap()
    {
        int32_t val = 99;
        aki::Value value_one(val);
        std::map<std::string, aki::Value> map = {{"Thursday", value_one}};

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiValueReturnMapBuffer()
    {
        std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
        aki::Value jsArray = aki::Value::NewArray();
        for (size_t i = 0; i < cppArray.size(); ++i) {
            jsArray.Set(i, std::move(cppArray[i]));
        }
        std::map<std::string, aki::Value> map = {{"Thursday", jsArray}};

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiValueReturnMapObject()
    {
        const int32_t val = 25;
        aki::Value jsStudentObj = aki::Value::NewObject();
        jsStudentObj.Set("name", "Mark");
        jsStudentObj.Set("age", val);
        std::map<std::string, aki::Value> map = {{"Monday", jsStudentObj}};

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnCharBuff()
    {
        char textstr[10] = "Work";
        aki::Value value(textstr);
        std::map<std::string, aki::Value> map = {{"Monday", value}};

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapClassNormal()
    {
        const int32_t val = 25;
        Student tester("Mark", val);
        aki::Value jsStudentObj((Student)tester);
        std::map<std::string, aki::Value> map = {{"Monday", jsStudentObj}};

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapInt(std::map<std::string, aki::Value> map)
    {
        int32_t val = 99;
        aki::Value value(val);
        map["Thursday"] = value;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapFloat(std::map<std::string, aki::Value> map)
    {
        float val = 3.1500000953674316;
        aki::Value value(val);
        map["Thursday"] = value;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapBool(std::map<std::string, aki::Value> map)
    {
        aki::Value value(true);
        map["Thursday"] = value;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapString(std::map<std::string, aki::Value> map)
    {
        aki::Value value("12345");
        map["Thursday"] = value;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapCharPointer(std::map<std::string, aki::Value> map)
    {
        char *teststr = "1234";
        aki::Value value(teststr);
        map["Thursday"] = value;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapBuffer(std::map<std::string, aki::Value> map)
    {
        std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
        aki::Value jsArray = aki::Value::NewArray();
        for (size_t i = 0; i < cppArray.size(); ++i) {
            jsArray.Set(i, std::move(cppArray[i]));
        }
        map["Thursday"] = jsArray;

        return map;
    }

    std::map<std::string, aki::Value> PassingAkiReturnMapObject(std::map<std::string, aki::Value> map)
    {
        const int32_t val = 15;
        aki::Value jsStudentObj = aki::Value::NewObject();
        jsStudentObj.Set("name", "Jack");
        jsStudentObj.Set("age", val);
        map["Monday"] = jsStudentObj;

        return map;
    }
};

JSBIND_CLASS(JsonObjectHandle) {
    JSBIND_CONSTRUCTOR<>();
    JSBIND_METHOD(PassingAkiReturnMap, "passingAkiReturnMap");
    JSBIND_METHOD(PassingAkiValueReturnMapBuffer, "passingAkiValueReturnMapBuffer");
    JSBIND_METHOD(PassingAkiValueReturnMapObject, "passingAkiValueReturnMapObject");
    JSBIND_METHOD(PassingAkiReturnCharBuff, "passingAkiReturnCharBuff");
    JSBIND_METHOD(PassingAkiReturnMapInt, "passingAkiReturnMapInt");
    JSBIND_METHOD(PassingAkiReturnMapFloat, "passingAkiReturnMapFloat");
    JSBIND_METHOD(PassingAkiReturnMapBool, "passingAkiReturnMapBool");
    JSBIND_METHOD(PassingAkiReturnMapString, "passingAkiReturnMapString");
    JSBIND_METHOD(PassingAkiReturnMapCharPointer, "passingAkiReturnMapCharPointer");
    JSBIND_METHOD(PassingAkiReturnMapBuffer, "passingAkiReturnMapBuffer");
    JSBIND_METHOD(PassingAkiReturnMapObject, "passingAkiReturnMapObject");
    JSBIND_METHOD(PassingAkiReturnMapClassNormal, "passingAkiReturnMapClassNormal");
}

static std::map<std::string, aki::Value> PassingAkiReturnMap()
{
    int32_t val = 99;
    aki::Value value_one(val);
    std::map<std::string, aki::Value> map = {{"Monday", value_one}};

    return map;
}

static std::map<std::string, aki::Value> PassingAkiValueReturnMapBuffer()
{
    std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
    aki::Value jsArray = aki::Value::NewArray();
    for (size_t i = 0; i < cppArray.size(); ++i) {
        jsArray.Set(i, std::move(cppArray[i]));
    }
    std::map<std::string, aki::Value> map = {{"Monday", jsArray}};

    return map;
}

static std::map<std::string, aki::Value> PassingAkiValueReturnMapObject()
{
    const int32_t val = 25;
    aki::Value jsStudentObj = aki::Value::NewObject();
    jsStudentObj.Set("name", "Joe");
    jsStudentObj.Set("age", val);
    std::map<std::string, aki::Value> map = {{"Monday", jsStudentObj}};

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapInt(std::map<std::string, aki::Value> map)
{
    int32_t val = 99;
    aki::Value value(val);
    map["Monday"] = value;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapFloat(std::map<std::string, aki::Value> map)
{
    float val = 3.1500000953674316;
    aki::Value value(val);
    map["Monday"] = value;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapBool(std::map<std::string, aki::Value> map)
{
    aki::Value value(true);
    map["Monday"] = value;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapString(std::map<std::string, aki::Value> map)
{
    aki::Value value("234");
    map["Monday"] = value;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapCharPointer(std::map<std::string, aki::Value> map)
{
    char *teststr = "1234";
    aki::Value value(teststr);
    map["Monday"] = value;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapBuffer(std::map<std::string, aki::Value> map)
{
    std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
    aki::Value jsArray = aki::Value::NewArray();
    for (size_t i = 0; i < cppArray.size(); ++i) {
        jsArray.Set(i, std::move(cppArray[i]));
    }
    map["Thursday"] = jsArray;

    return map;
}

static std::map<std::string, aki::Value> PassingAkiReturnMapObject(std::map<std::string, aki::Value> map)
{
    const int32_t val = 35;
    aki::Value jsStudentObj = aki::Value::NewObject();
    jsStudentObj.Set("name", "Joe");
    jsStudentObj.Set("age", val);
    map["Monday"] = jsStudentObj;

    return map;
}

int GetAge(Student person) { return person.age_; }

JSBIND_GLOBAL() {
    JSBIND_FUNCTION(PassingAkiReturnMap, "passingAkiReturnMap");
    JSBIND_FUNCTION(PassingAkiValueReturnMapBuffer, "passingAkiValueReturnMapBuffer");
    JSBIND_FUNCTION(PassingAkiValueReturnMapObject, "passingAkiValueReturnMapObject");
    JSBIND_FUNCTION(PassingAkiReturnMapInt, "passingAkiReturnMapInt");
    JSBIND_FUNCTION(PassingAkiReturnMapFloat, "passingAkiReturnMapFloat");
    JSBIND_FUNCTION(PassingAkiReturnMapBool, "passingAkiReturnMapBool");
    JSBIND_FUNCTION(PassingAkiReturnMapString, "passingAkiReturnMapString");
    JSBIND_FUNCTION(PassingAkiReturnMapCharPointer, "passingAkiReturnMapCharPointer");
    JSBIND_FUNCTION(PassingAkiReturnMapBuffer, "passingAkiReturnMapBuffer");
    JSBIND_FUNCTION(PassingAkiReturnMapObject, "passingAkiReturnMapObject");
    JSBIND_FUNCTION(GetAge, "getAge");
}

JSBIND_ADDON(aki_map);
} // namespace
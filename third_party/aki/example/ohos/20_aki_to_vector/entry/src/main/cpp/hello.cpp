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

#include <vector>
#include <aki/jsbind.h>

namespace {
    class Student {
    public:
        std::string name_;
        char *job_ = nullptr;
        char score[8] = {'1', '2', '3', '\0'};
        int age_;
        Student(std::string name, int age)
        {
            name_ = name;
            age_ = age;
            job_ = "student";
        }
        std::string GetName() { return name_; }
        std::string GetJob() { return job_; }
        char *Getscore() { return score; }
    };

    JSBIND_CLASS(Student) {
        JSBIND_CONSTRUCTOR<std::string, int>(); // 绑定构造函数
        JSBIND_METHOD(GetName);
        JSBIND_METHOD(GetJob);
        JSBIND_METHOD(Getscore);
    }

    class VectorTestObject {
    public:
        VectorTestObject() = default;
        std::vector<aki::Value> PassingAkiReturnVector()
        {
            int32_t val = 42;
            std::vector<aki::Value> vec;
            vec.push_back(aki::Value(val));
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorInt(std::vector<aki::Value> vec)
        {
            int32_t val = 100;
            vec.push_back(aki::Value(val));
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorBool(std::vector<aki::Value> vec)
        {
            aki::Value value(false);
            vec.push_back(value);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorFloat(std::vector<aki::Value> vec)
        {
            float val = 3.1500000953674316;
            vec.push_back(aki::Value(val));
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorString(std::vector<aki::Value> vec)
        {
            aki::Value value("234");
            vec.push_back(value);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorCharPointer(std::vector<aki::Value> vec)
        {
            char *teststr = "d";
            aki::Value value(teststr);
            vec.push_back(value);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorConstCharPointer()
        {
            std::vector<aki::Value> vec;
            aki::Value value("12356789");
            vec.push_back(value);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnMapCharBuffer(std::vector<aki::Value> vec)
        {
            char teststr[10] = "123456789";
            aki::Value value(teststr);
            vec.push_back(value);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorArrayBuffer(std::vector<aki::Value> vec)
        {
            std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
            aki::Value jsArray = aki::Value::NewArray();
            for (size_t i = 0; i < cppArray.size(); ++i) {
                jsArray.Set(i, std::move(cppArray[i]));
            }
            vec.push_back(jsArray);
            return vec;
        }

        std::vector<aki::Value> PassingAkiValueReturnVectorObject()
        {
            std::vector<aki::Value> vec;
            const int32_t val = 20;
            aki::Value jsStudentObj = aki::Value::NewObject();
            jsStudentObj.Set("name", "zhangsan");
            jsStudentObj.Set("age", val);
            vec.push_back(jsStudentObj);
            return vec;
        }

        std::vector<aki::Value> PassingAkiReturnVectorObject(std::vector<aki::Value> vec)
        {
            const int32_t val = 22;
            aki::Value jsStudentObj = aki::Value::NewObject();
            jsStudentObj.Set("name", "lisi");
            jsStudentObj.Set("age", val);
            vec.push_back(jsStudentObj);
            return vec;
        }
    };


JSBIND_CLASS(VectorTestObject) {
    JSBIND_CONSTRUCTOR<>();
    JSBIND_METHOD(PassingAkiReturnVector, "passingAkiReturnVector");
    JSBIND_METHOD(PassingAkiReturnVectorInt, "passingAkiReturnVectorInt");
    JSBIND_METHOD(PassingAkiReturnVectorBool, "passingAkiReturnVectorBool");
    JSBIND_METHOD(PassingAkiReturnVectorFloat, "passingAkiReturnVectorFloat");
    JSBIND_METHOD(PassingAkiReturnVectorString, "passingAkiReturnVectorString");
    JSBIND_METHOD(PassingAkiReturnVectorCharPointer, "passingAkiReturnVectorCharPointer");
    JSBIND_METHOD(PassingAkiReturnVectorConstCharPointer, "passingAkiReturnVectorConstCharPointer");
    JSBIND_METHOD(PassingAkiReturnMapCharBuffer, "passingAkiReturnMapCharBuffer");
    JSBIND_METHOD(PassingAkiReturnVectorArrayBuffer, "passingAkiReturnVectorArrayBuffer");
    JSBIND_METHOD(PassingAkiValueReturnVectorObject, "passingAkiValueReturnVectorObject");
    JSBIND_METHOD(PassingAkiReturnVectorObject, "passingAkiReturnVectorObject");
}

static std::vector<aki::Value> PassingAkiReturnVector()
{
    int32_t val = 42;
    std::vector<aki::Value> vec;
    vec.push_back(aki::Value(val));
    return vec;
}

static std::vector<aki::Value> PassingAkiReturnVectorInt(std::vector<aki::Value> vec)
{
    int32_t val = 100;
    vec.push_back(aki::Value(val));
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorBool(std::vector<aki::Value> vec)
{
    aki::Value value(false);
    vec.push_back(value);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorFloat(std::vector<aki::Value> vec)
{
    float val = 3.1500000953674316;
    vec.push_back(aki::Value(val));
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorString(std::vector<aki::Value> vec)
{
    aki::Value value("234");
    vec.push_back(value);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorCharPointer(std::vector<aki::Value> vec)
{
    char *teststr = "d";
    aki::Value value(teststr);
    vec.push_back(value);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorConstCharPointer()
{
    std::vector<aki::Value> vec;
    aki::Value value("12356789");
    vec.push_back(value);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnMapCharBuffer(std::vector<aki::Value> vec)
{
    char teststr[10] = "123456789";
    aki::Value value(teststr);
    vec.push_back(value);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorArrayBuffer(std::vector<aki::Value> vec)
{
    std::vector<std::string> cppArray = {"Element1", "Element2", "Element3"};
    aki::Value jsArray = aki::Value::NewArray();
    for (size_t i = 0; i < cppArray.size(); ++i) {
        jsArray.Set(i, std::move(cppArray[i]));
    }
    vec.push_back(jsArray);
    return vec;
}

std::vector<aki::Value> PassingAkiValueReturnVectorObject()
{
    std::vector<aki::Value> vec;
    const int32_t val = 20;
    aki::Value jsStudentObj = aki::Value::NewObject();
    jsStudentObj.Set("name", "zhangsan");
    jsStudentObj.Set("age", val);
    vec.push_back(jsStudentObj);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnVectorObject(std::vector<aki::Value> vec)
{
    const int32_t val = 22;
    aki::Value jsStudentObj = aki::Value::NewObject();
    jsStudentObj.Set("name", "lisi");
    jsStudentObj.Set("age", val);
    vec.push_back(jsStudentObj);
    return vec;
}

std::vector<aki::Value> PassingAkiReturnClassNormal()
{
    const int32_t val = 25;
    std::vector<aki::Value> vec;
    Student tester("Mark", val);
    aki::Value jsStudentObj((Student)tester);
    vec.push_back(jsStudentObj);
    return vec;
}

int GetAge(Student person) { return person.age_; }

std::string GetName(Student person) { return person.name_; }

JSBIND_GLOBAL() {
    JSBIND_FUNCTION(PassingAkiReturnVector, "passingAkiReturnVector");
    JSBIND_FUNCTION(PassingAkiReturnVectorInt, "passingAkiReturnVectorInt");
    JSBIND_FUNCTION(PassingAkiReturnVectorBool, "passingAkiReturnVectorBool");
    JSBIND_FUNCTION(PassingAkiReturnVectorFloat, "passingAkiReturnVectorFloat");
    JSBIND_FUNCTION(PassingAkiReturnVectorString, "passingAkiReturnVectorString");
    JSBIND_FUNCTION(PassingAkiReturnVectorCharPointer, "passingAkiReturnVectorCharPointer");
    JSBIND_FUNCTION(PassingAkiReturnVectorConstCharPointer, "passingAkiReturnVectorConstCharPointer");
    JSBIND_FUNCTION(PassingAkiReturnMapCharBuffer, "passingAkiReturnMapCharBuffer");
    JSBIND_FUNCTION(PassingAkiReturnVectorArrayBuffer, "passingAkiReturnVectorArrayBuffer");
    JSBIND_FUNCTION(PassingAkiValueReturnVectorObject, "passingAkiValueReturnVectorObject");
    JSBIND_FUNCTION(PassingAkiReturnVectorObject, "passingAkiReturnVectorObject");
    JSBIND_FUNCTION(PassingAkiReturnClassNormal, "passingAkiReturnClassNormal");
}

    JSBIND_ADDON(aki_to_vector);
} // namespace
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
#include "napi/native_api.h"
#include "aki/jsbind.h"
#include "hilog/log.h"
#undef LOG_TAG
#define LOG_TAG "test class extends"

namespace {
class Person {
public:
    Person(std::string name, int age)
    {
        OH_LOG_INFO(LOG_APP, "enter Person");
        name_ = name;
        age_ = age;
    }

    virtual void SetName(std::string name)
    {
        OH_LOG_INFO(LOG_APP, "enter Person SetName");
        name_ = name;
    };

    virtual void SetAge(int age)
    {
        OH_LOG_INFO(LOG_APP, "enter Person SetAge");
        age_ = age;
    };

    virtual std::string GetName() const
    {
        OH_LOG_INFO(LOG_APP, "enter Person GetName");
        return name_;
    }

    virtual int GetAge() const
    {
        OH_LOG_INFO(LOG_APP, "enter Person GetAge");
        return age_;
    }

    virtual ~Person()
    {
        OH_LOG_INFO(LOG_APP, "enter ~Person");
    }

private:
    std::string name_;
    int age_;
};

class Student : public Person {
public:
    Student(std::string name, int age) : Person(name, age)
    {
        OH_LOG_INFO(LOG_APP, "enter Student");
    }

    void SetGrade(int grade)
    {
        OH_LOG_INFO(LOG_APP, "enter Student SetGrade");
        grade_ = grade;
    }

    int GetGrade(void) const
    {
        OH_LOG_INFO(LOG_APP, "enter Student GetGrade");
        return grade_;
    }

    int GetAge() const override
    {
        OH_LOG_INFO(LOG_APP, "enter Student GetAge");
        return Person::GetAge();
    }

    void SetAge(int age) override
    {
        OH_LOG_INFO(LOG_APP, "enter Student SetAge");
        Person::SetAge(age);
    }

    ~Student() override
    {
        OH_LOG_INFO(LOG_APP, "enter ~Student");
    }

    int grade_;
};

JSBIND_CLASS(Person)
{
    JSBIND_CONSTRUCTOR<std::string, int>();
    JSBIND_METHOD(SetName);
    JSBIND_METHOD(SetAge);
    JSBIND_METHOD(GetName);
    JSBIND_METHOD(GetAge);
}

JSBIND_CLASS(Student)
{
    // 导入父类
    JSBIND_EXTEND(Person);
    
    JSBIND_CONSTRUCTOR<std::string, int>();
    JSBIND_PROPERTY(grade_);
    JSBIND_METHOD(SetGrade);
    JSBIND_METHOD(GetGrade);
    JSBIND_METHOD(SetAge);
    JSBIND_METHOD(GetAge);
}

JSBIND_ADDON(entry);
};

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
#include <napi/native_api.h>
#include <aki/jsbind.h>

namespace {
aki::Value passingNullReturnNull(aki::Value value)
{
    if (value.IsNull()) {
        return value;
    }
    
    return aki::Value::NewObject();  // js undefined value
}

void passingNullToJs(aki::Callback<void(napi_value)> func)
{
    napi_value nullValue;
    if (napi_ok == napi_get_null(aki::JSBind::GetScopedEnv(), &nullValue)) {
        func(nullValue);
    }
}


struct Student {
    std::string name_;
    int age_;
        
public:
    Student()
    {
        name_ = "aki student";  // student name
    }
    
    std::string GetName() const
    {
        return name_;
    }
    
    void SetName(std::string name)
    {
        name_ = name;
    }
    
    int GetAge() const
    {
        return age_;
    }
    
    void SetAge(int age)
    {
        age_ = age;
    }
};


bool PassingJsNullToObjectPtrConvertToNullptr(Student* s)
{
    if (s == nullptr) {
        // js null value convert to nullptr
        return true;
    }
    
    return false;
}

Student* ReturnNullptrToObjectPtrConvertToJsNull()
{
    // will convert to js null
    return nullptr;
}

bool PassingJsNullToSharedPtrConvertToNullptr(std::shared_ptr<Student> s)
{
    if (s == nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr<Student> ReturnNullptrToSharedPtrConvertToJsNull()
{
    return nullptr;
}

bool PassingJsNullToReferenceConvertToNullptr(Student& s)
{
    if (reinterpret_cast<void*>(&s) == nullptr) {
        return true;
    }
    
    return false;
}


JSBIND_CLASS(Student)
{
    JSBIND_CONSTRUCTOR<>();
    JSBIND_FIELD("name", GetName, SetName);
    JSBIND_FIELD("age", GetAge, SetAge);
}

JSBIND_GLOBAL()
{
    JSBIND_FUNCTION(passingNullReturnNull);
    JSBIND_FUNCTION(passingNullToJs);
    
    JSBIND_FUNCTION(PassingJsNullToObjectPtrConvertToNullptr);
    JSBIND_FUNCTION(ReturnNullptrToObjectPtrConvertToJsNull);
    
    JSBIND_FUNCTION(PassingJsNullToSharedPtrConvertToNullptr);
    JSBIND_FUNCTION(ReturnNullptrToSharedPtrConvertToJsNull);
    
    JSBIND_FUNCTION(PassingJsNullToReferenceConvertToNullptr);
}
}; // end namespace
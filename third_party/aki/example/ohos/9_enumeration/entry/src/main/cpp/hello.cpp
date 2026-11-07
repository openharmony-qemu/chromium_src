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

#include <aki/jsbind.h>

enum TypeFlags {
    NONE,
    NUM,
    STRING,
    BUTT = -1
};

enum class ClassEnumFrom100 { 
    NONE = 100,
    NUM,
    STRING,
    BUTT
};

enum class ClassEnumFrom200 {
    NONE = 200,
    NUM,
    STRING,
    BUTT
};

TypeFlags Foo(TypeFlags flag) {
    return TypeFlags::BUTT;
}

TypeFlags Passing(TypeFlags flag) {
    return flag;
}

ClassEnumFrom100 Foofrom100(ClassEnumFrom100 flag) {
    return ClassEnumFrom100::BUTT;
}

ClassEnumFrom100 Passingfrom100(ClassEnumFrom100 flag) {
    return flag;
}

ClassEnumFrom200 Foofrom200(ClassEnumFrom200 flag) {
    return ClassEnumFrom200::BUTT;
}

ClassEnumFrom200 Passingfrom200(ClassEnumFrom200 flag) {
    return flag;
}

JSBIND_GLOBAL() {
    JSBIND_FUNCTION(Foo);
    JSBIND_FUNCTION(Passing);
    JSBIND_FUNCTION(Foofrom100);
    JSBIND_FUNCTION(Passingfrom100);
    JSBIND_FUNCTION(Foofrom200);
    JSBIND_FUNCTION(Passingfrom200);
}

JSBIND_ENUM(TypeFlags) {
    JSBIND_ENUM_VALUE(NONE);
    JSBIND_ENUM_VALUE(NUM);
    JSBIND_ENUM_VALUE(STRING);
}

JSBIND_ENUM(ClassEnumFrom100) {
    JSBIND_ENUM_VALUE(NONE);
    JSBIND_ENUM_VALUE(NUM);
    JSBIND_ENUM_VALUE(STRING);
    JSBIND_ENUM_VALUE(BUTT);
    
}

JSBIND_ENUM(ClassEnumFrom200) {
    JSBIND_ENUM_VALUE(NONE);
    JSBIND_ENUM_VALUE(NUM);
    JSBIND_ENUM_VALUE(STRING);
    JSBIND_ENUM_VALUE(BUTT);
}


JSBIND_ADDON(enumeration);
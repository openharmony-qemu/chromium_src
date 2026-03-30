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

enum TypeEnum {
    NONE,
    NUM,
    STRING,
    ERRTPYE = -1
};

enum NumberEnum {
    ZERO  = 0,
    ONE,
    FIFTY = 50,
    ERRNUMBER = -1
};

enum class ClassNumberEnum {
    ZERO = 100,
    ONE,
    FIFTY,
    ERRNUMBER
};


TypeEnum PassingTypeEnumReturnTypeEnum(TypeEnum flag)
{
    return flag;
}

NumberEnum PassingNumberEnumReturnNumberEnum(NumberEnum flag)
{
    return flag;
}

ClassNumberEnum PassingClassEnumReturnClassEnum(ClassNumberEnum flag)
{
    return flag;
}


JSBIND_GLOBAL()
{
    JSBIND_FUNCTION(PassingTypeEnumReturnTypeEnum);
    JSBIND_FUNCTION(PassingNumberEnumReturnNumberEnum);
    JSBIND_FUNCTION(PassingClassEnumReturnClassEnum);
}

JSBIND_ENUM(TypeEnum)
{
    JSBIND_ENUM_VALUE(NONE);
    JSBIND_ENUM_VALUE(NUM);
    JSBIND_ENUM_VALUE(STRING);
}

JSBIND_ENUM(NumberEnum)
{
    JSBIND_ENUM_VALUE(ZERO);
    JSBIND_ENUM_VALUE(ONE);
    JSBIND_ENUM_VALUE(FIFTY);
}

JSBIND_ENUM(ClassNumberEnum)
{
    JSBIND_ENUM_VALUE(ZERO);
    JSBIND_ENUM_VALUE(ONE);
    JSBIND_ENUM_VALUE(FIFTY);
}
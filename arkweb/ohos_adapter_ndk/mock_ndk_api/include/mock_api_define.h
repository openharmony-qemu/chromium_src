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

#ifndef MOCK_API_DEFINE_H
#define MOCK_API_DEFINE_H

#define GET_MOCK_KEY(func_name) \
g_mock_##func_name

#define GET_MOCK_CLASS_KEY(class_name, func_name) \
class_name::g_mock_##func_name

#define DECLARE_MOCK_KEY(func_name) \
static bool GET_MOCK_KEY(func_name)

#define DECLARE_IMPLEMENT_INSTANCE(class_name) \
static class_name& GetInstance() {\
    static class_name instance; \
    return instance; \
}

#define IMPLEMENT_MOCK_KEY(class_name,func_name) \
bool class_name::GET_MOCK_KEY(func_name) = false

#define SET_MOCK_KEY_TRUE(class_name,func_name) \
class_name::GET_MOCK_KEY(func_name) = true

#define SET_MOCK_KEY_FALSE(class_name,func_name) \
class_name::GET_MOCK_KEY(func_name) = false

#define DECLARE_REAL_FUNC(return_type, func_name,func_input) \
return_type __real_##func_name func_input


#define IMPLEMENT_WRAP_FUNCTION(return_type, class_name, func_name, func_input, ...) \
    return_type __wrap_##func_name func_input { \
        return GET_MOCK_CLASS_KEY(class_name, func_name) ? class_name::GetInstance().func_name(__VA_ARGS__) \
                                  : __real_##func_name(__VA_ARGS__); \
    }

#endif // MOCK_API_DEFINE_H

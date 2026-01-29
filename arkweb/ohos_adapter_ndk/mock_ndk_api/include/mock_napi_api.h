/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MOCK_NAPI_ADPI_H
#define MOCK_NAPI_ADPI_H

#include <gmock/gmock.h>
#include "napi/native_api.h"
#include "js_native_api.h"

namespace OHOS {
namespace NWeb {

class MockNapiFunctions {
public:
    virtual ~MockNapiFunctions() = default;
    
    MOCK_METHOD(napi_status, napi_create_ark_runtime, (napi_env* env));
    MOCK_METHOD(napi_status, napi_load_module_with_info, (napi_env env, const char* path,
                                                        const char* module_info, napi_value* result));
    MOCK_METHOD(napi_status, napi_create_object, (napi_env env, napi_value* result));
    MOCK_METHOD(napi_status, napi_get_named_property, (napi_env env, napi_value object, 
                                                    const char* utf8name, napi_value* result));
    MOCK_METHOD(napi_status, napi_create_uint32, (napi_env env, uint32_t value, napi_value* result));
    MOCK_METHOD(napi_status, napi_set_named_property, (napi_env env, napi_value object, 
                                                    const char* utf8name, napi_value value));
    MOCK_METHOD(napi_status, napi_create_string_utf8, (napi_env env, const char* str, 
                                                    size_t length, napi_value* result));
    MOCK_METHOD(napi_status, napi_call_function, (napi_env env, napi_value recv, napi_value func,
                                                size_t argc, const napi_value* argv, napi_value* result));
    MOCK_METHOD(napi_status, napi_open_handle_scope, (napi_env env, napi_handle_scope* result));
    MOCK_METHOD(napi_status, napi_close_handle_scope, (napi_env env, napi_handle_scope scope));
    MOCK_METHOD(napi_status, napi_create_int32, (napi_env env, int32_t value, napi_value* result));
};

extern MockNapiFunctions* g_MockNapiFunctions;
extern MockNapiFunctions* GetMockNapiFunctions();
extern void SetMockNapiFunctions(MockNapiFunctions* mock);

extern "C" {
    napi_status napi_create_ark_runtime(napi_env* env);
    napi_status napi_load_module_with_info(napi_env env, const char* path, 
 	                                       const char* module_info, napi_value* result);
    napi_status napi_create_object(napi_env env, napi_value* result);
    napi_status napi_get_named_property(napi_env env, napi_value object, 
 	                                    const char* utf8name, napi_value* result);
    napi_status napi_create_uint32(napi_env env, uint32_t value, napi_value* result);
    napi_status napi_set_named_property(napi_env env, napi_value object, 
 	                                    const char* utf8name, napi_value value);
    napi_status napi_create_string_utf8(napi_env env, const char* str, 
 	                                    size_t length, napi_value* result);
    napi_status napi_call_function(napi_env env, napi_value recv, napi_value func, 
 	                               size_t argc, const napi_value* argv, napi_value* result);
    napi_status napi_open_handle_scope(napi_env env, napi_handle_scope* result);
    napi_status napi_close_handle_scope(napi_env env, napi_handle_scope scope);
    napi_status napi_create_int32(napi_env env, int32_t value, napi_value* result);
}
}   // namespace NWeb
}   // namespace OHOS

#endif  // MOCK_NAPI_ADPI_H
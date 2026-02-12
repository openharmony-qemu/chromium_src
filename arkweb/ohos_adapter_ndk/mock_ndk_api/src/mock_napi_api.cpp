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

#include "../include/mock_napi_api.h"
#include <cstdio>

namespace OHOS {
namespace NWeb {
MockNapiFunctions* g_MockNapiFunctions = nullptr;
MockNapiFunctions* GetMockNapiFunctions() {
    return g_MockNapiFunctions;
}

void SetMockNapiFunctions(MockNapiFunctions* mock) {
    g_MockNapiFunctions = mock;
}

extern "C" {
    napi_status napi_create_ark_runtime(napi_env* env) {
        printf("Mock napi_create_ark_runtime called\n");
        if (env == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_ark_runtime(env);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_load_module_with_info(napi_env env, const char* path, 
                                           const char* module_info, napi_value* result) {
        if (path == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_load_module_with_info(env, path, module_info, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_object(napi_env env, napi_value* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_object(env, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_get_named_property(napi_env env, napi_value object, 
                                        const char* utf8name, napi_value* result) {
        if (utf8name == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_get_named_property(env, object, utf8name, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_uint32(napi_env env, uint32_t value, napi_value* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_uint32(env, value, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_set_named_property(napi_env env, napi_value object, 
                                        const char* utf8name, napi_value value) {
        if (utf8name == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_set_named_property(env, object, utf8name, value);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_string_utf8(napi_env env, const char* str, 
                                        size_t length, napi_value* result) {
        if (str == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_string_utf8(env, str, length, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_call_function(napi_env env, napi_value recv, napi_value func, 
                                   size_t argc, const napi_value* argv, napi_value* result) {
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_call_function(env, recv, func, argc, argv, result);
        }
        return napi_generic_failure;
    }

    napi_status napi_open_handle_scope(napi_env env, napi_handle_scope* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_open_handle_scope(env, result);
        }
        return napi_generic_failure;
    }

    napi_status napi_close_handle_scope(napi_env env, napi_handle_scope scope) {
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_close_handle_scope(env, scope);
        }
        return napi_generic_failure;
    }

    napi_status napi_create_int32(napi_env env, int32_t value, napi_value* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_int32(env, value, result);
        }
        return napi_generic_failure;
    }
}
}   // namespace NWeb
}   // namespace OHOS
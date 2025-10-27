/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef AUTO_NAPI_REF_H
#define AUTO_NAPI_REF_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

struct AutoNapiRef {
    AutoNapiRef() {}
    AutoNapiRef(napi_env env, napi_value value)
    {
        CreateReference(env, value);
    }
    AutoNapiRef(const AutoNapiRef&) = delete;
    AutoNapiRef& operator=(const AutoNapiRef&) = delete;

    void CreateReference(napi_env env, napi_value value) {
        Reset();
        env_ = env;
        if (env) {
            napi_create_reference(env, value, 1, &ref_);
        }
    }

    ~AutoNapiRef()
    {
        if (env_ && ref_) {
            napi_delete_reference(env_, ref_);
        }
    }

    napi_value GetRefValue()
    {
        napi_value value = nullptr;
        if (env_ && ref_) {
            napi_get_reference_value(env_, ref_, &value);
        }
        return value;
    }

    void Reset()
    {
        if (env_ && ref_) {
            napi_delete_reference(env_, ref_);
        }
        ref_ = nullptr;
        env_ = nullptr;
    }

    napi_ref ref_ = nullptr;
    napi_env env_ = nullptr;
};

#endif // AUTO_NAPI_REF_H

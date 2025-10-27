/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_HAP_VALUE_VECTOR_H_
#define ARK_WEB_HAP_VALUE_VECTOR_H_
#pragma once

#include "base/include/ark_web_types.h"

struct _ark_web_hap_value_t;

typedef struct {
    int size;
    ArkWebString* key;
    struct _ark_web_hap_value_t** value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebHapValueMap;

const ArkWebHapValueMap ark_web_hap_value_map_default = {
    0,
};

typedef struct {
    int size;
    struct _ark_web_hap_value_t** value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebHapValueVector;

const ArkWebHapValueVector ark_web_hap_value_vector_default = {
    0,
};

#endif // ARK_WEB_HAP_VALUE_VECTOR_H_

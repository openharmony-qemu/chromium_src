/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_FRAME_RATE_SETTING_ADAPTER_VECTOR_H
#define ARK_FRAME_RATE_SETTING_ADAPTER_VECTOR_H
#pragma once

#include <stdint.h>

#include "base/include/ark_web_memory.h"

typedef struct _ArkFrameRateSettingAdapter {
    int32_t min_ { 0 };
    int32_t max_ { 0 };
    int32_t preferredFrameRate_ { 0 };
} ArkFrameRateSettingAdapter;

const ArkFrameRateSettingAdapter ark_web_value_default = { 0 };

typedef struct {
    int size;
    ArkFrameRateSettingAdapter* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkFrameRateSettingAdapterVector;

const ArkFrameRateSettingAdapterVector ark_frame_rate_setting_adapter_vector_default = { 0 };

#endif // ARK_FRAME_RATE_SETTING_ADAPTER_VECTOR_H

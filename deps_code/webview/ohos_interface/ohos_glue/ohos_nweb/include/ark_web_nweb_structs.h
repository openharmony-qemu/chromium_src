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

#ifndef ARK_WEB_NWEB_STRUCTS_H
#define ARK_WEB_NWEB_STRUCTS_H

#include <stdint.h>
#include <functional>

typedef struct {
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    int32_t second;
} ArkWebDateTime;

const ArkWebDateTime ark_web_date_time_default = {
    0,
};

typedef int64_t (*AccessibilityIdGenerateFunc)();

typedef void (*NativeArkWebOnValidCallback)(const char*);

typedef void (*NativeArkWebOnDestroyCallback)(const char*);

typedef char* (*NativeArkWebOnJavaScriptProxyCallback)(const char**, int32_t);

using WebSnapshotCallback = std::function<void(const char*, bool, float, void*, int, int)>;

#endif // ARK_WEB_NWEB_STRUCTS_H
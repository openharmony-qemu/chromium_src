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

#ifndef ARK_WEB_MACROS_H_
#define ARK_WEB_MACROS_H_
#pragma once

#define SAFE_FREE(ptr, func) \
    if (ptr) {               \
        func(ptr);           \
        ptr = nullptr;       \
    }

#define ARK_WEB_EXPORT __attribute__((visibility("default")))

#define ARK_WEB_CALLBACK

#define ARK_WEB_RETURN_VOID
 
#define ARK_WEB_NO_SANITIZE __attribute__((no_sanitize("cfi-icall", "cfi")))

#endif // ARK_WEB_MACROS_H_

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
 
#ifndef OHOS_NWEB_SRC_NDK_COMMON_MEM_HOOK_H
#define OHOS_NWEB_SRC_NDK_COMMON_MEM_HOOK_H
 
#include <cstddef>
#include <memory>
 
namespace OHOS::NWeb {
 
char* malloc_wrapper(size_t size);
 
}
 
#endif  // OHOS_NWEB_SRC_NDK_COMMON_MEM_HOOK_H

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

#include "nweb_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

bool g_is_native_api_enable = false;
OnArkWebFreeMemoryFunc g_free_memory_func = nullptr;

void EnableNativeApi(void) {
  g_is_native_api_enable = true;
}

bool IsNativeApiEnable() {
  return g_is_native_api_enable;
}

void SetFreeMemoryFunc(OnArkWebFreeMemoryFunc func) {
  g_free_memory_func = func;
}

void FreeExternalMemory(void* addr) {
  if (!addr) {
    return;
  }

  if (!g_free_memory_func) {
    return;
  }

  g_free_memory_func(addr);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

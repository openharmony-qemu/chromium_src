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

#ifndef BASE_PROCESS_PROCESS_HANDLE_EXT_H_
#define BASE_PROCESS_PROCESS_HANDLE_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/base_export.h"
#include "base/process/process_handle.h"

namespace base {

#if BUILDFLAG(ARKWEB_USE_UNIQUE_RENDERER_PROCESS_ID)
// Returns the global id of the current process.
BASE_EXPORT ProcessId GetCurrentRealPid();
#endif

}  // namespace base

#endif  // BASE_PROCESS_PROCESS_HANDLE_EXT_H_

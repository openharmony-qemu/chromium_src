/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_interface.h"
#include "ohos/adapter/common/shared_library.h"
namespace ohos::adapter::multiprocess {

common::SharedLibrary childprocess("child_process");

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t OH_Ability_StartNativeChildProcessFn(const char* entry,
                                             NativeChildProcess_Args args,
                                             NativeChildProcess_Options options,
                                             int32_t* pid) {
  using StartNativeChildProcessType = int32_t(const char*, NativeChildProcess_Args,
                                         NativeChildProcess_Options, int32_t*);
  if (childprocess.IsLoaded()) {
    auto fn = childprocess.GetFunction<StartNativeChildProcessType>(
        "OH_Ability_StartNativeChildProcess");
    if (fn != nullptr) {
      return fn(entry, args, options, pid);
    }
  }
  return -1;
}

bool IsStartNativeChildProcessAvailable() {
  static bool available = childprocess.HasSymbol("OH_Ability_StartNativeChildProcess");
  return available;
}
}  // namespace ohos::adapter::multiprocess

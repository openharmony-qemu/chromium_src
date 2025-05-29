// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_interface.h"

#include "ohos/adapter/common/shared_library.h"
namespace ohos::adapter::multiprocess {

common::SharedLibrary childprocess("child_process");

__attribute__((no_sanitize("cfi", "cfi-icall"))) int32_t
OH_Ability_StartNativeChildProcessFn(const char* entry,
                                     NativeChildProcess_Args args,
                                     NativeChildProcess_Options options,
                                     int32_t* pid) {
  using StartNativeChildProcessType =
      int32_t(const char*, NativeChildProcess_Args, NativeChildProcess_Options,
              int32_t*);
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
  static bool available =
      childprocess.HasSymbol("OH_Ability_StartNativeChildProcess");
  return available;
}
}  // namespace ohos::adapter::multiprocess

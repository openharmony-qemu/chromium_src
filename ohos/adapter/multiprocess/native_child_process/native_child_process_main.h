// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_NATIVE_CHILD_PROCESS_MAIN_H_
#define OHOS_ADAPTER_MULTIPROCESS_NATIVE_CHILD_PROCESS_MAIN_H_

#include <AbilityKit/native_child_process.h>

#include "ohos/adapter/export.h"

extern "C" {
void ADAPTER_EXPORT_API ChildMain(NativeChildProcess_Args args);
}
#endif // OHOS_ADAPTER_MULTIPROCESS_NATIVE_CHILD_PROCESS_MAIN_H_
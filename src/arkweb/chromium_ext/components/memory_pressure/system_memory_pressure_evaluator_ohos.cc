// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/memory_pressure/system_memory_pressure_evaluator_ohos.h"

namespace memory_pressure {
namespace ohos {

// FIXME: Specific functions will be adapted later.
bool SystemMemoryPressureEvaluator::GetSystemMemoryInfo(
    base::SystemMemoryInfoKB* mem_info) {
  return base::GetSystemMemoryInfo(mem_info);
}

}  // namespace ohos
}  // namespace memory_pressure

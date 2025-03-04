// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_
#define COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_

#include "base/process/process_metrics.h"
#include "components/memory_pressure/system_memory_pressure_evaluator.h"

namespace memory_pressure {
namespace ohos {

// FIXME: Specific functions will be adapted later.
// Ohos memory pressure voter. Because there is no OS provided signal this
// polls at a low frequency, and applies internal hysteresis.
class SystemMemoryPressureEvaluator
    : public memory_pressure::SystemMemoryPressureEvaluator {
 protected:
  // This is just a wrapper for base:: function;
  // declared as virtual for unit testing
  virtual bool GetSystemMemoryInfo(base::SystemMemoryInfoKB* mem_info);
};

}  // namespace ohos
}  // namespace memory_pressure

#endif  // COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <hitrace/trace.h>

#include <type_traits>

#include "base/trace_event/ohos_hitrace_adapter.h"

namespace base::trace_event::ohos {
/** Adapter to direct invoke system api, this adapter is thread safe as
 * `OH_Hitrace_...` API is guaranteed to be thread safe  */
class HitraceSystemAdapter final : public HitraceAdapter {
 public:
  ~HitraceSystemAdapter() override = default;
  void StartSyncTrace(const char* name) override;
  void EndSyncTrace() override;
  void StartAsyncTrace(const char* name, int32_t taskId) override;
  void EndAsyncTrace(const char* name, int32_t taskId) override;
  void CounterTrace(const char* name, int64_t count) override;

 private:
  HitraceSystemAdapter() = default;
  friend HitraceAdapter& GetSystemSingleton();
};

void HitraceSystemAdapter::StartSyncTrace(const char* name) {
  OH_HiTrace_StartTrace(name);
}

void HitraceSystemAdapter::EndSyncTrace() {
  OH_HiTrace_FinishTrace();
}

void HitraceSystemAdapter::StartAsyncTrace(const char* name, int32_t taskId) {
  OH_HiTrace_StartAsyncTrace(name, taskId);
}

void HitraceSystemAdapter::EndAsyncTrace(const char* name, int32_t taskId) {
  OH_HiTrace_FinishAsyncTrace(name, taskId);
}

void HitraceSystemAdapter::CounterTrace(const char* name, int64_t count) {
  OH_HiTrace_CountTrace(name, count);
}

HitraceAdapter& GetSystemSingleton() {
  static HitraceSystemAdapter singleton{};
  return singleton;
}
}  // namespace base::trace_event::ohos
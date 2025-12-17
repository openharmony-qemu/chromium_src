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

#ifndef GPU_HANG_ADAPTER_H
#define GPU_HANG_ADAPTER_H

#include <string>

#include "base/task/task_observer.h"
#include "base/threading/hang_watcher.h"

namespace gpu {

std::string GetProcessName();
void LogGpuHungEvent(int32_t thread_type, int32_t count);
bool IsHangTestEnabled(const std::string& thread_name);
void ReportGpuFreeze();
void SimulateHangForTesting(int32_t& test_count, bool hang_test,
                            const std::string& thread_name);

class GpuHangAdapter : public base::TaskObserver {
public:
  ~GpuHangAdapter();
  static std::unique_ptr<GpuHangAdapter> CreateGpuHangAdapterForGpuMain();
  static std::unique_ptr<GpuHangAdapter> CreateGpuHangAdapterForCompositorGpu();

private:
  GpuHangAdapter(std::string thread_name);
  GpuHangAdapter() = delete;

  static std::unique_ptr<GpuHangAdapter> CreateGpuHangAdapterInner(std::string thread_name);

  // Implements TaskObserver.
  void WillProcessTask(const base::PendingTask& pending_task,
                       bool was_blocked_or_low_priority) override;
  void DidProcessTask(const base::PendingTask& pending_task) override;

  int32_t test_count_ = 0;
  std::string thread_name_;
  std::optional<base::WatchHangsInScope> hang_watch_scope_;
  base::ScopedClosureRunner unregister_for_hang_watching_;
  bool checkHangWatch_ = false;
  bool hang_test_ = false;
};
}
#endif
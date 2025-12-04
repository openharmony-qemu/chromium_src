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

#include "arkweb/chromium_ext/gpu/ipc/service/gpu_hang_adapter.h"

#include "base/memory/ptr_util.h"
#include "base/task/current_thread.h"
#include "base/task/task_observer.h"
#include "base/threading/hang_watcher.h"

#include <fstream>
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace gpu {
constexpr int32_t TEST_LOOP = 1000;
constexpr int32_t TEST_SLEEP = 40;
constexpr int32_t HANG_WATCH_TIME = 5;

std::string GetProcessName() {
  std::ifstream input_file("/proc/self/cmdline");
  if (!input_file.is_open()) {
    LOG(ERROR) << "Error: can not open file";
    return "";
  }

  std::string processName = "";
  if (!std::getline(input_file, processName)) {
    LOG(ERROR) << "Error: Failed to read process name from /proc/self/cmd";
  }
  return processName;
}

void LogGpuHungEvent(int32_t thread_type, int32_t count) {
  LOG(ERROR) << "gpu thread hung detected!! count " << count << " thread_type "
    << static_cast<int32_t>(thread_type);
  auto process_name = GetProcessName();
  auto packageName = OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .GetSystemPropertiesInstance().GetBundleName();
  ReportRenderJsFreeze(getpid(), packageName, process_name, "gpu thread freeze",
    static_cast<int32_t>(getuid()));
}

bool IsHangTestEnabled(const std::string& thread_name) {
  auto& system_properties = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                            .GetSystemPropertiesInstance();
  return system_properties.GetStringParameter("web.gpu.hang.test", "false") == thread_name;
}

void ReportGpuFreeze() {
  constexpr int32_t DUMP_DELAY = 2;
  auto process_name = GetProcessName();
  auto package_name = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetSystemPropertiesInstance().GetBundleName();
  ReportRenderJsFreeze(getpid(), package_name, process_name,
                        "gpu process freeze", static_cast<int32_t>(getuid()));
  sleep(DUMP_DELAY);
}

void SimulateHangForTesting(int32_t& test_count, bool hang_test,
                            const std::string& thread_name) {
  if (hang_test && (test_count++ % TEST_LOOP == 0)) {
    LOG(INFO) << "Hang test on " << thread_name << " for " << TEST_SLEEP;
    sleep(TEST_SLEEP);
  }
}

GpuHangAdapter::GpuHangAdapter(std::string thread_name):thread_name_(thread_name) {
  LOG(INFO) << "GpuHangAdapter::GpuHangAdapter " << thread_name_;
  if (thread_name == "compositor_gpu" &&
    base::HangWatcher::IsCompositorGpuThreadHangWatchingEnabled()) {
      LOG(INFO) << "compositor_gpu init hang_watcher";
      unregister_for_hang_watching_ = base::HangWatcher::RegisterThread(
        base::HangWatcher::ThreadType::kCompositorGpuThread);
      checkHangWatch_ = true;
  }

  if (thread_name == "gpu_main" &&
    base::HangWatcher::IsInProcessGpuThreadHangWatchingEnabled()) {
      LOG(INFO) << "gpu_main init hang_watcher";
      unregister_for_hang_watching_ = base::HangWatcher::RegisterThread(
        base::HangWatcher::ThreadType::kInProcessGpuThread);
      checkHangWatch_ = true;
  }

  base::CurrentThread::Get()->AddTaskObserver(this);

  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  hang_test_ = system_properties_adapter.GetStringParameter("web.gpu.hang.test", "false") == thread_name;
  if (hang_test_) {
    LOG(INFO) << "hang test for thread " << thread_name;
  }
}

GpuHangAdapter::~GpuHangAdapter() {
  LOG(INFO) << "GpuHangAdapter::~GpuHangAdapter " << thread_name_;
  base::CurrentThread::Get()->RemoveTaskObserver(this);
}

// static
std::unique_ptr<GpuHangAdapter> GpuHangAdapter::CreateGpuHangAdapterForGpuMain() {
  return CreateGpuHangAdapterInner("gpu_main");
}

// static
std::unique_ptr<GpuHangAdapter> GpuHangAdapter::CreateGpuHangAdapterForCompositorGpu() {
  return CreateGpuHangAdapterInner("compositor_gpu");
}

// static
std::unique_ptr<GpuHangAdapter> GpuHangAdapter::CreateGpuHangAdapterInner(std::string thread_name) {
  auto adapter = base::WrapUnique(new GpuHangAdapter(thread_name));
  return adapter;
}

void GpuHangAdapter::WillProcessTask(const base::PendingTask& pending_task,
                                        bool was_blocked_or_low_priority) {
  if (checkHangWatch_) {
    hang_watch_scope_.emplace(base::Seconds(HANG_WATCH_TIME));
  }

  if (hang_test_ && (test_count_++ % TEST_LOOP == 0)) {
    LOG(INFO) << "hang test on " << thread_name_ << " for " << TEST_SLEEP;
    sleep(TEST_SLEEP);
  }
}

void GpuHangAdapter::DidProcessTask(const base::PendingTask& pending_task) {
  if (checkHangWatch_) {
    hang_watch_scope_.reset();
  }
}
}
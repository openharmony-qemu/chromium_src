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

#ifndef DFX_APPFREEZE_MONITOR_RENDER_IMPL_H_
#define DFX_APPFREEZE_MONITOR_RENDER_IMPL_H_

#include "arkweb/chromium_ext/content/browser/dfx/mojom/dfx_reporting.mojom.h"
#include "base/task/single_thread_task_executor.h"
#include "mojo/public/cpp/bindings/remote.h"

void ReportRenderFreeze(int32_t pid, const std::string& processName, const std::string& freezeMsg, int32_t uid);

class AppfreezeMonitorImpl {
public:
  AppfreezeMonitorImpl();
  ~AppfreezeMonitorImpl() = default;
  static std::shared_ptr<AppfreezeMonitorImpl> GetInstance();
  void GetRemoteAndSend(int32_t pid, const std::string& processName, const std::string& freezeMsg, int32_t uid);
  void Init();
  bool IsReported() {
    return reported_;
  }
private:
  std::unique_ptr<base::SingleThreadTaskExecutor> task_executor_ =
    std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
  mojo::Remote<dfx::mojom::FreezeReporter> remote_;
  bool reported_ = false;
  bool initialized_ = false;
};
#endif
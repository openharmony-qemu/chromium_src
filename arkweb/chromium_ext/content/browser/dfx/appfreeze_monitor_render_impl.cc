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

#include "appfreeze_monitor_render_impl.h"

#include <fstream>

#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "base/command_line.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/platform.h"

std::string GetProcessName() {
  std::ifstream input_file("/proc/self/cmdline");
  if (!input_file.is_open()) {
    LOG(ERROR) << "Error: Could not open /proc/self/cmdline";
    return "";
  }

  std::string processName = "";
  if (!std::getline(input_file, processName)) {
    LOG(ERROR) << "Error: Failed to read process name from /proc/self/cmdline";
  }
  return processName;
}

void ReportRenderFreeze() {
  std::shared_ptr<AppfreezeMonitorImpl> instance = AppfreezeMonitorImpl::GetInstance();
  if (instance && !instance->IsReported()) {
    instance->GetRemoteAndSend();
  }
}
AppfreezeMonitorImpl::AppfreezeMonitorImpl() {
  Init();
}
void AppfreezeMonitorImpl::Init() {
  if (initialized_) {
    return;
  }
  if (blink::Platform::Current() && blink::Platform::Current()->GetBrowserInterfaceBroker()) {
    blink::Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
      remote_.BindNewPipeAndPassReceiver());
    initialized_ = true;
  } else {
    LOG(ERROR) << "Init FreezeReporter failed!";
    return;
  }
}

std::shared_ptr<AppfreezeMonitorImpl> AppfreezeMonitorImpl::GetInstance() {
  static std::shared_ptr<AppfreezeMonitorImpl> instance = std::make_shared<AppfreezeMonitorImpl>();
  return instance;
}

void AppfreezeMonitorImpl::GetRemoteAndSend() {
  if (remote_.is_bound()) {
    dfx::mojom::FreezeInfoPtr freezeInfoPtr = dfx::mojom::FreezeInfo::New(base::GetCurrentRealPid(), GetProcessName(),
      "render freeze", static_cast<int32_t>(getuid()));
    remote_->ReportRenderFreeze(std::move(freezeInfoPtr));
    reported_ = true;
  }
}
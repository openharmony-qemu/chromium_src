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

#include "arkweb/chromium_ext/content/browser/dfx/appfreeze_monitor_render_impl.h"
#include <mutex>

std::mutex lock;

void ReportRenderFreeze() {
  std::lock_guard<std::mutex> locks(lock);
  std::shared_ptr<AppfreezeMonitorImpl> appfreezeMonitorImpl = AppfreezeMonitorImpl::GetInstance();
  if (appfreezeMonitorImpl->reported_) {
    return;
  }
  appfreezeMonitorImpl->GetRemoteAndSend("PROCESS_FREEZE_WARNING");
  appfreezeMonitorImpl->reported_ = true;
}

std::shared_ptr<AppfreezeMonitorImpl> AppfreezeMonitorImpl::GetInstance() {
  static std::shared_ptr<AppfreezeMonitorImpl> instance = std::make_shared<AppfreezeMonitorImpl>();
  return instance;
}
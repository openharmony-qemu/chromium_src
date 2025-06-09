/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/power_monitor/power_monitor.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter {
void PowerMonitor::SetMonitor(OhosPowerMonitor* monitor) {
  if (monitor_ == nullptr) {
    monitor_ = monitor;
  }
}
void PowerMonitor::ReleaseMonitor() {
  if (monitor_ != nullptr) {
    monitor_ = nullptr;
  }
}

void PowerMonitor::OnSuspend() {
  if (monitor_ != nullptr) {
    monitor_->OnSuspend();
  }
}
void PowerMonitor::OnResume() {
  if (monitor_ != nullptr) {
    monitor_->OnResume();
  }
}

void PowerMonitor::OnPowerStateChanged() {
  if (monitor_ != nullptr) {
    monitor_->OnPowerStateChanged();
  }
}

bool PowerMonitor::IsOnBatteryPower() {
  if (auto func = ohos::adapter::GetJSFunction("PowerMonitor.IsOnBatteryPower")) {
    return func->Invoke<bool>();
  }
  return false;
}

JSBIND_CLASS(PowerMonitor) {
  JSBIND_METHOD(OnSuspend);
  JSBIND_METHOD(OnResume);
  JSBIND_METHOD(OnPowerStateChanged);
}
}  // namespace ohos::adapter

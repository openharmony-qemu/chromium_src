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

#include "ohos/adapter/device_user_auth/device_user_auth.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::device_user_auth {

// static
DeviceUserAuthAdapter& DeviceUserAuthAdapter::GetInstance() {
  static DeviceUserAuthAdapter instance;
  return instance;
}

DeviceUserAuthAdapter::BiometricCheckResult
DeviceUserAuthAdapter::CheckBiometricAvailable() {
  std::promise<BiometricCheckResult> promise;
  std::future<BiometricCheckResult> future = promise.get_future();
  std::function<void(bool, int)> callback =
      [&promise](bool can_biom, int biom_type) {
        promise.set_value({can_biom, biom_type});
      };
  auto func = ohos::adapter::GetJSFunction(
      "DeviceUserAuthAdapter.CheckBiometricAvailable");
  if (func) {
    func->Invoke<void>(callback);
    return future.get();
  }
  LOGE("[DeviceUserAuthAdapter] CheckBiometricAvailable function not found");
  return {false, -1};
}

bool DeviceUserAuthAdapter::StartUserAuth(const std::string& prompt_string,
                                          bool enable_biom,
                                          int biom_type) {
  std::promise<bool> promise;
  std::function<void(bool)> callback = [&promise](bool ret_value) -> void {
    promise.set_value(ret_value);
  };
  auto func =
      ohos::adapter::GetJSFunction("DeviceUserAuthAdapter.StartUserAuth");
  if (func) {
    func->Invoke<void>(prompt_string, callback, enable_biom, biom_type);
    bool result = promise.get_future().get();
    return result;
  }
  LOGE("[DeviceUserAuthAdapter] StartUserAuth function not found");
  return false;
}

}  // namespace ohos::adapter::device_user_auth

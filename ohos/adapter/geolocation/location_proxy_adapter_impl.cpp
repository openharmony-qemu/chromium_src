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

#include "location_proxy_adapter_impl.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "location_adapter.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"

namespace ohos::adapter {
namespace {
bool RequestPermission() {
  return permission::PermissionManagerAdapter::RequestPermission(
      permission::OHOSPermissionType::LOCATION);
}
}  // namespace

bool LocationProxyAdapter::StartListening(
    std::shared_ptr<LocationCallbackAdapter> location_callback,
    bool high_accuracy) {
  if (!location_callback) {
    LOGE("[LocationProxyAdapter] location_callback is null");
    return false;
  }
  if (!RequestPermission()) {
    LOGE("[LocationProxyAdapter] requestPermission denied");
    return false;
  }
  auto start_listening =
      ohos::adapter::GetJSFunction("GeolocationAdapter.StartListening");
  if (!start_listening) {
    LOGE("[LocationProxyAdapter] GetJSFunction failed");
    return false;
  }
  std::function<void(aki::Value)> on_location_report =
      [=](aki::Value location) {
        std::unique_ptr<LocationInfo> location_info =
            std::make_unique<LocationInfo>(location);
        location_callback->OnLocationReport(location_info);
      };
  std::function<void(int)> on_error_report = [=](int errcode) {
    location_callback->OnErrorReport(errcode);
  };
  start_listening->Invoke<void>(on_location_report, on_error_report,
                                high_accuracy);
  return true;
}

bool LocationProxyAdapter::StopListening(
    std::shared_ptr<LocationCallbackAdapter> callback) {
  if (auto stop_listening =
          ohos::adapter::GetJSFunction("GeolocationAdapter.StopListening")) {
    stop_listening->Invoke<void>();
  }
  return true;
}
}  // namespace ohos::adapter

/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "vibration_manager_ohos.h"

#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

using namespace OHOS::NWeb;
namespace device {

VibrationManagerOhos::VibrationManagerOhos(
    mojo::PendingRemote<mojom::VibrationManagerListener> listener)
    : VibrationManagerImpl(std::move(listener)) {}

VibrationManagerOhos::~VibrationManagerOhos() = default;

void VibrationManagerOhos::PlatformVibrate(int64_t milliseconds) {
  auto vibratorAdapter = OhosAdapterHelperExt::CreateVibratorAdapter();
  if (vibratorAdapter == nullptr) {
    LOG(ERROR) << "VibrationManagerOhos: Failed to create VibratorAdapter";
    return;
  }

  int32_t ret = vibratorAdapter->Vibrate(static_cast<int32_t>(milliseconds));
  if (ret != 0) {
    LOG(ERROR) << "VibrationManagerOhos: Vibrate failed with error code: " << ret;
  }
}

void VibrationManagerOhos::PlatformCancel() {
  auto vibratorAdapter = OhosAdapterHelperExt::CreateVibratorAdapter();
  if (vibratorAdapter == nullptr) {
    LOG(ERROR) << "VibrationManagerOhos: Failed to create VibratorAdapter";
    return;
  }

  int32_t ret = vibratorAdapter->Cancel();
  if (ret != 0) {
    LOG(ERROR) << "VibrationManagerOhos: Cancel failed with error code: " << ret;
  }
}

void VibrationManagerOhos::Create(
    mojo::PendingReceiver<mojom::VibrationManager> receiver,
    mojo::PendingRemote<mojom::VibrationManagerListener> listener) {
  mojo::MakeSelfOwnedReceiver(
      std::make_unique<VibrationManagerOhos>(std::move(listener)),
      std::move(receiver));
}

}  // namespace device

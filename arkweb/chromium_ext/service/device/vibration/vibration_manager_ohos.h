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

#ifndef ARKWEB_CHROMIUM_EXT_SERVICE_DEVICE_VIBRATION_VIBRATION_MANAGER_OHOS_H_
#define ARKWEB_CHROMIUM_EXT_SERVICE_DEVICE_VIBRATION_VIBRATION_MANAGER_OHOS_H_

#include "services/device/vibration/vibration_manager_impl.h"

namespace device {

class VibrationManagerOhos : public VibrationManagerImpl {
 public:
  static void Create(
      mojo::PendingReceiver<mojom::VibrationManager> receiver,
      mojo::PendingRemote<mojom::VibrationManagerListener> listener);

  explicit VibrationManagerOhos(
      mojo::PendingRemote<mojom::VibrationManagerListener> listener);
  VibrationManagerOhos(const VibrationManagerOhos&) = delete;
  VibrationManagerOhos& operator=(const VibrationManagerOhos&) = delete;
  ~VibrationManagerOhos() override;

 protected:
  void PlatformVibrate(int64_t milliseconds) override;
  void PlatformCancel() override;
};

}  // namespace device

#endif  // ARKWEB_CHROMIUM_EXT_SERVICE_DEVICE_VIBRATION_VIBRATION_MANAGER_OHOS_H_

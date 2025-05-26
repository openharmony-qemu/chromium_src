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

#include "arkweb_device_data_manager_utils.h"

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "ui/events/devices/device_data_manager.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

namespace ui {

ArkWebDeviceDataManagerUtils::ArkWebDeviceDataManagerUtils(
    DeviceDataManager* device_data_manger)
    : device_data_manager_(device_data_manger) {
  DCHECK(device_data_manger);
}

ArkWebDeviceDataManagerUtils::~ArkWebDeviceDataManagerUtils() = default;

namespace {

bool InputDeviceEqualsEx(const ui::InputDevice& a, const ui::InputDevice& b) {
  return a.id == b.id && a.enabled == b.enabled &&
         a.suspected_keyboard_imposter == b.suspected_keyboard_imposter &&
         a.suspected_mouse_imposter == b.suspected_mouse_imposter;
}

constexpr uint32_t TAG_KEYBOARD_TYPE = (1 << 1);
constexpr uint32_t TAG_MOUSE_TYPE = (1 << 2);
constexpr uint32_t TAG_TOUCHPAD_TYPE = (1 << 3);
const std::string CHANGED_TYPE = "change";
const std::string IGNORE_MOUSE_DEVICE_NAME = "hw_fingerprint_mouse";

class MMIListenerAdapterImpl : public OHOS::NWeb::MMIListenerAdapter {
 public:
  MMIListenerAdapterImpl(
      ArkWebDeviceDataManagerUtils* utils,
      const scoped_refptr<base::SequencedTaskRunner>& sequenced_task_runner)
      : utils_(utils),
        mmi_adapter_(
            OHOS::NWeb::OhosAdapterHelperExt::GetInstance().CreateMMIAdapter()),
        sequenced_task_runner_(sequenced_task_runner) {}
  ~MMIListenerAdapterImpl() override = default;

  void OnDeviceAdded(int32_t deviceId, const std::string& type) override {
    if (!ui::DeviceDataManager::HasInstance()) {
      return;
    }

    std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapterImpl> adapter =
        std::make_shared<OHOS::NWeb::MMIDeviceInfoAdapterImpl>();

    mmi_adapter_->GetDeviceInfo(deviceId, adapter);
    if (!sequenced_task_runner_) {
      LOG(ERROR) << "OnDeviceAdded sequenced_task_runner is null";
      return;
    }

    OHOS::NWeb::MMIDeviceInfo info = TransformToMMIDeviceInfo(adapter);
    if ((info.name.find(IGNORE_MOUSE_DEVICE_NAME) != std::string::npos) &&
        (info.type & TAG_MOUSE_TYPE)) {
      LOG(INFO) << "OnDeviceAdded ignore this mouse device";
      return;
    }
    sequenced_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(
            [](const OHOS::NWeb::MMIDeviceInfo& info,
               ArkWebDeviceDataManagerUtils* utils) {
              ui::InputDevice device(
                  info.id, ui::InputDeviceType::INPUT_DEVICE_USB, info.name);
              ui::TouchpadDevice touchpadDevice(
                  info.id, ui::InputDeviceType::INPUT_DEVICE_USB, info.name);
              ui::KeyboardDevice keyboardDevice(
                  info.id, ui::InputDeviceType::INPUT_DEVICE_USB, info.name);
              if (info.type & TAG_MOUSE_TYPE) {
                utils->AddMouseDevice(device);
              }
              if (info.type & TAG_TOUCHPAD_TYPE) {
                utils->AddTouchpadDevice(touchpadDevice);
              }
              if (info.type & TAG_KEYBOARD_TYPE) {
                utils->AddKeyboardDevice(keyboardDevice);
              }
            },
            info, utils_));
  }

  void OnDeviceRemoved(int32_t deviceId, const std::string& type) override {
    if (ui::DeviceDataManager::HasInstance()) {
      if (!sequenced_task_runner_) {
        LOG(ERROR) << "OnDeviceRemoved sequenced_task_runner is null";
        return;
      }
      sequenced_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(
              [](int32_t deviceId, ArkWebDeviceDataManagerUtils* utils) {
                ui::InputDevice device(
                    deviceId, ui::InputDeviceType::INPUT_DEVICE_USB, "");
                utils->DeleteDevice(device);
              },
              deviceId, utils_));
    }
  }

 private:
  ArkWebDeviceDataManagerUtils* utils_;
  std::unique_ptr<OHOS::NWeb::MMIAdapter> mmi_adapter_;
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_;
};
}  // namespace

void ArkWebDeviceDataManagerUtils::AddKeyboardDevice(
    const KeyboardDevice& device) {
  for (auto item = device_data_manager_->keyboard_devices_.begin();
       item != device_data_manager_->keyboard_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      return;
    }
  }
  LOG(INFO) << "DeviceDataManager add keyboard device id: " << device.id;
  device_data_manager_->keyboard_devices_.push_back(device);
  device_data_manager_->NotifyObserversKeyboardDeviceConfigurationChanged();
}

void ArkWebDeviceDataManagerUtils::AddMouseDevice(const InputDevice& device) {
  for (auto item = device_data_manager_->mouse_devices_.begin();
       item != device_data_manager_->mouse_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      return;
    }
  }
  LOG(INFO) << "DeviceDataManager add mouse device id: " << device.id;
  device_data_manager_->mouse_devices_.push_back(device);
  device_data_manager_->NotifyObserversMouseDeviceConfigurationChanged();
}

void ArkWebDeviceDataManagerUtils::AddTouchpadDevice(
    const TouchpadDevice& device) {
  for (auto item = device_data_manager_->touchpad_devices_.begin();
       item != device_data_manager_->touchpad_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      return;
    }
  }
  LOG(INFO) << "DeviceDataManager add touchpad device id: " << device.id;
  device_data_manager_->touchpad_devices_.push_back(device);
  device_data_manager_->NotifyObserversTouchpadDeviceConfigurationChanged();
}

void ArkWebDeviceDataManagerUtils::DeleteDevice(const InputDevice& device) {
  for (auto item = device_data_manager_->mouse_devices_.begin();
       item != device_data_manager_->mouse_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      device_data_manager_->mouse_devices_.erase(item);
      LOG(INFO) << "DeviceDataManager remove mouse device id: " << device.id;
      device_data_manager_->NotifyObserversMouseDeviceConfigurationChanged();
      break;
    }
  }
  for (auto item = device_data_manager_->touchpad_devices_.begin();
       item != device_data_manager_->touchpad_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      device_data_manager_->touchpad_devices_.erase(item);
      LOG(INFO) << "DeviceDataManager remove touchpad device id: " << device.id;
      device_data_manager_->NotifyObserversTouchpadDeviceConfigurationChanged();
      break;
    }
  }
  for (auto item = device_data_manager_->keyboard_devices_.begin();
       item != device_data_manager_->keyboard_devices_.end(); ++item) {
    if (InputDeviceEqualsEx(*item, device)) {
      device_data_manager_->keyboard_devices_.erase(item);
      LOG(INFO) << "DeviceDataManager remove keyboard device id: " << device.id;
      device_data_manager_->NotifyObserversKeyboardDeviceConfigurationChanged();
      return;
    }
  }
}

void ArkWebDeviceDataManagerUtils::SetupDeviceListeners() {
  auto& helper = OHOS::NWeb::OhosAdapterHelperExt::GetInstance();
  mmi_adapter_ = helper.CreateMMIAdapter();
  if (mmi_adapter_ == nullptr) {
    LOG(ERROR) << "DeviceDataManager mmi_adapter_ is nullptr";
    return;
  }

  sequenced_task_runner_ =
      content::GetUIThreadTaskRunner({});
  if (!sequenced_task_runner_) {
    LOG(ERROR) << "DeviceDataManager GetUIThreadTaskRunner is null";
    return;
  }

  dev_listener_ =
      std::make_shared<MMIListenerAdapterImpl>(
          this, sequenced_task_runner_);
  mmi_adapter_->RegisterDevListener(
      CHANGED_TYPE, dev_listener_);

  std::vector<int32_t> device_ids;
  mmi_adapter_->GetDeviceIds(device_ids);
  for (auto id : device_ids) {
    std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapterImpl> adapter =
        std::make_shared<OHOS::NWeb::MMIDeviceInfoAdapterImpl>();
    mmi_adapter_->GetDeviceInfo(id, adapter);
    OHOS::NWeb::MMIDeviceInfo info = TransformToMMIDeviceInfo(adapter);
    if ((info.name.find(IGNORE_MOUSE_DEVICE_NAME) != std::string::npos) &&
        (info.type & TAG_MOUSE_TYPE)) {
      LOG(INFO) << "DeviceDataManager ignore this mouse device";
      continue;
    }

    auto addMMIDeviceInfoFunction =
        [](const OHOS::NWeb::MMIDeviceInfo& deviceInfo,
           ArkWebDeviceDataManagerUtils* utils) {
          ui::InputDevice device(deviceInfo.id,
                                 ui::InputDeviceType::INPUT_DEVICE_USB,
                                 deviceInfo.name);
          ui::TouchpadDevice touchpadDevice(
              deviceInfo.id, ui::InputDeviceType::INPUT_DEVICE_USB,
              deviceInfo.name);
          ui::KeyboardDevice keyboardDevice(
              deviceInfo.id, ui::InputDeviceType::INPUT_DEVICE_USB,
              deviceInfo.name);
          if (deviceInfo.type & TAG_MOUSE_TYPE) {
            utils->AddMouseDevice(device);
          }
          if (deviceInfo.type & TAG_TOUCHPAD_TYPE) {
            utils->AddTouchpadDevice(touchpadDevice);
          }
          if (deviceInfo.type & TAG_KEYBOARD_TYPE) {
            utils->AddKeyboardDevice(keyboardDevice);
          }
        };
    if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
      sequenced_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(addMMIDeviceInfoFunction, info,
                                    base::Unretained(this)));
    } else {
      addMMIDeviceInfoFunction(info, this);
    }
  }
}

void ArkWebDeviceDataManagerUtils::CleanupDeviceListeners() {
  if (mmi_adapter_ != nullptr) {
    mmi_adapter_->UnregisterDevListener(CHANGED_TYPE);
    dev_listener_ = nullptr;
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

}  // namespace ui

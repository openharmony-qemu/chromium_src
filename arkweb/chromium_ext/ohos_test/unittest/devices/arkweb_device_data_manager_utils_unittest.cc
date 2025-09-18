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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "ui/events/devices/input_device.h"
#define private public
#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/ui/events/devices/arkweb_device_data_manager_utils.cc"
#include "arkweb/chromium_ext/ui/events/devices/arkweb_device_data_manager_utils.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "base/scoped_observation.h"
#include "base/test/task_environment.h"
#include "base/test/test_mock_time_task_runner.h"
#include "content/browser/scheduler/browser_io_thread_delegate.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/browser/scheduler/browser_task_priority.h"
#include "content/browser/scheduler/browser_ui_thread_scheduler.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/mmi_adapter.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/devices/input_device_event_observer.h"
#include "ui/events/devices/keyboard_device.h"

using namespace content;
using namespace OHOS::NWeb;
using testing::SetArgReferee;
using testing::Return;
using testing::_;
using testing::DoAll;

namespace ui {
class ArkWebDeviceDataManagerUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    auto ui_sequence_manager_ = base::sequence_manager::CreateUnboundSequenceManager(base::sequence_manager::SequenceManager::Settings::Builder().SetPrioritySettings(content::internal::CreateBrowserTaskPrioritySettings()).Build());
    auto browser_ui_thread_scheduler = content::BrowserUIThreadScheduler::CreateForTesting(ui_sequence_manager_.get());
    content::BrowserTaskExecutor::CreateForTesting(std::move(browser_ui_thread_scheduler), std::make_unique<content::BrowserIOThreadDelegate>());
    DeviceDataManager::CreateInstance();
    device_data_manager_ = DeviceDataManager::GetInstance();
    utils_ = std::make_unique<ArkWebDeviceDataManagerUtils>(device_data_manager_);
  }
  void TearDown() override {
    DeviceDataManager::DeleteInstance();
    utils_.reset();
    device_data_manager_ = nullptr;
  }
  raw_ptr<DeviceDataManager> device_data_manager_;
  std::unique_ptr<ArkWebDeviceDataManagerUtils> utils_;
};

class MockMMIAdapter : public MMIAdapter {
public:
    MOCK_METHOD(char*, KeyCodeToString, (int32_t keyCode), (override));
    MOCK_METHOD(int32_t, RegisterMMIInputListener, (std::shared_ptr<MMIInputListenerAdapter> eventCallback), (override));
    MOCK_METHOD(void, UnregisterMMIInputListener, (int32_t monitorId), (override));
    MOCK_METHOD(int32_t, RegisterDevListener, (std::string type, std::shared_ptr<MMIListenerAdapter> listener), (override));
    MOCK_METHOD(int32_t, UnregisterDevListener, (std::string type), (override));
    MOCK_METHOD(int32_t, GetKeyboardType, (int32_t deviceId, int32_t& type), (override));
    MOCK_METHOD(int32_t, GetDeviceIds, (std::vector<int32_t>& ids), (override));
    MOCK_METHOD(int32_t, GetDeviceInfo, (int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info), (override));
    MOCK_METHOD(int32_t, GetMaxTouchPoints, (), (override));
};

class FakeHelperExt : public OHOS::NWeb::OhosAdapterHelperExt {
public:
    static MockMMIAdapter* g_mockAdapter;
    std::unique_ptr<OHOS::NWeb::MMIAdapter> CreateMMIAdapter() {
        if (g_mockAdapter) {
            return std::unique_ptr<OHOS::NWeb::MMIAdapter>(g_mockAdapter);
        }
        return nullptr;
    }
};
MockMMIAdapter* FakeHelperExt::g_mockAdapter = nullptr;

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddMouseDevice001) {
  InputDevice test_mouse(2, InputDeviceType::INPUT_DEVICE_USB, "mouse1");
  utils_->AddMouseDevice(test_mouse);
  EXPECT_FALSE(device_data_manager_->mouse_devices_.empty());
  EXPECT_EQ(device_data_manager_->mouse_devices_.front().id, test_mouse.id);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddMouseDevice002) {
  InputDevice test_mouse(2, InputDeviceType::INPUT_DEVICE_USB, "mouse1");
  utils_->AddMouseDevice(test_mouse);
  utils_->DeleteDevice(test_mouse);
  EXPECT_TRUE(device_data_manager_->mouse_devices_.empty());
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddKeyboardDevice001) {
  KeyboardDevice test_keyboard(1, InputDeviceType::INPUT_DEVICE_USB, "kbd1");
  utils_->AddKeyboardDevice(test_keyboard);
  EXPECT_FALSE(device_data_manager_->keyboard_devices_.empty());
  EXPECT_EQ(device_data_manager_->keyboard_devices_.front().id, test_keyboard.id);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddKeyboardDevice002) {
  KeyboardDevice test_keyboard(1, InputDeviceType::INPUT_DEVICE_USB, "kbd1");
  utils_->AddKeyboardDevice(test_keyboard);
  utils_->DeleteDevice(test_keyboard);
  EXPECT_TRUE(device_data_manager_->mouse_devices_.empty());
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddTouchpadDevice001) {
  TouchpadDevice test_touchpad(3, InputDeviceType::INPUT_DEVICE_USB, "tp1");
  utils_->AddTouchpadDevice(test_touchpad);
  EXPECT_FALSE(device_data_manager_->touchpad_devices_.empty());
  EXPECT_EQ(device_data_manager_->touchpad_devices_.front().id, test_touchpad.id);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, TestAddTouchpadDevice002) {
  TouchpadDevice test_touchpad(3, InputDeviceType::INPUT_DEVICE_USB, "tp1");
  utils_->AddTouchpadDevice(test_touchpad);
  utils_->DeleteDevice(test_touchpad);
  EXPECT_TRUE(device_data_manager_->touchpad_devices_.empty());
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, DeleteDevice_NotFound) {
  InputDevice device(99, InputDeviceType::INPUT_DEVICE_USB, "unknown");
  utils_->DeleteDevice(device);
  EXPECT_TRUE(device_data_manager_->keyboard_devices_.empty());
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, SetupDeviceListeners_MMIAdapterNull) {
  FakeHelperExt::g_mockAdapter = nullptr;
  utils_->SetupDeviceListeners();
  EXPECT_NE(utils_->mmi_adapter_, nullptr);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, SetupDeviceListeners_Normal) {
  auto mock = new MockMMIAdapter();
  testing::Mock::AllowLeak(mock);
  FakeHelperExt::g_mockAdapter = mock;
  std::vector<int32_t> ids = {1, 2};
  EXPECT_CALL(*mock, GetDeviceIds(_)).WillOnce(DoAll(SetArgReferee<0>(ids), Return(0)));
  EXPECT_CALL(*mock, GetDeviceInfo(_, _)).Times(2);
  EXPECT_CALL(*mock, RegisterDevListener(_, _)).Times(1);
  utils_->SetupDeviceListeners();
  EXPECT_NE(utils_->mmi_adapter_, nullptr);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, SetupDeviceListeners_IgnoreMouse) {
  auto mock = new MockMMIAdapter();
  testing::Mock::AllowLeak(mock);
  FakeHelperExt::g_mockAdapter = mock;
  std::vector<int32_t> ids = {5};
  EXPECT_CALL(*mock, GetDeviceIds(_)).WillOnce(DoAll(SetArgReferee<0>(ids), Return(0)));
  EXPECT_CALL(*mock, GetDeviceInfo(_, _)).WillOnce([](int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info) {
      info->SetId(5);
      info->SetType((1 << 2));
      info->SetName("hw_fingerprint_mouse");
      return 0;
  });
  EXPECT_CALL(*mock, RegisterDevListener(_, _)).Times(1);
  utils_->SetupDeviceListeners();
  EXPECT_NE(utils_->mmi_adapter_, nullptr);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, CleanupDeviceListeners_WithAdapter) {
  auto mock = std::make_unique<MockMMIAdapter>();
  testing::Mock::AllowLeak(mock.get());
  EXPECT_CALL(*mock, UnregisterDevListener(_)).Times(1);
  utils_->mmi_adapter_ = std::move(mock);
  utils_->CleanupDeviceListeners();
  EXPECT_EQ(utils_->dev_listener_, nullptr);
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, CleanupDeviceListeners_NullAdapter) {
  utils_->mmi_adapter_ = nullptr;
  utils_->CleanupDeviceListeners();
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, OnDeviceAdded_NoInstance) {
  scoped_refptr<base::SequencedTaskRunner> task_runner = nullptr;
  auto listener = std::make_shared<MMIListenerAdapterImpl>(
      utils_.get(), task_runner);
  listener->OnDeviceAdded(1, "mouse");
  listener.reset();
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, OnDeviceAdded_NullTaskRunner) {
  auto listener = std::make_shared<MMIListenerAdapterImpl>(
      utils_.get(), nullptr);
  listener->OnDeviceAdded(2, "mouse");
  listener.reset();
}

TEST_F(ArkWebDeviceDataManagerUtilsTest, OnDeviceRemoved_Normal) {
  auto runner = content::GetUIThreadTaskRunner({});
  auto listener = std::make_shared<MMIListenerAdapterImpl>(
      utils_.get(), runner);
  listener->OnDeviceRemoved(3, "mouse");
  listener.reset();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
} // namespace ui
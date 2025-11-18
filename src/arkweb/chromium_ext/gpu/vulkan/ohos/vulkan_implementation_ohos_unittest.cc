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

#include "gpu/vulkan/ohos/vulkan_implementation_ohos.h"
#include "gpu/vulkan/vulkan_device_queue.h"
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "gpu/vulkan/vulkan_ycbcr_info.h"
#include "base/logging.h"


namespace gpu {

class VulkanImplementationOhosTest : public testing::Test {
protected:
  void SetUp() override {
    impl_ = std::make_unique<VulkanImplementationOhos>();
  }
  std::unique_ptr<VulkanImplementationOhos> impl_;
  raw_ptr<gpu::VulkanImplementation> vulkan_implementation_;
};

TEST_F(VulkanImplementationOhosTest, InitializeVulkanInstance) {
  EXPECT_TRUE(impl_->InitializeVulkanInstance(true));
  EXPECT_NE(impl_->GetVulkanInstance(), nullptr);
}

TEST_F(VulkanImplementationOhosTest, CreateViewSurface) {
  ASSERT_TRUE(impl_->InitializeVulkanInstance(true));
  EXPECT_EQ(impl_->CreateViewSurface(0), nullptr);
  auto* tracker = NWebNativeWindowTracker::GetInstance();
  void* fake_window = reinterpret_cast<void*>(0x1234);
  int32_t window_id = tracker->AddNativeWindow(nullptr);
  
  auto surface = impl_->CreateViewSurface(static_cast<gfx::AcceleratedWidget>(window_id));
  EXPECT_EQ(surface, nullptr);

  tracker->DestroyNativeWindow(window_id);
}

TEST_F(VulkanImplementationOhosTest, DeviceExtensions) {
  EXPECT_TRUE(impl_->GetRequiredDeviceExtensions().empty());
  EXPECT_FALSE(impl_->GetOptionalDeviceExtensions().empty());
}

TEST_F(VulkanImplementationOhosTest, GetPhysicalDevicePresentationSupport) {
  EXPECT_TRUE(impl_->GetPhysicalDevicePresentationSupport(
      VK_NULL_HANDLE, {}, 0));
}

}

/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#include "base/memory/singleton.h"

namespace gpu {

class GpuSurfaceIdTrackerTest : public testing::Test {
  protected:
    void SetUp() {
        tracker_ = GpuSurfaceIdTracker::GetInstance();
    }

    int AddTestSurface(gfx::AcceleratedWidget widget,
                       const std::string& surface_id) {
        std::string non_const_id = surface_id;
        GpuSurfaceIdTracker::SurfaceRecord record(widget, non_const_id);
        return tracker_->AddSurfaceForNativeWidget(std::move(record));
    }

    GpuSurfaceIdTracker* tracker_;
};

TEST_F(GpuSurfaceIdTrackerTest, AcquireNativeImageSurfaceId_ValidHandle) {
    const std::string kTestId = "test_surface_id";
    int handle = AddTestSurface(123, kTestId);

    std::string result = tracker_->AcquireNativeImageSurfaceId(handle);
    EXPECT_EQ(result, kTestId);
    EXPECT_EQ(tracker_->GetSurfaceCount(), 1u);
}

TEST_F(GpuSurfaceIdTrackerTest, AcquireNativeImageSurfaceId_InvalidHandle) {
    std::string result = tracker_->AcquireNativeImageSurfaceId(999);
    EXPECT_TRUE(result.empty());
}

TEST_F(GpuSurfaceIdTrackerTest, AcquireNativeImageSurfaceId_EmptyMap) {
    ASSERT_EQ(tracker_->GetSurfaceCount(), 1u);
    std::string result = tracker_->AcquireNativeImageSurfaceId(1);
    EXPECT_FALSE(result.empty());
}

TEST_F(GpuSurfaceIdTrackerTest, AcquireNativeImageSurfaceId_AfterRemoval) {
    const std::string kTestId = "test_surface_id";
    int handle = AddTestSurface(123, kTestId);
    tracker_->RemoveSurface(handle);

    std::string result = tracker_->AcquireNativeImageSurfaceId(handle);
    EXPECT_TRUE(result.empty());
    EXPECT_EQ(tracker_->GetSurfaceCount(), 1u);
}

} // namespace gpu
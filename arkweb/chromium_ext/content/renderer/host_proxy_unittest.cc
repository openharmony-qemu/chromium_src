// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB)
#include "content/renderer/host_proxy.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace content {

// Unable to use real Windows classes in UT testing
// Simulate Windows interface classes
class VoidClassTest {
 public:
  VoidClassTest* Get() { return this; }
};

class HostProxyTest : public testing::Test {
 public:
  HostProxyTest() = default;

  HostProxyTest(const HostProxyTest&) = delete;
  HostProxyTest& operator=(const HostProxyTest&) = delete;
  void SetUp() override {}
};

TEST_F(HostProxyTest, GetSurfaceFromKernelAddZeroId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = 0;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_NE(window, nullptr);
}

TEST_F(HostProxyTest, GetSurfaceFromKernelAddExtremeId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = -1;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_NE(window, nullptr);
}

TEST_F(HostProxyTest, GetSurfaceFromKernelAddNormalId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = 100;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_NE(window, nullptr);
}

TEST_F(HostProxyTest, DestroySurfaceFromKernelZeroId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = 0;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  host->DestroySurfaceFromKernel(surface_id);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_EQ(window, nullptr);
}

TEST_F(HostProxyTest, DestroySurfaceFromKernelExtremeId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = -1;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  host->DestroySurfaceFromKernel(surface_id);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_EQ(window, nullptr);
}

TEST_F(HostProxyTest, DestroySurfaceFromKernelNormalId) {
  auto host = new HostProxy();
  ASSERT_NE(host, nullptr);
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto nativewindow = VoidClassTest().Get();
  int32_t surface_id = 100;
  surface_id = nativeWindowTrackerPtr->AddNativeWindow(nativewindow);
  host->DestroySurfaceFromKernel(surface_id);
  auto window = host->GetSurfaceFromKernel(surface_id);
  EXPECT_EQ(window, nullptr);
}

}  // namespace content
#endif

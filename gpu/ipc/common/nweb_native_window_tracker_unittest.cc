// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)
#define private public
#include "nweb_native_window_tracker.h"
#undef private
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

namespace {

// Unable to use real Windows classes in UT testing
// Simulate Windows interface classes
class VoidClassTest {
 public:
  VoidClassTest* Get() { return this; }
};

class AafwkBrowserClientAdapterTest
    : public OHOS::NWeb::AafwkBrowserClientAdapter {
 public:
  explicit AafwkBrowserClientAdapterTest() { vct_ = VoidClassTest().Get(); }

  ~AafwkBrowserClientAdapterTest() override = default;

  // Simulate obtaining windows from browser processes
  void* QueryRenderSurface(int32_t surface_id) override { return vct_; }

  void ReportThread(OHOS::NWeb::ResSchedStatusAdapter status,
                    int32_t process_id,
                    int32_t thread_id,
                    OHOS::NWeb::ResSchedRoleAdapter role) override {}

  void PassSurface(int64_t surface_id) override {}

  void DestroyRenderSurface(int32_t surface_id) override {
    vct_ = nullptr;

    // Pointer needs to be cleared to prevent adding new members when calling
    // GetNativeWindow
    NWebNativeWindowTracker::GetInstance()->g_browser_client_ = nullptr;
  }

 private:
  VoidClassTest* vct_ = nullptr;
};

class NWebNativeWindowTrackerTest : public testing::Test {
 public:
  NWebNativeWindowTrackerTest() = default;

  NWebNativeWindowTrackerTest(const NWebNativeWindowTrackerTest&) = delete;
  NWebNativeWindowTrackerTest& operator=(const NWebNativeWindowTrackerTest&) =
      delete;
};
}  // namespace

TEST_F(NWebNativeWindowTrackerTest, GetInstance) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  EXPECT_NE(nativeWindowTrackerPtr, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, AddNativeWindow_NotNull) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto window = VoidClassTest().Get();
  auto native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 1);
}

TEST_F(NWebNativeWindowTrackerTest, AddNativeWindow_Null) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto window = nullptr;
  auto native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 2);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_NotAddNormalId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 100;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_NotAddExtremeId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = -1;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_NotAddZeroId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 0;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_Add) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto window = VoidClassTest().Get();
  int32_t native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  // The native_findow_id of the AddNativeWindow method will automatically
  // increment
  EXPECT_EQ(native_window_id, 3);
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_NE(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_HasBrowserClientNormalId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  nativeWindowTrackerPtr->g_browser_client_ =
      std::make_shared<AafwkBrowserClientAdapterTest>();
  int32_t native_window_id = 100;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // If d_browser_client_ is not null ptr，
  // the window will be obtained from the browser process by calling the binder
  // (sync call)
  EXPECT_NE(nativeWindow, nullptr);
  nativeWindowTrackerPtr->g_browser_client_ = nullptr;
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_HasBrowserClientExtremeId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  nativeWindowTrackerPtr->g_browser_client_ =
      std::make_shared<AafwkBrowserClientAdapterTest>();
  int32_t native_window_id = -1;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // If d_browser_client_ is not null ptr，
  // the window will be obtained from the browser process by calling the binder
  // (sync call)
  EXPECT_NE(nativeWindow, nullptr);
  nativeWindowTrackerPtr->g_browser_client_ = nullptr;
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow_HasBrowserClientZeroId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  nativeWindowTrackerPtr->g_browser_client_ =
      std::make_shared<AafwkBrowserClientAdapterTest>();
  int32_t native_window_id = 0;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // If d_browser_client_ is not null ptr，
  // the window will be obtained from the browser process by calling the binder
  // (sync call)
  EXPECT_NE(nativeWindow, nullptr);
  nativeWindowTrackerPtr->g_browser_client_ = nullptr;
}

TEST_F(NWebNativeWindowTrackerTest, DestroyNativeWindow_NoAddNormalId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 101;
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  // To determine the success of Destroy, it is necessary to verify it through
  // GetActiveWindow
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, DestroyNativeWindow_NoAddExtremeId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = -2;
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  // To determine the success of Destroy, it is necessary to verify it through
  // GetActiveWindow
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, DestroyNativeWindow_Add) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);
  nativeWindowTrackerPtr->next_native_window_id_=1;
  auto window = VoidClassTest().Get();
  int32_t native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 1);
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest,
       DestroyNativeWindow_HasBrowserClientNormalId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  nativeWindowTrackerPtr->g_browser_client_ =
      std::make_shared<AafwkBrowserClientAdapterTest>();
  int32_t native_window_id = 102;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_NE(nativeWindow, nullptr);
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  nativeWindow = nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest,
       DestroyNativeWindow_HasBrowserClientExtremeId) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  nativeWindowTrackerPtr->g_browser_client_ =
      std::make_shared<AafwkBrowserClientAdapterTest>();
  int32_t native_window_id = -2;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_NE(nativeWindow, nullptr);
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  nativeWindow = nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebNativeWindowTrackerTest, GetNativeWindow001) {
  auto nativeWindowTrackerPtr = NWebNativeWindowTracker::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);
  nativeWindowTrackerPtr->next_native_window_id_=1;
  auto window = VoidClassTest().Get();
  int32_t native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  ASSERT_EQ(native_window_id,1);
  EXPECT_EQ(nativeWindowTrackerPtr->g_browser_client_, nullptr);
}

}  // namespace gpu
#endif

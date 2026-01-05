// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#include "nweb_video_native_window.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

namespace {

class VoidClassTest {
 public:
  VoidClassTest* Get() { return this; }
};

class NWebVideoNativeWindowTest : public testing::Test {
 public:
  NWebVideoNativeWindowTest() = default;

  NWebVideoNativeWindowTest(const NWebVideoNativeWindowTest&) = delete;
  NWebVideoNativeWindowTest& operator=(const NWebVideoNativeWindowTest&) =
      delete;
};
}  // namespace

TEST_F(NWebVideoNativeWindowTest, GetInstance) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  EXPECT_NE(nativeWindowTrackerPtr, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, AddNativeWindow_NotNull) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto window = VoidClassTest().Get();
  auto native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 1);
}

TEST_F(NWebVideoNativeWindowTest, AddNativeWindow_Null) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  auto window = nullptr;
  auto native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 2);
}

TEST_F(NWebVideoNativeWindowTest, GetNativeWindow_NotAddNormalId) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 100;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, GetNativeWindow_NotAddExtremeId) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = -1;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, GetNativeWindow_NotAddZeroId) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 0;
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  // When there are no members, one should not obtain information about the
  // window
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, GetNativeWindow_Add) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
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

TEST_F(NWebVideoNativeWindowTest, DestroyNativeWindow_NoAddNormalId) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = 101;
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  // To determine the success of Destroy, it is necessary to verify it through
  // GetActiveWindow
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, DestroyNativeWindow_NoAddExtremeId) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);

  int32_t native_window_id = -2;
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  // To determine the success of Destroy, it is necessary to verify it through
  // GetActiveWindow
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

TEST_F(NWebVideoNativeWindowTest, DestroyNativeWindow_Add) {
  auto nativeWindowTrackerPtr = NWebVideoNativeWindow::GetInstance();
  ASSERT_NE(nativeWindowTrackerPtr, nullptr);
  auto window = VoidClassTest().Get();
  int32_t native_window_id = nativeWindowTrackerPtr->AddNativeWindow(window);
  EXPECT_EQ(native_window_id, 1);
  nativeWindowTrackerPtr->DestroyNativeWindow(native_window_id);
  void* nativeWindow =
      nativeWindowTrackerPtr->GetNativeWindow(native_window_id);
  EXPECT_EQ(nativeWindow, nullptr);
}

}  // namespace gpu
#endif

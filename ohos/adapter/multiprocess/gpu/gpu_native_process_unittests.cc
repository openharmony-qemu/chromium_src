// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_process.h"

#include <unistd.h>

#include "gpu_native_process_test_utils.h"
#include "gtest/gtest.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos::adapter::multiprocess {
using ohos::adapter::xcomponent::WindowAdapter;

class GpuNativeProcessTest : public ::testing::Test {
 protected:
  void SetUp() override { process_ = &GpuNativeProcess::GetInstance(); }
  GpuNativeProcess* process_;
};

TEST_F(GpuNativeProcessTest, GetPidTest) {
  EXPECT_EQ(getpid(), process_->GetPid());
}

TEST_F(GpuNativeProcessTest, AddWindowTest) {
  std::string window_id = "browser1";
  NativeWindow native_window;
  process_->AddWindow(window_id, &native_window);
  EXPECT_NE(nullptr, WindowAdapter::GetInstance().GetWindow(window_id));
  WindowAdapter::GetInstance().RemoveWindow(window_id);
}

TEST_F(GpuNativeProcessTest, RemoveWindowTest) {
  std::string window_id = "browser1";
  NativeWindow native_window;
  WindowAdapter::GetInstance().AddWindow(window_id, &native_window);
  process_->RemoveWindow(window_id);
  EXPECT_EQ(nullptr, WindowAdapter::GetInstance().GetWindow(window_id));
}

TEST_F(GpuNativeProcessTest, SetWindowWidgetTest) {
  std::string window_id = "browser1";
  NativeWindow native_window;
  WindowAdapter::GetInstance().AddWindow(window_id, &native_window);
  EXPECT_NE(nullptr, WindowAdapter::GetInstance().GetWindow(window_id));
  int new_widget_id = 2;
  process_->SetWindowWidget(&native_window, new_widget_id);
  EXPECT_EQ(new_widget_id, WindowAdapter::GetInstance().GetWidgetId(window_id));
  WindowAdapter::GetInstance().RemoveWindow(window_id);
}
}  // namespace ohos::adapter::multiprocess

// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)
#include "base/posix/global_descriptors.h"
#include "content/public/common/content_descriptors.h"
#include "content/renderer/render_remote_proxy_ohos.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "res_sched_client_adapter.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace content {

class MockAafwkBrowserClientAdapter
    : public OHOS::NWeb::AafwkBrowserClientAdapter {
 public:
  MockAafwkBrowserClientAdapter() = default;

  ~MockAafwkBrowserClientAdapter() override = default;

  void* QueryRenderSurface(int32_t surface_id) override { return this; }
  void ReportThread(OHOS::NWeb::ResSchedStatusAdapter status,
                    int32_t process_id,
                    int32_t thread_id,
                    OHOS::NWeb::ResSchedRoleAdapter role) override {}

  void PassSurface(int64_t surface_id) override {}

  void DestroyRenderSurface(int32_t surface_id) override {}
};

class RenderRemoteProxyUnitTest : public testing::Test {
 public:
  RenderRemoteProxyUnitTest() {}
  ~RenderRemoteProxyUnitTest() override {}

 protected:
  void SetUp() override {}
};

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserWithNegative) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowser(-1, -1, -1, nullptr);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_EQ(g_fds->Get(kMojoIPCChannel), -1);
  EXPECT_EQ(g_fds->Get(kFieldTrialDescriptor), -1);
  EXPECT_EQ(g_fds->Get(kCrashDumpSignal), -1);
  EXPECT_EQ(NWebNativeWindowTracker::Get()->g_browser_client_, nullptr);
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserWithZero) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowser(0, 0, 0, nullptr);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_GE(g_fds->Get(kMojoIPCChannel), 0);
  EXPECT_GE(g_fds->Get(kFieldTrialDescriptor), 0);
  EXPECT_GE(g_fds->Get(kCrashDumpSignal), 0);
  EXPECT_EQ(NWebNativeWindowTracker::Get()->g_browser_client_, nullptr);
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserWithNormalParam) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> clientAdapter =
      std::make_shared<MockAafwkBrowserClientAdapter>();
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowser(1, 1, 1, clientAdapter);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_GE(g_fds->Get(kMojoIPCChannel), 0);
  EXPECT_GE(g_fds->Get(kFieldTrialDescriptor), 0);
  EXPECT_GE(g_fds->Get(kCrashDumpSignal), 0);
  EXPECT_EQ(NWebNativeWindowTracker::Get()->g_browser_client_, clientAdapter);
}

}  // namespace content
#endif

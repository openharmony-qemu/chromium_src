// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB)
#include "base/command_line.h"
#include "base/posix/global_descriptors.h"
#include "content/public/common/content_descriptors.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "content/renderer/render_remote_proxy_ohos.h"
#undef private
#endif
#include "gpu/ipc/common/nweb_native_window_tracker.h"
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

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserFdWithNegative) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowserFd(-1, -1, -1);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_EQ(g_fds->Get(kMojoIPCChannel), -1);
  EXPECT_EQ(g_fds->Get(kFieldTrialDescriptor), -1);
  EXPECT_EQ(g_fds->Get(kCrashDumpSignal), -1);
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserFdWithZero) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowserFd(0, 0, 0);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_GE(g_fds->Get(kMojoIPCChannel), 0);
  EXPECT_GE(g_fds->Get(kFieldTrialDescriptor), 0);
  EXPECT_GE(g_fds->Get(kCrashDumpSignal), 0);
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowserFdWithNormalParam) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  renderRemoteProxy->NotifyBrowserFd(1, 1, 1);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_GE(g_fds->Get(kMojoIPCChannel), 0);
  EXPECT_GE(g_fds->Get(kFieldTrialDescriptor), 0);
  EXPECT_GE(g_fds->Get(kCrashDumpSignal), 0);
}

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

TEST_F(RenderRemoteProxyUnitTest, WaitForBrowserFdWhenIsForTest) {
  bool original_is_for_test = RenderRemoteProxy::is_for_test_;
  RenderRemoteProxy::is_for_test_ = true;
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  EXPECT_TRUE(renderRemoteProxy->WaitForBrowserFd());
  RenderRemoteProxy::is_for_test_ = original_is_for_test;
}

TEST_F(RenderRemoteProxyUnitTest, WaitForBrowserFdTimeout) {
  bool original_is_for_test = RenderRemoteProxy::is_for_test_;
  bool original_is_browser_fd_received = RenderRemoteProxy::is_browser_fd_received_;
  RenderRemoteProxy::is_for_test_ = false;
  RenderRemoteProxy::is_browser_fd_received_ = false;
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  EXPECT_FALSE(renderRemoteProxy->WaitForBrowserFd());
  RenderRemoteProxy::is_for_test_ = original_is_for_test;
  RenderRemoteProxy::is_browser_fd_received_ = original_is_browser_fd_received;
}

TEST_F(RenderRemoteProxyUnitTest, WaitForBrowserFdSuccess) {
  bool original_is_for_test = RenderRemoteProxy::is_for_test_;
  bool original_is_browser_fd_received = RenderRemoteProxy::is_browser_fd_received_;
  RenderRemoteProxy::is_for_test_ = false;
  RenderRemoteProxy::is_browser_fd_received_ = true;
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();
  EXPECT_TRUE(renderRemoteProxy->WaitForBrowserFd());
  RenderRemoteProxy::is_for_test_ = original_is_for_test;
  RenderRemoteProxy::is_browser_fd_received_ = original_is_browser_fd_received;
}

TEST_F(RenderRemoteProxyUnitTest, SetBrowserFdTest) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);

  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();

  // Test SetBrowserFd with valid file descriptors
  renderRemoteProxy->SetBrowserFd(10, 20, 30);

  EXPECT_NE(g_fds, nullptr);
  EXPECT_EQ(g_fds->Get(kMojoIPCChannel), 10);
  EXPECT_EQ(g_fds->Get(kFieldTrialDescriptor), 20);
  EXPECT_EQ(g_fds->Get(kCrashDumpSignal), 30);

  // Check that fds_channel_ready_ is set to true
  EXPECT_TRUE(RenderRemoteProxy::IsFdsChannelReady());
}

TEST_F(RenderRemoteProxyUnitTest, IsFdsChannelReady_InitiallyFalse) {
  bool original_fds_channel_ready = RenderRemoteProxy::fds_channel_ready_;
  RenderRemoteProxy::fds_channel_ready_ = false;

  EXPECT_FALSE(RenderRemoteProxy::IsFdsChannelReady());

  RenderRemoteProxy::fds_channel_ready_ = original_fds_channel_ready;
}

TEST_F(RenderRemoteProxyUnitTest, IsFdsChannelReady_AfterSetBrowserFd) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);

  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();

  EXPECT_FALSE(RenderRemoteProxy::IsFdsChannelReady());

  renderRemoteProxy->SetBrowserFd(5, 6, 7);

  EXPECT_TRUE(RenderRemoteProxy::IsFdsChannelReady());
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowser_WhenFdsChannelReady) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);

  // First set fds_channel_ready_ to true
  bool original_fds_channel_ready = RenderRemoteProxy::fds_channel_ready_;
  RenderRemoteProxy::fds_channel_ready_ = true;

  std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> clientAdapter =
      std::make_shared<MockAafwkBrowserClientAdapter>();
  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();

  // This should go through the fds_channel_ready_ == true branch
  renderRemoteProxy->NotifyBrowser(1, 1, 1, clientAdapter);

  RenderRemoteProxy::fds_channel_ready_ = original_fds_channel_ready;
}

TEST_F(RenderRemoteProxyUnitTest, NotifyBrowser_WhenFdsChannelReady_NoClientAdapter) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  base::GlobalDescriptors::Mapping mapping;
  g_fds->Reset(mapping);

  // First set fds_channel_ready_ to true
  bool original_fds_channel_ready = RenderRemoteProxy::fds_channel_ready_;
  RenderRemoteProxy::fds_channel_ready_ = true;

  std::shared_ptr<content::RenderRemoteProxy> renderRemoteProxy =
      std::make_shared<content::RenderRemoteProxy>();

  // This should go through the fds_channel_ready_ == true branch with null clientAdapter
  renderRemoteProxy->NotifyBrowser(1, 1, 1, nullptr);

  RenderRemoteProxy::fds_channel_ready_ = original_fds_channel_ready;
}

TEST_F(RenderRemoteProxyUnitTest, ParseFdsFromCommandLineTest) {
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* cmd_line = const_cast<base::CommandLine*>(
      base::CommandLine::ForCurrentProcess());

  // Set FD values via command line switches
  cmd_line->AppendSwitchASCII("ipc-fd", "100");
  cmd_line->AppendSwitchASCII("shared-fd", "200");
  cmd_line->AppendSwitchASCII("crash-fd", "300");

  RenderRemoteProxy::Fds fds;
  RenderRemoteProxy::ParseFdsFromCommandLine(*cmd_line, fds);

  EXPECT_EQ(fds.ipcFd, 100);
  EXPECT_EQ(fds.sharedFd, 200);
  EXPECT_EQ(fds.crashFd, 300);
}

TEST_F(RenderRemoteProxyUnitTest, ParseFdsFromCommandLine_EmptySwitches) {
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* cmd_line = const_cast<base::CommandLine*>(
      base::CommandLine::ForCurrentProcess());

  // No FD switches set
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = -1;
  fds.sharedFd = -1;
  fds.crashFd = -1;

  RenderRemoteProxy::ParseFdsFromCommandLine(*cmd_line, fds);

  // Should remain at default values
  EXPECT_EQ(fds.ipcFd, -1);
  EXPECT_EQ(fds.sharedFd, -1);
  EXPECT_EQ(fds.crashFd, -1);
}

TEST_F(RenderRemoteProxyUnitTest, ParseFdsFromCommandLine_PartialSwitches) {
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* cmd_line = const_cast<base::CommandLine*>(
      base::CommandLine::ForCurrentProcess());

  // Only set ipc-fd
  cmd_line->AppendSwitchASCII("ipc-fd", "50");

  RenderRemoteProxy::Fds fds;
  fds.ipcFd = -1;
  fds.sharedFd = -1;
  fds.crashFd = -1;

  RenderRemoteProxy::ParseFdsFromCommandLine(*cmd_line, fds);

  EXPECT_EQ(fds.ipcFd, 50);
  EXPECT_EQ(fds.sharedFd, -1);
  EXPECT_EQ(fds.crashFd, -1);
}

TEST_F(RenderRemoteProxyUnitTest, Fds_HasAllFds_True) {
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = 100;
  fds.sharedFd = 200;
  fds.crashFd = 300;

  EXPECT_TRUE(fds.HasAllFds());
}

TEST_F(RenderRemoteProxyUnitTest, Fds_HasAllFds_False_NegativeIpcFd) {
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = -1;
  fds.sharedFd = 200;
  fds.crashFd = 300;

  EXPECT_FALSE(fds.HasAllFds());
}

TEST_F(RenderRemoteProxyUnitTest, Fds_HasAllFds_False_NegativeSharedFd) {
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = 100;
  fds.sharedFd = -1;
  fds.crashFd = 300;

  EXPECT_FALSE(fds.HasAllFds());
}

TEST_F(RenderRemoteProxyUnitTest, Fds_HasAllFds_False_NegativeCrashFd) {
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = 100;
  fds.sharedFd = 200;
  fds.crashFd = -1;

  EXPECT_FALSE(fds.HasAllFds());
}

TEST_F(RenderRemoteProxyUnitTest, Fds_HasAllFds_False_AllNegative) {
  RenderRemoteProxy::Fds fds;
  fds.ipcFd = -1;
  fds.sharedFd = -1;
  fds.crashFd = -1;

  EXPECT_FALSE(fds.HasAllFds());
}

TEST_F(RenderRemoteProxyUnitTest, StaticMembers_InitialState) {
  // Test that static members can be accessed and modified
  bool original_is_browser_fd_received = RenderRemoteProxy::is_browser_fd_received_;
  bool original_fds_channel_ready = RenderRemoteProxy::fds_channel_ready_;

  RenderRemoteProxy::is_browser_fd_received_ = false;
  RenderRemoteProxy::fds_channel_ready_ = false;

  EXPECT_FALSE(RenderRemoteProxy::is_browser_fd_received_);
  EXPECT_FALSE(RenderRemoteProxy::fds_channel_ready_);

  RenderRemoteProxy::is_browser_fd_received_ = true;
  RenderRemoteProxy::fds_channel_ready_ = true;

  EXPECT_TRUE(RenderRemoteProxy::is_browser_fd_received_);
  EXPECT_TRUE(RenderRemoteProxy::fds_channel_ready_);

  // Restore original values
  RenderRemoteProxy::is_browser_fd_received_ = original_is_browser_fd_received;
  RenderRemoteProxy::fds_channel_ready_ = original_fds_channel_ready;
}

}  // namespace content
#endif

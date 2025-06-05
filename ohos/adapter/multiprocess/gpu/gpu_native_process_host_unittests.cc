/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AbilityKit/native_child_process.h>

#include "gpu_native_process_host.h"
#include "gpu_native_process_test_utils.h"
#include "gtest/gtest.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;

namespace ohos::adapter::multiprocess {
class MockGpuNativeIpcProxy : public GpuNativeIpcProxy {
 public:
  explicit MockGpuNativeIpcProxy() : GpuNativeIpcProxy(&remote_proxy) {}
  MOCK_METHOD1(
      InitializeWindowAdapter,
      OH_IPC_ErrorCode(const std::vector<std::pair<std::string, void*>>&));
  MOCK_METHOD1(GetPid, OH_IPC_ErrorCode(int*));
  MOCK_METHOD2(AddWindow, OH_IPC_ErrorCode(const std::string&, void*));
  MOCK_METHOD1(RemoveWindow, OH_IPC_ErrorCode(const std::string&));
  MOCK_METHOD2(SetWindowWidget, OH_IPC_ErrorCode(const std::string&, int));
  MOCK_METHOD2(NotifyWindowChanged,
               OH_IPC_ErrorCode(const std::string&, void*));
  OHIPCRemoteProxy remote_proxy;
};

class GpuNativeProcessHostTest : public ::testing::Test {
 protected:
  void SetUp() override {
    process_host_ = &GpuNativeProcessHost::GetInstance();
    ipc_proxy_ = std::make_shared<NiceMock<MockGpuNativeIpcProxy>>();
    NativeApiMockManager::GetInstance().SetApiImpl(&api_);
  }
  void TearDown() override {
    CommandLine::ResetForCurrentProcess();
    process_host_->Reset();
  }
  GpuNativeProcessHost* process_host_{nullptr};
  NiceMock<NativeMockApi> api_;
  std::shared_ptr<NiceMock<MockGpuNativeIpcProxy>> ipc_proxy_{nullptr};
};

TEST_F(GpuNativeProcessHostTest, InitializeTest) {
  EXPECT_FALSE(process_host_->IsInitialized());
  OHIPCRemoteProxy remote_proxy;
  process_host_->Initialize(&remote_proxy);
  EXPECT_TRUE(process_host_->IsInitialized());
}

TEST_F(GpuNativeProcessHostTest, NeedSendRequestTest) {
  CommandLine::ResetForCurrentProcess();
  EXPECT_FALSE(process_host_->NeedSendRequest());
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(process_host_->IsInitialized());
  EXPECT_TRUE(CommandLine::InitForCurrentProcess(std::vector<std::string>()));
  EXPECT_TRUE(process_host_->NeedSendRequest());
  CommandLine::ResetForCurrentProcess();
  EXPECT_TRUE(CommandLine::InitForCurrentProcess({"--in-process-gpu"}));
  EXPECT_TRUE(
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kInProcessGpu));
  EXPECT_FALSE(process_host_->NeedSendRequest());
}

TEST_F(GpuNativeProcessHostTest, InitializeWindowAdapterTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess(std::vector<std::string>()));
  EXPECT_CALL(*ipc_proxy_, InitializeWindowAdapter(_))
      .WillOnce(Return(OH_IPC_SUCCESS));
  std::vector<std::pair<std::string, void*>> windows;
  EXPECT_EQ(0, process_host_->InitializeWindowAdapter(windows));
  testing::Mock::VerifyAndClearExpectations(&ipc_proxy_);
  EXPECT_CALL(*ipc_proxy_, InitializeWindowAdapter(_))
      .WillOnce(Return(OH_IPC_INNER_ERROR));
  EXPECT_NE(0, process_host_->InitializeWindowAdapter(windows));
}

TEST_F(GpuNativeProcessHostTest, GetPidTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess(std::vector<std::string>()));
  int pid = 12345;
  EXPECT_CALL(*ipc_proxy_, GetPid(_))
      .WillOnce(DoAll(SetArgPointee<0>(pid), Return(OH_IPC_SUCCESS)));
  EXPECT_EQ(pid, process_host_->GetPid());
}

TEST_F(GpuNativeProcessHostTest, AddWindowTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess({"--in-process-gpu"}));
  NativeWindow native_window;
  EXPECT_CALL(*ipc_proxy_, AddWindow(_, _)).Times(0);
  process_host_->AddWindow("browser1", &native_window);
}

TEST_F(GpuNativeProcessHostTest, RemoveWindowTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess({"--in-process-gpu"}));
  NativeWindow native_window;
  EXPECT_CALL(*ipc_proxy_, RemoveWindow(_)).Times(0);
  process_host_->RemoveWindow("browser1");
}

TEST_F(GpuNativeProcessHostTest, SetWindowWidgetTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess({"--in-process-gpu"}));
  EXPECT_CALL(*ipc_proxy_, SetWindowWidget(_, _)).Times(0);
  process_host_->SetWindowWidget("browser1", 1);
}

TEST_F(GpuNativeProcessHostTest, NotifyWindowChangedTest) {
  process_host_->Initialize(ipc_proxy_);
  EXPECT_TRUE(CommandLine::InitForCurrentProcess({"--in-process-gpu"}));
  NativeWindow native_window;
  EXPECT_CALL(*ipc_proxy_, NotifyWindowChanged(_, _)).Times(0);
  process_host_->NotifyWindowChanged("browser1", &native_window);
}

TEST_F(GpuNativeProcessHostTest, StartGpuProcessTest) {
  EXPECT_CALL(api_, CreateNativeChildProcess(_, _))
      .WillOnce(Return(NCP_NO_ERROR));
  EXPECT_EQ(NCP_NO_ERROR, process_host_->StartGpuProcess(nullptr));
  testing::Mock::VerifyAndClearExpectations(&api_);
  EXPECT_CALL(api_, CreateNativeChildProcess(_, _))
      .WillOnce(Return(NCP_ERR_MAX_CHILD_PROCESSES_REACHED));
  EXPECT_NE(NCP_NO_ERROR, process_host_->StartGpuProcess(nullptr));
}
}  // namespace ohos::adapter::multiprocess

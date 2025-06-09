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

#include "gmock/gmock.h"
#include "gpu_native_ipc_proxy.h"
#include "gpu_native_process_test_utils.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::WithArg;
using ::testing::WithArgs;

namespace ohos::adapter::multiprocess {

class NativeIpcRequestTest : public NativeIpcRequest {
 public:
  std::string ToString() const override { return "NativeIpcRequestTest"; }
  OHIPCParcel* GetDataParcel() { return data_parcel_; }
  OHIPCParcel* GetReplyParcel() { return reply_parcel_; }
};

class NativeIpcRequestSuccessTest : public NativeIpcRequestTest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override {
    static_cast<void>(remote_proxy);
    return OH_IPC_SUCCESS;
  }
};

class NativeIpcRequestFailedTest : public NativeIpcRequestTest {
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override {
    static_cast<void>(remote_proxy);
    return OH_IPC_CHECK_PARAM_ERROR;
  }
};

class NativeIpcProxyTest : public NativeIpcProxy {
 public:
  explicit NativeIpcProxyTest(OHIPCRemoteProxy* remote_proxy)
      : NativeIpcProxy(remote_proxy) {}

  OH_IPC_ErrorCode TestSuccessRequest() {
    return PerformRequest(std::make_unique<NativeIpcRequestSuccessTest>());
  }

  OH_IPC_ErrorCode TestFailedRequest() {
    return PerformRequest(std::make_unique<NativeIpcRequestFailedTest>());
  }

  OH_IPC_ErrorCode TestNullRequest() { return PerformRequest(nullptr); }
};

class GpuNativeIpcProxyTest : public ::testing::Test {
 protected:
  void SetUp() override {
    NativeApiMockManager::GetInstance().SetApiImpl(&api_);
  }
  NiceMock<NativeMockApi> api_;
  OHIPCRemoteProxy remote_proxy_;
  GpuNativeIpcProxy ipc_proxy_ = GpuNativeIpcProxy(&remote_proxy_);
};

TEST(NativeIpcProxyTest, IpcRequestPrepareTest) {
  NativeIpcRequestSuccessTest request_test;

  OH_IPC_ErrorCode ret = request_test.Prepare();
  EXPECT_EQ(ret, OH_IPC_SUCCESS);
  EXPECT_TRUE(request_test.GetDataParcel() != nullptr);
  EXPECT_TRUE(request_test.GetReplyParcel() != nullptr);
}

TEST(NativeIpcProxyTest, IpcRequestPerformTest) {
  NiceMock<NativeMockApi> api;
  NativeApiMockManager::GetInstance().SetApiImpl(&api);
  OHIPCRemoteProxy remote_proxy;
  NativeIpcProxyTest proxy(&remote_proxy);

  OH_IPC_ErrorCode ret = proxy.TestSuccessRequest();
  EXPECT_EQ(ret, OH_IPC_SUCCESS);

  ret = proxy.TestFailedRequest();
  EXPECT_NE(ret, OH_IPC_SUCCESS);

  ret = proxy.TestNullRequest();
  EXPECT_EQ(ret, OH_IPC_CHECK_PARAM_ERROR);
}

TEST_F(GpuNativeIpcProxyTest, GetPidTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArgs<1, 3>(Invoke([](int code, OHIPCParcel* reply) -> int {
        EXPECT_EQ(code, static_cast<int>(GpuNativeIpcOpCode::GET_PID));
        EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(reply, 12345));
        return OH_IPC_SUCCESS;
      })));
  int pid;
  EXPECT_EQ(OH_IPC_SUCCESS, ipc_proxy_.GetPid(&pid));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.GetPid(&pid));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.GetPid(nullptr));
}

TEST_F(GpuNativeIpcProxyTest, InitializeWindowAdapterTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArg<1>(Invoke([](int code) -> int {
        EXPECT_EQ(code, static_cast<int>(
                            GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER));
        return OH_IPC_SUCCESS;
      })));

  std::vector<std::pair<std::string, void*>> windows;
  EXPECT_EQ(OH_IPC_SUCCESS, ipc_proxy_.InitializeWindowAdapter(windows));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.InitializeWindowAdapter(windows));
}

TEST_F(GpuNativeIpcProxyTest, AddWindowTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArg<1>(Invoke([](int code) -> int {
        EXPECT_EQ(code, static_cast<int>(GpuNativeIpcOpCode::ADD_WINDOW));
        return OH_IPC_SUCCESS;
      })));

  NativeWindow native_window;
  EXPECT_EQ(OH_IPC_SUCCESS, ipc_proxy_.AddWindow("browser1", &native_window));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.AddWindow("browser1", &native_window));

  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.AddWindow("browser1", nullptr));
}

TEST_F(GpuNativeIpcProxyTest, RemoveWindowTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArg<1>(Invoke([](int code) -> int {
        EXPECT_EQ(code, static_cast<int>(GpuNativeIpcOpCode::REMOVE_WINDOW));
        return OH_IPC_SUCCESS;
      })));
  EXPECT_EQ(OH_IPC_SUCCESS, ipc_proxy_.RemoveWindow("browser1"));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.RemoveWindow("browser1"));
}

TEST_F(GpuNativeIpcProxyTest, SetWindowWidgetTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArg<1>(Invoke([](int code) -> int {
        EXPECT_EQ(code,
                  static_cast<int>(GpuNativeIpcOpCode::SET_WINDOW_WIDGET));
        return OH_IPC_SUCCESS;
      })));
  EXPECT_EQ(OH_IPC_SUCCESS, ipc_proxy_.SetWindowWidget("browser1", 1));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.SetWindowWidget("browser1", 1));
}

TEST_F(GpuNativeIpcProxyTest, NotifyWindowChangeTest) {
  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(WithArg<1>(Invoke([](int code) -> int {
        EXPECT_EQ(code,
                  static_cast<int>(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE));
        return OH_IPC_SUCCESS;
      })));
  NativeWindow native_window;
  EXPECT_EQ(OH_IPC_SUCCESS,
            ipc_proxy_.NotifyWindowChange("browser1", &native_window));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, IpcRemoteProxySendRequest(_, _, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_READ_ERROR));
  EXPECT_NE(OH_IPC_SUCCESS,
            ipc_proxy_.NotifyWindowChange("browser1", &native_window));

  EXPECT_NE(OH_IPC_SUCCESS, ipc_proxy_.NotifyWindowChange("browser1", nullptr));
}
}  // namespace ohos::adapter::multiprocess

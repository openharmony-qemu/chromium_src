// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_ipc_stub.h"

#include "gmock/gmock.h"
#include "gpu_native_process_test_utils.h"
#include "gtest/gtest.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using ohos::adapter::xcomponent::WindowAdapter;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;

namespace ohos::adapter::multiprocess {
class GpuNativeIpcStubTest : public ::testing::Test {
 protected:
  void SetUp() override {
    CreateParcels();
    NativeApiMockManager::GetInstance().SetApiImpl(&api_);
  }
  void TearDown() override { ReleaseParcels(); }
  void CreateParcels() {
    data_ = OH_IPCParcel_Create();
    EXPECT_NE(nullptr, data_);
    reply_ = OH_IPCParcel_Create();
    EXPECT_NE(nullptr, reply_);
  }
  void ReleaseParcels() {
    if (data_) {
      OH_IPCParcel_Destroy(data_);
    }
    if (reply_) {
      OH_IPCParcel_Destroy(reply_);
    }
  }
  void ResetParcels() {
    ReleaseParcels();
    CreateParcels();
  }

  OHIPCParcel* data_;
  OHIPCParcel* reply_;
  NiceMock<NativeMockApi> api_;
};

TEST_F(GpuNativeIpcStubTest, HandleInvalidCodeTest) {
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::INVALID_OPERATION_CODE,
                                     data_, reply_, nullptr));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::MAX_OPERATION_CODE,
                                     data_, reply_, nullptr));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::GET_PID, data_,
                                     nullptr, nullptr));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::GET_PID, nullptr,
                                     reply_, nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleGetPidTest) {
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  EXPECT_EQ(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::GET_PID, data_, reply_,
                                     nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleInitializeWindowAdapterTest) {
  NativeWindow native_window;
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillRepeatedly(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetStringAttr(_, _, _)).WillRepeatedly(Return(0));
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  WriteWindowsIntoParcel(data_, 3);
  EXPECT_EQ(OH_IPC_SUCCESS, stub.HandleRemoteRequest(
                                GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER,
                                data_, reply_, nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleAddWindowTest) {
  NativeWindow native_window;
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  WriteWindowIntoParcel(data_, 1);
  EXPECT_EQ(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::ADD_WINDOW, data_,
                                     reply_, nullptr));
  testing::Mock::VerifyAndClearExpectations(&api_);
  ResetParcels();
  WriteWindowIntoParcel(data_, 1);
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(nullptr), Return(0)));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::ADD_WINDOW, data_,
                                     reply_, nullptr));
  testing::Mock::VerifyAndClearExpectations(&api_);
  ResetParcels();
  WriteWindowIntoParcel(data_, 1);
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(-1)));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::ADD_WINDOW, data_,
                                     reply_, nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleRemoveWindowTest) {
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteString(data_, "browser1"));
  EXPECT_EQ(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::REMOVE_WINDOW, data_,
                                     reply_, nullptr));
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::REMOVE_WINDOW, data_,
                                     reply_, nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleSetWindowWidgetTest) {
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  NativeWindow native_window;
  std::string window_id = "browser1";
  WindowAdapter::GetInstance().AddWindow(window_id, &native_window);
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteString(data_, window_id.c_str()));
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(data_, 3));
  EXPECT_EQ(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::SET_WINDOW_WIDGET,
                                     data_, reply_, nullptr));
  testing::Mock::VerifyAndClearExpectations(&api_);
  ResetParcels();
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteString(data_, window_id.c_str()));
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(data_, 3));
  WindowAdapter::GetInstance().RemoveWindow(window_id);
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::SET_WINDOW_WIDGET,
                                     data_, reply_, nullptr));
}

TEST_F(GpuNativeIpcStubTest, HandleNotifyWindowChangeTest) {
  GpuNativeIpcStub& stub = GpuNativeIpcStub::GetInstance();
  int widget_id = 1;
  NativeWindow native_window;
  std::string window_id = WindowAdapter::GetInstance().GetWindowId(widget_id);
  WriteWindowIntoParcel(data_, widget_id);
  WindowAdapter::GetInstance().AddWindow(window_id, &native_window);
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetStringAttr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_EQ(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE,
                                     data_, reply_, nullptr));
  testing::Mock::VerifyAndClearExpectations(&api_);
  ResetParcels();
  WriteWindowIntoParcel(data_, widget_id);
  WindowAdapter::GetInstance().RemoveWindow(window_id);
  EXPECT_NE(OH_IPC_SUCCESS,
            stub.HandleRemoteRequest(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE,
                                     data_, reply_, nullptr));
}
}  // namespace ohos::adapter::multiprocess

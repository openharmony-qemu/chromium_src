// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "gpu_native_process_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
#include "third_party/bounds_checking_function/include/securec.h"
#endif

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::SetArgPointee;

namespace ohos::adapter::multiprocess {
class NativeWindowParcelTest : public ::testing::Test {
  void SetUp() override {
    NativeApiMockManager::GetInstance().SetApiImpl(&api_);
  }

 protected:
  NativeMockApi api_;
};

OHIPCParcel* CreateMockParcelWithWindowData() {
  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);
  EXPECT_FALSE(OH_IPCParcel_WriteInt32(parcel, 0));
  EXPECT_FALSE(OH_IPCParcel_WriteInt32(parcel, 0));
  EXPECT_FALSE(FillUpParcel(parcel, WindowAttrType::MAX_TYPE_COUNT));
  EXPECT_FALSE(OH_IPCParcel_WriteString(parcel, ""));
  return parcel;
}

TEST_F(NativeWindowParcelTest, AttrToParcelBasic) {
  int width = 1920;
  int height = 1080;
  char app_framework[64] = "test-framework";

  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _))
      .WillOnce(Invoke([](NativeWindow* window, int op_code, int* width,
                          int* height) -> int {
        *width = window->width;
        *height = window->height;
        return 0;
      }));

  EXPECT_CALL(api_, GetInt32Attr(_, _, _))
      .WillRepeatedly(
          Invoke([](NativeWindow* window, int type, int* val) -> int {
            *val = window->attrs[type - WindowAttrType::FORMAT];
            return 0;
          }));

  EXPECT_CALL(api_, GetStringAttr(_, _, _))
      .WillOnce(Invoke([](NativeWindow* window, int type, char* val) -> int {
        constexpr size_t kMaxBufferSize = 64;
        strcpy_s(val, kMaxBufferSize, window->app_framework);
        return 0;
      }));

  NativeWindow native_window{
      width, height, {0, 1, 2, 3, 4, 5, 6, 7}, app_framework};

  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);
  EXPECT_FALSE(NativeWindowAttrToParcel(&native_window, parcel));

  int width_check;
  int height_check;
  EXPECT_FALSE(OH_IPCParcel_ReadInt32(parcel, &width_check));
  EXPECT_EQ(width_check, width);
  EXPECT_FALSE(OH_IPCParcel_ReadInt32(parcel, &height_check));
  EXPECT_EQ(height_check, height);
  EXPECT_FALSE(CheckParcel(parcel, WindowAttrType::MAX_TYPE_COUNT));
  EXPECT_STREQ(OH_IPCParcel_ReadString(parcel), app_framework);
  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, AttrToParcelError) {
  NativeWindow native_window;
  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);

  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrToParcel(&native_window, parcel));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetInt32Attr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrToParcel(&native_window, parcel));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, GetStringAttr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrToParcel(&native_window, parcel));

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, AttrFromParcelBasic) {
  int width = 1920;
  int height = 1080;
  char app_framework[64] = "test-framework";
  char tmp[64] = {'\0'};
  NativeWindow native_window{
      0, 0, std::vector<int>(WindowAttrType::MAX_TYPE_COUNT), tmp};

  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _))
      .WillOnce(Invoke(
          [](NativeWindow* window, int op_code, int width, int height) -> int {
            window->width = width;
            window->height = height;
            return 0;
          }));

  EXPECT_CALL(api_, SetInt32Attr(_, _, _))
      .WillRepeatedly(
          Invoke([](NativeWindow* window, int type, int val) -> int {
            GTEST_COUT << "Set attr[" << (type - WindowAttrType::FORMAT)
                       << "] = " << val << std::endl;
            window->attrs[type - WindowAttrType::FORMAT] = val;
            return 0;
          }));

  EXPECT_CALL(api_, SetStringAttr(_, _, _))
      .WillOnce(Invoke([](NativeWindow* window, int type, char* val) -> int {
        GTEST_COUT << "app framework: " << val << std::endl;
        window->app_framework = val;
        return 0;
      }));

  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);

  EXPECT_FALSE(OH_IPCParcel_WriteInt32(parcel, height));
  EXPECT_FALSE(OH_IPCParcel_WriteInt32(parcel, width));
  EXPECT_FALSE(FillUpParcel(parcel, WindowAttrType::MAX_TYPE_COUNT));
  EXPECT_FALSE(OH_IPCParcel_WriteString(parcel, app_framework));

  EXPECT_FALSE(NativeWindowAttrFromParcel(parcel, &native_window));
  EXPECT_EQ(native_window.width, width);
  EXPECT_EQ(native_window.height, height);
  for (int i = 0; i < WindowAttrType::MAX_TYPE_COUNT; i++) {
    EXPECT_EQ(native_window.attrs[i], i);
  }
  EXPECT_STREQ(native_window.app_framework, app_framework);

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, AttrFromParcelError) {
  char tmp[64] = {'\0'};
  NativeWindow native_window{
      0, 0, std::vector<int>(WindowAttrType::MAX_TYPE_COUNT), tmp};
  OHIPCParcel* parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrFromParcel(parcel, &native_window));
  testing::Mock::VerifyAndClearExpectations(&api_);

  OH_IPCParcel_Destroy(parcel);
  parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrFromParcel(parcel, &native_window));
  testing::Mock::VerifyAndClearExpectations(&api_);

  OH_IPCParcel_Destroy(parcel);
  parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetStringAttr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowAttrFromParcel(parcel, &native_window));

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, NativeWindowWriteToParcelBasic) {
  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);
  NativeWindow native_window;
  EXPECT_CALL(api_, MockNativeWindowWriteToParcel(_, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, GetStringAttr(_, _, _)).WillOnce(Return(0));
  EXPECT_FALSE(NativeWindowWriteToParcel(&native_window, parcel));

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, NativeWindowWriteToParcelError) {
  OHIPCParcel* parcel = OH_IPCParcel_Create();
  EXPECT_NE(parcel, nullptr);
  NativeWindow native_window;
  EXPECT_CALL(api_, MockNativeWindowWriteToParcel(_, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowWriteToParcel(&native_window, parcel));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, MockNativeWindowWriteToParcel(_, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_WRITE_ERROR));
  EXPECT_TRUE(NativeWindowWriteToParcel(&native_window, parcel));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, MockNativeWindowWriteToParcel(_, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetInt32Attr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowWriteToParcel(&native_window, parcel));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, MockNativeWindowWriteToParcel(_, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, GetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, GetStringAttr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowWriteToParcel(&native_window, parcel));

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, NativeWindowReadFromParcelBasic) {
  OHIPCParcel* parcel = CreateMockParcelWithWindowData();
  EXPECT_NE(parcel, nullptr);
  NativeWindow native_window;
  void* window_read = nullptr;
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetStringAttr(_, _, _)).WillOnce(Return(0));
  EXPECT_FALSE(NativeWindowReadFromParcel(parcel, &window_read));
  EXPECT_EQ(window_read, &native_window);

  OH_IPCParcel_Destroy(parcel);
}

TEST_F(NativeWindowParcelTest, NativeWindowReadFromParcelError) {
  OHIPCParcel* parcel = CreateMockParcelWithWindowData();
  EXPECT_NE(parcel, nullptr);
  NativeWindow native_window;
  void* window_read = nullptr;
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowReadFromParcel(parcel, &window_read));
  testing::Mock::VerifyAndClearExpectations(&api_);

  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(nullptr), Return(0)));
  EXPECT_TRUE(NativeWindowReadFromParcel(parcel, &window_read));
  testing::Mock::VerifyAndClearExpectations(&api_);

  OH_IPCParcel_Destroy(parcel);
  parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _))
      .WillOnce(Return(OH_IPC_PARCEL_WRITE_ERROR));
  EXPECT_TRUE(NativeWindowReadFromParcel(parcel, &window_read));

  OH_IPCParcel_Destroy(parcel);
  parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowReadFromParcel(parcel, &window_read));

  OH_IPCParcel_Destroy(parcel);
  parcel = CreateMockParcelWithWindowData();
  EXPECT_CALL(api_, MockNativeWindowReadFromParcel(_, _))
      .WillOnce(DoAll(SetArgPointee<1>(&native_window), Return(0)));
  EXPECT_CALL(api_, SetBufferGeometry(_, _, _, _)).WillOnce(Return(0));
  EXPECT_CALL(api_, SetInt32Attr(_, _, _)).WillRepeatedly(Return(0));
  EXPECT_CALL(api_, SetStringAttr(_, _, _)).WillOnce(Return(-1));
  EXPECT_TRUE(NativeWindowReadFromParcel(parcel, &window_read));

  OH_IPCParcel_Destroy(parcel);
}
}  // namespace ohos::adapter::multiprocess

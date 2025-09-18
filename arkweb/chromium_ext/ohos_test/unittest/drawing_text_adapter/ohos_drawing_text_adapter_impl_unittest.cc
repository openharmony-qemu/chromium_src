/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arkweb/ohos_adapter_ndk/drawing_text_adapter/ohos_drawing_text_adapter_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ohos_sdk/openharmony/native/sysroot/usr/include/native_drawing/drawing_text_font_descriptor.h"

namespace OHOS::NWeb {
using ::testing::_;
using ::testing::Return;
class MockOHDrawingTextSupport {
 public:
  static MockOHDrawingTextSupport& getInstance() {
    static MockOHDrawingTextSupport instance;
    return instance;
  }
  MOCK_METHOD(OH_Drawing_FontDescriptor*,
              OH_Drawing_MatchFontDescriptors,
              (OH_Drawing_FontDescriptor * desc, size_t* num));
  MOCK_METHOD(void,
              OH_Drawing_DestroyFontDescriptors,
              (OH_Drawing_FontDescriptor * descriptors, size_t num));
  MOCK_METHOD(OH_Drawing_FontDescriptor*,
              OH_Drawing_GetFontDescriptorByFullName,
              (const OH_Drawing_String* fullName,
               OH_Drawing_SystemFontType fontType));
  MOCK_METHOD(OH_Drawing_Array*,
              OH_Drawing_GetSystemFontFullNamesByType,
              (OH_Drawing_SystemFontType fontType));
  MOCK_METHOD(const OH_Drawing_String*,
              OH_Drawing_GetSystemFontFullNameByIndex,
              (OH_Drawing_Array * fullNameArray, size_t index));
  MOCK_METHOD(void,
              OH_Drawing_DestroySystemFontFullNames,
              (OH_Drawing_Array * fullNameArray));
  MOCK_METHOD(size_t,
              OH_Drawing_GetDrawingArraySize,
              (OH_Drawing_Array * drawingArray));
  MOCK_METHOD(OH_Drawing_FontConfigInfo*,
              OH_Drawing_GetSystemFontConfigInfo,
              (OH_Drawing_FontConfigInfoErrorCode * errorCode));
  MOCK_METHOD(void,
              OH_Drawing_DestroySystemFontConfigInfo,
              (OH_Drawing_FontConfigInfo * drawFontCfgInfo));
};

#ifdef __cplusplus
extern "C" {
#endif
OH_Drawing_FontDescriptor* OH_Drawing_MatchFontDescriptors(
    OH_Drawing_FontDescriptor* desc,
    size_t* num) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_MatchFontDescriptors(desc, num);
}
void OH_Drawing_DestroyFontDescriptors(OH_Drawing_FontDescriptor* descriptors,
                                       size_t num) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_DestroyFontDescriptors(descriptors, num);
}
OH_Drawing_FontDescriptor* OH_Drawing_GetFontDescriptorByFullName(
    const OH_Drawing_String* fullName,
    OH_Drawing_SystemFontType fontType) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_GetFontDescriptorByFullName(fullName, fontType);
}
OH_Drawing_Array* OH_Drawing_GetSystemFontFullNamesByType(
    OH_Drawing_SystemFontType fontType) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_GetSystemFontFullNamesByType(fontType);
}
const OH_Drawing_String* OH_Drawing_GetSystemFontFullNameByIndex(
    OH_Drawing_Array* fullNameArray,
    size_t index) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_GetSystemFontFullNameByIndex(fullNameArray, index);
}
void OH_Drawing_DestroySystemFontFullNames(OH_Drawing_Array* fullNameArray) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_DestroySystemFontFullNames(fullNameArray);
}
size_t OH_Drawing_GetDrawingArraySize(OH_Drawing_Array* drawingArray) {
  return MockOHDrawingTextSupport::getInstance().OH_Drawing_GetDrawingArraySize(
      drawingArray);
}
OH_Drawing_FontConfigInfo* OH_Drawing_GetSystemFontConfigInfo(
    OH_Drawing_FontConfigInfoErrorCode* errorCode) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_GetSystemFontConfigInfo(errorCode);
}
void OH_Drawing_DestroySystemFontConfigInfo(
    OH_Drawing_FontConfigInfo* drawFontCfgInfo) {
  return MockOHDrawingTextSupport::getInstance()
      .OH_Drawing_DestroySystemFontConfigInfo(drawFontCfgInfo);
}
#ifdef __cplusplus
}
#endif

class OhosDrawingTextAdapterImplTest : public ::testing::Test {};

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontFullNamesByType_001) {
  void* drawingArray = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontFullNamesByType(_))
      .WillOnce(Return(nullptr));
  int result =
      OhosDrawingTextFontAdapterImpl::GetInstance()
          .GetSystemFontFullNamesByType(0, &drawingArray);
  EXPECT_EQ(result, -1);
  EXPECT_EQ(drawingArray, nullptr);
}

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontFullNamesByType_002) {
  void* drawingArray = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontFullNamesByType(_))
      .WillOnce(Return(reinterpret_cast<OH_Drawing_Array*>(0x1234)));
  int result = OhosDrawingTextFontAdapterImpl::GetInstance()
                   .GetSystemFontFullNamesByType(
                       static_cast<int32_t>(OH_Drawing_SystemFontType::GENERIC),
                       &drawingArray);
  EXPECT_EQ(result, 0);
  EXPECT_NE(drawingArray, nullptr);
}

TEST_F(OhosDrawingTextAdapterImplTest, GetFontDescriptorByFullName_001) {
  OH_Drawing_String drawingString = {};
  OH_Drawing_SystemFontType systemFontType = OH_Drawing_SystemFontType::GENERIC;
  void* drawingFontDescriptor = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetFontDescriptorByFullName(_, _))
      .WillOnce(Return(nullptr));
  int result =
      OhosDrawingTextFontAdapterImpl::GetInstance().GetFontDescriptorByFullName(
          &drawingString, systemFontType, &drawingFontDescriptor);
  EXPECT_EQ(result, -1);
  EXPECT_EQ(drawingFontDescriptor, nullptr);
}

TEST_F(OhosDrawingTextAdapterImplTest, GetFontDescriptorByFullName_002) {
  OH_Drawing_String drawingString;
  OH_Drawing_SystemFontType systemFontType = OH_Drawing_SystemFontType::GENERIC;
  void* drawingFontDescriptor = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetFontDescriptorByFullName(_, _))
      .WillOnce(Return(reinterpret_cast<OH_Drawing_FontDescriptor*>(0x1234)));
  int result =
      OhosDrawingTextFontAdapterImpl::GetInstance().GetFontDescriptorByFullName(
          &drawingString, systemFontType, &drawingFontDescriptor);
  EXPECT_EQ(result, 0);
  EXPECT_NE(drawingFontDescriptor, nullptr);
}
struct OH_Drawing_Array {};

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontFullNameByIndex_001) {
  const void* drawingString = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontFullNameByIndex(_, _))
      .WillOnce(Return(nullptr));
  int result = OhosDrawingTextFontAdapterImpl::GetInstance()
                   .GetSystemFontFullNameByIndex(nullptr, 0, &drawingString);
  EXPECT_EQ(result, -1);
  EXPECT_EQ(drawingString, nullptr);
}

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontFullNameByIndex_002) {
  OH_Drawing_Array drawingArray;
  const char* expectedFontName = "TestFont";
  const void* mockFontName = static_cast<const void*>(expectedFontName);
  const void* drawingString = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontFullNameByIndex(_, _))
      .WillOnce(
          Return(reinterpret_cast<const OH_Drawing_String*>(mockFontName)));
  int result =
      OhosDrawingTextFontAdapterImpl::GetInstance()
          .GetSystemFontFullNameByIndex(&drawingArray, 0, &drawingString);
  EXPECT_EQ(result, 0);
  EXPECT_NE(drawingString, nullptr);
  EXPECT_STREQ(static_cast<const char*>(drawingString), expectedFontName);
}

TEST_F(OhosDrawingTextAdapterImplTest, DestroySystemFontFullNames_001) {
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_DestroySystemFontFullNames(_))
      .WillOnce(Return());
  OH_Drawing_Array drawingArray;
  EXPECT_NO_FATAL_FAILURE(
      OhosDrawingTextFontAdapterImpl::GetInstance().DestroySystemFontFullNames(
          &drawingArray));
}

class OhosDrawingTextTypographyAdapterImplTest : public ::testing::Test {};

TEST(OhosDrawingTextTypographyAdapterImplTest, GetSystemFontConfigInfo_001) {
  OH_Drawing_FontConfigInfoErrorCode errorCode;
  OH_Drawing_FontConfigInfo fontConfigInfoInstance;
  OH_Drawing_FontConfigInfo* fontConfigInfo = &fontConfigInfoInstance;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontConfigInfo(_))
      .WillOnce(Return(nullptr));
  int result = OhosDrawingTextTypographyAdapterImpl::GetInstance()
                   .GetSystemFontConfigInfo(
                       &errorCode, reinterpret_cast<void**>(&fontConfigInfo));

  EXPECT_EQ(result, -1);
  EXPECT_EQ(fontConfigInfo, nullptr);
}

TEST(OhosDrawingTextTypographyAdapterImplTest, GetSystemFontConfigInfo_002) {
  OH_Drawing_FontConfigInfoErrorCode errorCode;
  OH_Drawing_FontConfigInfo* fontConfigInfo = nullptr;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetSystemFontConfigInfo(_))
      .WillOnce(Return(reinterpret_cast<OH_Drawing_FontConfigInfo*>(0x1234)));
  int result = OhosDrawingTextTypographyAdapterImpl::GetInstance()
                   .GetSystemFontConfigInfo(
                       &errorCode, reinterpret_cast<void**>(&fontConfigInfo));

  EXPECT_EQ(result, 0);
  EXPECT_NE(fontConfigInfo, nullptr);
}

TEST(OhosDrawingTextTypographyAdapterImplTest, GetDrawingArraySize_001) {
  void* drawingArray = nullptr;
  int32_t sizeOfArray = 0;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetDrawingArraySize(_))
      .WillOnce(Return(-1));
  int result =
      OhosDrawingTextTypographyAdapterImpl::GetInstance().GetDrawingArraySize(
          drawingArray, sizeOfArray);

  EXPECT_EQ(result, -1);
}

TEST(OhosDrawingTextTypographyAdapterImplTest, GetDrawingArraySize_002) {
  void* drawingArray = nullptr;
  int32_t sizeOfArray = 0;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_GetDrawingArraySize(_))
      .WillOnce(Return(1));
  int result =
      OhosDrawingTextTypographyAdapterImpl::GetInstance().GetDrawingArraySize(
          drawingArray, sizeOfArray);

  EXPECT_EQ(result, 0);
}

TEST(OhosDrawingTextTypographyAdapterImplTest,
     DestroySystemFontConfigInfo_001) {
  OH_Drawing_FontConfigInfo fontConfigInfo;
  EXPECT_CALL(MockOHDrawingTextSupport::getInstance(),
              OH_Drawing_DestroySystemFontConfigInfo(_))
      .WillOnce(Return());
  EXPECT_NO_FATAL_FAILURE(OhosDrawingTextTypographyAdapterImpl::GetInstance()
                              .DestroySystemFontConfigInfo(&fontConfigInfo));
}

}  // namespace OHOS::NWeb
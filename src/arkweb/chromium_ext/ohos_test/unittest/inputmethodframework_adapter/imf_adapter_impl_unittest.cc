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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "base/logging.h"
#include <inputmethod/inputmethod_controller_capi.h>
#define private public
#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.cpp"

namespace OHOS::NWeb {

class MockInputMethod {
public:
    MOCK_METHOD(InputMethod_ErrorCode, OH_TextConfig_GetCursorInfo,
                (InputMethod_TextConfig*, InputMethod_CursorInfo**));
    MOCK_METHOD(InputMethod_ErrorCode, OH_CursorInfo_GetRect,
                (InputMethod_CursorInfo*, double*, double*, double*, double*));
    MOCK_METHOD(InputMethod_ErrorCode, OH_CursorInfo_SetRect,
                (InputMethod_CursorInfo*, double, double, double, double));
};

class MockIMFTextListenerAdapter : public IMFTextListenerAdapter {
 public:
  MOCK_METHOD(void, InsertText, (const std::u16string& text), (override));
  MOCK_METHOD(void, DeleteForward, (int32_t length), (override));
  MOCK_METHOD(void, DeleteBackward, (int32_t length), (override));
  MOCK_METHOD(void, SendKeyEventFromInputMethod, (), (override));
  MOCK_METHOD(void,
              SendKeyboardStatus,
              (const IMFAdapterKeyboardStatus& keyboardStatus),
              (override));
  MOCK_METHOD(void,
              SendFunctionKey,
              (std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey),
              (override));
  MOCK_METHOD(void, SetKeyboardStatus, (bool status), (override));
  MOCK_METHOD(void,
              MoveCursor,
              (const IMFAdapterDirection direction),
              (override));
  MOCK_METHOD(void,
              HandleSetSelection,
              (int32_t start, int32_t end),
              (override));
  MOCK_METHOD(void, HandleExtendAction, (int32_t action), (override));
  MOCK_METHOD(void,
              HandleSelect,
              (int32_t keyCode, int32_t cursorMoveSkip),
              (override));
  MOCK_METHOD(int32_t, GetTextIndexAtCursor, (), (override));
  MOCK_METHOD(std::u16string,
              GetLeftTextOfCursor,
              (int32_t number),
              (override));
  MOCK_METHOD(std::u16string,
              GetRightTextOfCursor,
              (int32_t number),
              (override));
  MOCK_METHOD(int32_t,
              SetPreviewText,
              (const std::u16string& text, int32_t start, int32_t end),
              (override));
  MOCK_METHOD(void, FinishTextPreview, (), (override));
  MOCK_METHOD(void, SetNeedUnderLine, (bool isNeedUnderline), (override));
  MOCK_METHOD(void,
              AutoFillWithIMFEvent,
              (bool isUsername,
               bool isOtherAccount,
               bool isNewPassword,
               const std::string& content),
              (override));
  MOCK_METHOD(void, KeyboardUpperRightCornerHide, (), (override));
};

class MockIMFTextConfigAdapter : public IMFTextConfigAdapter {
 public:
  MOCK_METHOD(std::shared_ptr<IMFInputAttributeAdapter>,
              GetInputAttribute,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<IMFCursorInfoAdapter>,
              GetCursorInfo,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<IMFSelectionRangeAdapter>,
              GetSelectionRange,
              (),
              (override));
  MOCK_METHOD(uint32_t, GetWindowId, (), (override));
  MOCK_METHOD(double, GetPositionY, (), (override));
  MOCK_METHOD(double, GetHeight, (), (override));
};

class MockIMFCursorInfoAdapter : public IMFCursorInfoAdapter {
 public:
  MOCK_METHOD(double, GetLeft, (), (override));
  MOCK_METHOD(double, GetTop, (), (override));
  MOCK_METHOD(double, GetWidth, (), (override));
  MOCK_METHOD(double, GetHeight, (), (override));
};

class MockIMFInputAttributeAdapter : public IMFInputAttributeAdapter {
 public:
  MOCK_METHOD(int32_t, GetInputPattern, (), (override));
  MOCK_METHOD(int32_t, GetEnterKeyType, (), (override));
};

class IMFTextEditorProxyImplTest : public ::testing::Test {
 protected:
  void SetUp() {
    key_adapter_ = std::make_shared<IMFAdapterFunctionKeyAdapterImpl>();
  }
  void TearDown() { key_adapter_.reset(); }
  std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> key_adapter_ = nullptr;
  MockInputMethod mock;
};

TEST_F(IMFTextEditorProxyImplTest, AdapterTextInputTypeToTextInputType) {
  InputMethod_TextInputType result = AdapterTextInputTypeToTextInputType(1);
  EXPECT_EQ(result, InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_MULTILINE);
  result = AdapterTextInputTypeToTextInputType(-1);
  result = AdapterTextInputTypeToTextInputType(0);
  result = AdapterTextInputTypeToTextInputType(2);
  result = AdapterTextInputTypeToTextInputType(3);
  result = AdapterTextInputTypeToTextInputType(4);
  result = AdapterTextInputTypeToTextInputType(5);
  result = AdapterTextInputTypeToTextInputType(6);
  result = AdapterTextInputTypeToTextInputType(7);
  result = AdapterTextInputTypeToTextInputType(8);
  result = AdapterTextInputTypeToTextInputType(9);
  result = AdapterTextInputTypeToTextInputType(10);
  result = AdapterTextInputTypeToTextInputType(11);
  result = AdapterTextInputTypeToTextInputType(12);
  result = AdapterTextInputTypeToTextInputType(20);
  EXPECT_EQ(result, InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_NONE);
}

TEST_F(IMFTextEditorProxyImplTest, AdapterEnterKeyTypeToOhEnterKeyType) {
  InputMethod_EnterKeyType result = AdapterEnterKeyTypeToOhEnterKeyType(1);
  EXPECT_EQ(result, InputMethod_EnterKeyType::IME_ENTER_KEY_NONE);
  result = AdapterEnterKeyTypeToOhEnterKeyType(0);
  result = AdapterEnterKeyTypeToOhEnterKeyType(2);
  result = AdapterEnterKeyTypeToOhEnterKeyType(3);
  result = AdapterEnterKeyTypeToOhEnterKeyType(4);
  result = AdapterEnterKeyTypeToOhEnterKeyType(5);
  result = AdapterEnterKeyTypeToOhEnterKeyType(6);
  result = AdapterEnterKeyTypeToOhEnterKeyType(7);
  result = AdapterEnterKeyTypeToOhEnterKeyType(8);
  result = AdapterEnterKeyTypeToOhEnterKeyType(20);
  EXPECT_EQ(result, InputMethod_EnterKeyType::IME_ENTER_KEY_UNSPECIFIED);
}

TEST_F(IMFTextEditorProxyImplTest, ohKeyboardStatusToAdapterKeyboardStatus) {
  IMFAdapterKeyboardStatus result = ohKeyboardStatusToAdapterKeyboardStatus(
      InputMethod_KeyboardStatus::IME_KEYBOARD_STATUS_HIDE);
  EXPECT_EQ(result, IMFAdapterKeyboardStatus::HIDE);
  result = ohKeyboardStatusToAdapterKeyboardStatus(
    InputMethod_KeyboardStatus::IME_KEYBOARD_STATUS_NONE);
  result = ohKeyboardStatusToAdapterKeyboardStatus(
    InputMethod_KeyboardStatus::IME_KEYBOARD_STATUS_SHOW);
}

TEST_F(IMFTextEditorProxyImplTest, GetCursorInfo) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetCursorInfo(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetCursorInfo(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  src = OH_TextConfig_Create();
  result = GetCursorInfo(nullptr, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetTextAvoidInfo) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetTextAvoidInfo(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetTextAvoidInfo(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetTextAvoidInfo(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetInputType) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetInputType(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  result = GetInputType(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetInputType(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetInputType(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetEnterKeyType) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetEnterKeyType(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  result = GetEnterKeyType(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetEnterKeyType(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetEnterKeyType(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetSelection) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetSelection(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  result = GetSelection(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetSelection(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetSelection(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetWindowId) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetWindowId(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  result = GetWindowId(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetWindowId(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetWindowId(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, SendKeyboardStatusFunc) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  InputMethod_KeyboardStatus ohStatus =
      InputMethod_KeyboardStatus::IME_KEYBOARD_STATUS_HIDE;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::SendKeyboardStatusFunc(proxy, ohStatus);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::SendKeyboardStatusFunc(proxy, ohStatus);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, SendEnterKeyFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_UNSPECIFIED);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  EXPECT_CALL(*listener, SendFunctionKey(testing::_)).Times(1);
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_UNSPECIFIED);
  IMFTextEditorProxyImpl::textListener_.reset();
}

TEST_F(IMFTextEditorProxyImplTest, SendEnterKeyFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*listener, SendFunctionKey(testing::_)).Times(2);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_NONE);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_GO);
  IMFTextEditorProxyImpl::textListener_.reset();
}

TEST_F(IMFTextEditorProxyImplTest, SendEnterKeyFunc_003) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*listener, SendFunctionKey(testing::_)).Times(2);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_SEARCH);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_SEND);
  IMFTextEditorProxyImpl::textListener_.reset();
}

TEST_F(IMFTextEditorProxyImplTest, SendEnterKeyFunc_004) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*listener, SendFunctionKey(testing::_)).Times(2);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_NEXT);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_DONE);
  IMFTextEditorProxyImpl::textListener_.reset();
}

TEST_F(IMFTextEditorProxyImplTest, SendEnterKeyFunc_005) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*listener, SendFunctionKey(testing::_)).Times(2);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_PREVIOUS);
  IMFTextEditorProxyImpl::SendEnterKeyFunc(
      proxy, InputMethod_EnterKeyType::IME_ENTER_KEY_NEWLINE);
  IMFTextEditorProxyImpl::textListener_.reset();
}

TEST_F(IMFTextEditorProxyImplTest, GetPreviewTextSupported) {
  InputMethod_TextConfig* dest = nullptr;
  InputMethod_TextConfig* src = nullptr;
  InputMethod_ErrorCode result = GetPreviewTextSupported(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  src = OH_TextConfig_Create();
  ASSERT_NE(nullptr, src);
  result = GetPreviewTextSupported(dest, src);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  dest = OH_TextConfig_Create();
  ASSERT_NE(nullptr, dest);
  result = GetPreviewTextSupported(dest, src);
  EXPECT_EQ(result, IME_ERR_OK);
  result = GetPreviewTextSupported(dest, nullptr);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
}

TEST_F(IMFTextEditorProxyImplTest, GetTextConfigFunc) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  InputMethod_TextConfig* config = OH_TextConfig_Create();
  IMFTextEditorProxyImpl::textConfig_ = nullptr;
  IMFTextEditorProxyImpl::GetTextConfigFunc(proxy, config);
  EXPECT_EQ(IMFTextEditorProxyImpl::textConfig_, nullptr);
  IMFTextEditorProxyImpl::textConfig_ = OH_TextConfig_Create();
  IMFTextEditorProxyImpl::GetTextConfigFunc(proxy, config);
  EXPECT_NE(IMFTextEditorProxyImpl::textConfig_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, InsertTextFunc) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  char16_t* text = new char16_t[14];
  std::memcpy(text, "Hello, World!", 14 * sizeof(char16_t));
  size_t length = 1;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::InsertTextFunc(proxy, text, length);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::InsertTextFunc(proxy, text, length);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
  delete[] text;
}

TEST_F(IMFTextEditorProxyImplTest, DeleteForwardFunc) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t length = 1;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::DeleteForwardFunc(proxy, length);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::DeleteForwardFunc(proxy, length);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, DeleteBackwardFunc) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t length = 1;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::DeleteBackwardFunc(proxy, length);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::DeleteBackwardFunc(proxy, length);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, MoveCursorFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::MoveCursorFunc(
      proxy, InputMethod_Direction::IME_DIRECTION_NONE);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, MoveCursorFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::MoveCursorFunc(
      proxy, InputMethod_Direction::IME_DIRECTION_NONE);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, HandleSetSelectionFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t start = 0;
  int32_t end = 0;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::HandleSetSelectionFunc(proxy, start, end);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, HandleSetSelectionFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t start = 0;
  int32_t end = 0;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::HandleSetSelectionFunc(proxy, start, end);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, HandleExtendActionFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::HandleExtendActionFunc(
      proxy, InputMethod_ExtendAction::IME_EXTEND_ACTION_SELECT_ALL);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, HandleExtendActionFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::HandleExtendActionFunc(
      proxy, InputMethod_ExtendAction::IME_EXTEND_ACTION_SELECT_ALL);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, GetLeftTextOfCursorFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t number = 0;
  char16_t text[] = u"a";
  size_t length = 1;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, number, text,
                                                  p_length);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, GetLeftTextOfCursorFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t number = 0;
  char16_t text[] = u"a";
  size_t length = 1;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, number, text,
                                                  p_length);
  EXPECT_EQ(length, 0);
}

TEST_F(IMFTextEditorProxyImplTest, GetRightTextOfCursorFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t number = 0;
  char16_t text[] = u"a";
  size_t length = 1;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, number, text,
                                                   p_length);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, GetRightTextOfCursorFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  int32_t number = 0;
  char16_t text[] = u"a";
  size_t length = 1;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, number, text,
                                                   p_length);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, GetTextIndexAtCursorFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  int result = IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(proxy);
  EXPECT_EQ(result, -1);
}

TEST_F(IMFTextEditorProxyImplTest, GetTextIndexAtCursorFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(proxy);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, ReceivePrivateCommandFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  char key[] = "exampleKey";
  size_t keyLength = strlen(key);
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t size = 1;
  IMFTextEditorProxyImpl::textListener_.reset();
  int result =
      IMFTextEditorProxyImpl::ReceivePrivateCommandFunc(proxy, commands, size);
  EXPECT_EQ(result, 0);
}

TEST_F(IMFTextEditorProxyImplTest, ReceivePrivateCommandFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  char key[] = "exampleKey";
  size_t keyLength = strlen(key);
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t size = 1;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::ReceivePrivateCommandFunc(proxy, commands, size);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, SetPreviewTextFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  const char16_t* text = u"a";
  size_t length = 1;
  int32_t start = 0;
  int32_t end = 0;
  IMFTextEditorProxyImpl::textListener_.reset();
  int result = IMFTextEditorProxyImpl::SetPreviewTextFunc(proxy, text, length,
                                                          start, end);
  EXPECT_EQ(result, 0);
}

TEST_F(IMFTextEditorProxyImplTest, SetPreviewTextFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  const char16_t* text = u"a";
  size_t length = 1;
  int32_t start = 0;
  int32_t end = 0;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::SetPreviewTextFunc(proxy, text, length, start, end);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, FinishTextPreviewFunc_001) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::FinishTextPreviewFunc(proxy);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, FinishTextPreviewFunc_002) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  ASSERT_NE(nullptr, proxy);
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::FinishTextPreviewFunc(proxy);
  EXPECT_NE(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, TextEditorProxy_SetTextFunc) {
  InputMethod_TextEditorProxy* textEditorProxy = nullptr;
  InputMethod_ErrorCode result =
      IMFTextEditorProxyImpl::TextEditorProxy_SetTextFunc(textEditorProxy);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  textEditorProxy = OH_TextEditorProxy_Create();
  result = IMFTextEditorProxyImpl::TextEditorProxy_SetTextFunc(textEditorProxy);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, TextEditorProxy_SetFunc) {
  InputMethod_TextEditorProxy* textEditorProxy = nullptr;
  InputMethod_ErrorCode result =
      IMFTextEditorProxyImpl::TextEditorProxy_SetFunc(textEditorProxy);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  textEditorProxy = OH_TextEditorProxy_Create();
  result = IMFTextEditorProxyImpl::TextEditorProxy_SetFunc(textEditorProxy);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, TextEditorProxyCreate) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  InputMethod_TextEditorProxy* result =
      IMFTextEditorProxyImpl::TextEditorProxyCreate(listener);
  EXPECT_NE(result, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, TextEditorProxyDestroy) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_.reset();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  IMFTextEditorProxyImpl::TextEditorProxyDestroy(proxy);
  EXPECT_EQ(IMFTextEditorProxyImpl::textListener_, nullptr);
}

TEST_F(IMFTextEditorProxyImplTest, ConstructCursorInfo_001) {
  InputMethod_TextConfig* textConfig = nullptr;
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  InputMethod_ErrorCode result = ConstructCursorInfo(textConfig, config);
  EXPECT_EQ(result, IME_ERR_NULL_POINTER);
  textConfig = OH_TextConfig_Create();
  result = ConstructCursorInfo(textConfig, config);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, ConstructCursorInfo_002) {
  InputMethod_TextConfig* textConfig = OH_TextConfig_Create();
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  InputMethod_ErrorCode result = ConstructCursorInfo(textConfig, config);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, ConstructTextConfig_001) {
  InputMethod_TextConfig* textConfig = nullptr;
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  InputMethod_ErrorCode result =
      IMFTextEditorProxyImpl::ConstructTextConfig(config);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, ConstructTextConfig_002) {
  InputMethod_TextConfig* textConfig = OH_TextConfig_Create();
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  InputMethod_ErrorCode result =
      IMFTextEditorProxyImpl::ConstructTextConfig(config);
  EXPECT_EQ(result, IME_ERR_OK);
}

TEST_F(IMFTextEditorProxyImplTest, DestroyTextConfig) {
  IMFTextEditorProxyImpl::textConfig_ = OH_TextConfig_Create();
  IMFTextEditorProxyImpl::DestroyTextConfig();
  EXPECT_EQ(IMFTextEditorProxyImpl::textConfig_, nullptr);
}

class IMFTextListenerAdapterImplTest : public ::testing::Test {
 protected:
  void SetUp() {
    mock_listener_adapter_ = std::make_shared<MockIMFTextListenerAdapter>();
    listener_adapter_ =
        std::make_shared<IMFTextListenerAdapterImpl>(mock_listener_adapter_);
    key_adapter_ = std::make_shared<IMFAdapterFunctionKeyAdapterImpl>();
  }
  void TearDown() {
    mock_listener_adapter_.reset();
    listener_adapter_.reset();
    key_adapter_.reset();
  }
  std::shared_ptr<IMFTextListenerAdapterImpl> listener_adapter_ = nullptr;
  std::shared_ptr<MockIMFTextListenerAdapter> mock_listener_adapter_ = nullptr;
  std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> key_adapter_ = nullptr;
};

TEST_F(IMFTextListenerAdapterImplTest, InsertText) {
  std::u16string text = u"test text";
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->InsertText(text);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->InsertText(text);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, DeleteForward) {
  int32_t length = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->DeleteForward(length);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->DeleteForward(length);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, DeleteBackward) {
  int32_t length = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->DeleteBackward(length);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->DeleteBackward(length);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, SendKeyEventFromInputMethod) {
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->SendKeyEventFromInputMethod();
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->SendKeyEventFromInputMethod();
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, SendKeyboardStatus) {
  IMFAdapterKeyboardStatus keyboardStatus = IMFAdapterKeyboardStatus::SHOW;
  const IMFAdapterKeyboardStatus& status = keyboardStatus;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->SendKeyboardStatus(status);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->SendKeyboardStatus(status);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, SendFunctionKey) {
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->SendFunctionKey(key_adapter_);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->SendFunctionKey(key_adapter_);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, SetKeyboardStatus) {
  bool status = true;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->SetKeyboardStatus(status);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->SetKeyboardStatus(status);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, MoveCursor) {
  const IMFAdapterDirection direction = IMFAdapterDirection::NONE;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->MoveCursor(direction);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->MoveCursor(direction);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, HandleSetSelection) {
  int32_t start = 0;
  int32_t end = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->HandleSetSelection(start, end);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->HandleSetSelection(start, end);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, HandleExtendAction) {
  int32_t action = 0;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->HandleExtendAction(action);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->HandleExtendAction(action);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, HandleSelect) {
  int32_t keyCode = 0;
  int32_t cursorMoveSkip = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->HandleSetSelection(keyCode, cursorMoveSkip);
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->HandleSetSelection(keyCode, cursorMoveSkip);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, GetTextIndexAtCursor) {
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  int32_t result = listener_adapter_->GetTextIndexAtCursor();
  EXPECT_EQ(result, -1);
  listener_adapter_->listener_ = mock_listener_adapter_;
  result = listener_adapter_->GetTextIndexAtCursor();
  EXPECT_NE(result, -1);
}

TEST_F(IMFTextListenerAdapterImplTest, GetLeftTextOfCursor) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  int32_t number = 3;
  char16_t text[] = u"Test Text";
  size_t length = 10;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, number, text,
                                                  p_length);
  EXPECT_EQ(*p_length, 0);
  listener_adapter_->listener_ = mock_listener_adapter_;
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, number, text,
                                                  p_length);
  EXPECT_EQ(*p_length, 0);
}

TEST_F(IMFTextListenerAdapterImplTest, GetRightTextOfCursor) {
  InputMethod_TextEditorProxy* proxy = OH_TextEditorProxy_Create();
  int32_t number = 3;
  char16_t text[] = u"Test Text";
  size_t length = 10;
  size_t* p_length = &length;
  IMFTextEditorProxyImpl::textListener_.reset();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, number, text,
                                                   p_length);
  EXPECT_EQ(*p_length, 0);
  listener_adapter_->listener_ = mock_listener_adapter_;
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, number, text,
                                                   p_length);
  EXPECT_EQ(*p_length, 0);
}

TEST_F(IMFTextListenerAdapterImplTest, SetPreviewText) {
  const std::u16string& text = u"test text";
  int32_t start = 1;
  int32_t end = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  int32_t result = listener_adapter_->SetPreviewText(text, start, end);
  EXPECT_EQ(result, -1);
  listener_adapter_->listener_ = mock_listener_adapter_;
  result = listener_adapter_->SetPreviewText(text, start, end);
  EXPECT_NE(result, -1);
}

TEST_F(IMFTextListenerAdapterImplTest, FinishTextPreview) {
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->FinishTextPreview();
  EXPECT_EQ(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  listener_adapter_->FinishTextPreview();
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, ReceivePrivateCommand_001) {
  char key[] = "";
  size_t keyLength = strlen(key);
  ASSERT_NE(listener_adapter_, nullptr);
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t num = 1;
  int32_t result = listener_adapter_->ReceivePrivateCommand(commands, num);
  EXPECT_EQ(result, 0);
}

TEST_F(IMFTextListenerAdapterImplTest, ReceivePrivateCommand_002) {
  InputMethod_PrivateCommand* commands[1] = {nullptr};
  size_t num = 1;
  ASSERT_NE(listener_adapter_, nullptr);
  int32_t result = listener_adapter_->ReceivePrivateCommand(commands, num);
  EXPECT_EQ(result, 0);
}

TEST_F(IMFTextListenerAdapterImplTest, ReceivePrivateCommand_003) {
  char key[] = "previewTextStyle";
  size_t keyLength = strlen(key);
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  char value[] = "underline";
  size_t valueLength = strlen(value);
  OH_PrivateCommand_SetStrValue(privateCommand, value, valueLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t num = 1;
  int32_t result = listener_adapter_->ReceivePrivateCommand(commands, num);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
  listener_adapter_->listener_ = nullptr;
  listener_adapter_->ReceivePrivateCommand(commands, num);
}

TEST_F(IMFTextListenerAdapterImplTest, ReceivePrivateCommand_004) {
  char key[] = "com.autofill.params.userName";
  size_t keyLength = strlen(key);
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  char value[] = "underline";
  size_t valueLength = strlen(value);
  OH_PrivateCommand_SetStrValue(privateCommand, value, valueLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t num = 1;
  int32_t result = listener_adapter_->ReceivePrivateCommand(commands, num);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

TEST_F(IMFTextListenerAdapterImplTest, ReceivePrivateCommand_005) {
  char key[] = "com.autofill.params.otherAccount";
  size_t keyLength = strlen(key);
  ASSERT_NE(listener_adapter_, nullptr);
  listener_adapter_->listener_ = mock_listener_adapter_;
  InputMethod_PrivateCommand* privateCommand =
      OH_PrivateCommand_Create(key, keyLength);
  char value[] = "underline";
  size_t valueLength = strlen(value);
  OH_PrivateCommand_SetStrValue(privateCommand, value, valueLength);
  InputMethod_PrivateCommand* commands[1] = {privateCommand};
  size_t num = 1;
  int32_t result = listener_adapter_->ReceivePrivateCommand(commands, num);
  EXPECT_NE(listener_adapter_->listener_, nullptr);
}

class IMFAdapterImplTest : public ::testing::Test {
 protected:
  void SetUp() { imf_adapter_ = std::make_shared<IMFAdapterImpl>(); }
  void TearDown() { imf_adapter_.reset(); }
  std::shared_ptr<IMFAdapterImpl> imf_adapter_ = nullptr;
};

TEST_F(IMFAdapterImplTest, Attach_WithTwoParams) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = false;
  ASSERT_NE(imf_adapter_, nullptr);
  bool result = imf_adapter_->Attach(listener, isShowKeyboard);
  EXPECT_EQ(result, false);
  listener = nullptr;
  imf_adapter_->textEditorProxy_ = nullptr;
  IMFTextEditorProxyImpl::textListener_.reset();
  result = imf_adapter_->Attach(listener, isShowKeyboard);
  EXPECT_EQ(result, false);
  listener = std::make_shared<MockIMFTextListenerAdapter>();
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  isShowKeyboard = true;
  result = imf_adapter_->Attach(listener, isShowKeyboard);
  EXPECT_EQ(result, false);
}

TEST_F(IMFAdapterImplTest, Attach_WithFourParams_001) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener = nullptr;
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config = nullptr;
  bool isResetListener = true;
  ASSERT_NE(imf_adapter_, nullptr);
  bool result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_FALSE(result);
}
TEST_F(IMFAdapterImplTest, Attach_WithFourParams_ConfigFlase) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config = nullptr;
  bool isResetListener = true;
  ASSERT_NE(imf_adapter_, nullptr);
  bool result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_EQ(result, false);

  config = std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute = nullptr;
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  isShowKeyboard = true;
  result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_EQ(result, false);

  attribute = std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor = nullptr;
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_EQ(result, false);
}
TEST_F(IMFAdapterImplTest, Attach_WithFourParams_003) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  bool isResetListener = false;
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  ASSERT_NE(imf_adapter_, nullptr);
  imf_adapter_->textEditorProxy_ =
      IMFTextEditorProxyImpl::TextEditorProxyCreate(listener);
  bool result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_TRUE(result);
  isResetListener = true;
  result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_TRUE(result);
}

TEST_F(IMFAdapterImplTest, Attach_WithFourParams_004) {
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  ASSERT_NE(imf_adapter_, nullptr);
  bool isShowKeyboard = true;
  bool isResetListener = false;
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*config, GetWindowId()).WillRepeatedly(testing::Return(0));
  bool result =
      imf_adapter_->Attach(listener, isShowKeyboard, config, isResetListener);
  EXPECT_TRUE(result);
}

TEST_F(IMFAdapterImplTest, AttachWithRequestKeyboardReason_001) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener = nullptr;
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config = nullptr;
  bool isResetListener = true;
  int32_t requestKeyboardReason = 1;
  ASSERT_NE(imf_adapter_, nullptr);
  bool result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_FALSE(result);
}

TEST_F(IMFAdapterImplTest, AttachWithRequestKeyboardReason_ConfigFlase) {
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config = nullptr;
  bool isResetListener = true;
  int32_t requestKeyboardReason = 1;
  ASSERT_NE(imf_adapter_, nullptr);
  bool result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_EQ(result, false);

  config = std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute = nullptr;
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  isShowKeyboard = true;
  ASSERT_NE(imf_adapter_, nullptr);
  result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_EQ(result, false);

  attribute = std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor = nullptr;
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_EQ(result, false);
}

TEST_F(IMFAdapterImplTest, AttachWithRequestKeyboardReason_003) {
  ASSERT_NE(imf_adapter_, nullptr);
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = false;
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  bool isResetListener = false;
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  imf_adapter_->textEditorProxy_ =
      IMFTextEditorProxyImpl::TextEditorProxyCreate(listener);
  int32_t requestKeyboardReason = 1;
  bool result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_TRUE(result);
  isResetListener = true;
  result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_TRUE(result);
}

TEST_F(IMFAdapterImplTest, AttachWithRequestKeyboardReason_004) {
  ASSERT_NE(imf_adapter_, nullptr);
  std::shared_ptr<MockIMFTextConfigAdapter> config =
      std::make_shared<MockIMFTextConfigAdapter>();
  std::shared_ptr<MockIMFTextListenerAdapter> listener =
      std::make_shared<MockIMFTextListenerAdapter>();
  bool isShowKeyboard = true;
  bool isResetListener = false;
  int32_t requestKeyboardReason = 1;
  std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
      std::make_shared<MockIMFInputAttributeAdapter>();
  EXPECT_CALL(*config, GetInputAttribute())
      .WillRepeatedly(testing::Return(attribute));
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  EXPECT_CALL(*config, GetCursorInfo()).WillRepeatedly(testing::Return(cursor));
  IMFTextEditorProxyImpl::textListener_ =
      std::make_shared<IMFTextListenerAdapterImpl>(listener);
  EXPECT_CALL(*config, GetWindowId()).WillRepeatedly(testing::Return(0));
  bool result = imf_adapter_->AttachWithRequestKeyboardReason(
      listener, isShowKeyboard, config, isResetListener, requestKeyboardReason);
  EXPECT_TRUE(result);
}

TEST_F(IMFAdapterImplTest, ShowCurrentInput) {
  ASSERT_NE(imf_adapter_, nullptr);
  IMFAdapterTextInputType inputType = IMFAdapterTextInputType::NUMBER;
  IMFTextEditorProxyImpl::textConfig_ = nullptr;
  imf_adapter_->ShowCurrentInput(inputType);
  EXPECT_EQ(IMFTextEditorProxyImpl::textConfig_, nullptr);
  IMFTextEditorProxyImpl::textConfig_ = OH_TextConfig_Create();
  imf_adapter_->ShowCurrentInput(inputType);
  EXPECT_NE(IMFTextEditorProxyImpl::textConfig_, nullptr);
  inputType = IMFAdapterTextInputType::NONE;
  imf_adapter_->inputMethodProxy_ = nullptr;
  imf_adapter_->ShowCurrentInput(inputType);
  EXPECT_NE(IMFTextEditorProxyImpl::textConfig_, nullptr);
}

TEST_F(IMFAdapterImplTest, HideTextInput) {
  ASSERT_NE(imf_adapter_, nullptr);
  imf_adapter_->inputMethodProxy_ = nullptr;
  imf_adapter_->HideTextInput();
  imf_adapter_->Close();
  EXPECT_EQ(imf_adapter_->inputMethodProxy_, nullptr);

  InputMethod_TextEditorProxy *textEditorProxy = OH_TextEditorProxy_Create();
  bool showKeyboard = true;
  InputMethod_AttachOptions *options = OH_AttachOptions_Create(showKeyboard);
  InputMethod_InputMethodProxy *inputMethodProxy = nullptr;
  OH_InputMethodController_Attach(textEditorProxy, options, &inputMethodProxy);
  imf_adapter_->inputMethodProxy_ = inputMethodProxy;
  imf_adapter_->HideTextInput();
  imf_adapter_->Close();
}

TEST_F(IMFAdapterImplTest, OnCursorUpdate_001) {
  ASSERT_NE(imf_adapter_, nullptr);
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor = nullptr;
  imf_adapter_->OnCursorUpdate(cursor);

  cursor = std::make_shared<MockIMFCursorInfoAdapter>();
  IMFTextEditorProxyImpl::textConfig_ = nullptr;
  imf_adapter_->OnCursorUpdate(cursor);

  IMFTextEditorProxyImpl::textConfig_ = OH_TextConfig_Create();
  EXPECT_CALL(*cursor, GetLeft()).WillRepeatedly(testing::Return(0.f));
  EXPECT_CALL(*cursor, GetTop()).WillRepeatedly(testing::Return(0.f));
  EXPECT_CALL(*cursor, GetWidth()).WillRepeatedly(testing::Return(0.f));
  EXPECT_CALL(*cursor, GetHeight()).WillRepeatedly(testing::Return(0.f));
  imf_adapter_->OnCursorUpdate(cursor);
  EXPECT_NE(cursor, nullptr);
}

TEST_F(IMFAdapterImplTest, OnCursorUpdate_002) {
  std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
      std::make_shared<MockIMFCursorInfoAdapter>();
  ASSERT_NE(imf_adapter_, nullptr);
  IMFTextEditorProxyImpl::textConfig_ = OH_TextConfig_Create();
  imf_adapter_->OnCursorUpdate(cursor);
  EXPECT_NE(cursor, nullptr);
}

TEST_F(IMFAdapterImplTest, OnSelectionChange) {
  std::u16string text = u"test text";
  int start = 1;
  int end = 3;
  ASSERT_NE(imf_adapter_, nullptr);
  imf_adapter_->inputMethodProxy_ = nullptr;
  imf_adapter_->OnSelectionChange(text, start, end);
  EXPECT_EQ(imf_adapter_->inputMethodProxy_, nullptr);
}

TEST_F(IMFAdapterImplTest, SendPrivateCommand) {
  const std::string autoFillText = "autofill.cancel";
  std::string commandKey = "";
  std::string commandValue = "";
  ASSERT_NE(imf_adapter_, nullptr);
  bool result = imf_adapter_->SendPrivateCommand(commandKey, commandValue);
  EXPECT_FALSE(result);
  commandKey = autoFillText;
  result = imf_adapter_->SendPrivateCommand(commandKey, commandValue);
  EXPECT_FALSE(result);
}

  TEST_F(IMFAdapterImplTest, ParseFillContentJsonValue_001) {
    std::string jsonInput = R"({"userName":})";
    std::vector<InputMethod_PrivateCommand*> privateCommands;
    bool result = imf_adapter_->ParseFillContentJsonValue(jsonInput, privateCommands);
    EXPECT_FALSE(result);

    jsonInput = "null";
    result = imf_adapter_->ParseFillContentJsonValue(jsonInput, privateCommands);
    EXPECT_FALSE(result);
  }

  TEST_F(IMFAdapterImplTest, ParseFillContentJsonValue_002) {
    std::vector<InputMethod_PrivateCommand*> privateCommands;
    std::string jsonInput = R"({"otherName":"","hasnoAccount":""})";
    bool result = imf_adapter_->ParseFillContentJsonValue(jsonInput, privateCommands);
    EXPECT_TRUE(result);

    jsonInput = R"({"userName":111,"hasAccount":111})";
    result = imf_adapter_->ParseFillContentJsonValue(jsonInput, privateCommands);
    EXPECT_TRUE(result);

    jsonInput = R"({"userName":"name","hasAccount":"account"})";
    result = imf_adapter_->ParseFillContentJsonValue(jsonInput, privateCommands);
    EXPECT_TRUE(result);
  }

  TEST_F(IMFAdapterImplTest, AttachParamsCheck_001) {
    std::shared_ptr<MockIMFTextListenerAdapter> listener = nullptr;
    bool isShowKeyboard = false;
    std::shared_ptr<MockIMFTextConfigAdapter> config = nullptr;
    bool isResetListener = false;
    bool result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_FALSE(result);

    listener = std::make_shared<MockIMFTextListenerAdapter>();
    result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_FALSE(result);
    config = std::make_shared<MockIMFTextConfigAdapter>();
    std::shared_ptr<MockIMFInputAttributeAdapter> attribute = nullptr;
    EXPECT_CALL(*config, GetInputAttribute())
        .WillOnce(testing::Return(attribute));
    result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_FALSE(result);

    attribute = std::make_shared<MockIMFInputAttributeAdapter>();
    EXPECT_CALL(*config, GetInputAttribute())
      .WillOnce(testing::Return(attribute));
    std::shared_ptr<MockIMFCursorInfoAdapter> cursor = nullptr;
    EXPECT_CALL(*config, GetCursorInfo()).WillOnce(testing::Return(cursor));
    result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_FALSE(result);
  }

  TEST_F(IMFAdapterImplTest, AttachParamsCheck_002) {
    std::shared_ptr<MockIMFTextListenerAdapter> listener =
        std::make_shared<MockIMFTextListenerAdapter>();
    std::shared_ptr<MockIMFTextConfigAdapter> config =
        std::make_shared<MockIMFTextConfigAdapter>();
    bool isShowKeyboard = false;
    bool isResetListener = false;
    std::shared_ptr<MockIMFInputAttributeAdapter> attribute =
        std::make_shared<MockIMFInputAttributeAdapter>();
    EXPECT_CALL(*config, GetInputAttribute())
        .WillRepeatedly(testing::Return(attribute));
    std::shared_ptr<MockIMFCursorInfoAdapter> cursor =
        std::make_shared<MockIMFCursorInfoAdapter>();
    EXPECT_CALL(*config, GetCursorInfo())
        .WillRepeatedly(testing::Return(cursor));

    IMFTextEditorProxyImpl::textListener_.reset();
    bool result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_TRUE(result);

    IMFTextEditorProxyImpl::textListener_ = std::make_shared<IMFTextListenerAdapterImpl>(listener);
    result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_TRUE(result);

    isResetListener = true;
    result = imf_adapter_->AttachParamsCheck(listener, isShowKeyboard,
                                                  config, isResetListener);
    EXPECT_TRUE(result);
  }

  TEST_F(IMFTextListenerAdapterImplTest, NotifyPanelStatusInfo) {
    MiscServices::PanelStatusInfo panelStatus;
    listener_adapter_->listener_ = mock_listener_adapter_;
    listener_adapter_->NotifyPanelStatusInfo(panelStatus);

    panelStatus.trigger = MiscServices::Trigger::IME_APP;
    listener_adapter_->NotifyPanelStatusInfo(panelStatus);

    listener_adapter_->listener_ = nullptr;
    listener_adapter_->NotifyPanelStatusInfo(panelStatus);
    EXPECT_EQ(listener_adapter_->listener_, nullptr);
  }

  TEST_F(IMFTextListenerAdapterImplTest, PrivateCommandGetStrValue) {
    InputMethod_PrivateCommand* privateCommand = nullptr;
    std::shared_ptr<std::string> result = PrivateCommandGetStrValue(privateCommand);
    EXPECT_EQ(result, nullptr);

    char key[] = "example key";
    size_t keyLength = strlen(key);
    InputMethod_PrivateCommand *command = OH_PrivateCommand_Create(key, keyLength);
    EXPECT_EQ(IME_ERR_OK, OH_PrivateCommand_SetStrValue(command, const_cast<char *>(key), keyLength));
    result = PrivateCommandGetStrValue(command);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, "example key");
    std::vector<InputMethod_PrivateCommand *> commands;
    commands.push_back(command);
    DestroyPrivateCommand(commands);

    int32_t ret = 0;
    bool isShowKeyboard = true;
    ReportImfErrorEvent(ret, isShowKeyboard);
  }

}  // namespace OHOS::NWeb

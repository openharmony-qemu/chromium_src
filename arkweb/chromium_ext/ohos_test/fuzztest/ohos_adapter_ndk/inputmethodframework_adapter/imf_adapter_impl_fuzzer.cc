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

#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <inputmethod/inputmethod_controller_capi.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_adapter_helper.h"
#include "third_party/cJSON/cJSON.h"

using namespace OHOS::NWeb;
namespace OHOS::NWeb {

class MockIMFTextListenerAdapter : public IMFTextListenerAdapter {
 public:
  void InsertText(const std::u16string& text) override {}
  void DeleteForward(int32_t length) override {}
  void DeleteBackward(int32_t length) override {}
  void SendKeyEventFromInputMethod() override {}
  void SendKeyboardStatus(const IMFAdapterKeyboardStatus& status) override {}
  void SendFunctionKey(
      std::shared_ptr<IMFAdapterFunctionKeyAdapter> key) override {}
  void SetKeyboardStatus(bool status) override {}
  void MoveCursor(const IMFAdapterDirection direction) override {}
  void HandleSetSelection(int32_t start, int32_t end) override {}
  void HandleExtendAction(int32_t action) override {}
  void HandleSelect(int32_t keyCode, int32_t skip) override {}
  int32_t GetTextIndexAtCursor() override { return 0; }
  std::u16string GetLeftTextOfCursor(int32_t number) override { return u""; }
  std::u16string GetRightTextOfCursor(int32_t number) override { return u""; }
  int32_t SetPreviewText(const std::u16string& text,
                         int32_t start,
                         int32_t end) override {
    return 0;
  }
  void FinishTextPreview() override {}
  int32_t ReceivePrivateCommand(InputMethod_PrivateCommand* cmd[],
                                size_t size) {
    return 0;
  }
  void NotifyPanelStatusInfo(const MiscServices::PanelStatusInfo& info) {}
  void SetNeedUnderLine(bool need) override {}
};

class MockIMFCursorInfoAdapter : public IMFCursorInfoAdapter {
 public:
  MockIMFCursorInfoAdapter(double x, double y, double width, double height)
      : x_(x), y_(y), width_(width), height_(height) {}

  double GetLeft() override { return x_; }
  double GetTop() override { return y_; }
  double GetWidth() override { return width_; }
  double GetHeight() override { return height_; }

 private:
  double x_ = 0.0;
  double y_ = 0.0;
  double width_ = 0.0;
  double height_ = 0.0;
};

class MockIMFAdapterFunctionKeyAdapter : public IMFAdapterFunctionKeyAdapter {
 public:
  IMFAdapterEnterKeyType GetEnterKeyType() override {
    return IMFAdapterEnterKeyType::UNSPECIFIED;
  }
  void SetEnterKeyType(IMFAdapterEnterKeyType keyType) {}
};

class MockIMFTextConfigAdapter : public IMFTextConfigAdapter {
 public:
  MockIMFTextConfigAdapter(uint32_t windowId = 0,
                           double positionY = 0.0,
                           double height = 0.0)
      : windowId_(windowId), positionY_(positionY), height_(height) {}

  std::shared_ptr<IMFInputAttributeAdapter> GetInputAttribute() override {
    return nullptr;
  }

  std::shared_ptr<IMFCursorInfoAdapter> GetCursorInfo() override {
    return nullptr;
  }

  std::shared_ptr<IMFSelectionRangeAdapter> GetSelectionRange() override {
    return nullptr;
  }

  uint32_t GetWindowId() override { return windowId_; }

  double GetPositionY() override { return positionY_; }

  double GetHeight() override { return height_; }

 private:
  uint32_t windowId_ = 0;
  double positionY_ = 0.0;
  double height_ = 0.0;
};

std::u16string GenerateRandomU16String(FuzzedDataProvider* fdp,
                                       size_t maxLength = 256) {
  size_t len = fdp->ConsumeIntegralInRange<size_t>(0, maxLength);
  std::u16string result;
  result.reserve(len);

  for (size_t i = 0; i < len; i++) {
    char16_t codePoint = fdp->ConsumeIntegralInRange<char16_t>(0x0020, 0xD7FF);
    result += codePoint;
  }
  return result;
}

void FuzzIMFAdapterImpl(FuzzedDataProvider* fdp) {
  auto listener = std::make_shared<MockIMFTextListenerAdapter>();
  uint32_t windowId = fdp->ConsumeIntegral<uint32_t>();
  double positionY = fdp->ConsumeFloatingPoint<double>();
  double height = fdp->ConsumeFloatingPoint<double>();
  auto config =
      std::make_shared<MockIMFTextConfigAdapter>(windowId, positionY, height);
  IMFAdapterImpl adapter;

  bool isShowKeyboard = fdp->ConsumeBool();
  bool isResetListener = fdp->ConsumeBool();
  int32_t requestReason = fdp->ConsumeIntegralInRange<int32_t>(0, 5);

  adapter.Attach(listener, isShowKeyboard);
  adapter.Attach(listener, isShowKeyboard, config, isResetListener);
  adapter.AttachWithRequestKeyboardReason(listener, isShowKeyboard, config,
                                          isResetListener, requestReason);

  IMFAdapterTextInputType inputType =
      static_cast<IMFAdapterTextInputType>(fdp->ConsumeIntegralInRange<int32_t>(
          0, static_cast<int32_t>(IMFAdapterTextInputType::NUMBER_DECIMAL)));
  adapter.ShowCurrentInput(inputType);
  adapter.HideTextInput();

  double x = fdp->ConsumeFloatingPoint<double>();
  double y = fdp->ConsumeFloatingPoint<double>();
  double width = fdp->ConsumeFloatingPoint<double>();
  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo =
      std::make_shared<MockIMFCursorInfoAdapter>(x, y, width, height);
  adapter.OnCursorUpdate(cursorInfo);

  std::u16string selectionText = GenerateRandomU16String(fdp, 100);
  int start = fdp->ConsumeIntegralInRange<int>(0, selectionText.length());
  int end = fdp->ConsumeIntegralInRange<int>(start, selectionText.length());
  adapter.OnSelectionChange(selectionText, start, end);

  std::string cmdKey = fdp->ConsumeRandomLengthString(32);
  std::string cmdValue = fdp->ConsumeRandomLengthString(256);
  adapter.SendPrivateCommand(cmdKey, cmdValue);

  adapter.Close();
}

void FixedIMFAdapterImpl() {
  auto listener = std::make_shared<MockIMFTextListenerAdapter>();
  uint32_t windowId = 12345;
  double positionY = 100.0;
  double height = 200.0;
  auto config =
      std::make_shared<MockIMFTextConfigAdapter>(windowId, positionY, height);
  IMFAdapterImpl adapter;

  adapter.Attach(listener, true);
  adapter.Attach(listener, false, config, true);
  adapter.AttachWithRequestKeyboardReason(listener, true, config, false, 3);

  adapter.ShowCurrentInput(IMFAdapterTextInputType::TEXT);
  adapter.HideTextInput();

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo =
      std::make_shared<MockIMFCursorInfoAdapter>(50.0, 100.0, 20.0, 30.0);
  adapter.OnCursorUpdate(cursorInfo);

  std::u16string selectionText = u"Hello, World!";
  adapter.OnSelectionChange(selectionText, 0, selectionText.length());

  adapter.SendPrivateCommand("testCommand", "testValue");

  adapter.Close();
}

void FuzzIMFTextEditorProxyImpl(FuzzedDataProvider* fdp) {
  auto listener = std::make_shared<MockIMFTextListenerAdapter>();

  InputMethod_TextEditorProxy* proxy =
      IMFTextEditorProxyImpl::TextEditorProxyCreate(listener);
  if (!proxy) {
    return;
  }

  InputMethod_TextConfig* textConfig = OH_TextConfig_Create();
  if (textConfig) {
    InputMethod_TextInputType inputType =
        static_cast<InputMethod_TextInputType>(
            fdp->ConsumeIntegralInRange<int32_t>(0, 10));
    OH_TextConfig_SetInputType(textConfig, inputType);

    InputMethod_EnterKeyType enterKeyType =
        static_cast<InputMethod_EnterKeyType>(
            fdp->ConsumeIntegralInRange<int32_t>(0, 5));
    OH_TextConfig_SetEnterKeyType(textConfig, enterKeyType);

    IMFTextEditorProxyImpl::GetTextConfigFunc(proxy, textConfig);

    OH_TextConfig_Destroy(textConfig);
  }

  std::u16string randomText = GenerateRandomU16String(fdp, 100);
  IMFTextEditorProxyImpl::InsertTextFunc(proxy, randomText.c_str(),
                                         randomText.length());

  int32_t deleteLength = fdp->ConsumeIntegralInRange<int32_t>(0, 50);
  if (fdp->ConsumeBool()) {
    IMFTextEditorProxyImpl::DeleteForwardFunc(proxy, deleteLength);
  } else {
    IMFTextEditorProxyImpl::DeleteBackwardFunc(proxy, deleteLength);
  }

  InputMethod_Direction direction = static_cast<InputMethod_Direction>(
      fdp->ConsumeIntegralInRange<int32_t>(0, 4));
  IMFTextEditorProxyImpl::MoveCursorFunc(proxy, direction);

  int32_t start = fdp->ConsumeIntegralInRange<int32_t>(0, 50);
  int32_t end = fdp->ConsumeIntegralInRange<int32_t>(start, 100);
  IMFTextEditorProxyImpl::HandleSetSelectionFunc(proxy, start, end);

  InputMethod_ExtendAction action = static_cast<InputMethod_ExtendAction>(
      fdp->ConsumeIntegralInRange<int32_t>(0, 5));
  IMFTextEditorProxyImpl::HandleExtendActionFunc(proxy, action);

  int32_t textNumber = fdp->ConsumeIntegralInRange<int32_t>(0, 20);
  char16_t leftText[100] = {0};
  size_t leftLength = 0;
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, textNumber, leftText,
                                                  &leftLength);

  char16_t rightText[100] = {0};
  size_t rightLength = 0;
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, textNumber, rightText,
                                                   &rightLength);

  IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(proxy);

  std::u16string previewText = GenerateRandomU16String(fdp, 50);
  IMFTextEditorProxyImpl::SetPreviewTextFunc(proxy, previewText.c_str(),
                                             previewText.length(), start, end);
  IMFTextEditorProxyImpl::FinishTextPreviewFunc(proxy);

  IMFTextEditorProxyImpl::TextEditorProxyDestroy(proxy);
}

void FixedIMFTextEditorProxyImpl() {
  auto listener = std::make_shared<MockIMFTextListenerAdapter>();

  InputMethod_TextEditorProxy* proxy =
      IMFTextEditorProxyImpl::TextEditorProxyCreate(listener);
  if (!proxy) {
    return;
  }

  InputMethod_TextConfig* textConfig = OH_TextConfig_Create();
  if (textConfig) {
    OH_TextConfig_SetInputType(
        textConfig, InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_TEXT);
    OH_TextConfig_SetEnterKeyType(textConfig,
                                  InputMethod_EnterKeyType::IME_ENTER_KEY_DONE);
    IMFTextEditorProxyImpl::GetTextConfigFunc(proxy, textConfig);
    OH_TextConfig_Destroy(textConfig);
  }

  std::u16string testText = u"Hello, Fixed World!";
  IMFTextEditorProxyImpl::InsertTextFunc(proxy, testText.c_str(),
                                         testText.length());
  IMFTextEditorProxyImpl::DeleteBackwardFunc(proxy, 5);
  IMFTextEditorProxyImpl::MoveCursorFunc(
      proxy, InputMethod_Direction::IME_DIRECTION_RIGHT);

  IMFTextEditorProxyImpl::HandleSetSelectionFunc(proxy, 0, 10);
  IMFTextEditorProxyImpl::HandleExtendActionFunc(
      proxy, InputMethod_ExtendAction::IME_EXTEND_ACTION_PASTE);

  char16_t leftText[100] = {0};
  size_t leftLength = 0;
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, 10, leftText,
                                                  &leftLength);

  char16_t rightText[100] = {0};
  size_t rightLength = 0;
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, 10, rightText,
                                                   &rightLength);

  IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(proxy);

  std::u16string previewText = u"Preview Text";
  IMFTextEditorProxyImpl::SetPreviewTextFunc(proxy, previewText.c_str(),
                                             previewText.length(), 0, 5);
  IMFTextEditorProxyImpl::FinishTextPreviewFunc(proxy);

  IMFTextEditorProxyImpl::TextEditorProxyDestroy(proxy);
}

}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  FuzzIMFAdapterImpl(&fdp);
  FuzzIMFTextEditorProxyImpl(&fdp);
  FixedIMFAdapterImpl();
  FixedIMFTextEditorProxyImpl();

  return 0;
}

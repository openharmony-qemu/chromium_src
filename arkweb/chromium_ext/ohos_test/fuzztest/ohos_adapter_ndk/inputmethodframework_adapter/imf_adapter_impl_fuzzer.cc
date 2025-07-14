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

class MockIMFInputAttributeAdapter : public IMFInputAttributeAdapter {
 public:
  MockIMFInputAttributeAdapter(
      IMFAdapterTextInputType pattern = IMFAdapterTextInputType::TEXT,
      IMFAdapterEnterKeyType keyType = IMFAdapterEnterKeyType::UNSPECIFIED)
      : inputPattern_(pattern), enterKeyType_(keyType) {}

  int32_t GetInputPattern() override {
    return static_cast<int32_t>(inputPattern_);
  }

  int32_t GetEnterKeyType() override {
    return static_cast<int32_t>(enterKeyType_);
  }

  void SetInputPattern(IMFAdapterTextInputType pattern) {
    inputPattern_ = pattern;
  }

  void SetEnterKeyType(IMFAdapterEnterKeyType keyType) {
    enterKeyType_ = keyType;
  }

 private:
  IMFAdapterTextInputType inputPattern_;
  IMFAdapterEnterKeyType enterKeyType_;
};

class MockIMFSelectionRangeAdapter : public IMFSelectionRangeAdapter {
 public:
  MockIMFSelectionRangeAdapter(int start = 0, int end = 0)
      : start_(start), end_(end) {}

  int32_t GetStart() override { return start_; }
  int32_t GetEnd() override { return end_; }

  void SetRange(int start, int end) {
    start_ = start;
    end_ = end;
  }

 private:
  int start_;
  int end_;
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

  void SetInputPattern(IMFAdapterTextInputType pattern) {
    if (inputAttribute_) {
      std::static_pointer_cast<MockIMFInputAttributeAdapter>(inputAttribute_)
          ->SetInputPattern(pattern);
    }
  }

  void SetEnterKeyType(IMFAdapterEnterKeyType keyType) {
    if (inputAttribute_) {
      std::static_pointer_cast<MockIMFInputAttributeAdapter>(inputAttribute_)
          ->SetEnterKeyType(keyType);
    }
  }

 private:
  std::shared_ptr<IMFInputAttributeAdapter> inputAttribute_;
  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo_;
  std::shared_ptr<IMFSelectionRangeAdapter> selectionRange_;
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

InputMethod_TextInputType AdapterTextInputTypeToTextInputTypeEx(
    int32_t inputType) {
  const static std::unordered_map<IMFAdapterTextInputType,
                                  InputMethod_TextInputType>
      INPUT_TYPE_MAP = {
          /* The text input type is NONE. */
          {IMFAdapterTextInputType::NONE, IME_TEXT_INPUT_TYPE_NONE},
          /* The text input type is TEXT. */
          {IMFAdapterTextInputType::TEXT, IME_TEXT_INPUT_TYPE_TEXT},
          /* The text input type is MULTILINE. */
          {IMFAdapterTextInputType::MULTILINE, IME_TEXT_INPUT_TYPE_MULTILINE},
          /* The text input type is NUMBER. */
          {IMFAdapterTextInputType::NUMBER, IME_TEXT_INPUT_TYPE_NUMBER},
          /* The text input type is PHONE. */
          {IMFAdapterTextInputType::PHONE, IME_TEXT_INPUT_TYPE_PHONE},
          /* The text input type is DATETIME. */
          {IMFAdapterTextInputType::DATETIME, IME_TEXT_INPUT_TYPE_DATETIME},
          /* The text input type is EMAIL ADDRESS. */
          {IMFAdapterTextInputType::EMAIL_ADDRESS,
           IME_TEXT_INPUT_TYPE_EMAIL_ADDRESS},
          /* The text input type is URL. */
          {IMFAdapterTextInputType::URL, IME_TEXT_INPUT_TYPE_URL},
          /* The text input type is VISIBLE PASSWORD. */
          {IMFAdapterTextInputType::VISIBLE_PASSWORD,
           IME_TEXT_INPUT_TYPE_VISIBLE_PASSWORD},
          /* The text input type is NUMBER PASSWORD. */
          {IMFAdapterTextInputType::NUMBER_PASSWORD,
           IME_TEXT_INPUT_TYPE_NUMBER_PASSWORD},
          /* The text input type is SCREEN LOCK PASSWORD. */
          {IMFAdapterTextInputType::SCREEN_LOCK_PASSWORD,
           IME_TEXT_INPUT_TYPE_SCREEN_LOCK_PASSWORD},
          /* The text input type is USER NAME. */
          {IMFAdapterTextInputType::USER_NAME, IME_TEXT_INPUT_TYPE_USER_NAME},
          /* The text input type is NEW PASSWORD. */
          {IMFAdapterTextInputType::NEW_PASSWORD,
           IME_TEXT_INPUT_TYPE_NEW_PASSWORD},
          /* The text input type is NUMBER DECIMAL. */
          {IMFAdapterTextInputType::NUMBER_DECIMAL,
           IME_TEXT_INPUT_TYPE_NUMBER_DECIMAL},
      };
  auto checkIter = INPUT_TYPE_MAP.find((IMFAdapterTextInputType)inputType);
  if (checkIter != INPUT_TYPE_MAP.end()) {
    return checkIter->second;
  }
  return IME_TEXT_INPUT_TYPE_NONE;
}

InputMethod_EnterKeyType AdapterEnterKeyTypeToOhEnterKeyTypeEx(
    int32_t enterKeyType) {
  const static std::unordered_map<IMFAdapterEnterKeyType,
                                  InputMethod_EnterKeyType>
      ENTER_KEY_TYPE_MAP = {
          /* The enter key type is UNSPECIFIED. */
          {IMFAdapterEnterKeyType::UNSPECIFIED, IME_ENTER_KEY_UNSPECIFIED},
          /* The enter key type is NONE. */
          {IMFAdapterEnterKeyType::NONE, IME_ENTER_KEY_NONE},
          /* The enter key type is GO. */
          {IMFAdapterEnterKeyType::GO, IME_ENTER_KEY_GO},
          /* The enter key type is SEARCH. */
          {IMFAdapterEnterKeyType::SEARCH, IME_ENTER_KEY_SEARCH},
          /* The enter key type is SEND. */
          {IMFAdapterEnterKeyType::SEND, IME_ENTER_KEY_SEND},
          /* The enter key type is NEXT. */
          {IMFAdapterEnterKeyType::NEXT, IME_ENTER_KEY_NEXT},
          /* The enter key type is DONE. */
          {IMFAdapterEnterKeyType::DONE, IME_ENTER_KEY_DONE},
          /* The enter key type is PREVIOUS. */
          {IMFAdapterEnterKeyType::PREVIOUS, IME_ENTER_KEY_PREVIOUS},
          /* The enter key type is NEWLINE. */
          {IMFAdapterEnterKeyType::NEW_LINE, IME_ENTER_KEY_NEWLINE},
      };
  auto checkIter =
      ENTER_KEY_TYPE_MAP.find((IMFAdapterEnterKeyType)enterKeyType);
  if (checkIter != ENTER_KEY_TYPE_MAP.end()) {
    return checkIter->second;
  }
  return IME_ENTER_KEY_UNSPECIFIED;
}

IMFAdapterKeyboardStatus ohKeyboardStatusToAdapterKeyboardStatusEx(
    InputMethod_KeyboardStatus keyboardStatus) {
  const static std::unordered_map<InputMethod_KeyboardStatus,
                                  IMFAdapterKeyboardStatus>
      KEY_BOARD_STATUS_MAP = {
          {IME_KEYBOARD_STATUS_NONE, IMFAdapterKeyboardStatus::NONE},
          {IME_KEYBOARD_STATUS_HIDE, IMFAdapterKeyboardStatus::HIDE},
          {IME_KEYBOARD_STATUS_SHOW, IMFAdapterKeyboardStatus::SHOW},
      };
  auto checkIter = KEY_BOARD_STATUS_MAP.find(keyboardStatus);
  if (checkIter != KEY_BOARD_STATUS_MAP.end()) {
    return checkIter->second;
  }
  return IMFAdapterKeyboardStatus::NONE;
}

InputMethod_ErrorCode GetCursorInfoEx(InputMethod_TextConfig* dest,
                                      InputMethod_TextConfig* src) {
  double left, top, width, height;
  InputMethod_CursorInfo *destInfo, *srcInfo;

  InputMethod_ErrorCode ret = OH_TextConfig_GetCursorInfo(src, &srcInfo);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get cursor info failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_CursorInfo_GetRect(srcInfo, &left, &top, &width, &height);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get rect failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_GetCursorInfo(dest, &destInfo);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get cursor info failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_CursorInfo_SetRect(destInfo, left, top, width, height);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set rect failed ret %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetTextAvoidInfoEx(InputMethod_TextConfig* dest,
                                         InputMethod_TextConfig* src) {
  InputMethod_TextAvoidInfo *destAvoidInfo, *srcAvoidInfo;
  double positionY, height;

  InputMethod_ErrorCode ret =
      OH_TextConfig_GetTextAvoidInfo(src, &srcAvoidInfo);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get avoid info failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextAvoidInfo_GetPositionY(srcAvoidInfo, &positionY);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get positionY failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextAvoidInfo_GetHeight(srcAvoidInfo, &height);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get height failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_GetTextAvoidInfo(dest, &destAvoidInfo);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get avoid info failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextAvoidInfo_SetPositionY(destAvoidInfo, positionY);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set positionY failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextAvoidInfo_SetHeight(destAvoidInfo, height);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set height failed ret %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetInputTypeEx(InputMethod_TextConfig* dest,
                                     InputMethod_TextConfig* src) {
  InputMethod_TextInputType inputType;

  InputMethod_ErrorCode ret = OH_TextConfig_GetInputType(src, &inputType);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get input type ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_SetInputType(dest, inputType);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set input type ret %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetEnterKeyTypeEx(InputMethod_TextConfig* dest,
                                        InputMethod_TextConfig* src) {
  InputMethod_EnterKeyType enterKeyType;

  InputMethod_ErrorCode ret = OH_TextConfig_GetEnterKeyType(src, &enterKeyType);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get enter key type ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_SetEnterKeyType(dest, enterKeyType);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set enter key type ret %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetSelectionEx(InputMethod_TextConfig* dest,
                                     InputMethod_TextConfig* src) {
  int32_t start, end;

  InputMethod_ErrorCode ret = OH_TextConfig_GetSelection(src, &start, &end);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get selection failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_SetSelection(dest, start, end);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set selection failed ret %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetWindowIdEx(InputMethod_TextConfig* dest,
                                    InputMethod_TextConfig* src) {
  int32_t windowId;

  InputMethod_ErrorCode ret = OH_TextConfig_GetWindowId(src, &windowId);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get windowId failed ret %{public}d", ret);
    return ret;
  }
  ret = OH_TextConfig_SetWindowId(dest, windowId);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set windowId ret failed %{public}d", ret);
    return ret;
  }
  return IME_ERR_OK;
}

InputMethod_ErrorCode GetPreviewTextSupportedEx(InputMethod_TextConfig* dest,
                                                InputMethod_TextConfig* src) {
  bool supported;

  InputMethod_ErrorCode ret =
      OH_TextConfig_IsPreviewTextSupported(src, &supported);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Get preview supported failed ret %{public}d",
            ret);
    return ret;
  }
  ret = OH_TextConfig_SetPreviewTextSupport(dest, supported);
  if (ret != IME_ERR_OK) {
    WVLOG_E("Inputmethod adapter Set preview supported failed ret %{public}d",
            ret);
    return ret;
  }
  return IME_ERR_OK;
}

void FuzzIMFAdapterImpl(FuzzedDataProvider* fdp) {
  auto listener = std::make_shared<MockIMFTextListenerAdapter>();
  uint32_t windowId = fdp->ConsumeIntegral<uint32_t>();
  double positionY = fdp->ConsumeFloatingPoint<double>();
  double height = fdp->ConsumeFloatingPoint<double>();
  IMFAdapterImpl adapter;

  for (int32_t inputType = static_cast<int32_t>(IMFAdapterTextInputType::NONE);
       inputType <=
       static_cast<int32_t>(IMFAdapterTextInputType::NUMBER_DECIMAL);
       ++inputType) {
    for (int32_t enterKey =
             static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
         enterKey <= static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE);
         ++enterKey) {
      auto config = std::make_shared<MockIMFTextConfigAdapter>(
          windowId, positionY, height);

      config->SetInputPattern(static_cast<IMFAdapterTextInputType>(inputType));
      config->SetEnterKeyType(static_cast<IMFAdapterEnterKeyType>(enterKey));

      bool isShowKeyboard = fdp->ConsumeBool();
      bool isResetListener = fdp->ConsumeBool();
      int32_t requestReason = fdp->ConsumeIntegralInRange<int32_t>(0, 5);
      adapter.Attach(listener, isShowKeyboard);
      adapter.Attach(listener, isShowKeyboard, config, isResetListener);
      adapter.AttachWithRequestKeyboardReason(listener, isShowKeyboard, config,
                                              isResetListener, requestReason);
    }
  }

  {
    const int32_t minType = static_cast<int32_t>(IMFAdapterTextInputType::NONE);
    const int32_t maxType =
        static_cast<int32_t>(IMFAdapterTextInputType::NUMBER_DECIMAL);

    std::vector<int32_t> testTypes;
    for (int32_t i = minType; i <= maxType; ++i) {
      testTypes.push_back(i);
    }

    for (int32_t type : testTypes) {
      InputMethod_TextInputType result =
          AdapterTextInputTypeToTextInputTypeEx(type);
      (void)result;
    }
  }

  {
    const int32_t minType =
        static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
    const int32_t maxType =
        static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE);

    std::vector<int32_t> testTypes;
    for (int32_t i = minType; i <= maxType; ++i) {
      testTypes.push_back(i);
    }

    for (int32_t type : testTypes) {
      InputMethod_EnterKeyType result =
          AdapterEnterKeyTypeToOhEnterKeyTypeEx(type);
      (void)result;
    }
  }

  {
    const std::vector<InputMethod_KeyboardStatus> keyboardStatuses = {
        IME_KEYBOARD_STATUS_NONE, IME_KEYBOARD_STATUS_HIDE,
        IME_KEYBOARD_STATUS_SHOW, static_cast<InputMethod_KeyboardStatus>(-1),
        static_cast<InputMethod_KeyboardStatus>(3)};
    for (InputMethod_KeyboardStatus status : keyboardStatuses) {
      IMFAdapterKeyboardStatus result =
          ohKeyboardStatusToAdapterKeyboardStatusEx(status);
      (void)result;
    }
  }

  InputMethod_TextConfig* srcConfig = OH_TextConfig_Create();
  InputMethod_TextConfig* destConfig = OH_TextConfig_Create();

  {
    InputMethod_ErrorCode ret = GetCursorInfoEx(destConfig, srcConfig);
    (void)ret;

    GetCursorInfoEx(nullptr, srcConfig);
    GetCursorInfoEx(destConfig, nullptr);
  }

  {
    GetTextAvoidInfoEx(destConfig, srcConfig);
    GetTextAvoidInfoEx(nullptr, srcConfig);
    GetTextAvoidInfoEx(destConfig, nullptr);
  }

  {
    for (int32_t i = -1; i <= 12; ++i) {
      InputMethod_TextInputType type =
          static_cast<InputMethod_TextInputType>(i);
      OH_TextConfig_SetInputType(srcConfig, type);
      GetInputTypeEx(destConfig, srcConfig);
    }
  }

  {
    std::vector<InputMethod_EnterKeyType> allEnterKeyTypes;

    for (int i = 0;
         i <= static_cast<int>(InputMethod_EnterKeyType::IME_ENTER_KEY_NEWLINE);
         ++i) {
      allEnterKeyTypes.push_back(static_cast<InputMethod_EnterKeyType>(i));
    }

    for (InputMethod_EnterKeyType type : allEnterKeyTypes) {
      OH_TextConfig_SetEnterKeyType(srcConfig, type);
      GetEnterKeyTypeEx(destConfig, srcConfig);
    }
  }

  {
    const std::vector<std::pair<int32_t, int32_t>> testCases = {
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<int32_t>()},
        {INT32_MIN, INT32_MAX}};

    for (const auto& [start, end] : testCases) {
      OH_TextConfig_SetSelection(srcConfig, start, end);
      GetSelectionEx(destConfig, srcConfig);
    }
  }

  {
    const std::vector<int32_t> testIds = {fdp->ConsumeIntegral<int32_t>(),
                                          fdp->ConsumeIntegral<int32_t>(),
                                          fdp->ConsumeIntegral<int32_t>(),
                                          INT32_MAX,
                                          -1,
                                          INT32_MIN};

    for (auto id : testIds) {
      OH_TextConfig_SetWindowId(srcConfig, id);
      GetWindowIdEx(destConfig, srcConfig);
    }
  }

  {
    const std::vector<bool> testValues = {fdp->ConsumeBool(),
                                          fdp->ConsumeBool()};
    for (auto value : testValues) {
      OH_TextConfig_SetPreviewTextSupport(srcConfig, value);
      GetPreviewTextSupportedEx(destConfig, srcConfig);
    }
  }

  bool isShowKeyboard = fdp->ConsumeBool();
  adapter.Attach(listener, isShowKeyboard);
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
  MiscServices::PanelStatusInfo panelInfo{};
  panelInfo.trigger = fdp->ConsumeBool()
                          ? MiscServices::Trigger::IME_APP
                          : static_cast<MiscServices::Trigger>(
                                fdp->ConsumeIntegralInRange<int>(0, 5));
  listener->NotifyPanelStatusInfo(panelInfo);
  adapter.Close();
  if (srcConfig) {
    OH_TextConfig_Destroy(srcConfig);
  }
  if (destConfig) {
    OH_TextConfig_Destroy(destConfig);
  }
}

void FixedIMFAdapterImpl() {
  struct TestCase {
    uint32_t windowId;
    double positionY;
    double height;
    bool isShowKeyboard;
    bool isResetListener;
    int32_t requestReason;
  };
  const std::vector<TestCase> testCases = {
      {12345, 100.0, 200.0, true, true, 0},
      {0, 50.0, 150.0, false, false, 1},
      {UINT32_MAX, 200.0, 300.0, true, false, 2},
      {4294967295, 0.0, 1000.0, false, true, 3},
      {1, -100.0, 500.0, true, true, 4},
      {1000, 1000.0, 100.0, false, false, 5},
  };
  for (const auto& test : testCases) {
    auto listener = std::make_shared<MockIMFTextListenerAdapter>();
    IMFAdapterImpl adapter;
    for (int32_t inputType = -1; inputType <= 13; ++inputType) {
      for (int32_t enterKey = 0; enterKey <= 8; ++enterKey) {
        auto config = std::make_shared<MockIMFTextConfigAdapter>(
            test.windowId, test.positionY, test.height);

        IMFAdapterTextInputType inputTypeEnum;
        if (inputType == static_cast<int32_t>(IMFAdapterTextInputType::NONE)) {
          inputTypeEnum = IMFAdapterTextInputType::NONE;
        } else if (inputType >=
                       static_cast<int32_t>(IMFAdapterTextInputType::TEXT) &&
                   inputType <= static_cast<int32_t>(
                                    IMFAdapterTextInputType::NUMBER_DECIMAL)) {
          inputTypeEnum = static_cast<IMFAdapterTextInputType>(inputType);
        } else {
          inputTypeEnum = IMFAdapterTextInputType::NUMBER_DECIMAL;
        }
        config->SetInputPattern(inputTypeEnum);

        IMFAdapterEnterKeyType enterKeyEnum;
        if (enterKey >=
                static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED) &&
            enterKey <=
                static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE)) {
          enterKeyEnum = static_cast<IMFAdapterEnterKeyType>(enterKey);
        } else {
          enterKeyEnum = IMFAdapterEnterKeyType::NEW_LINE;
        }
        config->SetEnterKeyType(enterKeyEnum);

        adapter.Attach(listener, test.isShowKeyboard);
        adapter.Attach(listener, test.isShowKeyboard, config,
                       test.isResetListener);
        adapter.AttachWithRequestKeyboardReason(listener, test.isShowKeyboard,
                                                config, test.isResetListener,
                                                test.requestReason);
      }
    }
    {
      const int32_t minType =
          static_cast<int32_t>(IMFAdapterTextInputType::NONE);
      const int32_t maxType =
          static_cast<int32_t>(IMFAdapterTextInputType::NUMBER_DECIMAL);
      std::vector<int32_t> testTypes;
      for (int32_t i = minType; i <= maxType; ++i) {
        testTypes.push_back(i);
      }
      for (int32_t type : testTypes) {
        InputMethod_TextInputType result =
            AdapterTextInputTypeToTextInputTypeEx(type);
        (void)result;
      }
    }
    {
      const int32_t minType =
          static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
      const int32_t maxType =
          static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE);
      std::vector<int32_t> testTypes;
      for (int32_t i = minType; i <= maxType; ++i) {
        testTypes.push_back(i);
      }
      for (int32_t type : testTypes) {
        InputMethod_EnterKeyType result =
            AdapterEnterKeyTypeToOhEnterKeyTypeEx(type);
        (void)result;
      }
    }
    {
      const std::vector<InputMethod_KeyboardStatus> keyboardStatuses = {
          IME_KEYBOARD_STATUS_NONE, IME_KEYBOARD_STATUS_HIDE,
          IME_KEYBOARD_STATUS_SHOW, static_cast<InputMethod_KeyboardStatus>(0),
          static_cast<InputMethod_KeyboardStatus>(2)};
      for (InputMethod_KeyboardStatus status : keyboardStatuses) {
        IMFAdapterKeyboardStatus result =
            ohKeyboardStatusToAdapterKeyboardStatusEx(status);
        (void)result;
      }
    }
    InputMethod_TextConfig* srcConfig = OH_TextConfig_Create();
    InputMethod_TextConfig* destConfig = OH_TextConfig_Create();
    {
      InputMethod_ErrorCode ret = GetCursorInfoEx(destConfig, srcConfig);
      (void)ret;
      GetCursorInfoEx(nullptr, srcConfig);
      GetCursorInfoEx(destConfig, nullptr);
    }
    {
      GetTextAvoidInfoEx(destConfig, srcConfig);
      GetTextAvoidInfoEx(nullptr, srcConfig);
      GetTextAvoidInfoEx(destConfig, nullptr);
    }
    {
      for (int32_t i = 0; i <= 12; ++i) {
        InputMethod_TextInputType type =
            static_cast<InputMethod_TextInputType>(i);
        OH_TextConfig_SetInputType(srcConfig, type);
        GetInputTypeEx(destConfig, srcConfig);
      }
    }
    {
      std::vector<InputMethod_EnterKeyType> allEnterKeyTypes;
      for (int i = 0; i <= static_cast<int>(
                               InputMethod_EnterKeyType::IME_ENTER_KEY_NEWLINE);
           ++i) {
        allEnterKeyTypes.push_back(static_cast<InputMethod_EnterKeyType>(i));
      }
      for (InputMethod_EnterKeyType type : allEnterKeyTypes) {
        OH_TextConfig_SetEnterKeyType(srcConfig, type);
        GetEnterKeyTypeEx(destConfig, srcConfig);
      }
    }
    {
      const std::vector<std::pair<int32_t, int32_t>> selectionTestCases = {
          {0, 0},
          {0, 10},
          {5, 5},
          {100, 200},
          {1, 10},
          {10, 5},
          {INT32_MIN, INT32_MAX}};
      for (const auto& [start, end] : selectionTestCases) {
        OH_TextConfig_SetSelection(srcConfig, start, end);
        GetSelectionEx(destConfig, srcConfig);
      }
    }
    {
      const std::vector<int32_t> testIds = {0,         1,  100,
                                            INT32_MAX, -1, INT32_MIN};
      for (auto id : testIds) {
        OH_TextConfig_SetWindowId(srcConfig, id);
        GetWindowIdEx(destConfig, srcConfig);
      }
    }
    {
      const std::vector<bool> testValues = {true, false};
      for (auto value : testValues) {
        OH_TextConfig_SetPreviewTextSupport(srcConfig, value);
        GetPreviewTextSupportedEx(destConfig, srcConfig);
      }
    }
    {
      adapter.Attach(listener, test.isShowKeyboard);
      std::vector<IMFAdapterTextInputType> inputTypes = {
          IMFAdapterTextInputType::TEXT, IMFAdapterTextInputType::NUMBER,
          IMFAdapterTextInputType::EMAIL_ADDRESS,
          IMFAdapterTextInputType::VISIBLE_PASSWORD};
      for (auto type : inputTypes) {
        adapter.ShowCurrentInput(type);
        adapter.HideTextInput();
      }
      std::vector<std::tuple<double, double, double>> cursorParams = {
          {50.0, 100.0, 20.0},
          {0.0, 0.0, 10.0},
          {1000.0, 1000.0, 50.0},
          {-50.0, 50.0, 30.0}};
      for (const auto& [x, y, width] : cursorParams) {
        std::shared_ptr<IMFCursorInfoAdapter> cursorInfo =
            std::make_shared<MockIMFCursorInfoAdapter>(x, y, width,
                                                       test.height);
        adapter.OnCursorUpdate(cursorInfo);
      }
      std::u16string selectionText = u"Hello, World!";
      adapter.OnSelectionChange(selectionText, 0, selectionText.length());
      adapter.OnSelectionChange(selectionText, selectionText.length(),
                                selectionText.length());
      adapter.OnSelectionChange(selectionText, 0, 0);
      std::vector<std::pair<std::string, std::string>> commands = {
          {"testCommand", "testValue"},
          {"empty", ""},
          {"longCommand", std::string(256, 'a')},
          {"unicode", "testCommand"}};
      for (const auto& [key, value] : commands) {
        adapter.SendPrivateCommand(key, value);
      }
      MiscServices::PanelStatusInfo panelInfo{};
      panelInfo.trigger = static_cast<MiscServices::Trigger>(2);
      panelInfo.visible = true;
      panelInfo.panelInfo.panelFlag = static_cast<MiscServices::PanelFlag>(500);
      panelInfo.panelInfo.panelType = static_cast<MiscServices::PanelType>(2);
      listener->NotifyPanelStatusInfo(panelInfo);
    }
    adapter.Close();
    if (srcConfig) {
      OH_TextConfig_Destroy(srcConfig);
    }
    if (destConfig) {
      OH_TextConfig_Destroy(destConfig);
    }
  }
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
  IMFTextEditorProxyImpl::DeleteForwardFunc(proxy, deleteLength);
  IMFTextEditorProxyImpl::DeleteBackwardFunc(proxy, deleteLength);
  InputMethod_Direction direction = static_cast<InputMethod_Direction>(
      fdp->ConsumeIntegralInRange<int32_t>(0, 4));
  IMFTextEditorProxyImpl::MoveCursorFunc(proxy, direction);
  int32_t start = fdp->ConsumeIntegralInRange<int32_t>(0, 50);
  int32_t end = fdp->ConsumeIntegralInRange<int32_t>(start, 100);
  IMFTextEditorProxyImpl::HandleSetSelectionFunc(proxy, start, end);
  InputMethod_ExtendAction action = static_cast<InputMethod_ExtendAction>(
      fdp->ConsumeIntegralInRange<int32_t>(0, 5));
  IMFTextEditorProxyImpl::HandleExtendActionFunc(proxy, action);
  int32_t textNumber = fdp->ConsumeIntegralInRange<int32_t>(0, 99);
  char16_t leftText[100] = {0};
  size_t leftLength = fdp->ConsumeIntegralInRange<size_t>(0, 99);
  leftLength = std::min(leftLength, static_cast<size_t>(99));
  IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(proxy, textNumber, leftText,
                                                  &leftLength);
  char16_t rightText[100] = {0};
  size_t rightLength = fdp->ConsumeIntegralInRange<size_t>(0, 99);
  rightLength = std::min(rightLength, static_cast<size_t>(99));
  IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(proxy, textNumber, rightText,
                                                   &rightLength);
  IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(proxy);
  InputMethod_PrivateCommand* privateCmd[5] = {nullptr};
  size_t cmdSize = fdp->ConsumeIntegralInRange<size_t>(0, 5);
  IMFTextEditorProxyImpl::ReceivePrivateCommandFunc(proxy, privateCmd, cmdSize);
  std::u16string previewText = GenerateRandomU16String(fdp, 150);
  int32_t previewStart = fdp->ConsumeIntegralInRange<int32_t>(-10, 200);
  int32_t previewEnd = fdp->ConsumeIntegralInRange<int32_t>(previewStart, 250);
  IMFTextEditorProxyImpl::SetPreviewTextFunc(proxy, previewText.c_str(),
                                             previewText.length(), previewStart,
                                             previewEnd);
  IMFTextEditorProxyImpl::FinishTextPreviewFunc(proxy);

  {
    for (int32_t i = 0; i <= 2; ++i) {
      InputMethod_KeyboardStatus kbStatus =
          static_cast<InputMethod_KeyboardStatus>(i);
      IMFTextEditorProxyImpl::SendKeyboardStatusFunc(proxy, kbStatus);
    }
  }

  {
    for (int32_t i = 0; i <= 8; ++i) {
      InputMethod_EnterKeyType enterKey =
          static_cast<InputMethod_EnterKeyType>(i);
      IMFTextEditorProxyImpl::SendEnterKeyFunc(proxy, enterKey);
    }
  }

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

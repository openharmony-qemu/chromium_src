/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_INPUTMETHOD_HANDLER_H_
#define OHOS_NWEB_SRC_NWEB_INPUTMETHOD_HANDLER_H_
#include <chrono>
#include <condition_variable>
#include <unordered_map>

#include "arkweb/build/features/features.h"
#include "cef_delegate/nweb_inputmethod_client.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/imf_adapter.h"

namespace OHOS::NWeb {
enum CompositionType {
  COMPOSITION_CURRENT,
  COMPOSITION_POSITION,
  COMPOSITION_REPLACE,
  COMPOSITION_CANCEL,
  COMPOSITION_DELETE,
  COMPOSITION_INVALID,
};

class NWebInputMethodHandler : public NWebInputMethodClient {
 public:
  enum class ReattachType {
    FROM_ONFOCUS,
    FROM_CONTINUE,
    FROM_ONDRAG,
  };
  NWebInputMethodHandler();
  ~NWebInputMethodHandler();
  NWebInputMethodHandler(const NWebInputMethodHandler&) = delete;
  NWebInputMethodHandler& operator=(const NWebInputMethodHandler&) = delete;

  void Attach(CefRefPtr<CefBrowser> browser,
              InputInfo inputInfo,
              bool is_need_reset_listener,
              int32_t enterKeyType) override;
  void Attach(CefRefPtr<CefBrowser> browser,
              InputInfo inputInfo,
              bool is_need_reset_listener,
              int32_t enterKeyType, int32_t requestKeyboardReason) override;
  void ShowTextInput() override;
  void HideTextInput(
      uint32_t nwebId = 0,
      HideTextinputType hideType = HideTextinputType::FROM_KERNEL,
      bool noNeedKeyboardByInput = false) override;
  void HideTextInputForce() override;
  void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                              const CefString& selected_text,
                              const CefRange& selected_range) override;
  void OnCursorUpdate(const CefRect& rect) override;
  void OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                          const CefString& text,
                          const CefRange& selected_range) override;
  void SetFocusStatus(bool focus_status) override;
  void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                         bool is_editable_node) override;
  bool GetIsEditableNode() override;

  bool Reattach(uint32_t nwebId, ReattachType type);
  void SetIMEStatus(bool status);
  void WebBlurKeyboardHide();
  void HandleSecurityLayer();
  void InsertText(const std::u16string& text);
  void DeleteBackward(int32_t length);
  void DeleteForward(int32_t length);
  void SendEnterKeyEvent(int32_t enterKeyType);
  void MoveCursor(const IMFAdapterDirection direction);
  void SetScreenOffSet(double x, double y);
  void SetVirtualDeviceRatio(float device_pixel_ratio);
  int32_t GetTextIndexAtCursor();
  std::u16string GetLeftTextOfCursor(int32_t number);
  std::u16string GetRightTextOfCursor(int32_t number);
  void SetWindowIdForIME(uint32_t windowId);
  int32_t SetPreviewText(const std::u16string& text,
                         int32_t start,
                         int32_t end);
  void FinishTextPreview();
  void SetNeedUnderLine(bool is_need_underline);
  bool HasComposition() override;
  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                    const CefRange& selected_range) override;
  void OnUpdateTextInputStateCalled(CefRefPtr<CefBrowser> browser,
                                    const CefString& text,
                                    const CefRange& selected_range,
                                    const CefRange& compositon_range) override;

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const std::string& content);

  void SetFillContent(const std::string& content, int32_t node_id) override {
    fill_content_ = content;
    fill_content_node_id_ = node_id;
  }
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  std::string GetSelectInfo();
#endif
#if BUILDFLAG(ARKWEB_AI_WRITE)
  int GetSelectStartIndex();
  int GetSelectEndIndex();
  std::string GetAllTextInfo();
#endif // ARKWEB_AI_WRITE
  bool IsAttached() override { return isAttached_; }
  void SetNeedReattachOnfocus() {
    if (isAttached_) {
      isNeedReattachOnfocus_ = true;
      isFocusSwitchOnBlur_ = true;
    }
  }

 private:
  void SetIMEStatusOnUI(bool status);
  void WebBlurKeyboardHideOnUI();
  void HandleSecurityLayerHandlerOnUI();
  void InsertTextHandlerOnUI(const std::u16string& text);
  void DeleteBackwardHandlerOnUI(int32_t length);
  void DeleteForwardHandlerOnUI(int32_t length);
  bool IsCorrectParam(int32_t number, int32_t& selectBegin, int32_t& selectEnd);
  bool ResetTextSelectiondata();
  IMFAdapterTextInputType TextInputModeToIMFAdapter(cef_text_input_mode_t mode);
  IMFAdapterTextInputType TextInputTypeToIMFAdapter(cef_text_input_type_t type);
  IMFAdapterEnterKeyType TextInputActionToIMFAdapter(InputInfo inputInfo);
  void ComputeEditorInfo(InputInfo inputInfo, int32_t customEnterKeyType);
  std::shared_ptr<IMFCursorInfoAdapter> GetCursorInfo();
  void PreviewTextHandlerOnUI(const std::u16string& text,
                              int32_t start,
                              int32_t end);
  void FinishPreviewTextOnUI();
  void SetNeedUnderLineOnUI(bool is_need_underline);
  void ClearComposingStatus();
  int32_t UpdateCompositionInfo(const std::u16string& text,
                                int32_t start,
                                int32_t end);
  int32_t GetCompositionTypeAndCheckInput(const std::u16string& text,
                                          int32_t start,
                                          int32_t end,
                                          CompositionType& composition_type);
  void CancelPreviewHandlerOnUI();
  void SendEnterKeyEventOnUI(int32_t enterKeyType);
  bool IsTextInputStateChange(const CefString& text,
                              const CefRange& selected_range,
                              const CefRange& compositon_range);
  bool AttachToSystemIME(bool is_need_reset_listener, int32_t requestKeyboardReason = 0);
  void SetNeedReattach(HideTextinputType hideType);
  bool IsKeyboardShow() override;
  bool NeedKeyboardShow();

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void AutoFillWithIMFEventOnUI(bool is_username,
                                bool is_other_account,
                                bool is_new_password,
                                const std::string& content);
#endif

  static uint32_t lastAttachNWebId_;
  static IMFAdapterTextInputType lastInputMode_;
  uint32_t nweb_id_ = 0;
  CefRefPtr<CefBrowser> browser_;
  bool ime_shown_ = false;
  bool ime_text_composing_ = false;
  std::u16string selected_text_;
  std::u16string composing_text_;
  std::u16string whole_text_;
  int selected_from_;
  int selected_to_;
  CefRect focus_rect_;
  double offset_x_ = 0;
  double offset_y_ = 0;
  float device_pixel_ratio_;
  bool focus_status_ = false;
  bool focus_rect_status_ = false;
  std::unique_ptr<IMFAdapter> inputmethod_adapter_ = nullptr;
  std::shared_ptr<IMFTextListenerAdapter> inputmethod_listener_ = nullptr;
  bool isAttached_ = false;
  bool show_keyboard_ = false;
  bool is_editable_node_ = false;
  bool isNeedReattachOncontinue_ = false;
  IMFAdapterTextInputType imf_input_mode_ = IMFAdapterTextInputType::TEXT;
  IMFAdapterEnterKeyType imf_input_action_ = IMFAdapterEnterKeyType::GO;
  bool type_text_flag_multi_line_ = false;
  std::chrono::high_resolution_clock::time_point lastCloseInputMethodTime_;
  bool isNeedReattachOnfocus_ = false;
  bool isFocusSwitchOnBlur_ = false;
  int32_t input_flags_ = 0;
  int32_t input_node_id_ = -1;
  bool input_is_password_ = false;

  int textCursorReady_ = 0;
  std::mutex textCursorMutex_;
  std::condition_variable textCursorCv_;
  bool is_need_notify_all_ = false;
  int32_t text_cursor_length_ = 0;
  uint32_t windowId_ = 0;
  const int32_t OK = 0;
  const int32_t ERROR = -1;
  bool is_need_underline_ = false;
  bool has_composition_ = false;
  std::u16string preview_text_cache_;
  int32_t composition_range_start_ = 0;
  int32_t composition_range_end_ = 0;
  CompositionType composition_type_ = COMPOSITION_INVALID;
  int32_t composition_cursor_index_ = 0;
  bool isManualCloseKeyboard_ = false;
  bool isAttachSuccess_ = true;
  cef_text_input_mode_t cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_DEFAULT;

  std::unordered_map<char16_t, int> keycode_map = {
      {'q', 0x51}, {'w', 0x57},  {'e', 0x45}, {'r', 0x52},  {'t', 0x54},
      {'y', 0x59}, {'u', 0x55},  {'i', 0x49}, {'o', 0x4F},  {'p', 0x50},
      {'a', 0x41}, {'s', 0x53},  {'d', 0x44}, {'f', 0x46},  {'g', 0x47},
      {'h', 0x48}, {'j', 0x4A},  {'k', 0x4B}, {'l', 0x4C},  {'z', 0x5A},
      {'x', 0x58}, {'c', 0x43},  {'v', 0x56}, {'b', 0x42},  {'n', 0x4E},
      {'m', 0x4D}, {'1', 0x31},  {'2', 0x32}, {'3', 0x33},  {'4', 0x34},
      {'5', 0x35}, {'6', 0x36},  {'7', 0x37}, {'8', 0x38},  {'9', 0x39},
      {'0', 0x30}, {'`', 0xC0},  {'-', 0xBD}, {'=', 0xBB},  {'[', 0xDB},
      {']', 0xDD}, {'\\', 0xDC}, {';', 0xBA}, {'\'', 0xDE}, {',', 0xBC},
      {'.', 0xBE}, {'/', 0xBF},  {' ', 0x20}, {'Q', 0x51},  {'W', 0x57},
      {'E', 0x45}, {'R', 0x52},  {'T', 0x54}, {'Y', 0x59},  {'U', 0x55},
      {'I', 0x49}, {'O', 0x4F},  {'P', 0x50}, {'A', 0x41},  {'S', 0x53},
      {'D', 0x44}, {'F', 0x46},  {'G', 0x47}, {'H', 0x48},  {'J', 0x4A},
      {'K', 0x4B}, {'L', 0x4C},  {'Z', 0x5A}, {'X', 0x58},  {'C', 0x43},
      {'V', 0x56}, {'B', 0x42},  {'N', 0x4E}, {'M', 0x4D},  {'!', 0x31},
      {'@', 0x32}, {'#', 0x33},  {'$', 0x34}, {'%', 0x35},  {'^', 0x36},
      {'&', 0x37}, {'*', 0x38},  {'(', 0x39}, {')', 0x30},  {'~', 0xC0},
      {'_', 0xBD}, {'+', 0xBB},  {'{', 0xDB}, {'}', 0xDD},  {'|', 0xDC},
      {':', 0xBA}, {'"', 0xDE},  {'<', 0xBC}, {'>', 0xBE},  {'?', 0xBF}};

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  std::string fill_content_;
  int32_t fill_content_node_id_ = -1;
#endif
  IMPLEMENT_REFCOUNTING(NWebInputMethodHandler);
};

enum ScanKeyCode {
  ESCAPE_SCAN_CODE = 0x009,
  DIGIT1_SCAN_CODE = 0x00A,
  DIGIT2_SCAN_CODE = 0x00B,
  DIGIT3_SCAN_CODE = 0x00C,
  DIGIT4_SCAN_CODE = 0x00D,
  DIGIT5_SCAN_CODE = 0x00E,
  DIGIT6_SCAN_CODE = 0x00F,
  DIGIT7_SCAN_CODE = 0x010,
  DIGIT8_SCAN_CODE = 0x011,
  DIGIT9_SCAN_CODE = 0x012,
  DIGIT0_SCAN_CODE = 0x013,
  MINUS_SCAN_CODE = 0x014,
  EQUAL_SCAN_CODE = 0x015,
  BACKSPACE_SCAN_CODE = 0x016,
  TAB_SCAN_CODE = 0x0017,
  KEYQ_SCAN_CODE = 0x0018,
  KEYW_SCAN_CODE = 0x0019,
  KEYE_SCAN_CODE = 0x001A,
  KEYR_SCAN_CODE = 0x001B,
  KEYT_SCAN_CODE = 0x001C,
  KEYY_SCAN_CODE = 0x001D,
  KEYU_SCAN_CODE = 0x001E,
  KEYI_SCAN_CODE = 0x001F,
  KEYO_SCAN_CODE = 0x0020,
  KEYP_SCAN_CODE = 0x0021,
  BRACKETLEFT_SCAN_CODE = 0x0022,
  BRACKETRIGHT_SCAN_CODE = 0x0023,
  ENTER_SCAN_CODE = 0x0024,
  CONTROLLEFT_SCAN_CODE = 0x0025,
  KEYA_SCAN_CODE = 0x0026,
  KEYS_SCAN_CODE = 0x0027,
  KEYD_SCAN_CODE = 0x0028,
  KEYF_SCAN_CODE = 0x0029,
  KEYG_SCAN_CODE = 0x002A,
  KEYH_SCAN_CODE = 0x002B,
  KEYJ_SCAN_CODE = 0x002C,
  KEYK_SCAN_CODE = 0x002D,
  KEYL_SCAN_CODE = 0x002E,
  SEMICOLON_SCAN_CODE = 0x002F,
  QUOTE_SCAN_CODE = 0x0030,
  BACKQUOTE_SCAN_CODE = 0x0031,
  SHIFTLEFT_SCAN_CODE = 0x0032,
  BACKSLASH_SCAN_CODE = 0x0033,
  KEYZ_SCAN_CODE = 0x0034,
  KEYX_SCAN_CODE = 0x0035,
  KEYC_SCAN_CODE = 0x0036,
  KEYV_SCAN_CODE = 0x0037,
  KEYB_SCAN_CODE = 0x0038,
  KEYN_SCAN_CODE = 0x0039,
  KEYM_SCAN_CODE = 0x003A,
  COMMA_SCAN_CODE = 0x003B,
  PERIOD_SCAN_CODE = 0x003C,
  SLASH_SCAN_CODE = 0x003D,
  SHIFTRIGHT_SCAN_CODE = 0x003E,
  NUMPADMULTIPLY_SCAN_CODE = 0x003F,
  ALTLEFT_SCAN_CODE = 0x0040,
  SPACE_SCAN_CODE = 0x0041,
  CAPSLOCK_SCAN_CODE = 0x0042,
  F1_SCAN_CODE = 0x0043,
  F2_SCAN_CODE = 0x0044,
  F3_SCAN_CODE = 0x0045,
  F4_SCAN_CODE = 0x0046,
  F5_SCAN_CODE = 0x0047,
  F6_SCAN_CODE = 0x0048,
  F7_SCAN_CODE = 0x0049,
  F8_SCAN_CODE = 0x004A,
  F9_SCAN_CODE = 0x004B,
  F10_SCAN_CODE = 0x004C,
  NUMLOCK_SCAN_CODE = 0x004D,
  SCROLLLOCK_SCAN_CODE = 0x004E,
  NUMPAD7_SCAN_CODE = 0x004F,
  NUMPAD8_SCAN_CODE = 0x0050,
  NUMPAD9_SCAN_CODE = 0x0051,
  NUMPADSUBTRACT_SCAN_CODE = 0x0052,
  NUMPAD4_SCAN_CODE = 0x0053,
  NUMPAD5_SCAN_CODE = 0x0054,
  NUMPAD6_SCAN_CODE = 0x0055,
  NUMPADADD_SCAN_CODE = 0x0056,
  NUMPAD1_SCAN_CODE = 0x0057,
  NUMPAD2_SCAN_CODE = 0x0058,
  NUMPAD3_SCAN_CODE = 0x0059,
  NUMPAD0_SCAN_CODE = 0x005A,
  NUMPADDECIMAL_SCAN_CODE = 0x005B,
  INTLBACKSLASH_SCAN_CODE = 0x005E,
  F11_SCAN_CODE = 0x005F,
  F12_SCAN_CODE = 0x0060,
  INTLRO_SCAN_CODE = 0x0061,
  CONVERT_SCAN_CODE = 0x0064,
  KANAMODE_SCAN_CODE = 0x0065,
  NONCONVERT_SCAN_CODE = 0x0066,
  NUMPADENTER_SCAN_CODE = 0x0068,
  CONTROLRIGHT_SCAN_CODE = 0x0069,
  NUMPADDIVIDE_SCAN_CODE = 0x006A,
  PRINTSCREEN_SCAN_CODE = 0x006B,
  ALTRIGHT_SCAN_CODE = 0x006C,
  HOME_SCAN_CODE = 0x006E,
  UP_SCAN_CODE = 0x006F,
  PAGE_UP_SCAN_CODE = 0x0070,
  LEFT_SCAN_CODE = 0x0071,
  RIGHT_SCAN_CODE = 0x0072,
  END_SCAN_CODE = 0x0073,
  DOWN_SCAN_CODE = 0x0074,
  PAGE_DOWN_SCAN_CODE = 0x0075,
  INSERT_SCAN_CODE = 0x0076,
  DELETE_SCAN_CODE = 0x0077,
  NUMPADEQUAL_SCAN_CODE = 0x007D,
  PAUSE_SCAN_CODE = 0x007F,
  NUMPADCOMMA_SCAN_CODE = 0x0081,
  METALEFT_SCAN_CODE = 0x0085,
  METARIGHT_SCAN_CODE = 0x0086,
};

enum class FocusType : int32_t {
  // Map to: blink.mojom.FocusType.kNone
  NONE = 0,

  // Map to: blink.mojom.FocusType.kScript
  SCRIPT = 1,

  // Map to: blink.mojom.FocusType.kForward
  FORWARD = 2,

  // Map to: blink.mojom.FocusType.kBackward
  BACKWARD = 3,

  // Map to: blink.mojom.FocusType.kSpatialNavigation
  SPATIALNAVIGATION = 4,

  // Map to: blink.mojom.FocusType.kMouse
  MOUSE = 5,

  // Map to: blink.mojom.FocusType.kAccessKey
  ACCESSKEY = 6,

  // Map to: blink.mojom.FocusType.kPage
  PAGE = 7,

  MAXVALUE = 7,
};
}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_NWEB_INPUTMETHOD_HANDLER_H_

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

#ifndef OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_INPUTMETHOD_CLIENT_H_
#define OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_INPUTMETHOD_CLIENT_H_

#include "include/cef_base.h"
#include "include/cef_browser.h"

namespace OHOS::NWeb {
class NWebInputMethodClient : public virtual CefBaseRefCounted {
 public:
  enum class HideTextinputType {
    FROM_KERNEL,
    FROM_ONBLUR,
    FROM_ONPAUSE,
  };

  typedef cef_text_input_info_t InputInfo;

  virtual ~NWebInputMethodClient() = default;
  virtual void Attach(CefRefPtr<CefBrowser> browser,
                      InputInfo inputInfo,
                      bool is_need_reset_listener,
                      int32_t enterKeyType) = 0;
  virtual void ShowTextInput() = 0;
  virtual void HideTextInput(
      uint32_t nweb_id = 0,
      HideTextinputType hideType = HideTextinputType::FROM_KERNEL) = 0;
  virtual void HideTextInputForce() = 0;
  virtual void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                      const CefString& selected_text,
                                      const CefRange& selected_range) = 0;
  virtual void OnCursorUpdate(const CefRect& rect) = 0;
  virtual void OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                                  const CefString& text,
                                  const CefRange& selected_range) = 0;
  virtual void SetFocusStatus(bool focus_status) = 0;
  virtual bool GetFocusStatus() = 0;
  virtual void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                                 bool is_editable_node) = 0;
  virtual bool GetIsEditableNode() = 0;
  virtual bool HasComposition() = 0;
  virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                            const CefRange& selected_range) = 0;
  virtual void OnUpdateTextInputStateCalled(CefRefPtr<CefBrowser> browser,
                                            const CefString& text,
                                            const CefRange& selected_range,
                                            const CefRange& compositon_range) = 0;
  virtual bool IsAttached() = 0;

#if defined(OHOS_PASSWORD_AUTOFILL)
  virtual void SetFillContent(const std::string& content, int32_t node_id) = 0;
#endif
};
}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_INPUTMETHOD_CLIENT_H_

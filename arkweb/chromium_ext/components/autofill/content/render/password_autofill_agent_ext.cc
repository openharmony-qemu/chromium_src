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

#include "arkweb/chromium_ext/components/autofill/content/render/password_autofill_agent_ext.h"

#include "arkweb/build/features/features.h"
#include "base/no_destructor.h"
#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_form_control_element.h"
#include "third_party/blink/public/web/web_form_element.h"
#include "third_party/blink/public/web/web_local_frame.h"

using blink::WebElement;
using blink::WebFormControlElement;
using blink::WebFormElement;
using blink::WebInputElement;
using blink::WebLocalFrame;
using blink::WebString;

namespace autofill {
using form_util::GetFieldRendererId;
using form_util::IsElementEditable;
extern WebInputElement FindUsernameElementPrecedingPasswordElement(
    WebLocalFrame* frame,
    const WebInputElement& password_element);
extern bool HasDocumentWithValidFrame(const WebInputElement& element);
extern bool IsUsernameAmendable(const WebInputElement& username_element,
                                bool is_password_field_selected);

PasswordAutofillAgentExt::PasswordAutofillAgentExt(
    content::RenderFrame* render_frame,
    blink::AssociatedInterfaceRegistry* registry)
    : PasswordAutofillAgent(render_frame, registry) {
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  ohos_last_supplied_password_info_iter_ =
      ohos_web_input_to_password_info_.end();
#endif
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
// Get placeholder of element
std::u16string GetPlaceHolderOfInput(const WebInputElement& element) {
  static base::NoDestructor<WebString> kPlaceholder("placeholder");
  if (element.HasAttribute(*kPlaceholder)) {
    return element.GetAttribute(*kPlaceholder).Utf16();
  }

  return std::u16string();
}

std::string GetAutocompleteAttribute(const WebInputElement& element) {
  return element.GetAttribute("autocomplete").Utf8();
}
#endif

void PasswordAutofillAgentExt::TrackAutofilledElement(
    const WebFormControlElement& element) {
#if BUILDFLAG(ARKWEB_BUGFIX_CRASH)
  if (autofill_agent_) {
    autofill_agent_->TrackAutofilledElement(element);
  }
#endif
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void PasswordAutofillAgentExt::OhosMaybeStoreFallbackData(
    const PasswordFormFillData& form_data) {
  if (!ohos_web_input_to_password_info_.empty()) {
    return;
  }
  // If for some reasons elements for filling were not found (for example
  // because they were renamed by JavaScript) then add fill data for
  // |web_input_to_password_info_|. When the user clicks on a password field
  // which is not a key in |web_input_to_password_info_|, the first element from
  // |web_input_to_password_info_| will be used in
  // PasswordAutofillAgent::FindPasswordInfoForElement to propose to fill.
  PasswordInfo password_info;
  password_info.fill_data = form_data;
  ohos_web_input_to_password_info_[WebInputElement()] = password_info;
  ohos_last_supplied_password_info_iter_ =
      ohos_web_input_to_password_info_.begin();
}

void PasswordAutofillAgentExt::OhosStoreInferredInfo(
    const PasswordFormFillData& form_data,
    WebInputElement username_element,
    WebInputElement password_element) {
  WebInputElement main_element =
      username_element.IsNull() ? password_element : username_element;

  LOG(INFO) << "[Autofill] Store inferred filling infomation:"
            << ", username element is null:" << username_element.IsNull()
            << ", password element is null:" << password_element.IsNull();
  PasswordInfo password_info;
  password_info.fill_data = form_data;
  if (!password_element.IsNull()) {
    password_info.password_field = FieldRef(password_element);
  }
  ohos_web_input_to_password_info_[main_element] = password_info;
  ohos_last_supplied_password_info_iter_ =
      ohos_web_input_to_password_info_.find(main_element);
  if (!main_element.IsPasswordFieldForAutofill()) {
    ohos_password_to_username_[password_element] = username_element;
  }
}

// mojom::PasswordAutofillAgent:
void PasswordAutofillAgentExt::SetParsedPasswordFormExt(
    const PasswordFormFillData& form_data) {
  bool username_password_fields_not_set =
      form_data.username_element_renderer_id.is_null() &&
      form_data.password_element_renderer_id.is_null();
  if (username_password_fields_not_set) {
    LOG(INFO) << "[Autofill] Username or password fields not set";
    // No fields for filling were found during parsing, which means filling
    // fallback case. So save data for fallback filling.
    OhosMaybeStoreFallbackData(form_data);
    return;
  }

  WebInputElement username_element, password_element;
  std::tie(username_element, password_element) =
      FindUsernamePasswordElements(form_data);
  bool is_single_username_fill =
      form_data.password_element_renderer_id.is_null();
  WebElement main_element =
      is_single_username_fill ? username_element : password_element;
  if (main_element.IsNull()) {
    LOG(INFO) << "[Autofill] Main element element is null";
    OhosMaybeStoreFallbackData(form_data);
    return;
  }

  OhosStoreInferredInfo(form_data, username_element, password_element);
}

bool PasswordAutofillAgentExt::IsPasswordAutofill(
    const blink::WebInputElement& input_element) {
  if (input_element.IsNull()) {
    LOG(ERROR) << "[Autofill] input_element is null";
    return false;
  }

  WebInputElement username_element;
  WebInputElement password_element;
  PasswordInfo* password_info = nullptr;
  if (IsElementEditable(input_element) &&
      OhosFindPasswordInfoForElement(input_element, UseFallbackData(true),
                                     &username_element, &password_element,
                                     &password_info)) {
    return true;
  }
  return false;
}

bool PasswordAutofillAgentExt::OhosFindPasswordInfoForElement(
    const WebInputElement& element,
    UseFallbackData use_fallback_data,
    WebInputElement* username_element,
    WebInputElement* password_element,
    PasswordInfo** password_info) {
  DCHECK(username_element && password_element && password_info);
  username_element->Reset();
  password_element->Reset();
  if (!element.IsPasswordFieldForAutofill()) {
    *username_element = element;
  } else {
    *password_element = element;

    // If there is a password field, but a request to the store hasn't been sent
    // yet, then do fetch saved credentials now.
    if (!sent_request_to_store_) {
      SendPasswordForms(false);
      return false;
    }

    auto iter = ohos_web_input_to_password_info_.find(element);
    if (iter == ohos_web_input_to_password_info_.end()) {
      PasswordToLoginMap::const_iterator password_iter =
          ohos_password_to_username_.find(element);
      if (password_iter == ohos_password_to_username_.end()) {
        if (!use_fallback_data || ohos_web_input_to_password_info_.empty()) {
          return false;
        }
        iter = ohos_last_supplied_password_info_iter_;
      } else {
        *username_element = password_iter->second;
      }
    }

    if (iter != ohos_web_input_to_password_info_.end()) {
      // It's a password field without corresponding username field. Try to find
      // the username field based on visibility.
      *username_element = FindUsernameElementPrecedingPasswordElement(
          render_frame()->GetWebFrame(), *password_element);
      *password_info = &iter->second;
      return true;
    }
    // Otherwise |username_element| has been set above.
  }

  auto iter = ohos_web_input_to_password_info_.find(*username_element);
  if (iter == ohos_web_input_to_password_info_.end()) {
    return false;
  }

  *password_info = &iter->second;
  if (password_element->IsNull()) {
    if (WebInputElement password_input = (*password_info)
                                             ->password_field.GetField()
                                             .DynamicTo<WebInputElement>()) {
      *password_element = password_input;
    }
  }
  return true;
}

// The password filling takes effect only in the input box of the form element.
bool PasswordAutofillAgentExt::RequestAutofill(
    const WebFormControlElement& control_element,
    bool is_text_changed) {
  const WebInputElement input_element =
      control_element.DynamicTo<WebInputElement>();
  if (input_element.IsNull()) {
    return false;
  }

  WebInputElement username_element;
  WebInputElement password_element;
  PasswordInfo* password_info = nullptr;
  if (!IsElementEditable(input_element) ||
      !OhosFindPasswordInfoForElement(input_element, UseFallbackData(true),
                                      &username_element, &password_element,
                                      &password_info)) {
    LOG(INFO) << "[Autofill] Find no password info for autofill request";
    return false;
  }

  if (!HasDocumentWithValidFrame(input_element)) {
    LOG(INFO) << "[Autofill] No document with valid frame for input element";
    return false;
  }

  WebFormElement form = !password_element.IsNull() ? password_element.Form()
                                                   : username_element.Form();
  // A Valid FormRendererId is start from 1, use 0 as an unowned form id for
  // FormControls outside the <form> which has no form_id.
  FormRendererId form_id(form.IsNull() ? 0 : form.UniqueRendererFormId());

  bool has_editable_username_element =
      !username_element.IsNull() && IsElementEditable(username_element);
  bool has_editable_password_element =
      !password_element.IsNull() && IsElementEditable(password_element);
  DCHECK(has_editable_username_element || has_editable_password_element);

  // Highlight the fields that are about to be filled by the user and remember
  // the old autofill state of |username_element| and |password_element|.
  autofill::InputFillRequestData username_data;
  if (has_editable_username_element) {
    username_autofill_state_ = username_element.GetAutofillState();

    username_data.field_renderer_id = GetFieldRendererId(username_element);
    username_data.is_focused = !input_element.IsPasswordFieldForAutofill();
    username_data.type = mojom::OhosInputElementType::kUsernameType;
    // TODO(ARKWEB_PASSWORD_AUTOFILL)
    // The "ElementBoundsInWindow" has changed "ConvertViewportToWindow" in base
    // 132.
    //  Returns the bounds of |element| in Window coordinates which are device
    //  scale independent. The bounds have been adjusted to include any
    //  transformations, including page scale. This function will update the
    //  layout if required.
    username_data.bounds =
        render_frame()->ElementBoundsInWindow(username_element);
    username_data.value = username_element.Value().Utf16();
    username_data.placeholder = GetPlaceHolderOfInput(username_element);
    username_data.autocomplete_attr =
        GetAutocompleteAttribute(username_element);
  } else {
    username_data.type = mojom::OhosInputElementType::kInvalid;
  }

  autofill::InputFillRequestData password_data;
  if (has_editable_password_element) {
    password_autofill_state_ = password_element.GetAutofillState();

    password_data.field_renderer_id = GetFieldRendererId(password_element);
    password_data.is_focused = !username_data.is_focused;
    password_data.type = mojom::OhosInputElementType::kPasswordType;
    // TODO(ARKWEB_PASSWORD_AUTOFILL)
    // The "ElementBoundsInWindow" has changed "ConvertViewportToWindow" in base
    // 132.
    //  Returns the bounds of |element| in Window coordinates which are device
    //  scale independent. The bounds have been adjusted to include any
    //  transformations, including page scale. This function will update the
    //  layout if required.
    password_data.bounds =
        render_frame()->ElementBoundsInWindow(password_element);
    password_data.value = password_element.Value().Utf16();
    password_data.placeholder = GetPlaceHolderOfInput(password_element);
    password_data.autocomplete_attr =
        GetAutocompleteAttribute(password_element);
  } else {
    password_data.type = mojom::OhosInputElementType::kInvalid;
  }

  auto it = ohos_password_form_status_map_.find(*form_id);
  if (it == ohos_password_form_status_map_.end()) {
    ohos_password_form_status_map_[*form_id] =
        mojom::OhosPasswordFormAutofillState::kNotRequested;
  }

  auto state = is_text_changed
                   ? mojom::OhosPasswordFormAutofillState::kTextChanged
                   : ohos_password_form_status_map_[*form_id];
  GetPasswordManagerDriver().OnRequestAutofill(form_id, state, username_data,
                                               password_data);

  if (ohos_password_form_status_map_[*form_id] ==
      mojom::OhosPasswordFormAutofillState::kNotRequested) {
    ohos_password_form_status_map_[*form_id] =
        mojom::OhosPasswordFormAutofillState::kHasBeenRequested;
  }
  return true;
}

bool PasswordAutofillAgentExt::FillAccountSuggestion(
    const WebFormControlElement& control_element,
    const std::u16string& username,
    const std::u16string& password) {
  // The element in context of the suggestion popup.
  WebInputElement element = control_element.DynamicTo<WebInputElement>();
  if (element.IsNull()) {
    LOG(INFO) << "[Autofill] Fill suggestion but element is null.";
    return false;
  }

  WebInputElement username_element;
  WebInputElement password_element;
  PasswordInfo* password_info = nullptr;
  if (!OhosFindPasswordInfoForElement(element, UseFallbackData(true),
                                      &username_element, &password_element,
                                      &password_info) ||
      (!password_element.IsNull() && !IsElementEditable(password_element))) {
    LOG(INFO) << "[Autofill] Fill suggestion but no username or password.";
    return false;
  }

  password_info->password_was_edited_last = false;
  if (element.IsPasswordFieldForAutofill()) {
    password_info->password_field_suggestion_was_accepted = true;
    password_info->password_field = FieldRef(password_element);
    ;
  }

  if (IsUsernameAmendable(username_element,
                          element.IsPasswordFieldForAutofill()) &&
      !(username.empty() && element.IsPasswordFieldForAutofill()) &&
      username_element.Value().Utf16() != username) {
    DoFillField(username_element, username,
                AutofillSuggestionTriggerSource::kUnspecified);
  }

  if (!password_element.IsNull()) {
    FillPasswordFieldAndSave(password_element, password,
                             AutofillSuggestionTriggerSource::kUnspecified);
    // TODO(crbug.com/1319364): As Touch-To-Fill and auto-submission don't
    // currently support filling single username fields, the code below is
    // within |!password_element.IsNull()|. Support such fields too and move the
    // code out the condition.
    // If the |username_element| is visible/focusable and the |password_element|
    // is not, trigger submission on the former as the latter unlikely has an
    // Enter listener.
    if (!username_element.IsNull() && username_element.IsFocusable() &&
        !password_element.IsFocusable()) {
      field_renderer_id_to_submit_ = GetFieldRendererId(username_element);
    } else {
      field_renderer_id_to_submit_ = GetFieldRendererId(password_element);
    }
  }

  element.SetSelectionRange(element.Value().length(), element.Value().length());

  return true;
}

void PasswordAutofillAgentExt::AutofillSurfaceClosed(
    bool show_virtual_keyboard) {
  LOG(INFO) << "AutofillSurfaceClosed, show_virtual_keyboard="
            << show_virtual_keyboard;
  auto focused_input = last_queried_element().DynamicTo<WebInputElement>();
  if (!focused_input || focused_input.IsReadOnly()) {
    LOG(ERROR) << "input element not focused or is read only";
    return;
  }

  if (show_virtual_keyboard && render_frame()) {
    render_frame()->ShowVirtualKeyboard();
  }
}
#endif

void PasswordAutofillAgentExt::CleanupOnDocumentShutdownExt() {
  ohos_password_form_status_map_.clear();
  ohos_web_input_to_password_info_.clear();
  ohos_password_to_username_.clear();
  ohos_last_supplied_password_info_iter_ =
      ohos_web_input_to_password_info_.end();
}
}  // namespace autofill

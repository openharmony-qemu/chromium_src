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
#include "arkweb/chromium_ext/components/autofill/content/render/autofill_agent_ext.h"

#include "base/functional/bind.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "components/autofill/content/renderer/password_generation_agent.h"
#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/web/web_autofill_state.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_form_control_element.h"
#include "third_party/blink/public/web/web_form_element.h"
#include "third_party/blink/public/web/web_input_element.h"
#include "third_party/blink/public/web/web_node.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/public/web/web_settings.h"
#include "third_party/blink/renderer/core/frame/settings.h"

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif

using blink::WebAutofillState;
using blink::WebDocument;
using blink::WebElement;
using blink::WebFormControlElement;
using blink::WebFormElement;
using blink::WebInputElement;
using blink::WebNode;

namespace autofill {
#if BUILDFLAG(ARKWEB_AUTOFILL)
constexpr base::TimeDelta kWaitTimeForScrollIntoViewMs =
    base::Milliseconds(700);
constexpr base::TimeDelta kWaitTimeForRequestAutoFillMs =
    base::Milliseconds(50);
#endif

AutofillAgentExt::AutofillAgentExt(
    content::RenderFrame* render_frame,
    Config config,
    std::unique_ptr<PasswordAutofillAgent> password_autofill_agent,
    std::unique_ptr<PasswordGenerationAgent> password_generation_agent,
    blink::AssociatedInterfaceRegistry* registry)
    : AutofillAgent(render_frame,
                    config,
                    std::move(password_autofill_agent),
                    std::move(password_generation_agent),
                    registry) {}

AutofillAgentExt::~AutofillAgentExt() {}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void AutofillAgentExt::ArkFillAccountSuggestion(
    const std::u16string& username,
    const std::u16string& password) {
  WebFormControlElement last_queried_element = last_queried_element_.GetField();
  if (!last_queried_element) {
    return;
  }

  bool handled =
      password_autofill_agent_->AsPasswordAutofillAgentExt()
          ->FillAccountSuggestion(last_queried_element, username, password);
  DCHECK(handled);
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
// LCOV_EXCL_START
void AutofillAgentExt::OhFormControlElementClicked() {
  if (!isAutofillEnabled()) {
    LOG(INFO) << "[PassWord Autofill] autofill interception successful.";
    return;
  }
  WebElement focused_element =
      unsafe_render_frame()->GetWebFrame()->GetDocument().FocusedElement();
  if (!focused_element.IsNull() && focused_element.IsFormControlElement()) {
    WebFormControlElement focused_form_control_element =
        focused_element.To<WebFormControlElement>();
    if (form_util::IsTextAreaElementOrTextInput(focused_form_control_element)) {
      LOG(INFO) << "[Autofill] Mouse down triggers RequestAutofill";
      bool result = password_autofill_agent_->AsPasswordAutofillAgentExt()
                        ->RequestAutofill(focused_form_control_element);
      if (result && base::ohos::IsPcDevice()) {
        is_popup_possibly_visible_ = true;
      }
    }
  }
}
// LCOV_EXCL_STOP
#endif

#if BUILDFLAG(ARKWEB_AUTOFILL)
void AutofillAgentExt::OhAutoFillFormControlElementClicked(
    const WebNode& node) {
  WebInputElement input_element = node.DynamicTo<WebInputElement>();
  if (input_element.IsNull() || !node.Focused()) {
    return;
  }
  auto is_password_autofill =
      password_autofill_agent_->AsPasswordAutofillAgentExt()
          ->IsPasswordAutofill(input_element);
  if (is_password_autofill) {
    return;
  }
  if (is_popup_created_by_focus_change_) {
    is_popup_created_by_focus_change_ = false;
    return;
  }
  HidePopup();
  HandleFocusChangeComplete(/*focused_node_was_last_clicked=*/node.Focused());
  is_need_to_created_popup_ = true;
  created_popup_time_ = base::TimeTicks::Now();
}
#endif

#if BUILDFLAG(ARKWEB_AUTOFILL)
// LCOV_EXCL_START
bool AutofillAgentExt::OhAutoFillDidChangeScrollOffset() {
  WebFormControlElement last_queried_element = last_queried_element_.GetField();
  if (!last_queried_element) {
    return false;
  }
  const WebInputElement input_element =
      last_queried_element.DynamicTo<WebInputElement>();
  if (input_element.IsNull()) {
    return false;
  }
  auto is_password_autofill =
      password_autofill_agent_->AsPasswordAutofillAgentExt()
          ->IsPasswordAutofill(input_element);
  if (is_need_to_created_popup_ && !is_password_autofill &&
      base::TimeTicks::Now() > created_popup_time_ &&
      base::TimeTicks::Now() <
          created_popup_time_ + kWaitTimeForScrollIntoViewMs) {
    if (autofill_scroll_timer_.IsRunning()) {
      autofill_scroll_timer_.Stop();
    }
    autofill_scroll_timer_.Start(FROM_HERE, kWaitTimeForRequestAutoFillMs,
                                 base::BindOnce(
                                     [](base::WeakPtr<AutofillAgent> self) {
                                       if (!self) {
                                         return;
                                       }
                                       self->HidePopup();
                                       self->HandleFocusChangeComplete(true);
                                     },
                                     weak_ptr_factory_.GetWeakPtr()));
    return true;
  }
  return false;
}
// LCOV_EXCL_STOP
#endif

bool AutofillAgentExt::FillFieldWithValue(FieldRendererId field_id,
                                          const std::u16string& value) {
  LOG(INFO) << "FillFieldWithValue";
  WebFormControlElement last_queried_element = last_queried_element_.GetField();
  if (!last_queried_element) {
    return true;
  }

  if (field_id != form_util::GetFieldRendererId(last_queried_element)) {
    // oh supports fill multiple form fields, including unfocused
    // field
    WebFormElement form = last_queried_element.Form();
    if (form.IsNull()) {
      WebDocument document = GetDocument();
      if (document.IsNull()) {
        return true;
      }
      WebFormControlElement fillElement =
          form_util::GetFormControlByRendererId(field_id);
      if (fillElement.IsNull()) {
        return true;
      }
      DoFillFieldWithValue(value, fillElement, WebAutofillState::kAutofilled);
      return true;
    }
    for (WebFormControlElement& formElement : form.GetFormControlElements()) {
      if (formElement.IsNull() ||
          form_util::GetFieldRendererId(formElement) != field_id) {
        continue;
      }
      DoFillFieldWithValue(value, formElement, WebAutofillState::kAutofilled);
      break;
    }
    return true;
  }
  return false;
}

bool AutofillAgentExt::isAutofillEnabled() const {
  if (GetDocument() && GetDocument().GetFrame()) {
    auto webview = GetDocument().GetFrame()->View();
    if (webview && webview->GetSettings()) {
      bool autofill_enabled = webview->GetSettings()->GetEnableAutoFill();
      return autofill_enabled;
    }
  }
  LOG(INFO) << "[PassWord Autofill] Failed to get the autofill interception state.";
  return true;
}
}  // namespace autofill

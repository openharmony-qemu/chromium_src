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

#ifndef COMPONENTS_AUTOFILL_CONTENT_RENDERER_PASSWORD_AUTOFILL_AGENT_EXT_H_
#define COMPONENTS_AUTOFILL_CONTENT_RENDERER_PASSWORD_AUTOFILL_AGENT_EXT_H_

#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "arkweb/build/features/features.h"

#include "base/types/strong_alias.h"
#include "components/autofill/core/common/password_form_fill_data.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/web/web_form_control_element.h"
#include "third_party/blink/public/web/web_input_element.h"

namespace autofill {
class PasswordAutofillAgent;

class PasswordAutofillAgentExt : public PasswordAutofillAgent {
 public:
  using UseFallbackData = base::StrongAlias<class UseFallbackDataTag, bool>;
  using WebInputToPasswordInfoMap =
      std::map<blink::WebInputElement, PasswordInfo>;
  using PasswordToLoginMap =
      std::map<blink::WebInputElement, blink::WebInputElement>;

  PasswordAutofillAgentExt(content::RenderFrame* render_frame,
                           blink::AssociatedInterfaceRegistry* registry);

  PasswordAutofillAgentExt(const PasswordAutofillAgentExt&) = delete;
  PasswordAutofillAgentExt& operator=(const PasswordAutofillAgentExt&) = delete;

  PasswordAutofillAgentExt* AsPasswordAutofillAgentExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  bool RequestAutofill(const blink::WebFormControlElement& control_element,
                       bool is_text_changed = false);

  bool FillAccountSuggestion(
      const blink::WebFormControlElement& control_element,
      const std::u16string& username,
      const std::u16string& password);

  // mojom::PasswordAutofillAgent:
  void SetParsedPasswordFormExt(const PasswordFormFillData& form_data);

  void AutofillSurfaceClosed(bool show_virtual_keyboard) override;

  bool IsPasswordAutofill(const blink::WebInputElement& input_element);
  bool OhosFindPasswordInfoForElement(const blink::WebInputElement& element,
                                      UseFallbackData use_fallback_data,
                                      blink::WebInputElement* username_element,
                                      blink::WebInputElement* password_element,
                                      PasswordInfo** password_info);

  void OhosMaybeStoreFallbackData(const PasswordFormFillData& form_data);

  void OhosStoreInferredInfo(const PasswordFormFillData& form_data,
                             blink::WebInputElement username_element,
                             blink::WebInputElement password_element);
  void CleanupOnDocumentShutdownExt();
  void TrackAutofilledElement(
      const blink::WebFormControlElement& element) override;

  // Indicates whether the field is filled, previewed, or not filled by
  // autofill.
  blink::WebAutofillState username_autofill_state_;
  // Indicates whether the field is filled, previewed, or not filled by
  // autofill.
  blink::WebAutofillState password_autofill_state_;

  // The mapping between input element with parsed PasswordForm(
  // inferred username or password).
  WebInputToPasswordInfoMap ohos_web_input_to_password_info_;
  // A (sort-of) reverse map to |ohos_web_input_to_password_info_|.
  PasswordToLoginMap ohos_password_to_username_;
  // The chronologically last insertion into |ohos_web_input_to_password_info_|.
  WebInputToPasswordInfoMap::iterator ohos_last_supplied_password_info_iter_;

  // Current ohos password autofill state of form_renderer_id.
  std::unordered_map<std::uint64_t, mojom::OhosPasswordFormAutofillState>
      ohos_password_form_status_map_;
#endif
};
}  // namespace autofill
#endif  // COMPONENTS_AUTOFILL_CONTENT_RENDERER_PASSWORD_AUTOFILL_AGENT_EXT_H_

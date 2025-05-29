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

#ifndef COMPONENTS_AUTOFILL_CONTENT_RENDERER_AUTOFILL_AGENT_EXT_H_
#define COMPONENTS_AUTOFILL_CONTENT_RENDERER_AUTOFILL_AGENT_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/compiler_specific.h"
#include "base/feature_list.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/raw_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/types/strong_alias.h"
#include "components/autofill/content/renderer/autofill_agent.h"

namespace blink {
class WebNode;
class WebFormControlElement;
class WebFormElement;
}  // namespace blink

namespace autofill {

class PasswordAutofillAgent;
class PasswordGenerationAgent;
class AutofillAgent;

class AutofillAgentExt : public AutofillAgent {
 public:
  AutofillAgentExt(
      content::RenderFrame* render_frame,
      Config config,
      std::unique_ptr<PasswordAutofillAgent> password_autofill_agent,
      std::unique_ptr<PasswordGenerationAgent> password_generation_agent,
      blink::AssociatedInterfaceRegistry* registry);

  AutofillAgentExt(const AutofillAgentExt&) = delete;
  AutofillAgentExt& operator=(const AutofillAgentExt&) = delete;
  ~AutofillAgentExt() override;
  AutofillAgentExt* AsAutofillAgentExt() override { return this; }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void ArkFillAccountSuggestion(const std::u16string& username,
                                const std::u16string& password);
  void OhFormControlElementClicked();
#endif
#if BUILDFLAG(ARKWEB_AUTOFILL)
  void OhAutoFillFormControlElementClicked(const blink::WebNode& node);
  bool OhAutoFillDidChangeScrollOffset();
  bool FillFieldWithValue(FieldRendererId field_id,
                          const std::u16string& value);
  void SetIsNeedToCreatedPopup(bool is_need_to_created_popup) {
    is_need_to_created_popup_ = is_need_to_created_popup;
  }
  void SetIsPopupCreatedByFocusChange(bool is_popup_created_by_focus_change) {
    is_popup_created_by_focus_change_ = is_popup_created_by_focus_change;
  }
  void SetCreatedPopupTime(base::TimeTicks created_popup_time) {
    created_popup_time_ = created_popup_time;
  }

  base::OneShotTimer autofill_scroll_timer_;
  bool is_popup_created_by_focus_change_ = false;
  bool is_need_to_created_popup_ = false;
  base::TimeTicks created_popup_time_ = base::TimeTicks::Now();
#endif
};
}  // namespace autofill
#endif  // COMPONENTS_AUTOFILL_CONTENT_RENDERER_AUTOFILL_AGENT_EXT_H_

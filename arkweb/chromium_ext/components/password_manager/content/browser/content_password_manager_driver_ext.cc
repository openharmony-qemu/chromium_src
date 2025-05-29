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

#include "arkweb/chromium_ext/components/password_manager/content/browser/content_password_manager_driver_ext.h"
#include "components/password_manager/content/browser/bad_message.h"

namespace password_manager {
ContentPasswordManagerDriverExt::ContentPasswordManagerDriverExt(
    content::RenderFrameHost* render_frame_host,
    PasswordManagerClient* client)
    : ContentPasswordManagerDriver(render_frame_host, client) {}

ContentPasswordManagerDriverExt::~ContentPasswordManagerDriverExt() {}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void ContentPasswordManagerDriverExt::OnRequestAutofill(
    autofill::FormRendererId form_id,
    const autofill::mojom::OhosPasswordFormAutofillState state,
    const autofill::InputFillRequestData& username_data,
    const autofill::InputFillRequestData& password_data) {
  if (!password_manager::bad_message::CheckFrameNotPrerendering(
          render_frame_host_)) {
    return;
  }

  // Remove sensitive information before sending to external systems.
  if (!GetLastCommittedURL().is_valid()) {
    return;
  }
  GURL page_origin = url::Origin::Create(GetLastCommittedURL()).GetURL();
  client_->AsPasswordManagerClientExt()->OnRequestAutofill(
      this, page_origin, form_id, state, username_data, password_data);
}

void ContentPasswordManagerDriverExt::FillAccountSuggestion(
    const GURL& page_url,
    const std::u16string& username,
    const std::u16string& password) {
  password_autofill_manager_.FillAccountSuggestion(page_url, username,
                                                   password);
}

void ContentPasswordManagerDriverExt::FillAccountSuggestion(
    const std::u16string& username,
    const std::u16string& password) {
  GetAutofillAgent()->FillAccountSuggestion(username, password);
}

void ContentPasswordManagerDriverExt::AutofillSurfaceClosed(
    bool show_virtual_keyboard) {
  if (const auto& agent = GetPasswordAutofillAgent()) {
    agent->AutofillSurfaceClosed(show_virtual_keyboard);
  }
}

void ContentPasswordManagerDriverExt::SendParsedPasswordFormToRenderer(
    const autofill::PasswordFormFillData& parsed_form_data_without_password) {
  if (const auto& agent = GetPasswordAutofillAgent()) {
    LOG(INFO) << "Set parsed password form.";
    agent->SetParsedPasswordForm(parsed_form_data_without_password);
  }
}
#endif

}  // namespace password_manager

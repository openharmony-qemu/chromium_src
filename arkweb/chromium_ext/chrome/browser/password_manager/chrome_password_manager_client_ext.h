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

#ifndef CHROME_BROWSER_PASSWORD_MANAGER_CHROME_PASSWORD_MANAGER_CLIENT_EXT_H_
#define CHROME_BROWSER_PASSWORD_MANAGER_CHROME_PASSWORD_MANAGER_CLIENT_EXT_H_

#include "chrome/browser/password_manager/chrome_password_manager_client.h"
#include "arkweb/build/features/features.h"

#include "components/autofill/core/common/unique_ids.h"
#include "components/password_manager/content/browser/content_password_manager_driver_factory.h"
#include "components/password_manager/core/browser/password_manager.h"

namespace content {
class WebContents;
}  // namespace content

class ChromePasswordManagerClient;

class ChromePasswordManagerClientExt
    : public ChromePasswordManagerClient {

public:
  explicit ChromePasswordManagerClientExt(content::WebContents* web_contents);
  ChromePasswordManagerClientExt(const ChromePasswordManagerClientExt&) = delete;
  ChromePasswordManagerClientExt& operator=(const ChromePasswordManagerClientExt&) =
      delete;

  ~ChromePasswordManagerClientExt();

  ChromePasswordManagerClientExt* AsChromePasswordManagerClientExt() override { return this; }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  struct AutofillIMFInfo {
    bool is_username = false;
    bool is_other_account = false;
    bool is_new_password = false;
  };

  void ProcessAutofillCancel(const std::string& fillContent);

  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const std::string& content);

  void FillData(const std::string& page_url,
                const std::string& username,
                const std::string& password,
                bool is_other_account);

  bool IsUsernamePasswordForm(autofill::FormRendererId form_id) {
    return form_id == last_fill_form_id_;
  }

  bool IsUsernamePasswordField(autofill::FieldRendererId field_id) {
    return field_id == last_fill_focus_renderer_id_;
  }

  void FillAccountSuggestion(const GURL& page_url,
                             const std::u16string& username,
                             const std::u16string& password) override;

  void OnRequestAutofill(
      password_manager::PasswordManagerDriver* driver,
      const GURL& page_url,
      autofill::FormRendererId form_id,
      const autofill::mojom::OhosPasswordFormAutofillState state,
      const autofill::InputFillRequestData& username_data,
      const autofill::InputFillRequestData& password_data) override;

  bool isSuppressing() { return suppressed_driver_.get(); }
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  std::optional<std::string> PasswordFormToJsonForRequest(
      const std::string& event,
      const GURL& page_url,
      const autofill::InputFillRequestData& username_data,
      const autofill::InputFillRequestData& password_data,
      AutofillIMFInfo* imf_info = nullptr);

  std::optional<std::string> PasswordFormToJsonForSave(
      const password_manager::PasswordForm& form);

  void SuppressKeyboard();
  void UnsuppressKeyboard();

  bool IsLoginInfoConsistentWithFilled(
      const password_manager::PasswordForm& info);

  void UpdateLastRequestFilledItems(
      const autofill::InputFillRequestData& username_data,
      const autofill::InputFillRequestData& password_data);

  void NotifyAutofillPopupShow(bool is_show);
  bool ArkPromptUserToSaveOrUpdatePassword(std::unique_ptr<password_manager::PasswordFormManagerForUI> form_to_save);
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  using AutofilledMap = std::set<std::string>;

  GURL form_to_request_url_;

  autofill::FormRendererId last_fill_form_id_;
  autofill::FieldRendererId last_fill_focus_renderer_id_;

  AutofilledMap auto_filled_forms_username_;
  AutofilledMap auto_filled_forms_password_;

  autofill::InputFillRequestData last_request_fill_username_;
  autofill::InputFillRequestData last_request_fill_password_;

  // Keyboard suppressor
  bool is_need_restore_keyboard_ = false;
  bool is_suppress_ime_callback_registered_ = false;
  raw_ptr<autofill::ContentAutofillDriver> suppressed_driver_ = nullptr;
  base::OneShotTimer unsuppress_timer_;

  base::WeakPtrFactory<ChromePasswordManagerClientExt> weak_ptr_factory_{this};
#endif
};
#endif  // CHROME_BROWSER_PASSWORD_MANAGER_CHROME_PASSWORD_MANAGER_CLIENT_EXT_H_

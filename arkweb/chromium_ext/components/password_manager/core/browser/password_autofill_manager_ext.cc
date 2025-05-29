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

#include "arkweb/chromium_ext/components/password_manager/core/browser/password_autofill_manager_ext.h"

#include "arkweb/build/features/features.h"
#include "components/autofill/core/common/password_form_fill_data.h"

namespace password_manager {
using autofill::PasswordFormFillData;

PasswordAutofillManagerExt::PasswordAutofillManagerExt(
    PasswordManagerDriver* password_manager_driver,
    autofill::AutofillClient* autofill_client,
    PasswordManagerClient* password_client)
    : PasswordAutofillManager(password_manager_driver,
                              autofill_client,
                              password_client) {}

PasswordAutofillManagerExt::~PasswordAutofillManagerExt() {}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
bool PasswordAutofillManagerExt::CanFillAccountSuggestion(
    const GURL& page_url) {
  url::Origin current_origin =
      url::Origin::Create(password_manager_driver_->GetLastCommittedURL());
  url::Origin request_origin = url::Origin::Create(page_url);
  if (current_origin.IsSameOriginWith(request_origin)) {
    return true;
  }
  return false;
}

void PasswordAutofillManagerExt::FillAccountSuggestion(
    const GURL& page_url,
    const std::u16string& username,
    const std::u16string& password) {
  if (!CanFillAccountSuggestion(page_url)) {
    LOG(INFO) << "Can not fill account suggestion cross origin.";
    return;
  }

  password_manager_driver_->AsPasswordManagerDriverExt()->FillAccountSuggestion(username, password);
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
PasswordFormFillData CreatePasswordFormFillDataWithoutPasswordInfo(
    const PasswordForm& form_on_page) {
  PasswordFormFillData result;

  result.form_renderer_id = form_on_page.form_data.unique_renderer_id;
  result.url = form_on_page.url;

  if (!form_on_page.only_for_fallback &&
      (form_on_page.HasPasswordElement() || form_on_page.IsSingleUsername())) {
    // Fill fields identifying information only for non-fallback case when
    // password element is found. In other cases a fill popup is shown on
    // clicking on each password field so no need in any field identifiers.
    result.username_element_renderer_id =
        form_on_page.username_element_renderer_id;
    result.username_may_use_prefilled_placeholder =
        form_on_page.username_may_use_prefilled_placeholder;

    result.password_element_renderer_id =
        form_on_page.password_element_renderer_id;
  }

  return result;
}
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
}  // namespace password_manager

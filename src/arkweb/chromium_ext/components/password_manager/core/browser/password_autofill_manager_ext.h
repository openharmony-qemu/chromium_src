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

#ifndef COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_AUTOFILL_MANAGER_EXT_H_
#define COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_AUTOFILL_MANAGER_EXT_H_

#include "components/password_manager/core/browser/password_autofill_manager.h"

namespace favicon_base {
struct FaviconImageResult;
}

namespace gfx {
class RectF;
}

namespace autofill {
struct PasswordFormFillData;
}

namespace password_manager {

class PasswordManagerClient;
class PasswordManagerDriver;
class PasswordManualFallbackMetricsRecorder;
class PasswordSuggestionGenerator;

// This class is responsible for filling password forms.
class PasswordAutofillManagerExt : public PasswordAutofillManager {
 public:
  PasswordAutofillManagerExt(PasswordManagerDriver* password_manager_driver,
                             autofill::AutofillClient* autofill_client,
                             PasswordManagerClient* password_client);

  PasswordAutofillManagerExt(const PasswordAutofillManagerExt&) = delete;
  PasswordAutofillManagerExt& operator=(const PasswordAutofillManagerExt&) =
      delete;
  ~PasswordAutofillManagerExt() override;

  PasswordAutofillManagerExt* AsWebPasswordAutofillManagerExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  bool CanFillAccountSuggestion(const GURL& page_url);

  void FillAccountSuggestion(const GURL& page_url,
                             const std::u16string& username,
                             const std::u16string& password);

#endif
};

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  autofill::PasswordFormFillData CreatePasswordFormFillDataWithoutPasswordInfo(
      const PasswordForm& form_on_page);
#endif
}  // namespace password_manager
#endif  // COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_AUTOFILL_MANAGER_EXT_H_

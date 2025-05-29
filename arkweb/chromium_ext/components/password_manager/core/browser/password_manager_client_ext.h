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

#ifndef ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_PASSWORD_MANAGER_CLIENT_EXT_H_
#define ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_PASSWORD_MANAGER_CLIENT_EXT_H_

#include "components/password_manager/core/browser/password_manager_client.h"
#include "arkweb/build/features/features.h"

namespace password_manager {
class PasswordManagerClientExt : public PasswordManagerClient {
 public:
  PasswordManagerClientExt() = default;

  PasswordManagerClientExt(const PasswordManagerClientExt&) = delete;
  PasswordManagerClientExt& operator=(const PasswordManagerClientExt&) = delete;

  ~PasswordManagerClientExt() override = default;

  PasswordManagerClientExt* AsPasswordManagerClientExt() override { return this; }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  virtual void FillAccountSuggestion(const GURL& page_url,
                                     const std::u16string& username,
                                     const std::u16string& password) {}

  virtual void OnRequestAutofill(
      PasswordManagerDriver* driver,
      const GURL& page_url,
      autofill::FormRendererId form_id,
      const autofill::mojom::OhosPasswordFormAutofillState state,
      const autofill::InputFillRequestData& username_data,
      const autofill::InputFillRequestData& password_data) {}
#endif
};
}  // namespace password_manager

#endif  // ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_PASSWORD_MANAGER_CLIENT_EXT_H_

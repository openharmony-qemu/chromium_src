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

#ifndef ARKWEB_CHROMIUM_EXT_COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_EXT_H_
#define ARKWEB_CHROMIUM_EXT_COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_EXT_H_

#include "components/password_manager/core/browser/password_manager_driver.h"

namespace password_manager {
class PasswordManagerDriverExt : public PasswordManagerDriver {
 public:
  PasswordManagerDriverExt() = default;

  PasswordManagerDriverExt(const PasswordManagerDriverExt&) = delete;
  PasswordManagerDriverExt& operator=(const PasswordManagerDriverExt&) = delete;

  ~PasswordManagerDriverExt() override = default;
  PasswordManagerDriverExt* AsPasswordManagerDriverExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  virtual void FillAccountSuggestion(const std::u16string& username,
                                     const std::u16string& password) {}

  virtual void SendParsedPasswordFormToRenderer(
      const autofill::PasswordFormFillData&
          parsed_form_data_without_password) {}

  virtual void AutofillSurfaceClosed(bool show_virtual_keyboard) {}
#endif
};
}  // namespace password_manager

#endif  // ARKWEB_CHROMIUM_EXT_COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_EXT_H_

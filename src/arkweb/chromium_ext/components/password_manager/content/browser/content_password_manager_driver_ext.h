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

#ifndef COMPONENTS_PASSWORD_MANAGER_CONTENT_BROWSER_CONTENT_PASSWORD_MANAGER_DRIVER_EXT_H_
#define COMPONENTS_PASSWORD_MANAGER_CONTENT_BROWSER_CONTENT_PASSWORD_MANAGER_DRIVER_EXT_H_

#include "arkweb/build/features/features.h"
#include "components/password_manager/content/browser/content_password_manager_driver.h"

namespace password_manager {
class ContentPasswordManagerDriver;

class ContentPasswordManagerDriverExt final
    : public ContentPasswordManagerDriver {
 public:
  ContentPasswordManagerDriverExt(content::RenderFrameHost* render_frame_host,
                                  PasswordManagerClient* client);

  ContentPasswordManagerDriverExt(const ContentPasswordManagerDriverExt&) =
      delete;
  ContentPasswordManagerDriverExt& operator=(
      const ContentPasswordManagerDriverExt&) = delete;

  ~ContentPasswordManagerDriverExt() override;
  ContentPasswordManagerDriverExt* AsContentPasswordManagerDriverExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void FillAccountSuggestion(const GURL& page_url,
                             const std::u16string& username,
                             const std::u16string& password);

  void FillAccountSuggestion(const std::u16string& username,
                             const std::u16string& password) override;

  void OnRequestAutofill(
      autofill::FormRendererId form_id,
      const autofill::mojom::OhosPasswordFormAutofillState state,
      const autofill::InputFillRequestData& username_data,
      const autofill::InputFillRequestData& password_data) override;

  void SendParsedPasswordFormToRenderer(
      const autofill::PasswordFormFillData& parsed_form_data_without_password)
      override;

  void AutofillSurfaceClosed(bool show_virtual_keyboard) override;
#endif
};
}  // namespace password_manager

#endif

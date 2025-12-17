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

namespace password_manager {

TEST_F(ContentPasswordManagerDriverTest, OnRequestAutofill) {
  std::unique_ptr<ContentPasswordManagerDriver> driver(
      new ContentPasswordManagerDriverExt(main_rfh(), &password_manager_client_));
  
  autofill::FormRendererId id;
  autofill::InputFillRequestData username_data;
  autofill::InputFillRequestData password_data;
  ASSERT_NO_FATAL_FAILURE(driver->OnRequestAutofill(id, autofill::mojom::OhosPasswordFormAutofillState::kNotRequested,
    username_data, password_data));
}

TEST_F(ContentPasswordManagerDriverTest, AutofillSurfaceClosed) {
  std::unique_ptr<ContentPasswordManagerDriver> driver(
      new ContentPasswordManagerDriverExt(main_rfh(), &password_manager_client_));
  ASSERT_TRUE(driver);
  ASSERT_NO_FATAL_FAILURE(driver->AutofillSurfaceClosed(true));
}

TEST_F(ContentPasswordManagerDriverTest, SendParsedPasswordFormToRenderer) {
  std::unique_ptr<ContentPasswordManagerDriver> driver(
      new ContentPasswordManagerDriverExt(main_rfh(), &password_manager_client_));
  ASSERT_TRUE(driver);
  PasswordFormFillData fill_data = GetTestPasswordFormFillData();
  ASSERT_NO_FATAL_FAILURE(driver->SendParsedPasswordFormToRenderer(fill_data));
}

}
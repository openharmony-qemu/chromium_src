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
 
#include <memory>

#include "arkweb/chromium_ext/components/password_manager/core/browser/password_autofill_manager_ext.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

constexpr int ID_VAL = 4;

class PasswordAutofillManagerExtTest : public testing::Test {
 protected:
  PasswordAutofillManagerExtTest()
      : test_username_(kAliceUsername), test_password_(kAlicePassword) {}

  void SetUp() override {
    // Add a preferred login.
    fill_data_.preferred_login.username_value = test_username_;
    fill_data_.preferred_login.password_value = test_password_;
    SetUseAPasskeyOnAnotherDeviceFeatureOnDesktop(/*enable=*/true);
  }

  void InitializePasswordAutofillManager(TestPasswordManagerClient* client,
                                         MockAutofillClient* autofill_client) {
    password_autofill_manager_ = std::make_unique<PasswordAutofillManagerExt>(
        client->mock_driver(), autofill_client, client);
    password_autofill_manager_->SetManualFallbackFlowForTest(
        std::make_unique<NiceMock<MockPasswordSuggestionFlow>>());
    favicon::MockFaviconService favicon_service;
    EXPECT_CALL(*client, GetFaviconService())
        .WillOnce(Return(&favicon_service));
    EXPECT_CALL(favicon_service,
                GetFaviconImageForPageURL(fill_data_.url, _, _))
        .WillOnce(Invoke(RespondWithTestIcon));
    password_autofill_manager_->OnAddPasswordFillData(fill_data_);
    testing::Mock::VerifyAndClearExpectations(client);
    // Suppress the warnings in the tests.
    EXPECT_CALL(*client, GetFaviconService()).WillRepeatedly(Return(nullptr));

    webauthn_credentials_delegate_ =
        std::make_unique<MockWebAuthnCredentialsDelegate>();

    ON_CALL(*client, GetWebAuthnCredentialsDelegateForDriver)
        .WillByDefault(Return(webauthn_credentials_delegate_.get()));
    ON_CALL(*webauthn_credentials_delegate_, HasPendingPasskeySelection)
        .WillByDefault(Return(false));

    EXPECT_CALL(*client->mock_driver(), CanShowAutofillUi)
        .WillRepeatedly(Return(true));
  }

  MockPasswordSuggestionFlow& manual_fallback_flow() {
    return *static_cast<MockPasswordSuggestionFlow*>(
        password_autofill_manager_->manual_fallback_flow());
  }

 protected:
  void SetUseAPasskeyOnAnotherDeviceFeatureOnDesktop(bool enable) {
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
    features_.Reset();
    if (enable) {
      features_.InitWithFeatures(
          {features::kPasswordManualFallbackAvailable,
           features::kWebAuthnUsePasskeyFromAnotherDeviceInContextMenu},
          {});
    } else {
      features_.InitWithFeatures(
          {features::kPasswordManualFallbackAvailable},
          {features::kWebAuthnUsePasskeyFromAnotherDeviceInContextMenu});
    }
#endif
  }

  std::unique_ptr<PasswordAutofillManagerExt> password_autofill_manager_;

  std::unique_ptr<MockWebAuthnCredentialsDelegate>
      webauthn_credentials_delegate_;

  std::u16string test_username_;
  std::u16string test_password_;

 private:
  autofill::PasswordFormFillData fill_data_;

  base::test::ScopedFeatureList features_;

  // The TestAutofillDriver uses a SequencedWorkerPool which expects the
  // existence of a MessageLoop.
  base::test::SingleThreadTaskEnvironment task_environment_;
};

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
TEST_F(PasswordAutofillManagerExtTest, CanFillAccountSuggestion001) {
  TestPasswordManagerClient client;
  InitializePasswordAutofillManager(&client, nullptr);
  std::string_view url_string("http://hostname.com/");
  GURL url(url_string);
  EXPECT_CALL(*client.mock_driver(),
              FillSuggestion(test_username_, test_password_, _));
  EXPECT_TRUE(
      password_autofill_manager_->FillSuggestionForTest(test_username_));
  testing::Mock::VerifyAndClearExpectations(client.mock_driver());

  password_autofill_manager_->FillAccountSuggestion(url, test_username_, test_password_);
  EXPECT_FALSE(password_autofill_manager_->CanFillAccountSuggestion(url));
}

TEST_F(PasswordAutofillManagerExtTest, CreatePasswordFormFillDataWithoutPasswordInfo001) {
  TestPasswordManagerClient client;
  InitializePasswordAutofillManager(&client, nullptr);
  PasswordForm form;
  form.only_for_fallback = true;
  autofill::PasswordFormFillData data = CreatePasswordFormFillDataWithoutPasswordInfo(form);
  EXPECT_EQ(data.url, form.url);
  form.only_for_fallback = false;
  form.password_element_renderer_id = autofill::FieldRendererId(ID_VAL);
  data = CreatePasswordFormFillDataWithoutPasswordInfo(form);
  EXPECT_EQ(data.url, form.url);
}

TEST_F(PasswordAutofillManagerExtTest, CreatePasswordFormFillDataWithoutPasswordInfo002) {
  TestPasswordManagerClient client;
  InitializePasswordAutofillManager(&client, nullptr);

  PasswordForm form;
  form.only_for_fallback = true;
  autofill::PasswordFormFillData data = CreatePasswordFormFillDataWithoutPasswordInfo(form);
  EXPECT_EQ(data.url, form.url);
  form.only_for_fallback = false;
  data = CreatePasswordFormFillDataWithoutPasswordInfo(form);
  EXPECT_EQ(data.url, form.url);
}

#endif
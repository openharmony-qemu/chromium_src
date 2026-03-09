/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/chrome/browser/password_manager/chrome_password_manager_client_ext.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "base/json/json_reader.h"
#include "build/build_config.h"
#include "arkweb/chromium_ext/components/autofill/core/common/arkweb_password_autofill_data.h"
#include "chrome/test/base/chrome_render_view_host_test_harness.h"
#include "components/password_manager/core/browser/password_form.h"
#include "crypto/sha2.h"
#include "testing/gtest/include/gtest/gtest.h"

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

namespace {

constexpr char kHashSalt[] = "OHOS@PASSWORD@AUTOFILL";

const base::Value* FindValueByKey(const base::Value::List& list,
                                  std::string_view key) {
  for (const auto& item : list) {
    if (!item.is_dict()) {
      continue;
    }
    const base::Value* value = item.GetDict().Find(key);
    if (value) {
      return value;
    }
  }
  return nullptr;
}

autofill::InputFillRequestData BuildFillData(
    autofill::FieldRendererId field_renderer_id,
    bool is_focused,
    autofill::mojom::OhosInputElementType type) {
  autofill::InputFillRequestData data;
  data.field_renderer_id = field_renderer_id;
  data.is_focused = is_focused;
  data.type = type;
  data.value = u"filled";
  data.placeholder = u"placeholder";
  return data;
}

class TestableChromePasswordManagerClientExt : public ChromePasswordManagerClientExt {
 public:
  explicit TestableChromePasswordManagerClientExt(content::WebContents* web_contents)
      : ChromePasswordManagerClientExt(web_contents) {}
  ~TestableChromePasswordManagerClientExt() override = default;

  void FillAccountSuggestion(const GURL& page_url,
                             const std::u16string& username,
                             const std::u16string& password) override {
    ++fill_account_suggestion_call_count;
    filled_page_url = page_url;
    filled_username = username;
    filled_password = password;
  }

  int fill_account_suggestion_call_count = 0;
  GURL filled_page_url;
  std::u16string filled_username;
  std::u16string filled_password;
};

class ChromePasswordManagerClientExtTest : public ChromeRenderViewHostTestHarness {
 public:
  void SetUp() override {
    ChromeRenderViewHostTestHarness::SetUp();
    client_ =
        std::make_unique<TestableChromePasswordManagerClientExt>(web_contents());
  }

  void TearDown() override {
    client_.reset();
    ChromeRenderViewHostTestHarness::TearDown();
  }

 protected:
  std::unique_ptr<TestableChromePasswordManagerClientExt> client_;
};

TEST_F(ChromePasswordManagerClientExtTest, ConstructorInitializesDefaultInputTypes) {
  EXPECT_EQ(client_->last_request_fill_username_.type,
            autofill::mojom::OhosInputElementType::kUsernameType);
  EXPECT_FALSE(client_->last_request_fill_username_.is_focused);
  EXPECT_EQ(client_->last_request_fill_password_.type,
            autofill::mojom::OhosInputElementType::kPasswordType);
  EXPECT_FALSE(client_->last_request_fill_password_.is_focused);
}

TEST_F(ChromePasswordManagerClientExtTest, UpdateLastRequestFilledItemsUpdatesBothFields) {
  const auto username = BuildFillData(
      autofill::FieldRendererId(11), true,
      autofill::mojom::OhosInputElementType::kUsernameType);
  const auto password = BuildFillData(
      autofill::FieldRendererId(22), false,
      autofill::mojom::OhosInputElementType::kPasswordType);

  client_->UpdateLastRequestFilledItems(username, password);

  EXPECT_EQ(client_->last_request_fill_username_.field_renderer_id,
            username.field_renderer_id);
  EXPECT_EQ(client_->last_request_fill_password_.field_renderer_id,
            password.field_renderer_id);
}

TEST_F(ChromePasswordManagerClientExtTest, FillDataStoresDigestAndTriggersSuggestion) {
  const auto username_id = autofill::FieldRendererId(101);
  const auto password_id = autofill::FieldRendererId(202);
  client_->last_request_fill_username_.field_renderer_id = username_id;
  client_->last_request_fill_password_.field_renderer_id = password_id;

  const std::string username = "user_a";
  const std::string password = "pwd_a";
  const std::string expected_digest =
      crypto::SHA256HashString(username + std::string(kHashSalt) + password);

  client_->FillData("https://example.com/login", username, password, false);

  EXPECT_EQ(client_->auto_filled_forms_username_[*username_id], expected_digest);
  EXPECT_EQ(client_->auto_filled_forms_password_[*password_id], expected_digest);
  EXPECT_EQ(client_->fill_account_suggestion_call_count, 1);
  EXPECT_EQ(client_->filled_page_url.spec(), "https://example.com/login");
  EXPECT_EQ(client_->filled_username, u"user_a");
  EXPECT_EQ(client_->filled_password, u"pwd_a");
}

TEST_F(ChromePasswordManagerClientExtTest, FillDataOtherAccountStoresEmptyDigest) {
  const auto username_id = autofill::FieldRendererId(303);
  const auto password_id = autofill::FieldRendererId(404);
  client_->last_request_fill_username_.field_renderer_id = username_id;
  client_->last_request_fill_password_.field_renderer_id = password_id;

  client_->FillData("https://example.com/login", "user_b", "pwd_b", true);

  EXPECT_TRUE(client_->auto_filled_forms_username_[*username_id].empty());
  EXPECT_TRUE(client_->auto_filled_forms_password_[*password_id].empty());
}

TEST_F(ChromePasswordManagerClientExtTest,
       FillDataWithoutRendererIdsDoesNotStoreDigestButTriggersSuggestion) {
  client_->last_request_fill_username_.field_renderer_id =
      autofill::FieldRendererId();
  client_->last_request_fill_password_.field_renderer_id =
      autofill::FieldRendererId();

  client_->FillData("https://example.com/login", "user_no_id", "pwd_no_id",
                    false);

  EXPECT_TRUE(client_->auto_filled_forms_username_.empty());
  EXPECT_TRUE(client_->auto_filled_forms_password_.empty());
  EXPECT_EQ(client_->fill_account_suggestion_call_count, 1);
  EXPECT_EQ(client_->filled_page_url.spec(), "https://example.com/login");
  EXPECT_EQ(client_->filled_username, u"user_no_id");
  EXPECT_EQ(client_->filled_password, u"pwd_no_id");
}

TEST_F(ChromePasswordManagerClientExtTest,
       IsLoginInfoConsistentWithFilledMatchesPasswordRendererIdAndErasesEntry) {
  const auto password_id = autofill::FieldRendererId(505);
  password_manager::PasswordForm form;
  form.username_value = u"user_c";
  form.password_value = u"pwd_c";
  form.password_element_renderer_id = password_id;
  const std::string digest =
      crypto::SHA256HashString("user_c" + std::string(kHashSalt) + "pwd_c");
  client_->auto_filled_forms_password_[*password_id] = digest;

  EXPECT_TRUE(client_->IsLoginInfoConsistentWithFilled(form));
  EXPECT_EQ(client_->auto_filled_forms_password_.count(*password_id), 0u);
}

TEST_F(ChromePasswordManagerClientExtTest,
       IsLoginInfoConsistentWithFilledFallsBackToUsernameRendererId) {
  const auto username_id = autofill::FieldRendererId(606);
  password_manager::PasswordForm form;
  form.username_value = u"user_d";
  form.password_value = u"pwd_d";
  form.username_element_renderer_id = username_id;
  const std::string digest =
      crypto::SHA256HashString("user_d" + std::string(kHashSalt) + "pwd_d");
  client_->auto_filled_forms_username_[*username_id] = digest;

  EXPECT_TRUE(client_->IsLoginInfoConsistentWithFilled(form));
  EXPECT_EQ(client_->auto_filled_forms_username_.count(*username_id), 0u);
}

TEST_F(ChromePasswordManagerClientExtTest, IsLoginInfoConsistentWithFilledReturnsFalseWhenDigestMismatch) {
  const auto password_id = autofill::FieldRendererId(707);
  password_manager::PasswordForm form;
  form.username_value = u"user_e";
  form.password_value = u"pwd_e";
  form.password_element_renderer_id = password_id;
  client_->auto_filled_forms_password_[*password_id] = "mismatch";

  EXPECT_FALSE(client_->IsLoginInfoConsistentWithFilled(form));
  EXPECT_EQ(client_->auto_filled_forms_password_.count(*password_id), 1u);
}

TEST_F(ChromePasswordManagerClientExtTest, IsLoginInfoConsistentWithFilledReturnsFalseWhenDigestEmpty) {
  const auto password_id = autofill::FieldRendererId(717);
  password_manager::PasswordForm form;
  form.username_value = u"user_empty";
  form.password_value = u"pwd_empty";
  form.password_element_renderer_id = password_id;
  client_->auto_filled_forms_password_[*password_id] = "";

  EXPECT_FALSE(client_->IsLoginInfoConsistentWithFilled(form));
  EXPECT_EQ(client_->auto_filled_forms_password_.count(*password_id), 1u);
}

TEST_F(ChromePasswordManagerClientExtTest,
       IsLoginInfoConsistentWithFilledReturnsFalseWhenRendererIdMissing) {
  password_manager::PasswordForm form;
  form.username_value = u"user_none";
  form.password_value = u"pwd_none";

  EXPECT_FALSE(client_->IsLoginInfoConsistentWithFilled(form));
}

TEST_F(ChromePasswordManagerClientExtTest, PasswordFormToJsonForSaveContainsExpectedData) {
  password_manager::PasswordForm form;
  form.url = GURL("https://example.com/path?query=1");
  form.username_value = u"user_json";
  form.password_value = u"pwd_json";
  form.all_alternative_usernames.emplace_back(
      password_manager::AlternativeElement::Value(u"alt_a"),
      autofill::FieldRendererId(),
      password_manager::AlternativeElement::Name(u"id_a"));
  form.all_alternative_usernames.emplace_back(
      password_manager::AlternativeElement::Value(u"alt_b"),
      autofill::FieldRendererId(),
      password_manager::AlternativeElement::Name(u"id_b"));

  std::optional<std::string> json = client_->PasswordFormToJsonForSave(form);
  ASSERT_TRUE(json.has_value());

  std::optional<base::Value> parsed = base::JSONReader::Read(*json);
  ASSERT_TRUE(parsed.has_value());
  ASSERT_TRUE(parsed->is_list());
  const base::Value::List& root = parsed->GetList();

  const base::Value* event = FindValueByKey(root, "event");
  ASSERT_NE(event, nullptr);
  EXPECT_EQ(event->GetString(), "save");

  const base::Value* source = FindValueByKey(root, "source");
  ASSERT_NE(source, nullptr);
  EXPECT_EQ(source->GetString(), "login");

  const base::Value* page_url = FindValueByKey(root, "pageUrl");
  ASSERT_NE(page_url, nullptr);
  EXPECT_EQ(page_url->GetString(), "https://example.com/");

  const base::Value* username = FindValueByKey(root, "username");
  ASSERT_NE(username, nullptr);
  ASSERT_TRUE(username->is_list());
  const base::Value::List& username_list = username->GetList();
  ASSERT_EQ(username_list.size(), 2u);
  const std::string* username_value =
      username_list[0].GetDict().FindString("value");
  ASSERT_NE(username_value, nullptr);
  EXPECT_EQ(*username_value, "user_json");
  const base::Value* selectable =
      username_list[1].GetDict().Find("selectableUsernames");
  ASSERT_NE(selectable, nullptr);
  ASSERT_TRUE(selectable->is_list());
  EXPECT_EQ(selectable->GetList().size(), 2u);
  EXPECT_EQ(selectable->GetList()[0].GetString(), "alt_a");
  EXPECT_EQ(selectable->GetList()[1].GetString(), "alt_b");

  const base::Value* password = FindValueByKey(root, "password");
  ASSERT_NE(password, nullptr);
  ASSERT_TRUE(password->is_list());
  ASSERT_EQ(password->GetList().size(), 1u);
  const std::string* password_value =
      password->GetList()[0].GetDict().FindString("value");
  ASSERT_NE(password_value, nullptr);
  EXPECT_EQ(*password_value, "pwd_json");
}

TEST_F(ChromePasswordManagerClientExtTest,
       PasswordFormToJsonForRequestWithoutCefBrowserReturnsNullopt) {
  const auto username = BuildFillData(
      autofill::FieldRendererId(808), true,
      autofill::mojom::OhosInputElementType::kUsernameType);
  const auto password = BuildFillData(
      autofill::FieldRendererId(909), false,
      autofill::mojom::OhosInputElementType::kPasswordType);

  EXPECT_FALSE(client_
                   ->PasswordFormToJsonForRequest(
                       "fill", GURL("https://example.com"), username, password)
                   .has_value());
}

TEST_F(ChromePasswordManagerClientExtTest, ProcessAutofillCancelResetsNeedRestoreFlag) {
  client_->is_need_restore_keyboard_ = true;
  client_->ProcessAutofillCancel("filled_content");
  EXPECT_FALSE(client_->is_need_restore_keyboard_);
}

TEST_F(ChromePasswordManagerClientExtTest, ProcessAutofillCancelSkipsWhenNoNeedRestore) {
  client_->is_need_restore_keyboard_ = false;
  client_->ProcessAutofillCancel("filled_content");
  EXPECT_FALSE(client_->is_need_restore_keyboard_);
}

TEST_F(ChromePasswordManagerClientExtTest, AutoFillWithIMFEventWithoutClientKeepsRestoreFlagFalse) {
  client_->is_need_restore_keyboard_ = false;
  client_->AutoFillWithIMFEvent(true, false, false, "from_imf");
  EXPECT_FALSE(client_->is_need_restore_keyboard_);
}

TEST_F(ChromePasswordManagerClientExtTest, SuppressKeyboardWithoutFocusedFrameKeepsRestoreFlagFalse) {
  client_->is_need_restore_keyboard_ = false;
  client_->SuppressKeyboard();
  EXPECT_FALSE(client_->is_need_restore_keyboard_);
}

TEST_F(ChromePasswordManagerClientExtTest, ArkPromptUserToSaveOrUpdatePasswordRejectsNullForm) {
  EXPECT_FALSE(client_->ArkPromptUserToSaveOrUpdatePassword(nullptr));
}

TEST_F(ChromePasswordManagerClientExtTest, OnRequestAutofillReturnsEarlyWhenClientMissing) {
  client_->form_to_request_url_ = GURL("https://before.test/");
  client_->last_fill_form_id_ = autofill::FormRendererId(11);
  client_->last_fill_focus_renderer_id_ = autofill::FieldRendererId(22);

  const auto username = BuildFillData(
      autofill::FieldRendererId(1), true,
      autofill::mojom::OhosInputElementType::kUsernameType);
  const auto password = BuildFillData(
      autofill::FieldRendererId(2), false,
      autofill::mojom::OhosInputElementType::kPasswordType);

  client_->OnRequestAutofill(
      nullptr, GURL("https://after.test/"), autofill::FormRendererId(99),
      autofill::mojom::OhosPasswordFormAutofillState::kNotRequested, username,
      password);

  EXPECT_EQ(client_->form_to_request_url_.spec(), "https://before.test/");
  EXPECT_EQ(client_->last_fill_form_id_, autofill::FormRendererId(11));
  EXPECT_EQ(client_->last_fill_focus_renderer_id_, autofill::FieldRendererId(22));
}

TEST_F(ChromePasswordManagerClientExtTest, AutofillIdentityCheckUsesLastFocusedIds) {
  client_->last_fill_form_id_ = autofill::FormRendererId(333);
  client_->last_fill_focus_renderer_id_ = autofill::FieldRendererId(444);

  EXPECT_TRUE(client_->IsUsernamePasswordForm(autofill::FormRendererId(333)));
  EXPECT_FALSE(client_->IsUsernamePasswordForm(autofill::FormRendererId(334)));
  EXPECT_TRUE(client_->IsUsernamePasswordField(autofill::FieldRendererId(444)));
  EXPECT_FALSE(client_->IsUsernamePasswordField(autofill::FieldRendererId(445)));
}

}  // namespace

#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

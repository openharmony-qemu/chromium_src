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

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST_P(LoginDatabaseTest, UpdateLoginDisplayName) {
  PasswordForm form;
  
  password_manager::PasswordStoreChangeList changes = db().AddLogin(form);
  EXPECT_FALSE(db().UpdateLoginDisplayName(form, nullptr));

  changes = db().AddLogin(form);
  form.url = GURL("http://0.com");
  form.signon_realm = "http://www.example.com/";
  form.action = GURL("http://www.example.com/action");
  form.password_element = u"pwd";
  form.password_value = u"example";
  EXPECT_FALSE(db().UpdateLoginDisplayName(form, &changes));
}
#endif
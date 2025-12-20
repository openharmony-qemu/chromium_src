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
bool LoginDatabase::UpdateLoginDisplayName(const PasswordForm& form,
                                           PasswordStoreChangeList* changes) {
  TRACE_EVENT0("passwords", "LoginDatabase::UpdateLoginDisplayName");
  if (changes) {
    changes->clear();
  }
  const PrimaryKeyAndPassword old_primary_key_password =
      GetPrimaryKeyAndPassword(form);
  DCHECK(!update_display_name_statement_.empty());
  sql::Statement s(
      db_.GetCachedStatement(SQL_FROM_HERE, update_display_name_statement_));
  int next_param = 0;
  s.BindString16(next_param++, form.display_name);

  // WHERE starts here.
  s.BindString(next_param++, form.url.spec());
  s.BindString16(next_param++, form.username_element);
  s.BindString16(next_param++, form.username_value);
  s.BindString16(next_param++, form.password_element);
  s.BindString(next_param++, form.signon_realm);
  // NOTE: Add new fields here only if the field is a part of the unique key.
  // Otherwise, add the field above "WHERE starts here" comment.

  if (!s.Run() || db_.GetLastChangeCount() == 0) {
    VLOG(0) << "Update login DisplayName failed.";
    return false;
  }

  if (changes) {
    changes->emplace_back(PasswordStoreChange::UPDATE, std::move(form),
                          /*password_changed=*/true);
  }

  return true;
}

void LoginDatabaseSetMigratePasswordsFlagToFile() {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&LoginDatabaseSetMigratePasswordsFlagToFile));
    return;
  }
 
  if (g_browser_process && g_browser_process->local_state()) {
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    g_browser_process->local_state()->CommitPendingWrite();
  }
}
#endif

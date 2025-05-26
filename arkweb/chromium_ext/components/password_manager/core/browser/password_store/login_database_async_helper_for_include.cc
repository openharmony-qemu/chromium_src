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

namespace password_manager {

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
void LoginDatabaseAsyncHelper::UpdateLoginDisplayName(
    const PasswordForm& form) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  BeginTransaction();
  PasswordStoreChangeList changes;
  if (login_db_ && login_db_->UpdateLoginDisplayName(form, &changes)) {
    if (password_sync_bridge_ && !changes.empty()) {
      password_sync_bridge_->ActOnPasswordStoreChanges(FROM_HERE, changes);
    }
  }
  // Sync metadata get updated in ActOnPasswordStoreChanges(). Therefore,
  // CommitTransaction() must be called after ActOnPasswordStoreChanges(),
  // because sync codebase needs to update metadata atomically together with
  // the login data.
  CommitTransaction();
}
#endif

}  // namespace password_manager

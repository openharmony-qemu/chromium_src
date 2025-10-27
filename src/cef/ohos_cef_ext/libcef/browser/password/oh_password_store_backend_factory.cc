/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 */

#include "components/password_manager/core/browser/password_store/password_store_backend.h"

#include "components/password_manager/core/browser/password_store/login_database.h"
#include "components/password_manager/core/browser/password_store/password_store_built_in_backend.h"
#include "components/password_manager/core/browser/password_store_factory_util.h"
#include "components/prefs/pref_service.h"

namespace password_manager {

std::unique_ptr<PasswordStoreBackend> PasswordStoreBackend::Create(
    const base::FilePath& login_db_path,
    PrefService* prefs) {
  return std::make_unique<PasswordStoreBuiltInBackend>(
      CreateLoginDatabaseForProfileStorage(login_db_path, prefs),
      syncer::WipeModelUponSyncDisabledBehavior::kNever, prefs, nullptr);
}

}  // namespace password_manager

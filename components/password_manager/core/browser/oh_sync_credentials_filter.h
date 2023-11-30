/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Created by lin on 19-1-16.
 */

#ifndef COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_OH_SYNC_CREDENTIALS_FILTER_H_
#define COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_OH_SYNC_CREDENTIALS_FILTER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "components/password_manager/core/browser/credentials_filter.h"
#include "components/password_manager/core/browser/password_form.h"
#include "components/password_manager/core/browser/password_manager_client.h"
#include "components/sync/driver/sync_service.h"

namespace password_manager {
struct PasswordForm;
class PasswordFormManager;

class OhSyncCredentialsFilter : public CredentialsFilter {
 public:
  using SyncServiceFactoryFunction =
      base::RepeatingCallback<const syncer::SyncService*(void)>;

  OhSyncCredentialsFilter(PasswordManagerClient* client);
  ~OhSyncCredentialsFilter() override;

  // CredentialsFilter
  bool ShouldSave(const PasswordForm& form) const override;
  bool ShouldSaveGaiaPasswordHash(const PasswordForm& form) const override;
  bool ShouldSaveEnterprisePasswordHash(
      const PasswordForm& form) const override;
  void ReportFormLoginSuccess(
      const PasswordFormManager& form_manager) const override;
  bool IsSyncAccountEmail(const std::string& username) const override;

 private:
  const raw_ptr<PasswordManagerClient> client_;

  const SyncServiceFactoryFunction sync_service_factory_function_;
};

}  // namespace password_manager
#endif  // COMPONENTS_PASSWORD_MANAGER_SYNC_BROWSER_AW_SYNC_CREDENTIALS_FILTER_H_

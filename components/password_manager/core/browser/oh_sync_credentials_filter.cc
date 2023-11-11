/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Created by lin on 19-1-16.
 */

#include "components/password_manager/core/browser/oh_sync_credentials_filter.h"

#include <algorithm>

#include "base/metrics/field_trial.h"
#include "base/metrics/user_metrics.h"
#include "components/password_manager/core/browser/password_form_manager.h"
#include "components/password_manager/core/browser/password_form_manager_for_ui.h"
#include "components/password_manager/core/browser/password_manager_util.h"
#include "components/password_manager/core/browser/password_sync_util.h"
#include "components/password_manager/core/common/password_manager_features.h"
#include "net/base/url_util.h"

namespace password_manager {

OhSyncCredentialsFilter::OhSyncCredentialsFilter(PasswordManagerClient* client)
    : client_(client) {}

OhSyncCredentialsFilter::~OhSyncCredentialsFilter() {}

bool OhSyncCredentialsFilter::ShouldSave(const PasswordForm& form) const {
  return !client_->IsIncognito();
}

bool OhSyncCredentialsFilter::ShouldSaveGaiaPasswordHash(
    const PasswordForm& form) const {
  return false;
}

bool OhSyncCredentialsFilter::ShouldSaveEnterprisePasswordHash(
    const PasswordForm& form) const {
  return !client_->IsIncognito() && sync_util::ShouldSaveEnterprisePasswordHash(
                                        form, *client_->GetPrefs());
}

bool OhSyncCredentialsFilter::IsSyncAccountEmail(
    const std::string& username) const {
  return sync_util::IsSyncAccountEmail(username, client_->GetIdentityManager());
}

void OhSyncCredentialsFilter::ReportFormLoginSuccess(
    const PasswordFormManager& form_manager) const {
  if (!form_manager.IsNewLogin()) {
    base::RecordAction(base::UserMetricsAction(
        "PasswordManager_SyncCredentialFilledAndLoginSuccessfull"));
  }
}

}  // namespace password_manager

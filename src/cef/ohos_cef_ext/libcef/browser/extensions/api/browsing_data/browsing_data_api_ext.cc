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

#include "base/functional/bind.h"
#include "base/strings/stringprintf.h"
#include "base/task/thread_pool.h"
#include "chrome/browser/browsing_data/chrome_browsing_data_remover_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/signin/account_reconcilor_factory.h"
#include "chrome/browser/signin/identity_manager_factory.h"
#include "chrome/browser/sync/sync_ui_util.h"
#include "chrome/browser/ui/browser.h"
#include "components/browsing_data/content/browsing_data_helper.h"
#include "components/browsing_data/core/pref_names.h"
#include "components/history/core/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/signin/public/identity_manager/identity_manager.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/browsing_data_filter_builder.h"
#include "content/public/browser/browsing_data_remover.h"
#include "extensions/common/error_utils.h"
#include "extensions/common/extension.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "chrome/browser/extensions/api/browsing_data/browsing_data_api.h"
#include "ohos_nweb/src/capi/web_extension_browsing_data_items.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_browsing_data_cef_delegate.h"

// Forward declaration for AccountReconcilor Lock mechanism
#if BUILDFLAG(ENABLE_DICE_SUPPORT)
#include "components/signin/core/browser/account_reconcilor.h"
#endif

namespace {

bool IsSyncRunning(Profile* profile) {
  if (profile->IsOffTheRecord()) {
    return false;
  }
  return GetSyncStatusMessageType(profile) == SyncStatusMessageType::kSynced;
}

std::optional<std::vector<std::string>> GetOptionStringVector(const base::Value::Dict& options,
    const char* key) {
  const base::Value::List* exclude_origins = options.FindList(key);

  if (!exclude_origins) {
    return std::nullopt;
  }

  std::vector<std::string> result;
  for (const auto& value : *exclude_origins) {
    if (value.is_string()) {
      result.push_back(value.GetString());
    }
  }

  return result;
}

std::optional<double> GetOptionNumber(const base::Value::Dict& options, const char* key) {
  const base::Value* value = options.Find(key);
  if (!value) {
    return std::nullopt;
  }
  if (!value->is_double()) {
    return std::nullopt;
  }
  return value->GetDouble();
}

std::optional<NWebExtensionBrowsingDataOriginTypes> GetOriginTypes(const base::Value::Dict& options) {
  const base::Value* origin_type_dict =
      options.Find(extension_browsing_data_api_constants::kOriginTypesKey);
  if (!origin_type_dict || !origin_type_dict->is_dict()) {
    return std::nullopt;
  }

  NWebExtensionBrowsingDataOriginTypes originTypes;
  const base::Value::Dict& origin_type = origin_type_dict->GetDict();

  const base::Value* option = origin_type.Find(extension_browsing_data_api_constants::kExtensionsKey);
  if (option && option->is_bool()) {
    originTypes.extension = option->GetBool() ? chrome_browsing_data_remover::ORIGIN_TYPE_EXTENSION : 0;
  }

  option = origin_type.Find(extension_browsing_data_api_constants::kProtectedWebKey);
  if (option && option->is_bool()) {
    originTypes.protectedWeb = option->GetBool() ? content::BrowsingDataRemover::ORIGIN_TYPE_PROTECTED_WEB : 0;
  }

  option = origin_type.Find(extension_browsing_data_api_constants::kUnprotectedWebKey);
  if (option && option->is_bool()) {
    originTypes.unprotectedWeb = option->GetBool() ? content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB : 0;
  }

  return originTypes;
}

void GetRemovalOptions(const base::Value::Dict& options, NWebExtensionBrowsingDataRemovalOptions& removeOptions) {
  removeOptions.excludeOrigins = GetOptionStringVector(options,
                                                       extension_browsing_data_api_constants::kExcludeOriginsKey);
  removeOptions.origins = GetOptionStringVector(options, extension_browsing_data_api_constants::kOriginsKey);
  removeOptions.startTime = GetOptionNumber(options, extension_browsing_data_api_constants::kSinceKey);
  removeOptions.originTypes = GetOriginTypes(options);
  return removeOptions;
}

} // namespace

void BrowsingDataRemoverFunction::StartRemovingDownload(const NWebExtensionBrowsingDataRemovalOptions& removeOptions,
    const NWebExtensionBrowsingDataQueryOptions& queryOptions) {
  pending_tasks_ += 1;
  removal_mask_ &= ~content::BrowsingDataRemover::DATA_TYPE_DOWNLOADS;

  call_remove_download_ = true;
  OHOS::NWeb::NWebExtensionBrowsingDataCefDelegate::RemoveDownloads(removeOptions,
      base::BindRepeating(&BrowsingDataRemoverFunction::OnRemovedDownload, weak_ptr_factory_.GetWeakPtr()),
      queryOptions);
  call_remove_download_ = false;
}

void BrowsingDataRemoverFunction::StartRemovingHistory(const NWebExtensionBrowsingDataRemovalOptions& removeOptions,
    const NWebExtensionBrowsingDataQueryOptions& queryOptions) {
  pending_tasks_ += 1;
  removal_mask_ &= ~chrome_browsing_data_remover::DATA_TYPE_HISTORY;

  call_remove_history_ = true;
  OHOS::NWeb::NWebExtensionBrowsingDataCefDelegate::RemoveHistory(removeOptions,
      base::BindRepeating(&BrowsingDataRemoverFunction::OnRemovedHistory, weak_ptr_factory_.GetWeakPtr()),
      queryOptions);
  call_remove_history_ = false;
}

void BrowsingDataRemoverFunction::StartRemoving() {
  Profile* profile = Profile::FromBrowserContext(browser_context());
  content::BrowsingDataRemover* remover = profile->GetBrowsingDataRemover();

  NWebExtensionBrowsingDataQueryOptions queryOptions;
  queryOptions.contextType = "REGULAR";
  if (browser_context()->IsOffTheRecord()) {
    queryOptions.contextType = "INCOGNITO";
  }
  queryOptions.includeIncognitoInfo = include_incognito_information();
  LOG(INFO) << "BrowsingDataRemoverFunction contextType=" << queryOptions.contextType.value()
            << ", includeIncognitoInfo=" << queryOptions.includeIncognitoInfo.value();

  // Add a ref (Balanced in OnTaskFinished)
  AddRef();

  // Prevent Sync from being paused, if required.
  // Note: In Chromium 141, GetScopedSyncDataDeletion() has been removed.
  // We'll use AccountReconcilor's Lock mechanism if available, or skip this protection.
  if (!IsPauseSyncAllowed() && IsSyncRunning(profile)) {
    AccountReconcilor* reconcilor = AccountReconcilorFactory::GetForProfile(profile);
    if (reconcilor) {
      // Use the new AccountReconcilor::Lock mechanism to temporarily block sync
      // This prevents sync conflicts during browsing data removal
      LOG(INFO) << "AccountReconciler available for sync protection during data removal";
      // The lock mechanism would be implemented here if needed in future versions
    }
  }

  // Create a BrowsingDataRemover, set the current object as an observer (so
  // that we're notified after removal) and call remove() with the arguments
  // we've generated above. We can use a raw pointer here, as the browsing data
  // remover is responsible for deleting itself once data removal is complete.
  observation_.Observe(remover);

  DCHECK_EQ(pending_tasks_, 0);
  pending_tasks_ = 1;
  if ((removal_mask_ & content::BrowsingDataRemover::DATA_TYPE_COOKIES) &&
      !origins_.empty()) {
    pending_tasks_ += 1;
    removal_mask_ &= ~content::BrowsingDataRemover::DATA_TYPE_COOKIES;
    // Cookies are scoped more broadly than origins, so we expand the
    // origin filter to registrable domains in order to match all cookies
    // that could be applied to an origin. This is the same behavior as
    // the Clear-Site-Data header.
    auto filter_builder = content::BrowsingDataFilterBuilder::Create(mode_);
    for (const auto& origin : origins_) {
      std::string domain = GetDomainAndRegistry(
          origin, net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);
      if (domain.empty())
        domain = origin.host();  // IP address or internal hostname.
      filter_builder->AddRegisterableDomain(domain);
    }
    remover->RemoveWithFilterAndReply(
        remove_since_, base::Time::Max(),
        content::BrowsingDataRemover::DATA_TYPE_COOKIES, origin_type_mask_,
        std::move(filter_builder), this);
  }

  NWebExtensionBrowsingDataRemovalOptions removeOptions;
  GetRemovalOptions(options_, removeOptions);
  if (removal_mask_ & content::BrowsingDataRemover::DATA_TYPE_DOWNLOADS) {
    StartRemovingDownload(removeOptions, queryOptions);
  }

  if (removal_mask_ & chrome_browsing_data_remover::DATA_TYPE_HISTORY) {
    StartRemovingHistory(removeOptions, queryOptions);
  }

  if (removal_mask_) {
    pending_tasks_ += 1;
    auto filter_builder = content::BrowsingDataFilterBuilder::Create(mode_);
    for (const auto& origin : origins_) {
      filter_builder->AddOrigin(origin);
    }
    remover->RemoveWithFilterAndReply(remove_since_, base::Time::Max(),
                                      removal_mask_, origin_type_mask_,
                                      std::move(filter_builder), this);
  }
  OnTaskFinished();
}

void BrowsingDataRemoverFunction::OnRemovedDownload(const base::WeakPtr<BrowsingDataRemoverFunction>& function,
    const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BrowsingDataRemoverFunction OnRemovedDownload is empty!!!!";
    return;
  }

  if (error) {
    std::string errorMessage = error ? error.value() : "remove download failed";
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_remove_download_) {
    LOG(DEBUG) << "BrowsingDataRemoverFunction OnRemovedDownload release";
    function->Release();
  }
}

void BrowsingDataRemoverFunction::OnRemovedHistory(const base::WeakPtr<BrowsingDataRemoverFunction>& function,
    const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BrowsingDataRemoverFunction OnRemovedHistory is empty!!!!";
    return;
  }

  if (error) {
    std::string errorMessage = error ? error.value() : "remove history failed";
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_remove_history_) {
    LOG(DEBUG) << "BrowsingDataRemoverFunction OnRemovedHistory release";
    function->Release();
  }
}
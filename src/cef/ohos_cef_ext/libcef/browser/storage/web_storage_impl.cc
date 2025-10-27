// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/storage/web_storage_impl.h"

#include <sstream>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "components/keyed_service/core/service_access_type.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "libcef/common/time_util.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "third_party/blink/public/mojom/quota/quota_types.mojom.h"
#include "libcef/browser/password/oh_password_store_factory.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
#include <algorithm>
#include <cmath>
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/task/bind_post_task.h"
#include "base/json/json_writer.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/values.h"
#include "base/ohos/nweb_engine_event_logger.h"
#include "components/password_manager/core/browser/form_parsing/form_data_parser.h"
#include "components/password_manager/core/browser/password_form.h"
#include "components/password_manager/core/browser/password_form_digest.h"
#include "components/password_manager/core/common/credential_manager_types.h"
#include "content/browser/browsing_data/browsing_data_filter_builder_impl.h"
#include "content/public/browser/browsing_data_filter_builder.h"
#include "chrome/browser/browser_process.h"
#include "cef/libcef/browser/prefs/browser_prefs.h"
#include "components/prefs/pref_service.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "arkweb/chromium_ext/base/ohos/nweb_engine_event_logger_code.h"
#include "components/os_crypt/sync/os_crypt_linux_for_include.h"
#include "content/public/browser/browser_thread.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
static const int kMigrateDelayBase = 2;
static const int kMigrateDelayTime = 10;
static const int kMigrateDataType = 1;
static const int kMaxDataCount = 50;
static const int kMaxUserNameLength = 128;
static const int kMaxPasswordLength = 256;
static const int kMaxUrlLength = 256;
static const int kMaxDiconnectCount = 3;
const std::string BUNDLE_NAME = "com.huawei.hmos.passwordvault";
const std::string ABILITY_NAME = "ServiceAbility";
const std::string TOKEN = "passwordVaultServiceKey";
const std::u16string MIGRATION_LABEL = u"33ee52bb-c2d5-4e13-ba45-94c2e206a6fd";
 
struct UserData {
  std::string username;
  std::string password;
  std::string url;
  int dataType;
};
 
base::Value UserDatatoJson(const UserData& userData) {
  base::Value obj(base::Value::Type::DICT);
  obj.GetDict().Set("username", base::Value(userData.username));
  obj.GetDict().Set("password", base::Value(userData.password));
  obj.GetDict().Set("url", base::Value(userData.url));
  obj.GetDict().Set("dataType", base::Value(userData.dataType));
  return obj;
}
 
void MigrationCallback::OnMigrationReply(int32_t errorCode, int32_t successCount,
    const std::vector<int32_t>& errorIndex, const std::vector<int32_t>& codeList) {
  if (errorCode == MIGRATION_DISCONNECT) {
    migration_disconnect_count_++;
  }
  for (const auto& element : codeList) {
    if (element == MIGRATION_DUPLICATE_DATA) {
      migration_success_count_++;
    }
  }
 
  migration_error_code_ = errorCode;
  if (migration_error_code_ == MIGRATION_DUPLICATE_DATA) {
    migration_error_code_ = MIGRATION_SUCCESS;
  }
  migration_success_count_ += successCount;
  error_index_ = errorIndex;
  code_list_ = codeList;
  migration_finished_ = true;
}
#endif

namespace {
// Do not keep a reference to the object returned by this method.
CefBrowserContext* GetBrowserContext(const CefBrowserContext::Getter& getter) {
  CEF_REQUIRE_UIT();

  DCHECK(!getter.is_null());

  // Will return nullptr if the BrowserContext has been shut down.
  return getter.Run();
}
}  // namespace
class GetStorageKeysTask
    : public base::RefCountedThreadSafe<GetStorageKeysTask> {
 public:
  GetStorageKeysTask(CefWebStorageImpl::GetOriginsCallback callback,
                     storage::QuotaManager* quota_manager);

  GetStorageKeysTask(const GetStorageKeysTask&) = delete;
  GetStorageKeysTask& operator=(const GetStorageKeysTask&) = delete;

  void Run();

 private:
  friend class base::RefCountedThreadSafe<GetStorageKeysTask>;
  ~GetStorageKeysTask();
  void OnStorageKeysObtained(blink::mojom::StorageType type,
                             const std::set<blink::StorageKey>& storage_keys);
  void OnUsageAndQuotaObtained(const blink::StorageKey& storage_key,
                               blink::mojom::QuotaStatusCode status_code,
                               int64_t usage,
                               int64_t quota);
  void CheckDone();
  void DoneOnUIThread();

  CefWebStorageImpl::GetOriginsCallback ui_callback_;
  scoped_refptr<storage::QuotaManager> quota_manager_;
  std::vector<std::string> origin_;
  std::vector<int64_t> usage_;
  std::vector<int64_t> quota_;
  size_t num_callbacks_to_wait_;
  size_t num_callbacks_received_;
};

GetStorageKeysTask::GetStorageKeysTask(
    CefWebStorageImpl::GetOriginsCallback callback,
    storage::QuotaManager* quota_manager)
    : ui_callback_(std::move(callback)), quota_manager_(quota_manager) {
  DCHECK(CEF_CURRENTLY_ON_UIT());
}

GetStorageKeysTask::~GetStorageKeysTask() {}

void GetStorageKeysTask::Run() {
  DCHECK(CEF_CURRENTLY_ON_UIT());
  CEF_POST_TASK(
      CEF_IOT,
      base::BindOnce(
          &storage::QuotaManager::GetStorageKeysForType, quota_manager_,
          blink::mojom::StorageType::kTemporary,
          base::BindOnce(&GetStorageKeysTask::OnStorageKeysObtained, this,
                         blink::mojom::StorageType::kTemporary)));
}

void GetStorageKeysTask::OnStorageKeysObtained(
    blink::mojom::StorageType type,
    const std::set<blink::StorageKey>& storage_keys) {
  DCHECK(CEF_CURRENTLY_ON_IOT());
  LOG(INFO) << "OnStorageKeysObtained storage_keys size: "
            << storage_keys.size();
  num_callbacks_to_wait_ = storage_keys.size();
  num_callbacks_received_ = 0u;
  for (const blink::StorageKey& storage_key : storage_keys) {
    quota_manager_->GetUsageAndQuota(
        storage_key, type,
        base::BindOnce(&GetStorageKeysTask::OnUsageAndQuotaObtained, this,
                       storage_key));
  }
  CheckDone();
}

void GetStorageKeysTask::OnUsageAndQuotaObtained(
    const blink::StorageKey& storage_key,
    blink::mojom::QuotaStatusCode status_code,
    int64_t usage,
    int64_t quota) {
  DCHECK(CEF_CURRENTLY_ON_IOT());
  if (status_code == blink::mojom::QuotaStatusCode::kOk) {
    origin_.push_back(storage_key.origin().GetURL().spec());
    usage_.push_back(usage);
    quota_.push_back(quota);
  }
  ++num_callbacks_received_;
  CheckDone();
}

void GetStorageKeysTask::CheckDone() {
  DCHECK(CEF_CURRENTLY_ON_IOT());
  if (num_callbacks_received_ == num_callbacks_to_wait_) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&GetStorageKeysTask::DoneOnUIThread, this));
  } else if (num_callbacks_received_ > num_callbacks_to_wait_) {
    NOTREACHED();
  }
}

// This method is to avoid copying the 3 vector arguments into a bound callback.
void GetStorageKeysTask::DoneOnUIThread() {
  DCHECK(CEF_CURRENTLY_ON_UIT());
  if (ui_callback_) {
    std::move(ui_callback_).Run(origin_, usage_, quota_);
  }
}

content::StoragePartition* GetStoragePartitionDelegate(
    CefBrowserContext* browser_context) {
  CEF_REQUIRE_UIT();
  return browser_context->AsBrowserContext()->GetDefaultStoragePartition();
}

storage::QuotaManager* GetQuotaManager(CefBrowserContext* browser_context) {
  CEF_REQUIRE_UIT();
  return GetStoragePartitionDelegate(browser_context)->GetQuotaManager();
}

void CefWebStorageImpl::DeleteAllData() {
  if (!ValidContext()) {
    StoreOrTriggerUIInitCallback(base::BindOnce(
        base::IgnoreResult(&CefWebStorageImpl::DeleteAllDataInternal), this));
    return;
  }
  return DeleteAllDataInternal();
}

void CefWebStorageImpl::DeleteAllDataInternal() {
  DCHECK(ValidContext());
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return;
  }
  GetStoragePartitionDelegate(browser_context)
      ->ClearData(
          // Clear all web storage data except cookies.
          content::StoragePartition::REMOVE_DATA_MASK_FILE_SYSTEMS |
              content::StoragePartition::REMOVE_DATA_MASK_INDEXEDDB |
              content::StoragePartition::REMOVE_DATA_MASK_LOCAL_STORAGE |
              content::StoragePartition::REMOVE_DATA_MASK_WEBSQL,
          content::StoragePartition::QUOTA_MANAGED_STORAGE_MASK_TEMPORARY,
          blink::StorageKey(), base::Time(), base::Time::Max(),
          base::DoNothing());
}

void CefWebStorageImpl::DeleteOrigin(const CefString& origin) {
  GURL gurl = GURL(origin.ToString16());
  if (!gurl.is_empty() && !gurl.is_valid()) {
    return;
  }
  if (!ValidContext()) {
    StoreOrTriggerUIInitCallback(base::BindOnce(
        base::IgnoreResult(&CefWebStorageImpl::DeleteOriginInternal), this,
        gurl));
    return;
  }
  return DeleteOriginInternal(gurl);
}

void CefWebStorageImpl::DeleteOriginInternal(const GURL& origin) {
  DCHECK(ValidContext());
  DCHECK(origin.is_empty() || origin.is_valid());
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    LOG(ERROR) << "CefWebStorageImpl::DeleteOriginInternal can not get "
                  "browser_context.";
    return;
  }
  GetStoragePartitionDelegate(browser_context)
      ->ClearDataForOrigin(
          // All (temporary) QuotaClient types.
          content::StoragePartition::REMOVE_DATA_MASK_FILE_SYSTEMS |
              content::StoragePartition::REMOVE_DATA_MASK_INDEXEDDB |
              content::StoragePartition::REMOVE_DATA_MASK_WEBSQL,
          content::StoragePartition::QUOTA_MANAGED_STORAGE_MASK_TEMPORARY,
          origin, base::DoNothing());
}

void CefWebStorageImpl::GetOrigins(CefRefPtr<CefGetOriginsCallback> callback) {
  if (!ValidContext()) {
    StoreOrTriggerUIInitCallback(base::BindOnce(
        base::IgnoreResult(&CefWebStorageImpl::GetOriginsInternal), this,
        callback));
    return;
  }
  return GetOriginsInternal(callback);
}

void CefWebStorageImpl::GetOriginsInternal(
    CefRefPtr<CefGetOriginsCallback> callback) {
  DCHECK(ValidContext());
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    LOG(ERROR)
        << "CefWebStorageImpl::GetOriginsInternal can not get browser_context.";
    return;
  }
  GetOriginsCallback ui_callback =
      base::BindOnce(&CefWebStorageImpl::GetOriginsCallbackImpl,
                     weak_factory_.GetWeakPtr(), callback);
  base::MakeRefCounted<GetStorageKeysTask>(std::move(ui_callback),
                                           GetQuotaManager(browser_context))
      ->Run();
}

void CefWebStorageImpl::GetOriginQuota(
    const CefString& origin,
    CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) {
  if (!ValidContext()) {
    StoreOrTriggerUIInitCallback(base::BindOnce(
        base::IgnoreResult(&CefWebStorageImpl::GetOriginUsageAndQuotaInternal),
        this, origin, true, callback));
    return;
  }
  return GetOriginUsageAndQuotaInternal(origin, true, callback);
}

void CefWebStorageImpl::GetOriginUsage(
    const CefString& origin,
    CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) {
  if (!ValidContext()) {
    StoreOrTriggerUIInitCallback(base::BindOnce(
        base::IgnoreResult(&CefWebStorageImpl::GetOriginUsageAndQuotaInternal),
        this, origin, false, callback));
    return;
  }
  return GetOriginUsageAndQuotaInternal(origin, false, callback);
}

void CefWebStorageImpl::OnUsageAndQuotaObtained(
    CefWebStorageImpl::QuotaUsageCallback ui_callback,
    blink::mojom::QuotaStatusCode status_code,
    int64_t usage,
    int64_t quota) {
  DCHECK(CEF_CURRENTLY_ON_IOT());
  if (status_code != blink::mojom::QuotaStatusCode::kOk) {
    usage = 0;
    quota = 0;
  }
  CEF_POST_TASK(CEF_UIT, base::BindOnce(std::move(ui_callback), usage, quota));
}

void CefWebStorageImpl::GetOriginUsageAndQuotaInternal(
    const CefString& origin,
    bool is_quota,
    CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) {
  DCHECK(CEF_CURRENTLY_ON_UIT());
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    LOG(ERROR)
        << "CefWebStorageImpl::GetOriginsInternal can not get browser_context.";
    return;
  }
  CefWebStorageImpl::QuotaUsageCallback ui_callback =
      base::BindOnce(&CefWebStorageImpl::CefGetOriginUsageOrQuotaCallbackImpl,
                     weak_factory_.GetWeakPtr(), callback, is_quota);
  CEF_POST_TASK(
      CEF_IOT,
      base::BindOnce(&storage::QuotaManager::GetUsageAndQuota,
                     GetQuotaManager(browser_context),
                     blink::StorageKey::CreateFirstParty(
                         url::Origin::Create(GURL(origin.ToString16()))),
                     blink::mojom::StorageType::kTemporary,
                     base::BindOnce(&CefWebStorageImpl::OnUsageAndQuotaObtained,
                                    this, std::move(ui_callback))));
}

void RunAsyncCompletionOnUIThread(CefRefPtr<CefCompletionCallback> callback) {
  if (!callback.get()) {
    return;
  }
  CEF_POST_TASK(CEF_UIT, base::BindOnce(&CefCompletionCallback::OnComplete,
                                        callback.get()));
}

void CefWebStorageImpl::Initialize(
    CefBrowserContext::Getter browser_context_getter,
    CefRefPtr<CefCompletionCallback> callback) {
  CEF_REQUIRE_UIT();
  DCHECK(!initialized_);
  DCHECK(!browser_context_getter.is_null());
  DCHECK(browser_context_getter_.is_null());
  browser_context_getter_ = browser_context_getter;
  initialized_ = true;
  if (!init_callbacks_.empty()) {
    for (auto& cb : init_callbacks_) {
      std::move(cb).Run();
    }
    init_callbacks_.clear();
  }
  RunAsyncCompletionOnUIThread(callback);
}

void CefWebStorageImpl::StoreOrTriggerUIInitCallback(
    base::OnceClosure callback) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, base::BindOnce(
                               &CefWebStorageImpl::StoreOrTriggerUIInitCallback,
                               this, std::move(callback)));
    return;
  }
  if (initialized_) {
    std::move(callback).Run();
  } else {
    init_callbacks_.emplace_back(std::move(callback));
  }
}

bool CefWebStorageImpl::ValidContext() const {
  return CEF_CURRENTLY_ON_UIT() && initialized_;
}

void CefWebStorageImpl::GetOriginsCallbackImpl(
    CefRefPtr<CefGetOriginsCallback> callback,
    const std::vector<std::string>& origin,
    const std::vector<int64_t>& usage,
    const std::vector<int64_t>& quota) {
  if (!callback.get()) {
    return;
  }
  int32_t nums = origin.size();
  std::vector<CefString> origins;
  std::vector<CefString> usages;
  std::vector<CefString> quotas;
  std::string res;
  for (int32_t i = 0; i < nums; i++) {
    origins.push_back(CefString(origin[i]));
    res = std::to_string(usage[i]);
    usages.push_back(CefString(res));
    res = std::to_string(quota[i]);
    quotas.push_back(CefString(res));
  }

  callback.get()->OnOrigins(origins);
  callback.get()->OnQuotas(quotas);
  callback.get()->OnUsages(usages);
  callback.get()->OnComplete();
}

void CefWebStorageImpl::CefGetOriginUsageOrQuotaCallbackImpl(
    CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback,
    bool is_quota,
    int64_t usage,
    int64_t quota) {
  if (!callback.get()) {
    return;
  }
  int64_t result = is_quota == true ? quota : usage;
  callback.get()->OnComplete(result);
}

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
void CefWebStorageImpl::GetSavedPasswordsInfoInternal(
    CefRefPtr<CefGetSavedPasswordsCallback> callback) {
  // oh_password_consumer_.RequestAutofillableLogins(callback);
}

void SetMigrationPasswordPrefs(std::string_view path, bool value) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&SetMigrationPasswordPrefs, path, value));
    return;
  }

  if (g_browser_process && g_browser_process->local_state()) {
    g_browser_process->local_state()->SetBoolean(path, value);
    g_browser_process->local_state()->CommitPendingWrite();
  }
}

bool VerifyMigrationDataBackupCompletion(const CefBrowserContext::Getter& getter, bool migrateBackupFlag) {
  if (migrateBackupFlag) {
    return true;
  }
  auto browser_context = GetBrowserContext(getter);
  if (!browser_context) {
    LOG(ERROR) << "[Autofill] can not get browser_context.";
    return false;
  }
  base::FilePath cache_path = browser_context->cache_path();
  if (cache_path.empty()) {
    LOG(ERROR) << "[Autofill] cache path is empty.";
    return false;
  }
  base::FilePath::CharType MigrateDir[] = FILE_PATH_LITERAL("migrate");
  base::FilePath::CharType MigrateBackupDir[] = FILE_PATH_LITERAL("migrate_bak");
  base::FilePath migrate_path = cache_path.Append(FILE_PATH_LITERAL(MigrateDir));
  base::FilePath migrate_backup_path = cache_path.Append(FILE_PATH_LITERAL(MigrateBackupDir));
  if (!base::PathExists(migrate_path)) {
    LOG(INFO) << "[Autofill] Migrate directory not exist.";
    std::string err_msg = "Migrate directory not exist, error_code:" +
                          std::to_string(DIRECTORY_OR_FILE_NOT_EXIST);
    base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                  base::ohos::kPasswordManagerError, err_msg);
    SetMigrationPasswordPrefs(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return false;
  }
  if (!base::CopyDirectory(migrate_path, migrate_backup_path, false)) {
    LOG(ERROR) << "[Autofill] migrate directory backup failed.";
    return false;
  }
  SetMigrationPasswordPrefs(browser_prefs::kMigrationDataBackupCompletion, true);
  LOG(INFO) << "[Autofill] migrate directory backup successful.";
  return true;
}

void CefWebStorageImpl::MigratePasswordsInfoInternal(bool migrateBackupFlag) {
  if (!VerifyMigrationDataBackupCompletion(browser_context_getter_, migrateBackupFlag)) {
    return;
  }
  oh_password_consumer_.RequestAndMigrateAutofillableLogins();
}

CefWebStorageImpl::OhPasswordStoreConsumer::OhPasswordStoreConsumer(
    CefWebStorageImpl* web_storage_impl)
    : web_storage_impl_(web_storage_impl) {}

CefWebStorageImpl::OhPasswordStoreConsumer::~OhPasswordStoreConsumer() {}

void CefWebStorageImpl::OhPasswordStoreConsumer::OnGetPasswordStoreResults(
    std::vector<std::unique_ptr<password_manager::PasswordForm>> results) {
  // This class overrides OnGetPasswordStoreResultsFrom() (the version of this
  // method that also receives the originating store), so the store-less version
  // never gets called.
  NOTREACHED();
}

bool ShouldMigratePassword(password_manager::PasswordForm* form, CefRefPtr<CefWebStorageImpl> web_storage_impl) {
  if (form->display_name == MIGRATION_LABEL) {
    return false;
  }
  password_manager::PasswordStore* password_store = web_storage_impl->GetPasswordStore();
  if (form->username_value.empty() || form->password_value.empty() || form->url.is_empty()) {
    form->display_name = MIGRATION_LABEL;
    password_store->UpdateLoginDisplayName(*form);
    LOG(WARNING) << "[Autofill] migration data value is empty.";
    return false;
  }
 
  if (form->username_value.length() > kMaxUserNameLength || form->password_value.length() > kMaxPasswordLength ||
      form->url.spec().length() > kMaxUrlLength) {
    form->display_name = MIGRATION_LABEL;
    password_store->UpdateLoginDisplayName(*form);
    LOG(WARNING) << "[Autofill] migration data length over max.";
    return false;
  }
 
  if (!form->url.SchemeIsHTTPOrHTTPS()) {
    form->display_name = MIGRATION_LABEL;
    password_store->UpdateLoginDisplayName(*form);
    LOG(WARNING) << "[Autofill] migration data url is invalid.";
    return false;
  }
 
  return true;
}

void UpdatePasswordDisplayName(const std::vector<CefString>& url,
                               const std::vector<CefString>& username,
                               std::shared_ptr<MigrationCallback>& migration_listener,
                               CefRefPtr<CefWebStorageImpl> web_storage_impl,
                               std::vector<std::unique_ptr<password_manager::PasswordForm>>& results) {
  password_manager::PasswordStore* password_store = web_storage_impl->GetPasswordStore();
  std::vector<int32_t> error_index = migration_listener->GetMigrationErrorIndex();
  std::vector<int32_t> code_list = migration_listener->GetMigrationCodeList();

  for (size_t i = 0; i < url.size(); i++) {
    auto it = std::find(error_index.begin(), error_index.end(), i);
    if (it != error_index.end() &&
        code_list[std::distance(error_index.begin(), it)] != MIGRATION_DUPLICATE_DATA) {
      continue;
    }

    for (auto& form : results) {
      if (form->url.spec() == url[i].ToString() && form->username_value == username[i].ToString16()) {
        form->display_name = MIGRATION_LABEL;
        password_store->UpdateLoginDisplayName(*form);
        break;
      }
    }
  }
}

bool ProcessAndSendMigrationRequest(base::Value& json_array, const std::vector<CefString>& url,
                                    const std::vector<CefString>& username,
                                    std::unique_ptr<OHOS::NWeb::MigrationManagerAdapter>& migration_manager_adapter,
                                    std::shared_ptr<MigrationCallback>& migration_listener,
                                    CefRefPtr<CefWebStorageImpl> web_storage_impl,
                                    std::vector<std::unique_ptr<password_manager::PasswordForm>>& results) {
  std::string json_string;
  base::JSONWriter::Write(json_array, &json_string);
  std::shared_ptr<std::string> shared_json = std::make_shared<std::string>(json_string);
 
  for (size_t j = 0; j <= kMaxDiconnectCount; j++) {
    if (!migration_manager_adapter->SendMigrationRequest(shared_json)) {
      LOG(ERROR) << "[Autofill] Connect PasswordVault failed.";
      std::string err_msg = "Connect PasswordVault failed, error_code:" +
                            std::to_string(PASSWORD_VAULT_CONNECT_FAILED);
      base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                    base::ohos::kPasswordManagerError, err_msg);
      migration_listener->SetMigrationErrorCode(MIGRATION_SERVICE_ABILITY_DISABLE);
      SetMigrationPasswordPrefs(browser_prefs::kMigratePasswordsToPasswordVault, true);
      return false;
    }
 
    while (!migration_listener->GetMigrationFinish()) {
      base::PlatformThread::Sleep(base::Milliseconds(kMigrateDelayTime));
    }
    if (migration_listener->GetMigrationErrorCode() == MIGRATION_STORAGE_FAILED) {
      LOG(ERROR) << "[Autofill] PasswordVault storage failed.";
      std::string err_msg = "PasswordVault storage failed, error_code:" +
                            std::to_string(PASSWORD_IMPORT_FAILED);
      base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                    base::ohos::kPasswordManagerError, err_msg);
      return false;
    } else if (migration_listener->GetMigrationErrorCode() == MIGRATION_NOT_SET_SCREEN_LOCK) {
      LOG(ERROR) << "[Autofill] The screen lock password not set.";
      std::string err_msg = "The screen lock password not set, error_code:" +
                            std::to_string(PASSWORD_IMPORT_FAILED);
      base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                    base::ohos::kPasswordManagerError, err_msg);
      return false;
    } else if (migration_listener->GetMigrationErrorCode() == MIGRATION_DISCONNECT) {
      if (j == kMaxDiconnectCount) {
        LOG(ERROR) << "[Autofill] Disconnect count over max.";
        std::string err_msg = "Disconnect count over max, error_code:" +
                              std::to_string(PASSWORD_VAULT_CONNECT_FAILED);
        base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                      base::ohos::kPasswordManagerError, err_msg);
        return false;
      }
      base::PlatformThread::Sleep(base::Seconds(kMigrateDelayTime * std::pow(kMigrateDelayBase, j)));
      continue;
    }
    break;
  }
 
  UpdatePasswordDisplayName(url, username, migration_listener, web_storage_impl, results);
 
  return true;
}

void MigratePasswordToPasswordVault(CefRefPtr<CefWebStorageImpl> web_storage_impl,
                                    std::unique_ptr<OHOS::NWeb::MigrationManagerAdapter>& migration_manager_adapter,
                                    std::shared_ptr<MigrationCallback>& migration_listener,
                                    std::vector<std::unique_ptr<password_manager::PasswordForm>>& results) {
  std::vector<CefString> url;
  std::vector<CefString> username;
  uint32_t count = 0;
  uint32_t size = 0;
  base::Value json_array(base::Value::Type::LIST);
  for (auto& form : results) {
    size++;
    bool ret = ShouldMigratePassword(form.get(), web_storage_impl);
    if (ret == false && size != results.size()) {
      continue;
    }
    if (ret == true) {
      url.push_back(CefString(form->url.spec()));
      username.push_back(CefString(base::UTF16ToUTF8(form->username_value)));
 
      json_array.GetList().Append(UserDatatoJson(UserData{base::UTF16ToUTF8(form->username_value),
                                                          base::UTF16ToUTF8(form->password_value),
                                                          form->url.spec(),
                                                          kMigrateDataType}));
      count++;
    }
    if (count == kMaxDataCount || (size == results.size() && count != 0)) {
      if (!ProcessAndSendMigrationRequest(json_array, url, username, migration_manager_adapter, migration_listener,
                                          web_storage_impl, results)) {
        return;
      }
      migration_listener->SetMigrationFinish(false);
      json_array.GetList().clear();
      url.clear();
      username.clear();
      count = 0;
    }
  }
}

void OnMigratePasswordToPasswordVault(CefRefPtr<CefWebStorageImpl> web_storage_impl,
    std::vector<std::unique_ptr<password_manager::PasswordForm>> results) {
  base::PlatformThread::Sleep(base::Seconds(kMigrateDelayTime / kMigrateDelayBase));
  std::unique_ptr<OHOS::NWeb::MigrationManagerAdapter> migration_manager_adapter;
  migration_manager_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateMigrationMgrAdapter();
  if (migration_manager_adapter == nullptr) {
     LOG(ERROR) << "[Autofill] << migration_manager_adapter create failed.";
     return;
  }
  std::shared_ptr<MigrationCallback> migration_listener = std::make_shared<MigrationCallback>();
  migration_manager_adapter->RegisterMigrationListener(migration_listener);
  if (migration_listener == nullptr) {
     LOG(ERROR) << "[Autofill] << migration_listener register failed.";
     return;
  }
  migration_manager_adapter->SetMigrationParam(BUNDLE_NAME, ABILITY_NAME, TOKEN);
 
  MigratePasswordToPasswordVault(web_storage_impl, migration_manager_adapter, migration_listener, results);
 
  if (migration_listener->GetMigrationErrorCode() == MIGRATION_SUCCESS &&
      migration_listener->GetMigrationDisconnectCount() == 0) {
    SetMigrationPasswordPrefs(browser_prefs::kMigratePasswordsToPasswordVault, true);
    LOG(INFO) << "[Autofill] Migrate password to passwordVault success, migration total count:"
      << results.size() << ", success count:" << migration_listener->GetMigrationSuccessCount();
    std::string err_msg = "Migrate password to passwordVault success, error_code:" +
      std::to_string(MIGRATE_SUCCESS) + ", migration total count:" + std::to_string(results.size()) +
      ", success count:" + std::to_string(migration_listener->GetMigrationSuccessCount());
    base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                  base::ohos::kPasswordManagerError, err_msg);
  }
}

void CefWebStorageImpl::OhPasswordStoreConsumer::GetPasswordStoreResultsFrom(
  std::vector<std::unique_ptr<password_manager::PasswordForm>> results) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&CefWebStorageImpl::OhPasswordStoreConsumer::GetPasswordStoreResultsFrom,
                       weak_ptr_factory_.GetWeakPtr(),
                       std::move(results)));
    return;
  }

  LOG(DEBUG) << "[Autofill]OhPasswordStoreConsumer::GetPasswordStoreResultsFrom.";
  if (!g_browser_process || !g_browser_process->local_state()) {
    LOG(ERROR) << "[Autofill]GetPasswordStoreResultsFrom:prefService is invalid.";
    return;
  }

  PrefService* local_state = g_browser_process->local_state();
  if (local_state->GetBoolean(browser_prefs::kMigratePasswordsToPasswordVault)) {
    return;
  }

  if (local_state->GetBoolean(browser_prefs::kMigrationQueryAssetfailure)) {
    local_state->SetBoolean(browser_prefs::kMigrationQueryAssetfailure, false);
    local_state->CommitPendingWrite();
    return;
  }
 
  base::ThreadPool::PostTask(
          FROM_HERE,
          {base::MayBlock(), base::TaskPriority::USER_VISIBLE},
          base::BindOnce(OnMigratePasswordToPasswordVault, (CefRefPtr<CefWebStorageImpl>)web_storage_impl_,
                         std::move(results)));
}

void CefWebStorageImpl::OhPasswordStoreConsumer::OnGetPasswordStoreResultsFrom(
    password_manager::PasswordStoreInterface* store,
    std::vector<std::unique_ptr<password_manager::PasswordForm>> results) {
  GetPasswordStoreResultsFrom(std::move(results));
}

void CefWebStorageImpl::OhPasswordStoreConsumer::RequestAutofillableLogins(
    CefRefPtr<CefGetSavedPasswordsCallback> callback) {
}

void CefWebStorageImpl::OhPasswordStoreConsumer::RequestAndMigrateAutofillableLogins() {
  password_manager::PasswordStore* password_store =
      web_storage_impl_->GetPasswordStore();
  if (!password_store) {
    return;
  }
  password_store->GetAutofillableLogins(weak_ptr_factory_.GetWeakPtr());
}

void CefWebStorageImpl::OnGetAutofillableLogins(
    CefRefPtr<CefGetSavedPasswordsCallback> callback,
    const std::vector<CefString>& url,
    const std::vector<CefString>& username,
    int iter_number) {
  // GetSavedPasswordsCallbackImpl(callback, url, username);
}

void CefWebStorageImpl::GetPasswordInternal(
    const CefString& url,
    const CefString& username,
    CefRefPtr<CefGetPasswordCallback> callback) {
}

void CefWebStorageImpl::GetPasswordCallbackImpl(
    CefRefPtr<CefGetPasswordCallback> callback,
    const std::u16string& password) {
}

void CefWebStorageImpl::GetSavedPasswordsCallbackImpl(
    CefRefPtr<CefGetSavedPasswordsCallback> callback,
    const std::vector<CefString>& url,
    const std::vector<CefString>& username) {
 }

password_manager::PasswordStore* CefWebStorageImpl::GetPasswordStore() {
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return nullptr;
  }
  return OhPasswordStoreFactory::GetPasswordStoreForContext(
             browser_context->AsBrowserContext(),
             ServiceAccessType::EXPLICIT_ACCESS)
      .get();
}

std::vector<std::unique_ptr<password_manager::PasswordForm>>
CefWebStorageImpl::GetPasswordFormsByUrl(
    password_manager::PasswordStore* password_store,
    const std::u16string& url_string) {
}

void CefWebStorageImpl::ClearPasswordInternal() {
  password_manager::PasswordStore* password_store = GetPasswordStore();
}

void CefWebStorageImpl::RemovePasswordInternal(const CefString& url_string,
                                               const CefString& username) {
}

void CefWebStorageImpl::ModifyPasswordInternal(const CefString& url,
                                               const CefString& old_username,
                                               const CefString& new_username,
                                               const CefString& new_password) {
}

void CefWebStorageImpl::RemovePasswordByUrlInternal(
    const CefString& url_string) {
}
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD)

// static
CefRefPtr<CefWebStorage> CefWebStorage::GetGlobalManager(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  return context ? context->GetWebStorage(callback) : nullptr;
}

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
// static
CefRefPtr<CefWebStorage> CefWebStorage::GetGlobalIncognitoManager(
    CefRefPtr<CefCompletionCallback> callback) {
  LOG(INFO) << "CefWebStorage::GetGlobalIncognitoManage.";
  CefRefPtr<CefRequestContext> context =
      CefRequestContext::GetGlobalOTRContext();
  return context ? context->GetWebStorage(callback) : nullptr;
}
#endif

void CefWebStorageImpl::GetPassword(
    const CefString& url,
    const CefString& username,
    CefRefPtr<CefGetPasswordCallback> callback) {
}

void CefWebStorageImpl::GetSavedPasswordsInfo(
    CefRefPtr<CefGetSavedPasswordsCallback> callback) {
}

void CefWebStorageImpl::MigratePasswordsInfo() {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&CefWebStorageImpl::MigratePasswordsInfo, weak_factory_.GetWeakPtr()));
    return;
  }

  bool migrateBackupFlag =
    g_browser_process->local_state()->GetBoolean(browser_prefs::kMigrationDataBackupCompletion);

  CEF_POST_TASK(
      CEF_IOT, base::BindOnce(&CefWebStorageImpl::MigratePasswordsInfoInternal,
                              weak_factory_.GetWeakPtr(), migrateBackupFlag));
#endif  // ARKWEB_EXT_PASSWORD
}

void CefWebStorageImpl::ClearPassword() {
}

void CefWebStorageImpl::RemovePassword(const CefString& url,
                                       const CefString& username) {
}

void CefWebStorageImpl::ModifyPassword(const CefString& url,
                                       const CefString& old_username,
                                       const CefString& new_username,
                                       const CefString& new_password) {
}

void CefWebStorageImpl::RemovePasswordByUrl(const CefString& url) {
}

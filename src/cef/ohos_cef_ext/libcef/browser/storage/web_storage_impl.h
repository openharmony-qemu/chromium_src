// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_SERVICE_WEB_STORAGE_IMPL_H_
#define CEF_LIBCEF_BROWSER_NET_SERVICE_WEB_STORAGE_IMPL_H_

#include <atomic>
#include <queue>

#include "base/threading/thread.h"
#include "include/cef_web_storage.h"
#include "libcef/browser/browser_context.h"
#include "libcef/browser/thread_util.h"
#include "storage/browser/quota/quota_manager.h"

#if BUILDFLAG(IS_ARKWEB)
#include "components/password_manager/core/browser/password_form.h"
#include "components/password_manager/core/browser/password_store/password_store.h"
#include "components/password_manager/core/browser/password_store/password_store_consumer.h"
#include "components/password_manager/core/browser/password_store/statistics_table.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

enum WebStorageMigration {
  MIGRATION_SERVICE_ABILITY_DISABLE = -2,
  MIGRATION_STORAGE_FAILED = -1,
  MIGRATION_DISCONNECT = 0,
  MIGRATION_SUCCESS,
  MIGRATION_DUPLICATE_DATA = 304,
  MIGRATION_NOT_SET_SCREEN_LOCK = 503
};
#endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

// Implementation of the CefWebStorage interface. May be created on any
// thread.
class CefWebStorageImpl : public CefWebStorage {
 public:
  CefWebStorageImpl(const CefWebStorageImpl&) = delete;
  CefWebStorageImpl& operator=(const CefWebStorageImpl&) = delete;

  CefWebStorageImpl() {}
  void Initialize(CefBrowserContext::Getter browser_context_getter,
                  CefRefPtr<CefCompletionCallback> callback);
  void DeleteAllData() override;
  void DeleteOrigin(const CefString& origin) override;
  void GetOrigins(CefRefPtr<CefGetOriginsCallback> callback) override;
  void GetOriginQuota(
      const CefString& origin,
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) override;
  void GetOriginUsage(
      const CefString& origin,
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) override;

  void ClearPassword() override;
  void RemovePassword(const CefString& url, const CefString& username) override;
  void ModifyPassword(const CefString& url,
                      const CefString& old_username,
                      const CefString& new_username,
                      const CefString& new_password) override;
  void RemovePasswordByUrl(const CefString& url) override;
  void GetPassword(const CefString& url,
                   const CefString& username,
                   CefRefPtr<CefGetPasswordCallback> callback) override;
  void GetSavedPasswordsInfo(
      CefRefPtr<CefGetSavedPasswordsCallback> callback) override;
  void MigratePasswordsInfo() override;

  using GetOriginsCallback =
      base::OnceCallback<void(const std::vector<std::string>&,
                              const std::vector<int64_t>&,
                              const std::vector<int64_t>&)>;
  using QuotaUsageCallback = base::OnceCallback<void(int64_t, int64_t)>;
  void DeleteAllDataInternal();
  void DeleteOriginInternal(const GURL& origin);
  void GetOriginsInternal(CefRefPtr<CefGetOriginsCallback> callback);
  void GetOriginUsageAndQuotaInternal(
      const CefString& origin,
      bool is_quota,
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback);
  void OnUsageAndQuotaObtained(QuotaUsageCallback ui_callback,
                               blink::mojom::QuotaStatusCode status_code,
                               int64_t usage,
                               int64_t quota);
  bool ValidContext() const;
  void StoreOrTriggerUIInitCallback(base::OnceClosure callback);
  void GetOriginsCallbackImpl(CefRefPtr<CefGetOriginsCallback> callback,
                              const std::vector<std::string>& origin,
                              const std::vector<int64_t>& usage,
                              const std::vector<int64_t>& quota);
  void CefGetOriginUsageOrQuotaCallbackImpl(
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback,
      bool is_quota,
      int64_t usage,
      int64_t quota);

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void GetSavedPasswordsInfoInternal(
      CefRefPtr<CefGetSavedPasswordsCallback> callback);
  void GetPasswordInternal(const CefString& url,
                           const CefString& username,
                           CefRefPtr<CefGetPasswordCallback> callback);
  void MigratePasswordsInfoInternal(bool migrateBackupFlag);
  void GetPasswordCallbackImpl(CefRefPtr<CefGetPasswordCallback> callback,
                               const std::u16string& password);
  void GetSavedPasswordsCallbackImpl(
      CefRefPtr<CefGetSavedPasswordsCallback> callback,
      const std::vector<CefString>& url,
      const std::vector<CefString>& username);
  void ClearPasswordInternal();
  void RemovePasswordByUrlInternal(const CefString& url);

  void ModifyPasswordInternal(const CefString& url,
                              const CefString& old_username,
                              const CefString& new_username,
                              const CefString& new_password);
  void RemovePasswordInternal(const CefString& url, const CefString& username);
  std::vector<std::unique_ptr<password_manager::PasswordForm>>
  GetPasswordFormsByUrl(password_manager::PasswordStore* password_store,
                        const std::u16string& url_string);
  password_manager::PasswordStore* GetPasswordStore();
  void OnGetAutofillableLogins(CefRefPtr<CefGetSavedPasswordsCallback> callback,
                               const std::vector<CefString>& url,
                               const std::vector<CefString>& username,
                               int iter_number);
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD)
 private:
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  class OhPasswordStoreConsumer
      : public password_manager::PasswordStoreConsumer {
   public:
    explicit OhPasswordStoreConsumer(CefWebStorageImpl* web_storage);
    ~OhPasswordStoreConsumer() override;

    void OnGetPasswordStoreResults(
        std::vector<std::unique_ptr<password_manager::PasswordForm>> results)
        override;

    // Send the password store's reply back to the handler.
    void OnGetPasswordStoreResultsFrom(
        password_manager::PasswordStoreInterface* store,
        std::vector<std::unique_ptr<password_manager::PasswordForm>> results)
        override;
    void RequestAutofillableLogins(
        CefRefPtr<CefGetSavedPasswordsCallback> callback);
    void RequestAndMigrateAutofillableLogins();

    void GetPasswordStoreResultsFrom(
        std::vector<std::unique_ptr<password_manager::PasswordForm>> results);

   protected:
    raw_ptr<CefWebStorageImpl> web_storage_impl_;

   private:
    base::WeakPtrFactory<OhPasswordStoreConsumer> weak_ptr_factory_{this};
  };

  OhPasswordStoreConsumer oh_password_consumer_{this};

  std::queue<CefRefPtr<CefGetSavedPasswordsCallback>> callback_queue_;
#endif

  // Only accessed on the UI thread. Will be non-null after Initialize().
  CefBrowserContext::Getter browser_context_getter_;
  bool initialized_ = false;
  std::vector<base::OnceClosure> init_callbacks_;
  base::WeakPtrFactory<CefWebStorageImpl> weak_factory_{this};
  IMPLEMENT_REFCOUNTING(CefWebStorageImpl);
};

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
class MigrationCallback : public OHOS::NWeb::MigrationListenerAdapter {
public:
  MigrationCallback() {}
  void OnMigrationReply(int32_t errorCode, int32_t successCount, const std::vector<int32_t>& errorIndex,
                        const std::vector<int32_t>& codeList) override;
  void SetMigrationFinish(bool isFinish) { migration_finished_ = isFinish; }
  bool GetMigrationFinish() { return migration_finished_; }
  int32_t SetMigrationErrorCode(int32_t errorCode) { return migration_error_code_ = errorCode; }
  int32_t GetMigrationErrorCode() { return migration_error_code_; }
  uint32_t GetMigrationSuccessCount() { return migration_success_count_; }
  uint32_t GetMigrationDisconnectCount() { return migration_disconnect_count_; }
  std::vector<int32_t> GetMigrationErrorIndex() { return error_index_; }
  std::vector<int32_t> GetMigrationCodeList() { return code_list_; }
 
private:
  bool migration_finished_ = false;
  int32_t migration_error_code_ = MIGRATION_SUCCESS;
  uint32_t migration_success_count_ = 0;
  uint32_t migration_disconnect_count_ = 0;
  std::vector<int32_t> error_index_ = {};
  std::vector<int32_t> code_list_ = {};
};
#endif

#endif  // CEF_LIBCEF_BROWSER_NET_SERVICE_WEB_STORAGE_IMPL_H_

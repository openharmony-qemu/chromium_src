// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_H_

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner_helpers.h"
#include "media/base/callback_registry.h"
#include "media/base/cdm_context.h"
#include "media/base/cdm_promise.h"
#include "media/base/cdm_promise_adapter.h"
#include "media/base/content_decryption_module.h"
#include "media/base/media_drm_storage.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_crypto_context.h"
#include "media/base/ohos/ohos_media_crypto_context_impl.h"
#include "media/base/ohos/ohos_media_drm_storage_bridge.h"
#include "media/base/provision_fetcher.h"
#include "url/origin.h"

#include "third_party/ohos_ndk/includes/ohos_adapter/drm_adapter.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class MEDIA_EXPORT OHOSMediaDrmBridge : public ContentDecryptionModule,
                                        public CdmContext {
 public:
  enum SecurityLevel {
    SECURITY_LEVEL_DEFAULT = 0,
    SECURITY_LEVEL_1 = 1,
    SECURITY_LEVEL_3 = 3,
  };

  enum OHOSMediaKeyType {
    OHOS_MEDIA_KEY_TYPE_OFFLINE = 0,
    OHOS_MEDIA_KEY_TYPE_ONLINE,
    OHOS_MEDIA_KEY_TYPE_RELEASE,
  };

  enum OHOSClearInfoType {
    OHOS_KEY_RELEASE = 0,
    OHOS_LOAD_FAIL,
  };

  using OHOSMediaCryptoReadyCB = OHOSMediaCryptoContext::OHOSMediaCryptoReadyCB;

  static bool IsKeySystemSupported(const std::string& key_system);
  static bool IsKeySystemSupportedWithType(
      const std::string& key_system,
      const std::string& container_mime_type);
  static bool IsPerApplicationProvisioningSupported() { return false; }
  static bool IsPersistentLicenseTypeSupported(const std::string& key_system);
  static std::vector<std::string> GetPlatformKeySystemNames();
  static std::vector<uint8_t> GetUUID(const std::string& key_system);

  OHOSMediaDrmBridge(const OHOSMediaDrmBridge&) = delete;
  OHOSMediaDrmBridge& operator=(const OHOSMediaDrmBridge&) = delete;
  base::WeakPtr<OHOSMediaDrmBridge> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }
  void SetServerCertificate(
      const std::vector<uint8_t>& certificate,
      std::unique_ptr<media::SimpleCdmPromise> promise) override;
  void CreateSessionAndGenerateRequest(
      CdmSessionType session_type,
      media::EmeInitDataType init_data_type,
      const std::vector<uint8_t>& init_data,
      std::unique_ptr<media::NewSessionCdmPromise> promise) override;
  void LoadSession(
      CdmSessionType session_type,
      const std::string& session_id,
      std::unique_ptr<media::NewSessionCdmPromise> promise) override;
  void UpdateSession(const std::string& session_id,
                     const std::vector<uint8_t>& response,
                     std::unique_ptr<media::SimpleCdmPromise> promise) override;
  void CloseSession(const std::string& session_id,
                    std::unique_ptr<media::SimpleCdmPromise> promise) override;
  void RemoveSession(const std::string& session_id,
                     std::unique_ptr<media::SimpleCdmPromise> promise) override;
  CdmContext* GetCdmContext() override;
  void DeleteOnCorrectThread() const override;

  std::unique_ptr<CallbackRegistration> RegisterEventCB(
      EventCB event_cb) override;

  OHOSMediaCryptoContext* GetOHOSMediaCryptoContext() override;

  void Provision(base::OnceCallback<void(bool)> provisioning_complete_cb);
  void Unprovision();

  bool IsSecureCodecRequired();

  void ResolvePromise(uint32_t promise_id);
  void ResolvePromiseWithSession(uint32_t promise_id,
                                 const std::string& session_id);
  void RejectPromise(uint32_t promise_id,
                     CdmPromise::Exception exception_code,
                     const std::string& error_message);

  void SetOHOSMediaCryptoReadyCB(OHOSMediaCryptoReadyCB media_crypto_ready_cb);
#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
  std::vector<uint8_t> GetSchemeUUID();
  void SetOHOSMediaCryptoAndLicenseReadyCB(
      OHOSMediaCryptoReadyCB media_license_ready_cb);
#endif

  void OnOHOSMediaCryptoReady(void* session);

  void OnProvisionRequest(const std::string& default_url,
                          const std::string& request_data);
  void OnProvisioningComplete(bool success);

  void OnPromiseResolved(uint32_t promise_id);
  void OnPromiseResolvedWithSession(uint32_t promise_id,
                                    const std::string& session_id);

  void OnPromiseRejected(uint32_t promise_id, const std::string& error_message);

  void OnStorageProvisioned();
  void OnStorageProvisionedResult(bool result);
  void OnStorageSaveInfo(const std::vector<uint8_t>& ket_set_id,
                         const std::string& mime_type,
                         const std::string& session_id,
                         int32_t key_type);
  void OnStorageSaveInfoUpdateResult(bool result);
  void OnStorageSaveInfoRemoveResult(bool result);

  void OnStorageLoadInfo(const std::string& session_id);
  void OnStorageLoadInfoResult(const std::string& session_id,
                               const std::vector<uint8_t>& key_set_id,
                               const std::string& mime,
                               uint32_t key_type);

  void OnStorageClearInfoForKeyRelease(const std::string& session_id);
  void OnStorageClearInfoForKeyReleaseResult(bool result);

  void OnStorageClearInfoForLoadFail(const std::string& session_id);
  void OnStorageClearInfoForLoadFailResult(bool result);

  void OnSessionMessage(const std::string& session_id,
                        uint32_t message_type,
                        const std::vector<uint8_t>& message);
  void OnSessionClosed(const std::string& session_id);

  void OnSessionKeysChange(const std::string& session_id,
                           const std::vector<std::string>& key_id_array,
                           const std::vector<uint32_t>& status_array,
                           bool has_additional_usable_key,
                           bool is_key_release);

  void OnSessionExpirationUpdate(const std::string& session_id,
                                 uint64_t expiry_time_ms);

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
  void OnMediaLicenseReady(bool success);
#endif
 private:
  friend class OHOSMediaDrmBridgeFactory;
  friend class base::DeleteHelper<OHOSMediaDrmBridge>;

  static scoped_refptr<OHOSMediaDrmBridge> CreateInternal(
      const std::vector<uint8_t>& scheme_uuid,
      const std::string& origin_id,
      SecurityLevel security_level,
      bool requires_media_crypto,
      std::unique_ptr<OHOSMediaDrmStorageBridge> storage,
      CreateFetcherCB create_fetcher_cb,
      const SessionMessageCB& session_message_cb,
      const SessionClosedCB& session_closed_cb,
      const SessionKeysChangeCB& session_keys_change_cb,
      const SessionExpirationUpdateCB& session_expiration_update_cb);

  OHOSMediaDrmBridge(
      const std::vector<uint8_t>& scheme_uuid,
      const std::string& origin_id,
      SecurityLevel security_level,
      bool requires_media_crypto,
      std::unique_ptr<OHOSMediaDrmStorageBridge> storage,
      const CreateFetcherCB& create_fetcher_cb,
      const SessionMessageCB& session_message_cb,
      const SessionClosedCB& session_closed_cb,
      const SessionKeysChangeCB& session_keys_change_cb,
      const SessionExpirationUpdateCB& session_expiration_update_cb);

  ~OHOSMediaDrmBridge() override;

  SecurityLevel GetSecurityLevel();
  void NotifyMediaCryptoReady();
  void SendProvisioningRequest(const GURL& default_url,
                               const std::string& request_data);
  void ProcessProvisionResponse(bool success, const std::string& response);
  void OnHasAdditionalUsableKey();
  std::vector<uint8_t> scheme_uuid_;
  std::unique_ptr<OHOSMediaDrmStorageBridge> storage_;
  CreateFetcherCB create_fetcher_cb_;
  std::unique_ptr<ProvisionFetcher> provision_fetcher_;
  base::OnceCallback<void(bool)> provisioning_complete_cb_;

  SessionMessageCB session_message_cb_;
  SessionClosedCB session_closed_cb_;
  SessionKeysChangeCB session_keys_change_cb_;
  SessionExpirationUpdateCB session_expiration_update_cb_;

  OHOSMediaCryptoReadyCB media_crypto_ready_cb_;
#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
  OHOSMediaCryptoReadyCB media_crypto_and_license_ready_cb_;
  bool is_license_ready_ = false;
#endif

  CallbackRegistry<EventCB::RunType> event_callbacks_;
  CdmPromiseAdapter cdm_promise_adapter_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  OHOSMediaCryptoContextImpl media_crypto_context_;

  std::unique_ptr<OHOS::NWeb::DrmAdapter> ohos_drm_adapter_;
  OHOS::NWeb::OHOSMediaKeySession ohos_media_key_session_;
  base::WeakPtrFactory<OHOSMediaDrmBridge> weak_factory_{this};
};

class OHOSDrmCallback : public OHOS::NWeb::DrmCallbackAdapter {
 public:
  OHOSDrmCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSMediaDrmBridge> media_drm_bridge);
   
  virtual ~OHOSDrmCallback();
  OHOSDrmCallback(const OHOSDrmCallback&) = delete;
  OHOSDrmCallback& operator=(const OHOSDrmCallback&) = delete;

  void OnSessionMessage(const std::string& session_id,
                        int32_t& type,
                        const std::vector<uint8_t>& message) override;
  void OnProvisionRequest(const std::string& default_url,
                          const std::string& request_data) override;
  void OnProvisioningComplete(bool success) override;
  void OnMediaKeySessionReady(void* session) override;
  void OnPromiseRejected(uint32_t promise_id,
                         const std::string& error_message) override;
  void OnPromiseResolved(uint32_t promise_id) override;
  void OnPromiseResolvedWithSession(uint32_t promise_id,
                                    const std::string& session_id) override;
  void OnSessionClosed(const std::string& session_id) override;

  void OnSessionKeysChange(const std::string& session_id,
                           const std::vector<std::string>& key_id_array,
                           const std::vector<uint32_t>& status_array,
                           bool has_additional_usable_key,
                           bool is_key_release) override;
  void OnSessionExpirationUpdate(const std::string& session_id,
                                 uint64_t expiration_time) override;

  void OnStorageProvisioned() override;
  void OnStorageSaveInfo(const std::vector<uint8_t>& ket_set_id,
                         const std::string& mime_type,
                         const std::string& session_id,
                         int32_t key_type) override;
  void OnStorageLoadInfo(const std::string& session_id) override;
  void OnStorageClearInfoForKeyRelease(const std::string& session_id) override;
  void OnStorageClearInfoForLoadFail(const std::string& session_id) override;
  void OnMediaLicenseReady(bool success) override;

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtr<OHOSMediaDrmBridge> media_drm_bridge_;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_H_

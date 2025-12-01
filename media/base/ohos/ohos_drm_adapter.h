// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_DRM_ADAPTER_H_
#define MEDIA_BASE_OHOS_OHOS_DRM_ADAPTER_H_

#include <pthread.h>

#include <multimedia/drm_framework/native_drm_common.h>
#include <multimedia/drm_framework/native_drm_err.h>
#include "base/logging.h"

#include "base/logging.h"
#include "media/base/ohos/ohos_drm_callback.h"

namespace media {

#define MAX_KEY_SET_ID_LEN 64

using OhosMediaKeySession = void*;

typedef enum OhosMediaKeyType {
  MEDIA_KEY_TYPE_OFFLINE = 0,
  MEDIA_KEY_TYPE_ONLINE,
  MEDIA_KEY_TYPE_RELEASE,
} OhosMediaKeyType;

typedef enum OhosClearInfoType {
  OHOS_KEY_RELEASE = 0,
  OHOS_LOAD_FAIL,
} OhosClearInfoType;

typedef enum OhosKeyStatus {
  KEY_STATUS_USABLE = 0,
  KEY_STATUS_EXPIRED = 1,
  KEY_STATUS_OUTPUT_NOT_ALLOWED = 2,
  KEY_STATUS_PENDING = 3,
  KEY_STATUS_INTERNAL_ERROR = 4,
  KEY_STATUS_USABLE_IN_FUTURE = 5,
} OhosKeyStatus;

typedef enum OhosDrmResult {
  OHOS_DRM_RESULT_ERROR = -1,
  OHOS_DRM_RESULT_OK = 0,
} OhosDrmResult;

class SessionId {
 public:
  static std::shared_ptr<SessionId> createPersistentSessionId(
      const std::string& session_id) {
    return std::make_shared<SessionId>(session_id, nullptr, 0);
  }

  static std::shared_ptr<SessionId> createTemporarySessionId(
      const std::string& session_id) {
    return std::make_shared<SessionId>(session_id, nullptr, 0);
  }

  SessionId(std::string eme_id,
            const uint8_t* key_set_id,
            int32_t key_set_id_len)
      : emeId_(eme_id), keySetIdLen_(key_set_id_len) {
    if (key_set_id != nullptr && key_set_id_len > 0) {
      if (key_set_id_len > MAX_KEY_SET_ID_LEN) {
        LOG(ERROR)
            << __func__
            << " [WiseplayDRM] SessionId: keySetId_ initialization failed!";
      } else {
        memcpy(keySetId_, key_set_id, key_set_id_len);
      }
    }
  }

  std::string EmeId() { return emeId_; }

  unsigned char* KeySetId() { return keySetId_; }

  int32_t KeySetIdLen() { return keySetIdLen_; }

  void SetKeySetId(unsigned char* key_set_id, int32_t key_set_id_len) {
    if (key_set_id_len > MAX_KEY_SET_ID_LEN) {
      LOG(ERROR) << " [WiseplayDRM] key_set_id_len error!";
      return;
    }
    if (key_set_id == nullptr || key_set_id_len <= 0) {
      (void)memset(keySetId_, 0, sizeof(keySetId_));
    } else {
      if (memcpy(keySetId_, key_set_id, key_set_id_len) != 0) {
        LOG(ERROR) << " [WiseplayDRM] memcpy failed!";
        return;
      }
    }
    keySetIdLen_ = key_set_id_len;
  }

 private:
  std::string emeId_;
  uint8_t keySetId_[MAX_KEY_SET_ID_LEN];
  int32_t keySetIdLen_;
};

class SessionInfo {
 public:
  SessionInfo(std::shared_ptr<SessionId> session_id,
              std::string mime_type,
              int32_t key_type)
      : session_id_(session_id), mime_type_(mime_type), keyType_(key_type) {}

  std::string MimeType() { return mime_type_; }

  int32_t KeyType() { return keyType_; }

  void SetKeyType(int32_t key_type) { keyType_ = key_type; }

  std::shared_ptr<SessionId> GetSessionId() { return session_id_; }

 private:
  std::shared_ptr<SessionId> session_id_;
  std::string mime_type_;
  int32_t keyType_;
};

class DrmCallbackImpl {
 public:
  DrmCallbackImpl(OhosDrmCallback* callback);
  ~DrmCallbackImpl() = default;

  void OnSessionMessage(const std::string& session_id,
                        int32_t& type,
                        const std::vector<uint8_t>& message);
  void OnProvisionRequest(const std::string& default_url,
                          const std::string& request_data);
  void OnProvisioningComplete(bool success);
  void OnMediaKeySessionReady(void* session);
  void OnPromiseRejected(uint32_t promise_id, const std::string& error_message);
  void OnPromiseResolved(uint32_t promise_id);
  void OnPromiseResolvedWithSession(uint32_t promise_id,
                                    const std::string& session_id);
  void OnSessionClosed(const std::string& session_id);

  void OnSessionKeysChange(const std::string& session_id,
                           const std::vector<std::string>& key_id_array,
                           const std::vector<uint32_t>& status_array,
                           bool has_additional_usable_key,
                           bool is_key_release);

  void OnSessionExpirationUpdate(const std::string& session_id,
                                 uint64_t expiration_time);

  void OnStorageProvisioned();
  void OnStorageSaveInfo(const std::vector<uint8_t>& ket_set_id,
                         const std::string& mime_type,
                         const std::string& session_id,
                         int32_t key_type);
  void OnStorageLoadInfo(const std::string& session_id);
  void OnStorageClearInfoForKeyRelease(const std::string& session_id);
  void OnStorageClearInfoForLoadFail(const std::string& session_id);

  void AddEmeId(const std::string& eme_id, bool is_release);
  void RemoveEmeId(const std::string& eme_id);
  std::unordered_map<std::string, bool>& EmeIdStatusMap();

 private:
  OhosDrmCallback* drm_callback_;
  std::unordered_map<std::string, bool> eme_id_status_map_;
};

class OhosDrmAdapter {
 public:
  OhosDrmAdapter() = default;
  ~OhosDrmAdapter();

  static std::unique_ptr<OhosDrmAdapter> CreateDrmAdapter();

  static Drm_ErrCode SystemCallBackWithObj(MediaKeySystem* media_key_system,
                                           DRM_EventType event_type,
                                           uint8_t* info,
                                           int32_t info_len,
                                           char* extra);
  static Drm_ErrCode SessoinEventCallBackWithObj(
      MediaKeySession* media_key_sessoin,
      DRM_EventType event_type,
      uint8_t* info,
      int32_t info_len,
      char* extra);
  static Drm_ErrCode SessoinKeyChangeCallBackWithObj(
      MediaKeySession* media_key_sessoin,
      DRM_KeysInfo* keys_info,
      bool new_keys_available);
  static Drm_ErrCode SystemCallBack(DRM_EventType event_type,
                                    uint8_t* info,
                                    int32_t info_len,
                                    char* extra);

  bool IsSupported(const std::string& name);
  bool IsSupported(const std::string& name, const std::string& mime_type);
  bool IsSupported(const std::string& name,
                   const std::string& mime_type,
                   int32_t level);
  std::vector<uint8_t> GetUUID(const std::string& name);
  void StorageProvisionedResult(bool result);
  void StorageSaveInfoResult(bool result, int32_t type);
  void StorageLoadInfoResult(const std::string& session_id,
                             const std::vector<uint8_t>& key_set_id,
                             const std::string& mime_type,
                             uint32_t key_type);

  void StorageClearInfoResult(bool result, int32_t type);

  int32_t CreateKeySystem(const std::string& name, int32_t security_level);
  int32_t ProcessKeySystemResponse(const std::string& response,
                                   bool is_response_received);
  int32_t GenerateMediaKeyRequest(const std::string& session_id,
                                  int32_t type,
                                  int32_t init_data_len,
                                  const std::vector<uint8_t>& init_data,
                                  const std::string& mime_type,
                                  uint32_t promise_id);
  int32_t UpdateSession(uint32_t promise_id,
                        const std::string& session_id,
                        std::vector<uint8_t> response);
  int32_t CloseSession(uint32_t promise_id, const std::string& session_id);
  int32_t RemoveSession(uint32_t promise_id, const std::string& session_id);
  int32_t LoadSession(uint32_t promise_id, const std::string& session_id);
  int32_t ReleaseMediaKeySystem();
  int32_t ReleaseMediaKeySession();

  int32_t SetConfigurationString(const std::string& config_name,
                                 const std::string& value);
  int32_t GetConfigurationString(const std::string& config_name,
                                 char* value,
                                 int32_t value_len);
  int32_t SetConfigurationByteArray(const std::string& config_name,
                                    const uint8_t* value,
                                    int32_t value_len);
  int32_t GetConfigurationByteArray(const std::string& config_name,
                                    uint8_t* value,
                                    int32_t* value_len);
  int32_t GetMaxContentProtectionLevel(int32_t& level);
  int32_t GetCertificateStatus(int32_t& cert_status);
  int32_t RegisterDrmCallback(OhosDrmCallback* callback);
  int32_t ClearMediaKeys();
  int32_t GetSecurityLevel();
  int32_t RequireSecureDecoderModule(const std::string& mime_type,
                                     bool& status);

 private:
  int32_t CreateMediaKeySession();
  void PutSessionInfo(std::shared_ptr<SessionId> session_id,
                      const std::string& mime_type,
                      int32_t type);
  std::shared_ptr<SessionInfo> GetSessionInfo(
      std::shared_ptr<SessionId> session_id);
  void RemoveSessionInfo(std::shared_ptr<SessionId> session_id);
  void LoadSessionInfo(const std::string& eme_id);
  void LoadSessionWithLoadedStorage(std::shared_ptr<SessionId> session_id,
                                    uint32_t promise_id);
  void UpdateSessionResult(bool is_key_release,
                           std::shared_ptr<SessionId> session_id,
                           unsigned char* media_key_id,
                           int32_t media_key_id_len);

  void SetKeyType(std::shared_ptr<SessionId> session_id, int32_t key_type);
  void SetKeySetId(std::shared_ptr<SessionId> session_id,
                   unsigned char* media_key_id,
                   int32_t media_key_id_len);

  std::shared_ptr<SessionId> GetSessionIdByEmeId(const std::string& eme_id);

  void ClearPersistentSessionInfoFroKeyRelease(
      std::shared_ptr<SessionId> session_id);
  void ClearPersistentSessionInfoForLoadFail(
      std::shared_ptr<SessionId> session_id);

  void HandleKeyUpdatedCallback(uint32_t promise_id, bool result);

  static void OnSessionExpirationUpdate(MediaKeySession* drm_key_sessoin,
                                        uint8_t* info,
                                        int32_t info_len);

  static std::unordered_map<MediaKeySystem*, std::shared_ptr<DrmCallbackImpl>>
      media_key_system_callback_map_;
  static std::unordered_map<MediaKeySession*, std::shared_ptr<DrmCallbackImpl>>
      media_key_session_callback_map_;

  MediaKeySystem* drm_key_system_ = nullptr;
  MediaKeySession* drm_key_session_ = nullptr;
  DRM_ContentProtectionLevel content_protection_level_ =
      CONTENT_PROTECTION_LEVEL_UNKNOWN;
  std::shared_ptr<DrmCallbackImpl> callback_ = nullptr;

  std::unordered_map<std::string, std::shared_ptr<SessionInfo>>
      eme_session_info_map_;
  uint32_t remove_session_promise_id_ = 0;
  uint32_t update_session_promiseId_ = 0;
  uint32_t load_session_promiseId_ = 0;
  std::string release_eme_id_ = "";
};
}  // namespace media
#endif  // MEDIA_BASE_OHOS_OHOS_DRM_ADAPTER_H_

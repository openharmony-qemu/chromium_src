// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_bridge.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>

#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/rand_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/sys_byteorder.h"
#include "base/system/sys_info.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/cdm_key_information.h"
#include "media/base/logging_override_if_enabled.h"
#include "media/base/media_drm_key_type.h"
#include "media/base/media_switches.h"
#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#include "media/base/ohos/ohos_media_drm_bridge_delegate.h"
#include "media/base/provision_fetcher.h"
#include "media/cdm/clear_key_cdm_common.h"
#include "third_party/widevine/cdm/widevine_cdm_common.h"
#include "third_party/wiseplay/cdm/wiseplay_cdm_common.h"

namespace media {

constexpr int32_t MEDIA_KEY_REQUEST_TYPE_UNKNOWN = 0;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_INITIAL = 1;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_RENEWAL = 2;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_RELEASE = 3;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_NONE = 4;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_UPDATE = 5;
constexpr size_t kHexStringOffSet = 2;
constexpr int32_t kSessionIdLength = 16;
constexpr double kMsInSecond = 1000.0;

namespace {

enum class RequestType : uint32_t {
  REQUEST_TYPE_INITIAL = 0,
  REQUEST_TYPE_RENEWAL = 1,
  REQUEST_TYPE_RELEASE = 2,
};

enum class KeyStatus : uint32_t {
  KEY_STATUS_USABLE = 0,
  KEY_STATUS_EXPIRED = 1,
  KEY_STATUS_OUTPUT_NOT_ALLOWED = 2,
  KEY_STATUS_PENDING = 3,
  KEY_STATUS_INTERNAL_ERROR = 4,
  KEY_STATUS_USABLE_IN_FUTURE = 5,
};

std::string ConvertInitDataType(media::EmeInitDataType init_data_type) {
  switch (init_data_type) {
    case media::EmeInitDataType::WEBM:
      return "video/webm";
    case media::EmeInitDataType::CENC:
      return "video/mp4";
    case media::EmeInitDataType::KEYIDS:
      return "keyids";
    default:
      NOTREACHED();
      return "unknown";
  }
}

OhosMediaDrmBridge::OhosMediaKeyType ConvertCdmSessionType(
    CdmSessionType session_type) {
  switch (session_type) {
    case CdmSessionType::kTemporary:
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
    case CdmSessionType::kPersistentLicense:
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE;
    default:
      LOG(WARNING) << " [WiseplayDRM] Unsupported session type "
                   << static_cast<int>(session_type)
                   << " using default OHOS_MEDIA_KEY_TYPE_ONLINE";
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
  }
}

MediaDrmKeyType ConvertOHOSMediaKeyType(
    OhosMediaDrmBridge::OhosMediaKeyType key_type) {
  switch (key_type) {
    case OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE:
      return MediaDrmKeyType::OFFLINE;
    case OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE:
      return MediaDrmKeyType::STREAMING;
    case OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE:
      return MediaDrmKeyType::RELEASE;
    default:
      LOG(WARNING) << " [WiseplayDRM] Unsupported ohos media key type "
                   << static_cast<int>(key_type) << " using default STREAMING";
      return MediaDrmKeyType::STREAMING;
  }
}

OhosMediaDrmBridge::OhosMediaKeyType ConvertMediaDrmKeyType(
    MediaDrmKeyType key_type) {
  switch (key_type) {
    case MediaDrmKeyType::OFFLINE:
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE;
    case MediaDrmKeyType::STREAMING:
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
    case MediaDrmKeyType::RELEASE:
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE;
    default:
      LOG(WARNING) << " [WiseplayDRM] Unsupported media key type "
                   << static_cast<int>(key_type)
                   << " using default OHOS_MEDIA_KEY_TYPE_ONLINE";
      return OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
  }
}

CdmMessageType GetMessageType(int32_t request_type) {
  switch (request_type) {
    case MEDIA_KEY_REQUEST_TYPE_INITIAL:
      return CdmMessageType::LICENSE_REQUEST;
    case MEDIA_KEY_REQUEST_TYPE_RENEWAL:
      return CdmMessageType::LICENSE_RENEWAL;
    case MEDIA_KEY_REQUEST_TYPE_RELEASE:
      return CdmMessageType::LICENSE_RELEASE;
    case MEDIA_KEY_REQUEST_TYPE_UNKNOWN:
    case MEDIA_KEY_REQUEST_TYPE_NONE:
    case MEDIA_KEY_REQUEST_TYPE_UPDATE:
    default:
      return CdmMessageType::LICENSE_REQUEST;
  }
}

CdmKeyInformation::KeyStatus ConvertKeyStatus(KeyStatus key_status,
                                              bool is_key_release) {
  switch (key_status) {
    case KeyStatus::KEY_STATUS_USABLE:
      return CdmKeyInformation::USABLE;
    case KeyStatus::KEY_STATUS_EXPIRED:
      return is_key_release ? CdmKeyInformation::RELEASED
                            : CdmKeyInformation::EXPIRED;
    case KeyStatus::KEY_STATUS_OUTPUT_NOT_ALLOWED:
      return CdmKeyInformation::OUTPUT_RESTRICTED;
    case KeyStatus::KEY_STATUS_PENDING:
      return CdmKeyInformation::KEY_STATUS_PENDING;
    case KeyStatus::KEY_STATUS_INTERNAL_ERROR:
      return CdmKeyInformation::INTERNAL_ERROR;
    case KeyStatus::KEY_STATUS_USABLE_IN_FUTURE:
      return CdmKeyInformation::EXPIRED;
  }
}

std::vector<uint8_t> fromHexString(const std::string& hex_string) {
  std::vector<uint8_t> data;
  for (size_t i = 0; i < hex_string.length(); i += kHexStringOffSet) {
    std::string byte_string = hex_string.substr(i, kHexStringOffSet);
    uint8_t byte =
        static_cast<uint8_t>(strtol(byte_string.c_str(), nullptr, 16));
    data.push_back(byte);
  }
  return data;
}

const uint8_t K_WIDEVINE_UUID[16] = {0xED, 0xEF, 0x8B, 0xA9, 0x79, 0xD6,
                                     0x4A, 0xCE, 0xA3, 0xC8, 0x27, 0xDC,
                                     0xD5, 0x1D, 0x21, 0xED};

class OHOSDrmAdapterUtil {
 public:
  static OHOSDrmAdapterUtil& GetInstance();
  OHOSDrmAdapterUtil();
  ~OHOSDrmAdapterUtil() = default;

  UUID GetUUID(const std::string& key_system);
  bool IsSupported(const std::string& name);
  bool IsSupported(const std::string& name, const std::string& mime_type);

 private:
  std::unique_ptr<OhosDrmAdapter> drm_adapter_ = nullptr;
};

OHOSDrmAdapterUtil& OHOSDrmAdapterUtil::GetInstance() {
  static OHOSDrmAdapterUtil ohos_adapter_util;
  return ohos_adapter_util;
}

OHOSDrmAdapterUtil::OHOSDrmAdapterUtil() {
  drm_adapter_ = std::make_unique<OhosDrmAdapter>();
}

UUID OHOSDrmAdapterUtil::GetUUID(const std::string& key_system) {
  if (drm_adapter_) {
    UUID uuid = drm_adapter_->GetUUID(key_system);
    return uuid;
  }
  LOG(WARNING) << "[WiseplayDRM] | OHOSDrmAdapterUtil::" << __FUNCTION__
               << " | drm_adapter_ is null.";
  return UUID();
}

bool OHOSDrmAdapterUtil::IsSupported(const std::string& name) {
  if (drm_adapter_) {
    return drm_adapter_->IsSupported(name);
  }
  return false;
}

bool OHOSDrmAdapterUtil::IsSupported(const std::string& name,
                                     const std::string& mime_type) {
  if (drm_adapter_) {
    return drm_adapter_->IsSupported(name, mime_type);
  }
  return false;
}

class KeySystemManager {
 public:
  KeySystemManager();
  KeySystemManager(const KeySystemManager&) = delete;
  KeySystemManager& operator=(const KeySystemManager&) = delete;

  UUID GetUUID(const std::string& key_system);
  std::vector<std::string> GetPlatformKeySystemNames();
  static KeySystemManager& GetInstance();

 private:
  using KeySystemUuidMap = OhosMediaDrmBridgeClient::KeySystemUuidMap;

  KeySystemUuidMap key_system_uuid_map_;
};

KeySystemManager& KeySystemManager::GetInstance() {
  static KeySystemManager key_system_manager;
  return key_system_manager;
}

KeySystemManager::KeySystemManager() {
  key_system_uuid_map_[kWidevineKeySystem] =
      UUID(K_WIDEVINE_UUID, K_WIDEVINE_UUID + std::size(K_WIDEVINE_UUID));
  UUID uuid_wiseplay =
      OHOSDrmAdapterUtil::GetInstance().GetUUID(kWisePlayKeySystem);
  if (uuid_wiseplay.size() > 0) {
    key_system_uuid_map_[kWisePlayKeySystem] = uuid_wiseplay;
  }

  OhosMediaDrmBridgeClient* client = GetMediaDrmBridgeClient();
  if (client) {
    client->AddKeySystemUUIDMappings(&key_system_uuid_map_);
  } else {
    LOG(WARNING) << "[WiseplayDRM] | KeySystemManager::" << __FUNCTION__
                 << " | OhosMediaDrmBridgeClient is null.";
  }
}

UUID KeySystemManager::GetUUID(const std::string& key_system) {
  KeySystemUuidMap::iterator it = key_system_uuid_map_.find(key_system);
  if (it == key_system_uuid_map_.end()) {
    LOG(WARNING) << "[WiseplayDRM] | KeySystemManager::" << __FUNCTION__
                 << " | key system not found: " << key_system;
    return UUID();
  }
  return it->second;
}

std::vector<std::string> KeySystemManager::GetPlatformKeySystemNames() {
  std::vector<std::string> key_systems;
  for (KeySystemUuidMap::iterator it = key_system_uuid_map_.begin();
       it != key_system_uuid_map_.end(); ++it) {
    if (it->first != kWidevineKeySystem) {
      key_systems.push_back(it->first);
    }
  }
  return key_systems;
}

KeySystemManager& GetKeySystemManager() {
  return KeySystemManager::GetInstance();
}

bool IsKeySystemSupportedWithTypeImpl(const std::string& key_system,
                                      const std::string& container_mime_type) {
  UUID scheme_uuid = GetKeySystemManager().GetUUID(key_system);
  if (scheme_uuid.empty()) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] Cannot get UUID for key system "
                 << key_system;
    return false;
  }

  bool supported = false;
  if (!container_mime_type.empty()) {
    supported = OHOSDrmAdapterUtil::GetInstance().IsSupported(
        key_system, container_mime_type);
  } else {
    supported = OHOSDrmAdapterUtil::GetInstance().IsSupported(key_system);
  }
  return supported;
}
}  // namespace

// static
bool OhosMediaDrmBridge::IsKeySystemSupported(const std::string& key_system) {
  return IsKeySystemSupportedWithTypeImpl(key_system, "");
}

// static
bool OhosMediaDrmBridge::IsPersistentLicenseTypeSupported(
    const std::string& /* key_system */) {
  return true;
}

// static
bool OhosMediaDrmBridge::IsKeySystemSupportedWithType(
    const std::string& key_system,
    const std::string& container_mime_type) {
  DCHECK(!container_mime_type.empty()) << "Call IsKeySystemSupported instead";

  return IsKeySystemSupportedWithTypeImpl(key_system, container_mime_type);
}

// static
std::vector<std::string> OhosMediaDrmBridge::GetPlatformKeySystemNames() {
  return GetKeySystemManager().GetPlatformKeySystemNames();
}

// static
std::vector<uint8_t> OhosMediaDrmBridge::GetUUID(const std::string& key_system) {
  return GetKeySystemManager().GetUUID(key_system);
}

// static
scoped_refptr<OhosMediaDrmBridge> OhosMediaDrmBridge::CreateInternal(
    const std::vector<uint8_t>& scheme_uuid,
    SecurityLevel security_level,
    bool requires_media_crypto,
    std::unique_ptr<OhosMediaDrmStorageBridge> storage,
    CreateFetcherCB create_fetcher_cb,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb) {
  DCHECK(!scheme_uuid.empty());
  scoped_refptr<OhosMediaDrmBridge> media_drm_bridge(new OhosMediaDrmBridge(
      scheme_uuid, security_level, requires_media_crypto, std::move(storage),
      std::move(create_fetcher_cb), session_message_cb, session_closed_cb,
      session_keys_change_cb, session_expiration_update_cb));

  return media_drm_bridge;
}

void OhosMediaDrmBridge::SetServerCertificate(
    const std::vector<uint8_t>& certificate,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << __func__ << " [WiseplayDRM](" << certificate.size() << " bytes)";
  DCHECK(!certificate.empty());
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);
  if (ohos_drm_adapter_) {
    std::string SERVER_CERTIFICATE = "serviceCertificate";
    int32_t ret = ohos_drm_adapter_->SetConfigurationByteArray(SERVER_CERTIFICATE,
        certificate.data(), certificate.size());
    LOG(INFO) << __func__ << " [WiseplayDRM], ret: " << ret;
    if (ret == 0) {
      ResolvePromise(promise_id);
    } else {
      RejectPromise(promise_id, CdmPromise::Exception::TYPE_ERROR,
                    "Set server certificate failed.");
    }
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::TYPE_ERROR,
                  "Set server certificate failed.");
  }
}

std::string GenerateSessionId() {
  uint8_t random_bytes[kSessionIdLength];
  base::RandBytes(random_bytes);
  return base::HexEncode(random_bytes);
}

void OhosMediaDrmBridge::CreateSessionAndGenerateRequest(
    CdmSessionType session_type,
    media::EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    std::unique_ptr<media::NewSessionCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());

  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);

  OhosMediaDrmBridgeClient* client = GetMediaDrmBridgeClient();
  std::vector<uint8_t> init_data_from_delegate;
  std::vector<std::string> optional_parameters_from_delegate;
  if (client) {
    OhosMediaDrmBridgeDelegate* delegate =
        client->GetMediaDrmBridgeDelegate(scheme_uuid_);
    if (delegate) {
      if (!delegate->OnCreateSession(init_data_type, init_data,
                                     &init_data_from_delegate,
                                     &optional_parameters_from_delegate)) {
        RejectPromise(promise_id, CdmPromise::Exception::TYPE_ERROR,
                      "Invalid init data.");
        return;
      }
    }
  }

  std::string mime_type = ConvertInitDataType(init_data_type);
  uint32_t key_type =
      static_cast<uint32_t>(ConvertCdmSessionType(session_type));

  std::string session_id = GenerateSessionId();
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->GenerateMediaKeyRequest(
        session_id, key_type, init_data_from_delegate.size(),
        init_data_from_delegate, mime_type, promise_id);
  }
}

void OhosMediaDrmBridge::LoadSession(
    CdmSessionType session_type,
    const std::string& session_id,
    std::unique_ptr<media::NewSessionCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(IsPersistentLicenseTypeSupported(""));
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);

  if (session_type != CdmSessionType::kPersistentLicense) {
    RejectPromise(promise_id, CdmPromise::Exception::NOT_SUPPORTED_ERROR,
                  "LoadSession() is only supported for 'persistent-license'.");
    return;
  }

  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->LoadSession(promise_id, session_id);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OhosDrmAdapter is nullptr.");
  }
}

void OhosMediaDrmBridge::UpdateSession(
    const std::string& session_id,
    const std::vector<uint8_t>& response,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());

  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);

  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->UpdateSession(promise_id, session_id, response);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OhosDrmAdapter is nullptr.");
  }
}

void OhosMediaDrmBridge::CloseSession(
    const std::string& session_id,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->CloseSession(promise_id, session_id);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OhosDrmAdapter is nullptr.");
  }
}

void OhosMediaDrmBridge::RemoveSession(
    const std::string& session_id,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->RemoveSession(promise_id, session_id);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OhosDrmAdapter is nullptr.");
  }
}

CdmContext* OhosMediaDrmBridge::GetCdmContext() {
  return this;
}

void OhosMediaDrmBridge::DeleteOnCorrectThread() const {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->DeleteSoon(FROM_HERE, this);
  } else {
    delete this;
  }
}

std::unique_ptr<CallbackRegistration> OhosMediaDrmBridge::RegisterEventCB(
    EventCB event_cb) {
  return event_callbacks_.Register(std::move(event_cb));
}

OhosMediaCryptoContext* OhosMediaDrmBridge::GetOhosMediaCryptoContext() {
  return &media_crypto_context_;
}

bool OhosMediaDrmBridge::IsSecureCodecRequired() {
  if (std::ranges::equal(scheme_uuid_, K_WIDEVINE_UUID)) {
    return SECURITY_LEVEL_1 == GetSecurityLevel();
  }
  if (std::ranges::equal(scheme_uuid_, kWiseplayUuid)) {
    return SECURITY_LEVEL_1 == GetSecurityLevel();
  }
  return false;
}

void OhosMediaDrmBridge::ResolvePromise(uint32_t promise_id) {
  cdm_promise_adapter_.ResolvePromise(promise_id);
}

void OhosMediaDrmBridge::ResolvePromiseWithSession(
    uint32_t promise_id,
    const std::string& session_id) {
  cdm_promise_adapter_.ResolvePromise(promise_id, session_id);
}

void OhosMediaDrmBridge::RejectPromise(uint32_t promise_id,
                                       CdmPromise::Exception exception_code,
                                       const std::string& error_message) {
  cdm_promise_adapter_.RejectPromise(promise_id, exception_code, 0,
                                     error_message);
}

void OhosMediaDrmBridge::SetOHOSMediaCryptoReadyCB(
    OhosMediaCryptoReadyCB media_crypto_ready_cb) {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OhosMediaDrmBridge::SetOHOSMediaCryptoReadyCB,
                       weak_factory_.GetWeakPtr(),
                       std::move(media_crypto_ready_cb)));
    return;
  }
  if (!media_crypto_ready_cb) {
    media_crypto_ready_cb_.Reset();
    return;
  }

  DCHECK(!media_crypto_ready_cb_);
  media_crypto_ready_cb_ = std::move(media_crypto_ready_cb);
  if (!ohos_media_key_session_) {
    LOG(WARNING) << "[WiseplayDRM] | OhosMediaDrmBridge::" << __FUNCTION__
                 << " | ohos_media_key_session_ is null.";
    return;
  }
  std::move(media_crypto_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

std::vector<uint8_t> OhosMediaDrmBridge::GetSchemeUUID() {
  return scheme_uuid_;
}

void OhosMediaDrmBridge::SetOHOSMediaCryptoAndLicenseReadyCB(
    OhosMediaCryptoReadyCB media_crypto_and_license_ready_cb) {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OhosMediaDrmBridge::SetOHOSMediaCryptoAndLicenseReadyCB,
                       weak_factory_.GetWeakPtr(),
                       std::move(media_crypto_and_license_ready_cb)));
    return;
  }
  if (!media_crypto_and_license_ready_cb) {
    media_crypto_and_license_ready_cb_.Reset();
    return;
  }

  DCHECK(!media_crypto_and_license_ready_cb_);
  media_crypto_and_license_ready_cb_ =
      std::move(media_crypto_and_license_ready_cb);

  if (!ohos_media_key_session_) {
    LOG(INFO) << __func__ << " [WiseplayDRM] key session not ready.";
    return;
  }
  if (!is_license_ready_) {
    LOG(INFO) << __func__ << " [WiseplayDRM] license not ready.";
    return;
  }

  std::move(media_crypto_and_license_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

void OhosMediaDrmBridge::OnMediaKeySessionReady(void* session) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OhosMediaDrmBridge::NotifyMediaCryptoReady,
                                weak_factory_.GetWeakPtr(), session));
}

void OhosMediaDrmBridge::OnProvisionRequest(const std::string& default_url,
                                            const std::string& request_data) {
  std::string provision_request_data = request_data;
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OhosMediaDrmBridge::SendProvisioningRequest,
                                weak_factory_.GetWeakPtr(), GURL(default_url),
                                std::move(provision_request_data)));
}

void OhosMediaDrmBridge::OnProvisioningComplete(bool success) {
  DCHECK(provisioning_complete_cb_);
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(std::move(provisioning_complete_cb_), success));
}

void OhosMediaDrmBridge::OnPromiseResolved(uint32_t promise_id) {
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OhosMediaDrmBridge::ResolvePromise,
                                weak_factory_.GetWeakPtr(), promise_id));
}

void OhosMediaDrmBridge::OnPromiseResolvedWithSession(
    uint32_t promise_id,
    const std::string& session_id) {
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OhosMediaDrmBridge::ResolvePromiseWithSession,
                     weak_factory_.GetWeakPtr(), promise_id, session_id));
}

void OhosMediaDrmBridge::OnPromiseRejected(uint32_t promise_id,
                                           const std::string& error_message) {
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OhosMediaDrmBridge::RejectPromise,
                                weak_factory_.GetWeakPtr(), promise_id,
                                CdmPromise::Exception::NOT_SUPPORTED_ERROR,
                                error_message));
}

void OhosMediaDrmBridge::OnStorageProvisionedResult(bool result) {
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageProvisionedResult(result);
  }
}

void OhosMediaDrmBridge::OnStorageProvisioned() {
  if (storage_) {
    storage_->OnProvisioned(
        base::BindOnce(&OhosMediaDrmBridge::OnStorageProvisionedResult,
                       weak_factory_.GetWeakPtr()));
  }
}

void OhosMediaDrmBridge::OnStorageSaveInfoUpdateResult(bool result) {
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageSaveInfoResult(result, -1);
  }
}

void OhosMediaDrmBridge::OnStorageSaveInfoRemoveResult(bool result) {
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageSaveInfoResult(
        result,
        OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE);
  }
}

void OhosMediaDrmBridge::OnStorageSaveInfo(
    const std::vector<uint8_t>& ket_set_id,
    const std::string& mime_type,
    const std::string& session_id,
    int32_t key_type) {
  if (storage_) {
    MediaDrmKeyType media_drm_key_type = ConvertOHOSMediaKeyType(
        static_cast<OhosMediaDrmBridge::OhosMediaKeyType>(key_type));
    if (key_type ==
        OhosMediaDrmBridge::OhosMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE) {
      storage_->OnSaveInfo(
          ket_set_id, mime_type, session_id,
          static_cast<int32_t>(media_drm_key_type),
          base::BindOnce(&OhosMediaDrmBridge::OnStorageSaveInfoRemoveResult,
                         weak_factory_.GetWeakPtr()));
    } else {
      storage_->OnSaveInfo(
          ket_set_id, mime_type, session_id,
          static_cast<int32_t>(media_drm_key_type),
          base::BindOnce(&OhosMediaDrmBridge::OnStorageSaveInfoUpdateResult,
                         weak_factory_.GetWeakPtr()));
    }
  }
}

void OhosMediaDrmBridge::OnStorageLoadInfoResult(
    const std::string& session_id,
    const std::vector<uint8_t>& key_set_id,
    const std::string& mime,
    uint32_t key_type) {
  if (ohos_drm_adapter_) {
    OhosMediaDrmBridge::OhosMediaKeyType ohos_media_key_type =
        ConvertMediaDrmKeyType(static_cast<MediaDrmKeyType>(key_type));
    ohos_drm_adapter_->StorageLoadInfoResult(
        session_id, key_set_id, mime,
        static_cast<uint32_t>(ohos_media_key_type));
  }
}

void OhosMediaDrmBridge::OnStorageLoadInfo(const std::string& session_id) {
  if (storage_) {
    storage_->OnLoadInfo(
        session_id, base::BindOnce(&OhosMediaDrmBridge::OnStorageLoadInfoResult,
                                   weak_factory_.GetWeakPtr()));
  }
}

void OhosMediaDrmBridge::OnStorageClearInfoForKeyReleaseResult(bool result) {
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageClearInfoResult(
        result, OhosClearInfoType::OHOS_KEY_RELEASE);
  }
}

void OhosMediaDrmBridge::OnStorageClearInfoForKeyRelease(
    const std::string& session_id) {
  if (storage_) {
    storage_->OnClearInfo(
        session_id,
        base::BindOnce(
            &OhosMediaDrmBridge::OnStorageClearInfoForKeyReleaseResult,
            weak_factory_.GetWeakPtr()));
  }
}

void OhosMediaDrmBridge::OnStorageClearInfoForLoadFailResult(bool result) {
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageClearInfoResult(
        result, OhosClearInfoType::OHOS_LOAD_FAIL);
  }
}

void OhosMediaDrmBridge::OnStorageClearInfoForLoadFail(
    const std::string& session_id) {
  if (storage_) {
    storage_->OnClearInfo(
        session_id,
        base::BindOnce(&OhosMediaDrmBridge::OnStorageClearInfoForLoadFailResult,
                       weak_factory_.GetWeakPtr()));
  }
}

void OhosMediaDrmBridge::OnSessionMessage(const std::string& session_id,
                                          uint32_t message_type,
                                          const std::vector<uint8_t>& message) {
  CdmMessageType cdm_message_type = GetMessageType(message_type);
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(session_message_cb_, session_id,
                                        cdm_message_type, message));
}

void OhosMediaDrmBridge::OnSessionClosed(const std::string& session_id) {
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(session_closed_cb_, session_id,
                                        CdmSessionClosedReason::kClose));
}

void OhosMediaDrmBridge::OnSessionKeysChange(
    const std::string& session_id,
    const std::vector<std::string>& key_id_array,
    const std::vector<uint32_t>& status_array,
    bool has_additional_usable_key,
    bool is_key_release) {
  CdmKeysInfo cdm_keys_info;
  if (key_id_array.size() == status_array.size()) {
    for (uint32_t i = 0; i < key_id_array.size(); i++) {
      std::string key_id = key_id_array[i];
      uint32_t status = status_array[i];

      CdmKeyInformation::KeyStatus key_status =
          ConvertKeyStatus(static_cast<KeyStatus>(status), is_key_release);
      cdm_keys_info.push_back(std::make_unique<CdmKeyInformation>(
          fromHexString(key_id), key_status, 0));
    }
  }
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(session_keys_change_cb_, session_id,
                     has_additional_usable_key, std::move(cdm_keys_info)));

  if (has_additional_usable_key) {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OhosMediaDrmBridge::OnHasAdditionalUsableKey,
                                  weak_factory_.GetWeakPtr()));
  }
}

void OhosMediaDrmBridge::OnSessionExpirationUpdate(
    const std::string& session_id,
    uint64_t expiry_time_ms) {
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(session_expiration_update_cb_, session_id,
                     base::Time::FromMillisecondsSinceUnixEpoch(static_cast<int64_t>(expiry_time_ms))));
}

void OhosMediaDrmBridge::OnMediaLicenseReady(bool success) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  is_license_ready_ = true;
  if (!media_crypto_and_license_ready_cb_) {
    LOG(WARNING) << __func__ << "  [WiseplayDRM] cb not set.";
    return;
  }

  std::move(media_crypto_and_license_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

OhosMediaDrmBridge::OhosMediaDrmBridge(
    const std::vector<uint8_t>& scheme_uuid,
    SecurityLevel security_level,
    bool requires_media_crypto,
    std::unique_ptr<OhosMediaDrmStorageBridge> storage,
    const CreateFetcherCB& create_fetcher_cb,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb)
    : scheme_uuid_(scheme_uuid),
      storage_(std::move(storage)),
      create_fetcher_cb_(create_fetcher_cb),
      session_message_cb_(session_message_cb),
      session_closed_cb_(session_closed_cb),
      session_keys_change_cb_(session_keys_change_cb),
      session_expiration_update_cb_(session_expiration_update_cb),
      task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()),
      media_crypto_context_(this),
      ohos_media_key_session_(nullptr) {
  ohos_drm_adapter_ = std::make_unique<OhosDrmAdapter>();
  if (ohos_drm_adapter_) {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

    if (scheme_uuid == GetKeySystemManager().GetUUID(kWidevineKeySystem)) {
      ohos_drm_adapter_->CreateKeySystem(kWidevineKeySystem, security_level);
    } else if (scheme_uuid ==
               GetKeySystemManager().GetUUID(kWisePlayKeySystem)) {
      ohos_drm_adapter_->CreateKeySystem(kWisePlayKeySystem, security_level);
    }
    ohos_drm_adapter_->RegisterDrmCallback(this);
  }
}

OhosMediaDrmBridge::~OhosMediaDrmBridge() {
  DCHECK(task_runner_->BelongsToCurrentThread());
  if (media_crypto_ready_cb_) {
    std::move(media_crypto_ready_cb_).Run(nullptr, false);
  }
  cdm_promise_adapter_.Clear(CdmPromiseAdapter::ClearReason::kDestruction);
}

OhosMediaDrmBridge::SecurityLevel OhosMediaDrmBridge::GetSecurityLevel() {
  OhosMediaDrmBridge::SecurityLevel security_level =
      SecurityLevel::SECURITY_LEVEL_DEFAULT;
  if (ohos_drm_adapter_) {
    int32_t level = ohos_drm_adapter_->GetSecurityLevel();
    security_level = static_cast<OhosMediaDrmBridge::SecurityLevel>(level);
  }
  return security_level;
}

void OhosMediaDrmBridge::NotifyMediaCryptoReady(void* session) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  ohos_media_key_session_ = session;
  if (!media_crypto_ready_cb_) {
    LOG(WARNING) << "[WiseplayDRM] | OhosMediaDrmBridge::" << __FUNCTION__
                 << " | media_crypto_ready_cb_ is null.";
    return;
  }
  std::move(media_crypto_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

void OhosMediaDrmBridge::SendProvisioningRequest(
    const GURL& default_url,
    const std::string& request_data) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(!provision_fetcher_) << "At most one provision request at any time.";
  DCHECK(create_fetcher_cb_);
  provision_fetcher_ = create_fetcher_cb_.Run();
  if (!provision_fetcher_) {
    LOG(ERROR) << __func__ << " [DRM] create fetcher failed.";
    return;
  }
#if BUILDFLAG(ENABLE_WISEPLAY)
  if (std::ranges::equal(scheme_uuid_, kWiseplayUuid)) {
    provision_fetcher_->RetrieveWiseplayCertificate(
        default_url, request_data,
        base::BindOnce(&OhosMediaDrmBridge::ProcessProvisionResponse,
                       weak_factory_.GetWeakPtr()));
    return;
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

  provision_fetcher_->Retrieve(
      default_url, request_data,
      base::BindOnce(&OhosMediaDrmBridge::ProcessProvisionResponse,
                     weak_factory_.GetWeakPtr()));
}

void OhosMediaDrmBridge::ProcessProvisionResponse(bool success,
                                                  const std::string& response) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(provision_fetcher_) << "No provision request pending.";

  provision_fetcher_.reset();

  if (!success) {
    LOG(ERROR)
        << " [WiseplayDRM] Device provision failure: can't get server response";
  }

  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->ProcessKeySystemResponse(response, success);
  }
}

void OhosMediaDrmBridge::OnHasAdditionalUsableKey() {
  DCHECK(task_runner_->BelongsToCurrentThread());
  event_callbacks_.Notify(Event::kHasAdditionalUsableKey);
}

Decryptor* OhosMediaDrmBridge::GetDecryptor() {
  return nullptr;
}
}  // namespace media

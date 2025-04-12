// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_bridge.h"

#include <stddef.h>
#include <memory>
#include <utility>

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/rand_util.h"
#include "base/ranges/algorithm.h"
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
#if defined(OHOS_ENABLE_WISEPLAY)
#include "media/cdm/wiseplay_cdm_common.h"
#endif
#include "ohos_adapter_helper.h"

namespace media {

constexpr int32_t MEDIA_KEY_REQUEST_TYPE_UNKNOWN = 0;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_INITIAL = 1;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_RENEWAL = 2;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_RELEASE = 3;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_NONE = 4;
constexpr int32_t MEDIA_KEY_REQUEST_TYPE_UPDATE = 5;
constexpr size_t HEX_STRING_OFFSET = 2;
constexpr int32_t SESSION_ID_LENGTH = 16;
constexpr double  MS_IN_SECOND = 1000.0;

namespace {

using CreateMediaDrmBridgeCB =
    base::OnceCallback<scoped_refptr<OHOSMediaDrmBridge>(
        const std::string& /* origin_id */)>;

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

OHOSMediaDrmBridge::OHOSMediaKeyType ConvertCdmSessionType(
    CdmSessionType session_type) {
  switch (session_type) {
    case CdmSessionType::kTemporary:
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
    case CdmSessionType::kPersistentLicense:
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE;

    default:
      LOG(WARNING) << "Unsupported session type "
                   << static_cast<int>(session_type);
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
  }
}

MediaDrmKeyType ConvertOHOSMediaKeyType(
    OHOSMediaDrmBridge::OHOSMediaKeyType key_type) {
  switch (key_type) {
    case OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE:
      return MediaDrmKeyType::OFFLINE;
    case OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE:
      return MediaDrmKeyType::STREAMING;
    case OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE:
      return MediaDrmKeyType::RELEASE;
    default:
      LOG(WARNING) << "Unsupported ohos media key type "
                   << static_cast<int>(key_type);
      return MediaDrmKeyType::STREAMING;
  }
}

OHOSMediaDrmBridge::OHOSMediaKeyType ConvertMediaDrmKeyType(
    MediaDrmKeyType key_type) {
  switch (key_type) {
    case MediaDrmKeyType::OFFLINE:
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_OFFLINE;
    case MediaDrmKeyType::STREAMING:
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
    case MediaDrmKeyType::RELEASE:
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE;
    default:
      LOG(WARNING) << "Unsupported media key type "
                   << static_cast<int>(key_type);
      return OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_ONLINE;
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

  NOTREACHED();
  return CdmMessageType::LICENSE_REQUEST;
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

  NOTREACHED();
  return CdmKeyInformation::INTERNAL_ERROR;
}

std::vector<uint8_t> fromHexString(const std::string& hexString) {
  std::vector<uint8_t> data;
  for (size_t i = 0; i < hexString.length(); i += HEX_STRING_OFFSET) {
    std::string byteString = hexString.substr(i, HEX_STRING_OFFSET);
    uint8_t byte =
        static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
    data.push_back(byte);
  }
  return data;
}

const uint8_t kWidevineUuid[16] = {0xED, 0xEF, 0x8B, 0xA9, 0x79, 0xD6,
                                   0x4A, 0xCE, 0xA3, 0xC8, 0x27, 0xDC,
                                   0xD5, 0x1D, 0x21, 0xED};

class OHOSDrmAdapterUtil {
 public:
  static OHOSDrmAdapterUtil& GetInstance();
  OHOSDrmAdapterUtil();
  ~OHOSDrmAdapterUtil() = default;

  UUID GetUUID(const std::string& key_system);
  bool IsSupported(const std::string& name);
  bool IsSupported2(const std::string& name, const std::string& mimeType);

 private:
  std::unique_ptr<OHOS::NWeb::DrmAdapter> drm_adapter_ = nullptr;
};

OHOSDrmAdapterUtil& OHOSDrmAdapterUtil::GetInstance() {
  static OHOSDrmAdapterUtil ohosAdapterUtil;
  return ohosAdapterUtil;
}

OHOSDrmAdapterUtil::OHOSDrmAdapterUtil() {
  drm_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDrmAdapter();
}

UUID OHOSDrmAdapterUtil::GetUUID(const std::string& key_system) {
  if (drm_adapter_) {
    UUID uuid = drm_adapter_->GetUUID(kWidevineKeySystem);
    return uuid;
  }
  return UUID();
}

bool OHOSDrmAdapterUtil::IsSupported(const std::string& name) {
  if (drm_adapter_) {
    return drm_adapter_->IsSupported(name);
  }
  return false;
}

bool OHOSDrmAdapterUtil::IsSupported2(const std::string& name,
                                      const std::string& mimeType) {
  if (drm_adapter_) {
    return drm_adapter_->IsSupported2(name, mimeType);
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

 private:
  using KeySystemUuidMap = OHOSMediaDrmBridgeClient::KeySystemUuidMap;

  KeySystemUuidMap key_system_uuid_map_;
};

KeySystemManager::KeySystemManager() {
  key_system_uuid_map_[kWidevineKeySystem] =
      UUID(kWidevineUuid, kWidevineUuid + std::size(kWidevineUuid));
#if defined(OHOS_ENABLE_WISEPLAY)
  key_system_uuid_map_[kWiseplayKeySystem] =
      UUID(kWiseplayUuid, kWiseplayUuid + std::size(kWiseplayUuid));
#endif
  OHOSMediaDrmBridgeClient* client = GetMediaDrmBridgeClient();
  if (client) {
    client->AddKeySystemUUIDMappings(&key_system_uuid_map_);
  }
}

UUID KeySystemManager::GetUUID(const std::string& key_system) {
  KeySystemUuidMap::iterator it = key_system_uuid_map_.find(key_system);
  if (it == key_system_uuid_map_.end()) {
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

KeySystemManager* GetKeySystemManager() {
  static KeySystemManager* ksm = new KeySystemManager();
  return ksm;
}

bool IsKeySystemSupportedWithTypeImpl(const std::string& key_system,
                                      const std::string& container_mime_type) {
  LOG(INFO) << "[DRM]" << __func__;
  if (key_system.empty()) {
    NOTREACHED();
    return false;
  }

  UUID scheme_uuid = GetKeySystemManager()->GetUUID(key_system);
  if (scheme_uuid.empty()) {
    LOG(ERROR) << "Cannot get UUID for key system " << key_system;
    return false;
  }

  bool supported = false;
  if (!container_mime_type.empty()) {
    supported = OHOSDrmAdapterUtil::GetInstance().IsSupported2(
        key_system, container_mime_type);
  } else {
    supported = OHOSDrmAdapterUtil::GetInstance().IsSupported(key_system);
  }
  return supported;
}
}  // namespace

OHOSDrmCallback::OHOSDrmCallback(OHOSMediaDrmBridge* media_drm_bridge)
    : media_drm_bridge_(media_drm_bridge) {
  DCHECK(media_drm_bridge_);
}

OHOSDrmCallback::~OHOSDrmCallback() {}

void OHOSDrmCallback::OnSessionMessage(const std::string& session_id,
                                       int32_t& type,
                                       const std::vector<uint8_t>& message) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnSessionMessage(session_id, type, message);
  }
}

void OHOSDrmCallback::OnProvisionRequest(const std::string& default_url,
                                         const std::string& request_data) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnProvisionRequest(default_url, request_data);
  }
}

void OHOSDrmCallback::OnProvisioningComplete(bool success) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnProvisioningComplete(success);
  }
}

void OHOSDrmCallback::OnMediaKeySessionReady(void* session) {
  LOG(INFO) << "[DRM]" << __func__;
  if (media_drm_bridge_) {
    media_drm_bridge_->OnOHOSMediaCryptoReady(session);
    LOG(INFO) << "[DRM]" << __func__;
  }
}

void OHOSDrmCallback::OnPromiseRejected(uint32_t promiseId,
                                        const std::string& errorMessage) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnPromiseRejected(promiseId, errorMessage);
  }
}

void OHOSDrmCallback::OnPromiseResolved(uint32_t promiseId) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnPromiseResolved(promiseId);
  }
}

void OHOSDrmCallback::OnPromiseResolvedWithSession(
    uint32_t promiseId,
    const std::string& session_id) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnPromiseResolvedWithSession(promiseId, session_id);
  }
}

void OHOSDrmCallback::OnSessionClosed(const std::string& session_id) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnSessionClosed(session_id);
  }
}

void OHOSDrmCallback::OnSessionKeysChange(
    const std::string& session_id,
    const std::vector<std::string>& keyIdArray,
    const std::vector<uint32_t>& statusArray,
    bool has_additional_usable_key,
    bool is_key_release) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnSessionKeysChange(session_id, keyIdArray, statusArray,
                                           has_additional_usable_key,
                                           is_key_release);
  }
}

void OHOSDrmCallback::OnSessionExpirationUpdate(const std::string& session_id,
                                                uint64_t expiration_time) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnSessionExpirationUpdate(session_id, expiration_time);
  }
}

void OHOSDrmCallback::OnStorageProvisioned() {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnStorageProvisioned();
  }
}

void OHOSDrmCallback::OnStorageSaveInfo(const std::vector<uint8_t>& ketSetId,
                                        const std::string& mimeType,
                                        const std::string& session_id,
                                        int32_t key_type) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnStorageSaveInfo(ketSetId, mimeType, session_id,
                                         key_type);
  }
}

void OHOSDrmCallback::OnStorageLoadInfo(const std::string& session_id) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnStorageLoadInfo(session_id);
  }
}

void OHOSDrmCallback::OnStorageClearInfoForKeyRelease(
    const std::string& session_id) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnStorageClearInfoForKeyRelease(session_id);
  }
}

void OHOSDrmCallback::OnStorageClearInfoForLoadFail(
    const std::string& session_id) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnStorageClearInfoForLoadFail(session_id);
  }
}

void OHOSDrmCallback::OnMediaLicenseReady(bool success) {
  if (media_drm_bridge_) {
    media_drm_bridge_->OnMediaLicenseReady(success);
  }
}

// static
bool OHOSMediaDrmBridge::IsKeySystemSupported(const std::string& key_system) {
  return IsKeySystemSupportedWithTypeImpl(key_system, "");
}

// static
bool OHOSMediaDrmBridge::IsPersistentLicenseTypeSupported(
    const std::string& /* key_system */) {
  return true;
}

// static
bool OHOSMediaDrmBridge::IsKeySystemSupportedWithType(
    const std::string& key_system,
    const std::string& container_mime_type) {
  DCHECK(!container_mime_type.empty()) << "Call IsKeySystemSupported instead";

  return IsKeySystemSupportedWithTypeImpl(key_system, container_mime_type);
}

// static
std::vector<std::string> OHOSMediaDrmBridge::GetPlatformKeySystemNames() {
  return GetKeySystemManager()->GetPlatformKeySystemNames();
}

// static
std::vector<uint8_t> OHOSMediaDrmBridge::GetUUID(
    const std::string& key_system) {
  return GetKeySystemManager()->GetUUID(key_system);
}

// static
scoped_refptr<OHOSMediaDrmBridge> OHOSMediaDrmBridge::CreateInternal(
    const std::vector<uint8_t>& scheme_uuid,
    const std::string& origin_id,
    SecurityLevel security_level,
    bool requires_media_crypto,
    std::unique_ptr<OHOSMediaDrmStorageBridge> storage,
    CreateFetcherCB create_fetcher_cb,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(!scheme_uuid.empty());
  scoped_refptr<OHOSMediaDrmBridge> media_drm_bridge(new OHOSMediaDrmBridge(
      scheme_uuid, origin_id, security_level, requires_media_crypto,
      std::move(storage), std::move(create_fetcher_cb), session_message_cb,
      session_closed_cb, session_keys_change_cb, session_expiration_update_cb));

  return media_drm_bridge;
}

void OHOSMediaDrmBridge::SetServerCertificate(
    const std::vector<uint8_t>& certificate,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  promise->reject(CdmPromise::Exception::NOT_SUPPORTED_ERROR, 0,
                  "SetServerCertificate() is not supported.");
}

std::string GenerateSessionId() {
  char random_bytes[SESSION_ID_LENGTH];
  base::RandBytes(random_bytes, SESSION_ID_LENGTH);
  return base::HexEncode(random_bytes, SESSION_ID_LENGTH);
}

void OHOSMediaDrmBridge::CreateSessionAndGenerateRequest(
    CdmSessionType session_type,
    media::EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    std::unique_ptr<media::NewSessionCdmPromise> promise) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(task_runner_->BelongsToCurrentThread());

  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);

  OHOSMediaDrmBridgeClient* client = GetMediaDrmBridgeClient();
  std::vector<uint8_t> init_data_from_delegate;
  std::vector<std::string> optional_parameters_from_delegate;
  if (client) {
    OHOSMediaDrmBridgeDelegate* delegate =
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
  LOG(INFO) << "[DRM]" << __func__ << ", session_id:" << session_id
            << ", mime_type:" << mime_type;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->GenerateMediaKeyRequest(session_id, key_type,
                                               init_data.size(), init_data,
                                               mime_type, promise_id);
  }
}

void OHOSMediaDrmBridge::LoadSession(
    CdmSessionType session_type,
    const std::string& session_id,
    std::unique_ptr<media::NewSessionCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(IsPersistentLicenseTypeSupported(""));
  LOG(INFO) << "[DRM]" << __func__
            << ", session_type:" << (int32_t)session_type;
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
                  "OHOSDrmAdapter is nullptr.");
  }
}

void OHOSMediaDrmBridge::UpdateSession(
    const std::string& session_id,
    const std::vector<uint8_t>& response,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[DRM]" << __func__ << ", session_id: " << session_id;

  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);

  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->UpdateSession(promise_id, session_id, response);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OHOSDrmAdapter is nullptr.");
  }
}

void OHOSMediaDrmBridge::CloseSession(
    const std::string& session_id,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[DRM]" << __func__;
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->CloseSession(promise_id, session_id);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OHOSDrmAdapter is nullptr.");
  }
}

void OHOSMediaDrmBridge::RemoveSession(
    const std::string& session_id,
    std::unique_ptr<media::SimpleCdmPromise> promise) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[DRM]" << __func__;
  uint32_t promise_id =
      cdm_promise_adapter_.SavePromise(std::move(promise), __func__);
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->RemoveSession(promise_id, session_id);
  } else {
    RejectPromise(promise_id, CdmPromise::Exception::INVALID_STATE_ERROR,
                  "OHOSDrmAdapter is nullptr.");
  }
}

CdmContext* OHOSMediaDrmBridge::GetCdmContext() {
  return this;
}

void OHOSMediaDrmBridge::DeleteOnCorrectThread() const {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->DeleteSoon(FROM_HERE, this);
  } else {
    delete this;
  }
}

std::unique_ptr<CallbackRegistration> OHOSMediaDrmBridge::RegisterEventCB(
    EventCB event_cb) {
  return event_callbacks_.Register(std::move(event_cb));
}

OHOSMediaCryptoContext* OHOSMediaDrmBridge::GetOHOSMediaCryptoContext() {
  return &media_crypto_context_;
}

bool OHOSMediaDrmBridge::IsSecureCodecRequired() {
  if (base::ranges::equal(scheme_uuid_, kWidevineUuid)) {
    return SECURITY_LEVEL_1 == GetSecurityLevel();
  }
#if defined(OHOS_ENABLE_WISEPLAY)
  if (base::ranges::equal(scheme_uuid_, kWiseplayUuid)) {
    return SECURITY_LEVEL_1 == GetSecurityLevel();
  }
#endif
  return false;
}

void OHOSMediaDrmBridge::Provision(
    base::OnceCallback<void(bool)> provisioning_complete_cb) {}

void OHOSMediaDrmBridge::Unprovision() {}

void OHOSMediaDrmBridge::ResolvePromise(uint32_t promise_id) {
  LOG(INFO) << "[DRM]" << __func__;
  cdm_promise_adapter_.ResolvePromise(promise_id);
}

void OHOSMediaDrmBridge::ResolvePromiseWithSession(
    uint32_t promise_id,
    const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__ << ", session_id: " << session_id;
  cdm_promise_adapter_.ResolvePromise(promise_id, session_id);
}

void OHOSMediaDrmBridge::RejectPromise(uint32_t promise_id,
                                       CdmPromise::Exception exception_code,
                                       const std::string& error_message) {
  LOG(INFO) << "[DRM]" << __func__;
  cdm_promise_adapter_.RejectPromise(promise_id, exception_code, 0,
                                     error_message);
}

void OHOSMediaDrmBridge::SetOHOSMediaCryptoReadyCB(
    OHOSMediaCryptoReadyCB media_crypto_ready_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OHOSMediaDrmBridge::SetOHOSMediaCryptoReadyCB,
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
  LOG(INFO) << "[DRM]" << __func__;
  if (!ohos_media_key_session_) {
    LOG(INFO) << "[DRM]" << __func__;
    return;
  }
  std::move(media_crypto_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

std::vector<uint8_t> OHOSMediaDrmBridge::GetSchemeUUID() {
  return scheme_uuid_;
}

void OHOSMediaDrmBridge::SetOHOSMediaCryptoAndLicenseReadyCB(
    OHOSMediaCryptoReadyCB media_crypto_and_license_ready_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OHOSMediaDrmBridge::SetOHOSMediaCryptoAndLicenseReadyCB,
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
    LOG(INFO) << "[DRM]" << __func__ << ", key session not ready.";
    return;
  }
  if (!is_license_ready_) {
    LOG(INFO) << "[DRM]" << __func__ << ", license not ready.";
    return;
  }
  LOG(INFO) << "[DRM]" << __func__;
  is_license_ready_ = false;
  std::move(media_crypto_and_license_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

void OHOSMediaDrmBridge::OnOHOSMediaCryptoReady(void* session) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[DRM]" << __func__ << ", session:" << session;
  ohos_media_key_session_ = session;
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaDrmBridge::NotifyMediaCryptoReady,
                                weak_factory_.GetWeakPtr()));
}

void OHOSMediaDrmBridge::OnProvisionRequest(const std::string& default_url,
                                            const std::string& request_data) {
  LOG(INFO) << "[DRM]" << __func__;
  std::string provision_request_data = request_data;
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaDrmBridge::SendProvisioningRequest,
                                weak_factory_.GetWeakPtr(), GURL(default_url),
                                std::move(provision_request_data)));
}

void OHOSMediaDrmBridge::OnProvisioningComplete(bool success) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(provisioning_complete_cb_);
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(std::move(provisioning_complete_cb_), success));
}

void OHOSMediaDrmBridge::OnPromiseResolved(uint32_t promise_id) {
  LOG(INFO) << "[DRM]" << __func__;
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaDrmBridge::ResolvePromise,
                                weak_factory_.GetWeakPtr(), promise_id));
}

void OHOSMediaDrmBridge::OnPromiseResolvedWithSession(
    uint32_t promise_id,
    const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__;
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSMediaDrmBridge::ResolvePromiseWithSession,
                     weak_factory_.GetWeakPtr(), promise_id, session_id));
}

void OHOSMediaDrmBridge::OnPromiseRejected(uint32_t promise_id,
                                           const std::string& error_message) {
  LOG(INFO) << "[DRM]" << __func__;
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaDrmBridge::RejectPromise,
                                weak_factory_.GetWeakPtr(), promise_id,
                                CdmPromise::Exception::NOT_SUPPORTED_ERROR,
                                error_message));
}

void OHOSMediaDrmBridge::OnStorageProvisionedResult(bool result) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageProvisionedResult(result);
  }
}

void OHOSMediaDrmBridge::OnStorageProvisioned() {
  LOG(INFO) << "[DRM]" << __func__;
  if (storage_) {
    storage_->OnProvisioned(
        base::BindOnce(&OHOSMediaDrmBridge::OnStorageProvisionedResult,
                       weak_factory_.GetWeakPtr()));
  }
}

void OHOSMediaDrmBridge::OnStorageSaveInfoUpdateResult(bool result) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageSaveInfoResult(result, -1);
  }
}

void OHOSMediaDrmBridge::OnStorageSaveInfoRemoveResult(bool result) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageSaveInfoResult(
        result,
        OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE);
  }
}

void OHOSMediaDrmBridge::OnStorageSaveInfo(
    const std::vector<uint8_t>& ket_set_id,
    const std::string& mime_type,
    const std::string& session_id,
    int32_t key_type) {
  LOG(INFO) << "[DRM]" << __func__;
  if (storage_) {
    MediaDrmKeyType media_drm_key_type = ConvertOHOSMediaKeyType(
        static_cast<OHOSMediaDrmBridge::OHOSMediaKeyType>(key_type));
    if (key_type ==
        OHOSMediaDrmBridge::OHOSMediaKeyType::OHOS_MEDIA_KEY_TYPE_RELEASE) {
      LOG(INFO) << "[DRM]" << __func__;
      storage_->OnSaveInfo(
          ket_set_id, mime_type, session_id,
          static_cast<int32_t>(media_drm_key_type),
          base::BindOnce(&OHOSMediaDrmBridge::OnStorageSaveInfoRemoveResult,
                         weak_factory_.GetWeakPtr()));
    } else {
      LOG(INFO) << "[DRM]" << __func__;
      storage_->OnSaveInfo(
          ket_set_id, mime_type, session_id,
          static_cast<int32_t>(media_drm_key_type),
          base::BindOnce(&OHOSMediaDrmBridge::OnStorageSaveInfoUpdateResult,
                         weak_factory_.GetWeakPtr()));
    }
  }
}

void OHOSMediaDrmBridge::OnStorageLoadInfoResult(
    const std::string& session_id,
    const std::vector<uint8_t>& key_set_id,
    const std::string& mime,
    uint32_t key_type) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    OHOSMediaDrmBridge::OHOSMediaKeyType ohos_media_key_type =
        ConvertMediaDrmKeyType(static_cast<MediaDrmKeyType>(key_type));
    ohos_drm_adapter_->StorageLoadInfoResult(
        session_id, key_set_id, mime,
        static_cast<uint32_t>(ohos_media_key_type));
  }
}

void OHOSMediaDrmBridge::OnStorageLoadInfo(const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__;
  if (storage_) {
    storage_->OnLoadInfo(
        session_id, base::BindOnce(&OHOSMediaDrmBridge::OnStorageLoadInfoResult,
                                   weak_factory_.GetWeakPtr()));
  }
}

void OHOSMediaDrmBridge::OnStorageClearInfoForKeyReleaseResult(bool result) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageClearInfoResult(
        result, OHOSClearInfoType::OHOS_KEY_RELEASE);
  }
}

void OHOSMediaDrmBridge::OnStorageClearInfoForKeyRelease(
    const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__;
  if (storage_) {
    storage_->OnClearInfo(
        session_id,
        base::BindOnce(
            &OHOSMediaDrmBridge::OnStorageClearInfoForKeyReleaseResult,
            weak_factory_.GetWeakPtr()));
  }
}

void OHOSMediaDrmBridge::OnStorageClearInfoForLoadFailResult(bool result) {
  LOG(INFO) << "[DRM]" << __func__;
  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->StorageClearInfoResult(
        result, OHOSClearInfoType::OHOS_LOAD_FAIL);
  }
}

void OHOSMediaDrmBridge::OnStorageClearInfoForLoadFail(
    const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__;
  if (storage_) {
    storage_->OnClearInfo(
        session_id,
        base::BindOnce(&OHOSMediaDrmBridge::OnStorageClearInfoForLoadFailResult,
                       weak_factory_.GetWeakPtr()));
  }
}

void OHOSMediaDrmBridge::OnSessionMessage(const std::string& session_id,
                                          uint32_t message_type,
                                          const std::vector<uint8_t>& message) {
  CdmMessageType cdm_message_type = GetMessageType(message_type);
  LOG(INFO) << "[DRM]" << __func__ << ", message_type: " << message_type
            << ", cdm_message_type:" << (uint32_t)cdm_message_type;
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(session_message_cb_, session_id,
                                        cdm_message_type, message));
}

void OHOSMediaDrmBridge::OnSessionClosed(const std::string& session_id) {
  LOG(INFO) << "[DRM]" << __func__;
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(session_closed_cb_, session_id,
                                        CdmSessionClosedReason::kClose));
}

void OHOSMediaDrmBridge::OnSessionKeysChange(
    const std::string& session_id,
    const std::vector<std::string>& keyIdArray,
    const std::vector<uint32_t>& statusArray,
    bool has_additional_usable_key,
    bool is_key_release) {
  LOG(INFO) << "[DRM]" << __func__ << ", has_additional_usable_key: "
            << has_additional_usable_key;
  CdmKeysInfo cdm_keys_info;
  if (keyIdArray.size() == statusArray.size()) {
    for (uint32_t i = 0; i < keyIdArray.size(); i++) {
      std::string key_id = keyIdArray[i];
      uint32_t status = statusArray[i];

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
    LOG(INFO) << "[DRM]" << __func__;
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSMediaDrmBridge::OnHasAdditionalUsableKey,
                                  weak_factory_.GetWeakPtr()));
  }
}

void OHOSMediaDrmBridge::OnSessionExpirationUpdate(
    const std::string& session_id,
    uint64_t expiry_time_ms) {
  LOG(INFO) << "[DRM]" << __func__;
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(session_expiration_update_cb_, session_id,
                     base::Time::FromDoubleT(expiry_time_ms / MS_IN_SECOND)));
}

void OHOSMediaDrmBridge::OnMediaLicenseReady(bool success) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(task_runner_->BelongsToCurrentThread());
  if (!media_crypto_and_license_ready_cb_) {
    is_license_ready_ = true;
    LOG(INFO) << "[DRM]" << __func__ << ", cb not set.";
    return;
  }
  LOG(INFO) << "[DRM]" << __func__ << ", session:" << ohos_media_key_session_;
  is_license_ready_ = false;
  std::move(media_crypto_and_license_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

OHOSMediaDrmBridge::OHOSMediaDrmBridge(
    const std::vector<uint8_t>& scheme_uuid,
    const std::string& origin_id,
    SecurityLevel security_level,
    bool requires_media_crypto,
    std::unique_ptr<OHOSMediaDrmStorageBridge> storage,
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
  LOG(INFO) << "[DRM]" << __func__;
  ohos_drm_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDrmAdapter();
  if (ohos_drm_adapter_) {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    auto drmCallback = std::make_unique<OHOSDrmCallback>(this);
    ohos_drm_adapter_->RegistDrmCallback(std::move(drmCallback));

    if (scheme_uuid == GetKeySystemManager()->GetUUID(kWidevineKeySystem)) {
      ohos_drm_adapter_->CreateKeySystem(kWidevineKeySystem, origin_id,
                                         security_level);
#if defined(OHOS_ENABLE_WISEPLAY)
    } else if (scheme_uuid ==
               GetKeySystemManager()->GetUUID(kWiseplayKeySystem)) {
      LOG(INFO) << "[DRM]" << __func__ << ", Create wiseplay,";
      ohos_drm_adapter_->CreateKeySystem(kWiseplayKeySystem, "",
                                         security_level);
#endif
    }
  }
}

OHOSMediaDrmBridge::~OHOSMediaDrmBridge() {
  DCHECK(task_runner_->BelongsToCurrentThread());
  if (media_crypto_ready_cb_) {
    std::move(media_crypto_ready_cb_).Run(nullptr, false);
  }
  cdm_promise_adapter_.Clear(CdmPromiseAdapter::ClearReason::kDestruction);
}

OHOSMediaDrmBridge::SecurityLevel OHOSMediaDrmBridge::GetSecurityLevel() {
  OHOSMediaDrmBridge::SecurityLevel securityLevel =
      SecurityLevel::SECURITY_LEVEL_DEFAULT;
  if (ohos_drm_adapter_) {
    int32_t level = ohos_drm_adapter_->GetSecurityLevel();
    securityLevel = static_cast<OHOSMediaDrmBridge::SecurityLevel>(level);
  }
  return securityLevel;
}

void OHOSMediaDrmBridge::NotifyMediaCryptoReady() {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(task_runner_->BelongsToCurrentThread());
  if (!media_crypto_ready_cb_) {
    LOG(INFO) << "[DRM]" << __func__ << ", cb is not set.";
    return;
  }
  std::move(media_crypto_ready_cb_)
      .Run(ohos_media_key_session_, IsSecureCodecRequired());
}

void OHOSMediaDrmBridge::SendProvisioningRequest(
    const GURL& default_url,
    const std::string& request_data) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(!provision_fetcher_) << "At most one provision request at any time.";
  DCHECK(create_fetcher_cb_);
  provision_fetcher_ = create_fetcher_cb_.Run();
  if (!provision_fetcher_) {
    LOG(ERROR) << "[DRM]" << __func__ << ", create fetcher failed.";
    return;
  }
#if defined(OHOS_ENABLE_WISEPLAY)
  if (base::ranges::equal(scheme_uuid_, kWiseplayUuid)) {
    LOG(INFO) << "[DRM]" << __func__ << ", RetrieveWiseplayLicense.";
    provision_fetcher_->RetrieveWiseplayLicense(
        default_url, request_data,
        base::BindOnce(&OHOSMediaDrmBridge::ProcessProvisionResponse,
                       weak_factory_.GetWeakPtr()));
    return;
  }
#endif
  LOG(INFO) << "[DRM]" << __func__ << ", Retrieve.";
  provision_fetcher_->Retrieve(
      default_url, request_data,
      base::BindOnce(&OHOSMediaDrmBridge::ProcessProvisionResponse,
                     weak_factory_.GetWeakPtr()));
}

void OHOSMediaDrmBridge::ProcessProvisionResponse(bool success,
                                                  const std::string& response) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK(provision_fetcher_) << "No provision request pending.";

  provision_fetcher_.reset();

  if (!success) {
    LOG(ERROR) << "Device provision failure: can't get server response";
  }

  if (ohos_drm_adapter_) {
    ohos_drm_adapter_->ProcessKeySystemResponse(response, success);
  }
}

void OHOSMediaDrmBridge::OnHasAdditionalUsableKey() {
  DCHECK(task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[DRM]" << __func__;
  event_callbacks_.Notify(Event::kHasAdditionalUsableKey);
}
}  // namespace media

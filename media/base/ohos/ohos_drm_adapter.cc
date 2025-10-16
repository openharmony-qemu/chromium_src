// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_drm_adapter.h"

#include <cstring.h>
#include <unistd.h>
#include <atomic>
#include <clocale>
#include <cstddef>
#include <map>
#include <memory>
#include <sstream>
#include <typeinfo>

#include <multimedia/drm_framework/native_mediakeysession.h>
#include <multimedia/drm_framework/native_mediakeysystem.h>

#include "base/logging.h"
#include "third_party/widevine/cdm/widevine_cdm_common.h"
#include "third_party/wiseplay/cdm/wiseplay_cdm_common.h"

#define SECURITY_LEVEL_1 1
#define SECURITY_LEVEL_3 3
#define SECURITY_LEVEL_UNKNOWN 0
#define INFO_SIZE 8
#define MAX_URL_LENGTH 2048
#define MAX_REQUEST_LENGTH 12288
namespace media {
std::unordered_map<MediaKeySystem*, std::shared_ptr<DrmCallbackImpl>>
    OhosDrmAdapter::media_key_system_callback_map_;
std::unordered_map<MediaKeySession*, std::shared_ptr<DrmCallbackImpl>>
    OhosDrmAdapter::media_key_session_callback_map_;

const std::string kPrivacyMode = "privacyMode";
const std::string kSessionSharing = "sessionSharing";
const std::string kEnable = "enable";
constexpr int32_t kHexOffset = 4;
constexpr uint64_t kMillisecondInSecond = 1000;
constexpr int32_t kExpirationInfoMaxLen = 16;

static std::unordered_map<std::string, uint32_t> KeyStatusMap{
    {"USABLE", KEY_STATUS_USABLE},
    {"EXPIRED", KEY_STATUS_EXPIRED},
    {"OUTPUT_NOT_ALLOWED", KEY_STATUS_OUTPUT_NOT_ALLOWED},
    {"PENDING", KEY_STATUS_PENDING},
    {"INTERNAL_ERROR", KEY_STATUS_INTERNAL_ERROR},
    {"USABLE_IN_FUTURE", KEY_STATUS_USABLE_IN_FUTURE},
};

DRM_ContentProtectionLevel GetContentProtectionLevelFromSecurityLevel(
    int32_t level_data) {
  DRM_ContentProtectionLevel content_protection_level =
      CONTENT_PROTECTION_LEVEL_UNKNOWN;
  switch (level_data) {
    case SECURITY_LEVEL_UNKNOWN:
      content_protection_level = CONTENT_PROTECTION_LEVEL_UNKNOWN;
      break;
    case SECURITY_LEVEL_1:
      content_protection_level = CONTENT_PROTECTION_LEVEL_HW_CRYPTO;
      break;
    case SECURITY_LEVEL_3:
      content_protection_level = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;
      break;
    default:
      break;
  }
  return content_protection_level;
}

int32_t GetSecurityLevelFromContentProtectionLevel(int32_t level_data) {
  int32_t security_level = SECURITY_LEVEL_3;
  switch (level_data) {
    case CONTENT_PROTECTION_LEVEL_UNKNOWN:
      security_level = SECURITY_LEVEL_UNKNOWN;
      break;
    case CONTENT_PROTECTION_LEVEL_SW_CRYPTO:
      security_level = SECURITY_LEVEL_3;
      break;
    case CONTENT_PROTECTION_LEVEL_HW_CRYPTO:
    case CONTENT_PROTECTION_LEVEL_ENHANCED_HW_CRYPTO:
      security_level = SECURITY_LEVEL_1;
      break;
    case CONTENT_PROTECTION_LEVEL_MAX:
    default:
      break;
  }
  return security_level;
}

uint64_t LoadTimestamp(const uint8_t* info, int32_t info_len) {
  uint64_t timestamp = 0;
  int32_t b_size = (info_len - 1) * INFO_SIZE;
  for (int32_t i = 0; i < info_len; ++i) {
    timestamp |= static_cast<uint64_t>(info[i]) << (b_size - i * INFO_SIZE);
  }
  return timestamp;
}

OhosDrmAdapter::~OhosDrmAdapter() {
  if (drm_key_session_ != nullptr) {
    ReleaseMediaKeySession();
  }
  if (drm_key_system_ != nullptr) {
    ReleaseMediaKeySystem();
  }
}

DrmCallbackImpl::DrmCallbackImpl(OhosDrmCallback* callback)
    : drm_callback_(callback) {
  eme_id_status_map_.clear();
}

void DrmCallbackImpl::OnSessionMessage(const std::string& session_id,
                                       int32_t& type,
                                       const std::vector<uint8_t>& message) {
  if (drm_callback_) {
    drm_callback_->OnSessionMessage(session_id, type, message);
  }
}

void DrmCallbackImpl::OnProvisionRequest(const std::string& default_url,
                                         const std::string& request_data) {
  if (drm_callback_) {
    drm_callback_->OnProvisionRequest(default_url, request_data);
  }
}

void DrmCallbackImpl::OnProvisioningComplete(bool success) {
  if (drm_callback_) {
    drm_callback_->OnProvisioningComplete(success);
  }
}

void DrmCallbackImpl::OnMediaKeySessionReady(void* session) {
  if (drm_callback_) {
    drm_callback_->OnMediaKeySessionReady(session);
  }
}

void DrmCallbackImpl::OnPromiseRejected(uint32_t promise_id,
                                        const std::string& error_message) {
  if (drm_callback_) {
    drm_callback_->OnPromiseRejected(promise_id, error_message);
  }
}

void DrmCallbackImpl::OnPromiseResolved(uint32_t promise_id) {
  if (drm_callback_) {
    drm_callback_->OnPromiseResolved(promise_id);
  }
}

void DrmCallbackImpl::OnPromiseResolvedWithSession(
    uint32_t promise_id,
    const std::string& session_id) {
  if (drm_callback_) {
    drm_callback_->OnPromiseResolvedWithSession(promise_id, session_id);
  }
}

void DrmCallbackImpl::OnSessionClosed(const std::string& session_id) {
  if (drm_callback_) {
    drm_callback_->OnSessionClosed(session_id);
  }
}

void DrmCallbackImpl::OnSessionKeysChange(
    const std::string& session_id,
    const std::vector<std::string>& key_id_array,
    const std::vector<uint32_t>& status_array,
    bool has_additional_usable_key,
    bool is_key_release) {
  if (drm_callback_) {
    drm_callback_->OnSessionKeysChange(session_id, key_id_array, status_array,
                                       has_additional_usable_key,
                                       is_key_release);
  }
}

void DrmCallbackImpl::OnSessionExpirationUpdate(const std::string& session_id,
                                                uint64_t expiration_time) {
  if (drm_callback_) {
    drm_callback_->OnSessionExpirationUpdate(session_id, expiration_time);
  }
}

void DrmCallbackImpl::OnStorageProvisioned() {
  if (drm_callback_) {
    drm_callback_->OnStorageProvisioned();
  }
}

void DrmCallbackImpl::OnStorageSaveInfo(const std::vector<uint8_t>& ket_set_id,
                                        const std::string& mime_type,
                                        const std::string& session_id,
                                        int32_t key_type) {
  if (drm_callback_) {
    drm_callback_->OnStorageSaveInfo(ket_set_id, mime_type, session_id,
                                     key_type);
  }
}

void DrmCallbackImpl::OnStorageLoadInfo(const std::string& session_id) {
  if (drm_callback_) {
    drm_callback_->OnStorageLoadInfo(session_id);
  }
}

void DrmCallbackImpl::OnStorageClearInfoForKeyRelease(
    const std::string& session_id) {
  if (drm_callback_) {
    drm_callback_->OnStorageClearInfoForKeyRelease(session_id);
  }
}

void DrmCallbackImpl::OnStorageClearInfoForLoadFail(
    const std::string& session_id) {
  if (drm_callback_) {
    drm_callback_->OnStorageClearInfoForLoadFail(session_id);
  }
}

void DrmCallbackImpl::AddEmeId(const std::string& eme_id, bool is_release) {
  eme_id_status_map_[eme_id] = is_release;
}

void DrmCallbackImpl::RemoveEmeId(const std::string& eme_id) {
  auto iter = eme_id_status_map_.find(eme_id);
  if (iter != eme_id_status_map_.end()) {
    eme_id_status_map_.erase(iter);
  }
}

std::unordered_map<std::string, bool>& DrmCallbackImpl::EmeIdStatusMap() {
  return eme_id_status_map_;
}

///////// OhosDrmAdapter
// static
std::unique_ptr<OhosDrmAdapter> OhosDrmAdapter::CreateDrmAdapter() {
  return std::make_unique<OhosDrmAdapter>();
}

bool OhosDrmAdapter::IsSupported(const std::string& name) {
  if (name.empty()) {
    return false;
  }
  bool is_supported = OH_MediaKeySystem_IsSupported(name.c_str());
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] name: " << name
            << " is_supported: " << is_supported;
  return is_supported;
}

bool OhosDrmAdapter::IsSupported(const std::string& name,
                                 const std::string& mime_type) {
  if (name.empty()) {
    return false;
  }
  if (mime_type.empty()) {
    return false;
  }
  bool is_supported =
      OH_MediaKeySystem_IsSupported2(name.c_str(), mime_type.c_str());
  return is_supported;
}

bool OhosDrmAdapter::IsSupported(const std::string& name,
                                 const std::string& mime_type,
                                 int32_t level) {
  if (name.empty()) {
    return false;
  }
  if (mime_type.empty()) {
    return false;
  }
  bool is_supported = OH_MediaKeySystem_IsSupported3(
      name.c_str(), mime_type.c_str(),
      static_cast<DRM_ContentProtectionLevel>(level));
  return is_supported;
}

std::vector<uint8_t> OhosDrmAdapter::GetUUID(const std::string& name) {
  std::vector<uint8_t> uuid;
  uuid.clear();
  // mainstream DRM keystem are Widevine,FairPlay,PlayReady and Wiseplay. so
  // size of 10 is far enough nowadays
  uint32_t count = 10;
  DRM_MediaKeySystemDescription infos[count];
  // infos is not null so memset should not have error
  (void)memset(infos, 0, sizeof(infos));
  Drm_ErrCode err_no = OH_MediaKeySystem_GetMediaKeySystems(infos, &count);
  if (err_no != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] Get UUID error: " << err_no;
    return uuid;
  }
  for (uint32_t i = 0; i < count; i++) {
    if (name == infos[i].name) {
      uuid.insert(uuid.begin(), infos[i].uuid, infos[i].uuid + DRM_UUID_LEN);
      break;
    }
  }
  return uuid;
}

int32_t OhosDrmAdapter::ReleaseMediaKeySystem() {
  if (drm_key_system_ == nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] drm_key_system_ is nullptr!";
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySystem_Destroy(drm_key_system_);
  drm_key_system_ = nullptr;
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] Failed to release MediaKeySystem.";
  }
  return ret;
}

int32_t OhosDrmAdapter::ReleaseMediaKeySession() {
  if (drm_key_session_ == nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] drm_key_session_ is nullptr!";
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySession_Destroy(drm_key_session_);
  drm_key_session_ = nullptr;
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] Failed to release MediaKeySessoin.";
  }
  return ret;
}

// static
Drm_ErrCode OhosDrmAdapter::SystemCallBackWithObj(
    MediaKeySystem* media_key_system,
    DRM_EventType event_type,
    uint8_t* info,
    int32_t info_len,
    char* extra) {
  LOG(INFO) << __FUNCTION__
            << " [WiseplayDRM] media_key_system: " << media_key_system
            << ", event_type: " << event_type;
  if (media_key_system == nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] error, media_key_system is nullptr.";
    return DRM_ERR_INVALID_VAL;
  }
  auto iter = media_key_system_callback_map_.find(media_key_system);
  if (iter == media_key_system_callback_map_.end()) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] error, media_key_system not found: "
                 << media_key_system;
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = DRM_ERR_OK;
  if (event_type == EVENT_PROVISION_REQUIRED) {
    LOG(ERROR) << __FUNCTION__
               << " [[WiseplayDRM]] This device is not provisioned!";
    unsigned char request[MAX_REQUEST_LENGTH] = {0x00};
    int32_t request_len = MAX_REQUEST_LENGTH;
    char default_url[MAX_URL_LENGTH] = {0x00};
    int32_t default_url_len = MAX_URL_LENGTH;
    ret = OH_MediaKeySystem_GenerateKeySystemRequest(
        media_key_system, request, &request_len, default_url, default_url_len);
    if (ret == DRM_ERR_OK) {
      std::shared_ptr<DrmCallbackImpl> callback = iter->second;
      std::vector<uint8_t> request_data;
      request_data.insert(request_data.begin(), request, request + request_len);
      std::string out;
      out.assign(request_data.begin(), request_data.end());
      if (callback) {
        callback->OnProvisionRequest(std::string(default_url), out);
      } else {
        LOG(WARNING) << __FUNCTION__
                     << " [WiseplayDRM] Provision drm callback is null";
      }
    }
  }
  return ret;
}

void OhosDrmAdapter::OnSessionExpirationUpdate(MediaKeySession* drm_key_sessoin,
                                               uint8_t* info,
                                               int32_t info_len) {
  auto iter = media_key_session_callback_map_.find(drm_key_sessoin);
  if (iter == media_key_session_callback_map_.end()) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] mediaKeySessoin not found.";
    return;
  }

  std::shared_ptr<DrmCallbackImpl> callback = iter->second;
  auto iter_status = callback->EmeIdStatusMap().begin();
  for (; iter_status != callback->EmeIdStatusMap().end(); iter_status++) {
    uint64_t timestamp = 0;
    if (info != nullptr && info_len > 0 && info_len <= kExpirationInfoMaxLen) {
      timestamp = LoadTimestamp(info, info_len);
    }
    callback->OnSessionExpirationUpdate(iter_status->first,
                                        timestamp * kMillisecondInSecond);
  }
}

Drm_ErrCode OhosDrmAdapter::SessoinEventCallBackWithObj(
    MediaKeySession* media_key_sessoin,
    DRM_EventType event_type,
    uint8_t* info,
    int32_t info_len,
    char* extra) {
  switch (event_type) {
    case EVENT_DRM_BASE:
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] EVENT_DRM_BASE";
      break;
    case EVENT_PROVISION_REQUIRED:
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] EVENT_PROVISION_REQUIRED";
      break;
    case EVENT_KEY_REQUIRED:
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] EVENT_KEY_REQUIRED";
      break;
    case EVENT_KEY_EXPIRED:
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] EVENT_KEY_EXPIRED";
      break;
    case EVENT_VENDOR_DEFINED:
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] EVENT_VENDOR_DEFINED";
      break;
    case EVENT_EXPIRATION_UPDATE:
      OnSessionExpirationUpdate(media_key_sessoin, info, info_len);
      break;
    default:
      break;
  }
  return DRM_ERR_OK;
}

std::string ToHexString(const unsigned char* data, size_t length) {
  static const char HEX_DIGITS[] = "0123456789ABCDEF";
  std::string hex_string;
  hex_string.reserve(length + length);
  for (size_t i = 0; i < length; ++i) {
    hex_string.push_back(HEX_DIGITS[data[i] >> kHexOffset]);
    hex_string.push_back(HEX_DIGITS[data[i] & 0x0F]);
  }
  return hex_string;
}

Drm_ErrCode OhosDrmAdapter::SessoinKeyChangeCallBackWithObj(
    MediaKeySession* media_key_sessoin,
    DRM_KeysInfo* keys_info,
    bool new_keys_available) {
  if (keys_info == nullptr) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] keys_info is nullptr.";
    return DRM_ERR_INVALID_VAL;
  }
  if (keys_info->keysInfoCount > 0) {
    for (uint32_t i = 0; i < keys_info->keysInfoCount; i++) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] "
                   << keys_info->statusValue[i];
    }
  }

  auto session_iter = media_key_session_callback_map_.find(media_key_sessoin);
  if (session_iter == media_key_session_callback_map_.end()) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] media_key_sessoin is invalid.";
    return DRM_ERR_INVALID_VAL;
  }

  std::shared_ptr<DrmCallbackImpl> callback = session_iter->second;

  std::vector<std::string> key_id_array;
  std::vector<uint32_t> status_array;
  if (callback->EmeIdStatusMap().size() == 0) {
    LOG(WARNING) << __func__ << " [WiseplayDRM] EmeIdStatusMap is empty";
  }
  for (uint32_t i = 0; i < keys_info->keysInfoCount; i++) {
    std::string status_str = std::string(keys_info->statusValue[i]);
    uint32_t status_code = KEY_STATUS_INTERNAL_ERROR;
    {
      auto iter = KeyStatusMap.find(status_str);
      if (iter != KeyStatusMap.end()) {
        status_code = iter->second;
      }
    }
    std::string key_id_str = ToHexString(keys_info->keyId[i], MAX_KEY_ID_LEN);
    key_id_array.push_back(key_id_str);
    status_array.push_back(status_code);
    if (callback) {
      auto iter = callback->EmeIdStatusMap().begin();
      for (; iter != callback->EmeIdStatusMap().end(); iter++) {
        callback->OnSessionKeysChange(iter->first, key_id_array, status_array,
                                      new_keys_available, iter->second);
      }
    }
  }
  return DRM_ERR_OK;
}

int32_t OhosDrmAdapter::CreateKeySystem(const std::string& name,
                                        int32_t security_level) {
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] KeySystem name: " << name
            << ", level: " << security_level;
  if (name.empty()) {
    return DRM_ERR_INVALID_VAL;
  }

  Drm_ErrCode ret = OH_MediaKeySystem_Create(name.c_str(), &drm_key_system_);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] OhosDrmAdapter::CreateKeySystem failed.";
    return OHOS_DRM_RESULT_ERROR;
  }
  LOG(WARNING)
      << __FUNCTION__
      << " [WiseplayDRM] OH_MediaKeySystem_Create success: drm_key_system_: "
      << drm_key_system_;
  if (name == kWidevineKeySystem) {
    SetConfigurationString(kPrivacyMode, kEnable);
    SetConfigurationString(kSessionSharing, kEnable);
  }

  content_protection_level_ =
      GetContentProtectionLevelFromSecurityLevel(security_level);
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::CreateMediaKeySession() {
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] ";
  if (drm_key_system_ == nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] drm_key_system_ is nullptr!";
    return OHOS_DRM_RESULT_ERROR;
  }
  if (drm_key_session_ != nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] drm_key_session_ already exist.";
    return OHOS_DRM_RESULT_OK;
  }
  Drm_ErrCode ret = DRM_ERR_OK;
  ret = OH_MediaKeySystem_CreateMediaKeySession(
      drm_key_system_, &content_protection_level_, &drm_key_session_);
  if (ret != DRM_ERR_OK) {
    LOG(ERROR)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_CreateMediaKeySession failed: "
        << ret;
    return OHOS_DRM_RESULT_ERROR;
  }
  OH_MediaKeySession_Callback session_callback = {
      SessoinEventCallBackWithObj, SessoinKeyChangeCallBackWithObj};
  media_key_session_callback_map_[drm_key_session_] = callback_;
  ret = OH_MediaKeySession_SetCallback(drm_key_session_, &session_callback);
  if (ret != DRM_ERR_OK) {
    LOG(ERROR) << __FUNCTION__
               << " [WiseplayDRM] OH_MediaKeySession_SetCallback failed: "
               << ret;
    return OHOS_DRM_RESULT_ERROR;
  }
  if (callback_) {
    callback_->OnMediaKeySessionReady(
        reinterpret_cast<OhosMediaKeySession>(drm_key_session_));
  }
  LOG(WARNING) << __FUNCTION__
               << " [WiseplayDRM] Create media session success.";
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::SetConfigurationString(const std::string& config_name,
                                               const std::string& value) {
  if (config_name.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (value.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySystem_SetConfigurationString(
      drm_key_system_, config_name.c_str(), value.c_str());
  if (ret != DRM_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_SetConfigurationString failed: "
        << ret;
    return DRM_ERR_INVALID_VAL;
  }
  return ret;
}

int32_t OhosDrmAdapter::GetConfigurationString(const std::string& config_name,
                                               char* value,
                                               int32_t value_len) {
  if (config_name.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (value == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySystem_GetConfigurationString(
      drm_key_system_, config_name.c_str(), value, value_len);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_GetConfigurationString failed: "
        << ret;
    return DRM_ERR_INVALID_VAL;
  }
  return ret;
}

int32_t OhosDrmAdapter::SetConfigurationByteArray(
    const std::string& config_name,
    const uint8_t* value,
    int32_t value_len) {
  if (config_name.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (value == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  uint8_t* non_const_ptr = const_cast<uint8_t*>(value);
  Drm_ErrCode ret = OH_MediaKeySystem_SetConfigurationByteArray(
      drm_key_system_, config_name.c_str(), non_const_ptr, value_len);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_SetConfigurationByteArray failed: "
        << ret;
    return DRM_ERR_INVALID_VAL;
  }
  return ret;
}

int32_t OhosDrmAdapter::GetConfigurationByteArray(
    const std::string& config_name,
    uint8_t* value,
    int32_t* value_len) {
  if (config_name.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (value == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  if (value_len == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySystem_GetConfigurationByteArray(
      drm_key_system_, config_name.c_str(), value, value_len);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_GetConfigurationByteArray failed: "
        << ret;
    return DRM_ERR_INVALID_VAL;
  }
  return ret;
}

int32_t OhosDrmAdapter::GetMaxContentProtectionLevel(int32_t& level) {
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }

  DRM_ContentProtectionLevel content_protection_level =
      CONTENT_PROTECTION_LEVEL_UNKNOWN;

  Drm_ErrCode ret = OH_MediaKeySystem_GetMaxContentProtectionLevel(
      drm_key_system_, &content_protection_level);
  level = content_protection_level;
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] "
                    "OH_MediaKeySystem_GetMaxContentProtectionLevel failed: "
                 << ret;
  }
  return ret;
}

void OhosDrmAdapter::StorageProvisionedResult(bool result) {
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] result: " << result;
  if (drm_key_session_ == nullptr) {
    if (result) {
      CreateMediaKeySession();
    }
  }
}

void OhosDrmAdapter::StorageSaveInfoResult(bool result, int32_t type) {
  if (!result) {
    if (callback_) {
      callback_->OnPromiseRejected(remove_session_promise_id_,
                                   "Fail to update persistent storage");
    }
    return;
  }
  if (type != MEDIA_KEY_TYPE_RELEASE) {
    HandleKeyUpdatedCallback(update_session_promiseId_, result);
    return;
  }

  if (!drm_key_system_ || !drm_key_session_) {
    return;
  }
  uint8_t release_request[MAX_MEDIA_KEY_REQUEST_DATA_LEN];
  int32_t release_request_len = MAX_MEDIA_KEY_REQUEST_DATA_LEN;
  std::shared_ptr<SessionId> session_id = GetSessionIdByEmeId(release_eme_id_);
  if (session_id == nullptr) {
    if (callback_) {
      callback_->OnPromiseRejected(remove_session_promise_id_,
                                   "Session doesn't exist");
    }
    return;
  }
  Drm_ErrCode ret = OH_MediaKeySession_GenerateOfflineReleaseRequest(
      drm_key_session_, session_id->KeySetId(), session_id->KeySetIdLen(),
      release_request, &release_request_len);
  if (ret != DRM_ERR_OK) {
    if (callback_) {
      callback_->OnPromiseRejected(remove_session_promise_id_,
                                   "Fail to generate key release request");
    }
    return;
  }
  if (callback_) {
    callback_->OnPromiseResolved(remove_session_promise_id_);
  }
  std::vector<uint8_t> request_data;
  request_data.insert(request_data.begin(), release_request,
                      release_request + release_request_len);
  int32_t request_type = static_cast<int32_t>(MEDIA_KEY_TYPE_RELEASE);
  if (callback_) {
    callback_->AddEmeId(session_id->EmeId(), true);
    callback_->OnSessionMessage(release_eme_id_, request_type, request_data);
  }
  return;
}

void OhosDrmAdapter::StorageLoadInfoResult(
    const std::string& eme_id,
    const std::vector<uint8_t>& key_set_id,
    const std::string& mime_type,
    uint32_t key_type) {
  if (key_set_id.size() == 0) {
    if (callback_) {
      callback_->OnPromiseResolvedWithSession(load_session_promiseId_, "");
    }
    return;
  }

  // Loading same persistent license into different sessions isn't
  // supported.
  if (GetSessionIdByEmeId(eme_id) != nullptr) {
    return;
  }

  std::shared_ptr<SessionId> session_id =
      std::make_shared<SessionId>(eme_id, key_set_id.data(), key_set_id.size());
  PutSessionInfo(session_id, mime_type, key_type);
  LoadSessionWithLoadedStorage(session_id, load_session_promiseId_);
}

void OhosDrmAdapter::StorageClearInfoResult(bool result, int32_t type) {
  if (type == OhosClearInfoType::OHOS_KEY_RELEASE) {
    HandleKeyUpdatedCallback(update_session_promiseId_, result);
  } else if (type == OhosClearInfoType::OHOS_LOAD_FAIL) {
    if (!result) {
      LOG(WARNING) << __FUNCTION__
                   << " [WiseplayDRM] Failed to clear persistent storage for "
                      "non-exist license";
    }
    if (callback_) {
      callback_->OnPromiseResolvedWithSession(load_session_promiseId_, "");
    }
  }
}

int32_t OhosDrmAdapter::ProcessKeySystemResponse(const std::string& response,
                                                 bool is_response_received) {
  if (drm_key_system_ == nullptr) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] drm_key_system_ is null.";
    return DRM_ERR_INVALID_VAL;
  }

  bool success = true;
  if (is_response_received) {
    int32_t response_len = response.size() / sizeof(uint8_t);
    std::vector<uint8_t> vec(response_len);
    // vec.length comes from reponse.size, so memcpy should not have problem
    memcpy(vec.data(), response.data(), response.size());
    Drm_ErrCode ret = OH_MediaKeySystem_ProcessKeySystemResponse(
        drm_key_system_, vec.data(), response_len);
    if (ret != DRM_ERR_OK) {
      LOG(WARNING) << __FUNCTION__
                   << " [WiseplayDRM] "
                      "OH_MediaKeySystem_ProcessKeySystemResponse failed: "
                   << ret;
      success = false;
    }
  } else {
    success = false;
  }

  if (!success) {
    return OHOS_DRM_RESULT_ERROR;
  }
  if (callback_) {
    callback_->OnStorageProvisioned();
  }
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::GetCertificateStatus(int32_t& cert_status) {
  if (drm_key_system_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  DRM_CertificateStatus cert = CERT_STATUS_INVALID;
  Drm_ErrCode ret =
      OH_MediaKeySystem_GetCertificateStatus(drm_key_system_, &cert);
  cert_status = cert;
  if (ret != DRM_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_GetCertificateStatus failed: "
        << ret;
  }
  return ret;
}

int32_t OhosDrmAdapter::RegisterDrmCallback(OhosDrmCallback* callback) {
  callback_ = std::make_shared<DrmCallbackImpl>(callback);
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] callback_: " << callback_
            << ", drm_key_system_: " << drm_key_system_;
  media_key_system_callback_map_[drm_key_system_] = callback_;

  Drm_ErrCode ret =
      OH_MediaKeySystem_SetCallback(drm_key_system_, SystemCallBackWithObj);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] OH_MediaKeySystem_SetCallback failed: "
                 << ret;
    return OHOS_DRM_RESULT_ERROR;
  }

  int32_t i_ret = CreateMediaKeySession();
  if (i_ret != 0) {
    LOG(WARNING)
        << __FUNCTION__
        << " [WiseplayDRM] OH_MediaKeySystem_CreateMediaKeySession failed.";
    return OHOS_DRM_RESULT_ERROR;
  }
  return DRM_ERR_OK;
}

void OhosDrmAdapter::UpdateSessionResult(bool is_key_release,
                                         std::shared_ptr<SessionId> session_id,
                                         unsigned char* media_key_id,
                                         int32_t media_key_id_len) {
  if (session_id == nullptr) {
    return;
  }

  std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
  if (info == nullptr) {
    return;
  }

  if (is_key_release) {
    ClearPersistentSessionInfoFroKeyRelease(session_id);
  } else if (info->KeyType() == MEDIA_KEY_TYPE_OFFLINE &&
             media_key_id_len > 0) {
    SetKeySetId(session_id, media_key_id, media_key_id_len);
    if (callback_) {
      callback_->AddEmeId(session_id->EmeId(), false);
    }
  } else {
    HandleKeyUpdatedCallback(update_session_promiseId_, true);
    if (callback_) {
      callback_->AddEmeId(session_id->EmeId(), false);
    }
  }
}

int32_t OhosDrmAdapter::UpdateSession(uint32_t promise_id,
                                      const std::string& eme_id,
                                      std::vector<uint8_t> response) {
  if (drm_key_session_ == nullptr) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] drm_key_session_ is nullptr!";
    return OHOS_DRM_RESULT_ERROR;
  }
  update_session_promiseId_ = promise_id;
  std::shared_ptr<SessionId> session_id = GetSessionIdByEmeId(eme_id);
  if (session_id == nullptr) {
    if (callback_) {
      callback_->OnPromiseRejected(
          promise_id, "Invalid session in updateSession: " + eme_id);
    }
    return OHOS_DRM_RESULT_ERROR;
  }
  std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
  if (info == nullptr) {
    return OHOS_DRM_RESULT_ERROR;
  }
  bool is_key_release = false;
  if (info->KeyType() == MEDIA_KEY_TYPE_RELEASE) {
    is_key_release = true;
  }

  int32_t media_key_id_len = 0;
  unsigned char media_key_id[64] = {0x00};
  if (is_key_release) {
    Drm_ErrCode ret = OH_MediaKeySession_ProcessOfflineReleaseResponse(
        drm_key_session_, session_id->KeySetId(), session_id->KeySetIdLen(),
        response.data(), response.size());
    if (ret != DRM_ERR_OK) {
      if (callback_) {
        callback_->OnPromiseRejected(promise_id, "Update session failed.");
      }
      return OHOS_DRM_RESULT_ERROR;
    }
  } else {
    Drm_ErrCode ret = OH_MediaKeySession_ProcessMediaKeyResponse(
        drm_key_session_, response.data(), response.size(), media_key_id,
        &media_key_id_len);
    if (ret != DRM_ERR_OK) {
      LOG(WARNING) << __FUNCTION__
                   << " [WiseplayDRM] "
                      "OH_MediaKeySession_ProcessMediaKeyResponse failed. ret: "
                   << ret;
      if (callback_) {
        callback_->OnPromiseRejected(promise_id, "Update session failed.");
      }
      return OHOS_DRM_RESULT_ERROR;
    }
  }
  UpdateSessionResult(is_key_release, session_id, media_key_id,
                      media_key_id_len);
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::CloseSession(uint32_t promise_id,
                                     const std::string& eme_id) {
  if (drm_key_system_ == nullptr) {
    if (callback_) {
      callback_->OnPromiseRejected(
          promise_id, "closeSession() called when MediaDrm is null.");
    }
    return OHOS_DRM_RESULT_ERROR;
  }

  std::shared_ptr<SessionId> session_id = GetSessionIdByEmeId(eme_id);
  if (session_id == nullptr) {
    if (callback_) {
      callback_->OnPromiseRejected(
          promise_id, "Invalid session_id in closeSession(): " + eme_id);
    }
    return OHOS_DRM_RESULT_ERROR;
  }

  RemoveSessionInfo(session_id);
  if (callback_) {
    callback_->RemoveEmeId(session_id->EmeId());
    callback_->OnPromiseResolved(promise_id);
    callback_->OnSessionClosed(eme_id);
  }
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::RemoveSession(uint32_t promise_id,
                                      const std::string& eme_id) {
  std::shared_ptr<SessionId> session_id = GetSessionIdByEmeId(eme_id);
  if (session_id == nullptr) {
    if (callback_) {
      callback_->OnPromiseRejected(promise_id, "Session doesn't exist");
    }
    return OHOS_DRM_RESULT_ERROR;
  }

  std::shared_ptr<SessionInfo> session_info = GetSessionInfo(session_id);
  if (session_info == nullptr) {
    callback_->OnPromiseRejected(promise_id, "SessionInfo doesn't exist");
    return OHOS_DRM_RESULT_ERROR;
  }
  if (session_info->KeyType() == MEDIA_KEY_TYPE_ONLINE) {
    callback_->OnPromiseRejected(
        promise_id, "Removing temporary session isn't implemented");
    return OHOS_DRM_RESULT_ERROR;
  }
  remove_session_promise_id_ = promise_id;

  release_eme_id_ = eme_id;
  if (callback_) {
    callback_->AddEmeId(session_id->EmeId(), true);
  }
  SetKeyType(session_id, MEDIA_KEY_TYPE_RELEASE);
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::LoadSession(uint32_t promise_id,
                                    const std::string& session_id) {
  load_session_promiseId_ = promise_id;
  LoadSessionInfo(session_id);
  return OHOS_DRM_RESULT_OK;
}

int32_t OhosDrmAdapter::ClearMediaKeys() {
  if (drm_key_session_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  Drm_ErrCode ret = OH_MediaKeySession_ClearMediaKeys(drm_key_session_);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] OH_MediaKeySession_ClearMediaKeys failed: "
                 << ret;
  }
  return ret;
}

int32_t OhosDrmAdapter::GetSecurityLevel() {
  if (drm_key_session_ == nullptr) {
    return OHOS_DRM_RESULT_ERROR;
  }
  DRM_ContentProtectionLevel level_data = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;
  Drm_ErrCode ret = OH_MediaKeySession_GetContentProtectionLevel(
      drm_key_session_, &level_data);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] "
                    "OH_MediaKeySession_GetContentProtectionLevel failed: "
                 << ret;
    return OHOS_DRM_RESULT_ERROR;
  }
  int32_t security_level = GetSecurityLevelFromContentProtectionLevel(
      static_cast<int32_t>(level_data));
  return security_level;
}

int32_t OhosDrmAdapter::RequireSecureDecoderModule(const std::string& mime_type,
                                                   bool& status) {
  if (mime_type.empty()) {
    return DRM_ERR_INVALID_VAL;
  }
  if (drm_key_session_ == nullptr) {
    return DRM_ERR_INVALID_VAL;
  }
  bool stas = false;
  Drm_ErrCode ret = OH_MediaKeySession_RequireSecureDecoderModule(
      drm_key_session_, mime_type.c_str(), &stas);
  status = stas;
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__
                 << " [WiseplayDRM] "
                    "OH_MediaKeySession_RequireSecureDecoderModule failed: "
                 << ret;
    return DRM_ERR_INVALID_VAL;
  }
  return ret;
}

int32_t OhosDrmAdapter::GenerateMediaKeyRequest(
    const std::string& eme_id,
    int32_t type,
    int32_t init_data_len,
    const std::vector<uint8_t>& init_data,
    const std::string& mime_type,
    uint32_t promise_id) {
  if (!callback_) {
    LOG(ERROR) << __func__ << " [WiseplayDRM] callback not set.";
    return OHOS_DRM_RESULT_ERROR;
  }

  if (drm_key_system_ == nullptr) {
    callback_->OnPromiseRejected(promise_id,
                                 "DrmKeySystem released previously.");
    return OHOS_DRM_RESULT_ERROR;
  }

  if (drm_key_session_ == nullptr) {
    callback_->OnPromiseRejected(promise_id,
                                 "DrmKeySessoin released previously.");
    return OHOS_DRM_RESULT_ERROR;
  }

  std::shared_ptr<SessionId> session_id = nullptr;
  if (type == DRM_MediaKeyType::MEDIA_KEY_TYPE_OFFLINE) {
    session_id = SessionId::createPersistentSessionId(eme_id);
  } else {
    session_id = SessionId::createTemporarySessionId(eme_id);
  }
  callback_->AddEmeId(session_id->EmeId(), false);
  DRM_MediaKeyRequestInfo info;
  DRM_MediaKeyRequest media_key_request;

  info.type = static_cast<DRM_MediaKeyType>(type);
  info.initDataLen = init_data_len;
  info.optionsCount = 0;
  if (mime_type.length() > MAX_MIMETYPE_LEN) {
    LOG(ERROR) << __FUNCTION__
               << " [WiseplayDRM] mime_type.length exceed MAX_MIMETYPE_LEN";
    return OHOS_DRM_RESULT_ERROR;
  }
  memcpy(info.mimeType, mime_type.c_str(), mime_type.length());
  if (init_data.size() > MAX_INIT_DATA_LEN) {
    LOG(ERROR) << __FUNCTION__
               << " [WiseplayDRM] init_data.length exceed MAX_INIT_DATA_LEN";
    return OHOS_DRM_RESULT_ERROR;
  }
  memcpy(info.initData, init_data.data(), init_data.size());
  Drm_ErrCode ret = OH_MediaKeySession_GenerateMediaKeyRequest(
      drm_key_session_, &info, &media_key_request);
  if (ret != DRM_ERR_OK) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] Generate request failed.";
    callback_->OnPromiseRejected(promise_id, "Generate request failed.");
    return OHOS_DRM_RESULT_ERROR;
  }

  int32_t request_type = static_cast<int32_t>(media_key_request.type);
  std::vector<uint8_t> request_data;
  request_data.insert(request_data.begin(), media_key_request.data,
                      media_key_request.data + media_key_request.dataLen);
  callback_->OnPromiseResolvedWithSession(promise_id, eme_id);
  callback_->OnSessionMessage(eme_id, request_type, request_data);
  PutSessionInfo(session_id, mime_type, request_type);
  return OHOS_DRM_RESULT_OK;
}

void OhosDrmAdapter::PutSessionInfo(std::shared_ptr<SessionId> session_id,
                                    const std::string& mime_type,
                                    int32_t type) {
  if (session_id == nullptr) {
    return;
  }
  std::shared_ptr<SessionInfo> info =
      std::make_shared<SessionInfo>(session_id, mime_type, type);
  eme_session_info_map_[session_id->EmeId()] = info;
}

// private
std::shared_ptr<SessionInfo> OhosDrmAdapter::GetSessionInfo(
    std::shared_ptr<SessionId> session_id) {
  if (session_id == nullptr) {
    return nullptr;
  }
  auto iter = eme_session_info_map_.find(session_id->EmeId());
  if (iter == eme_session_info_map_.end()) {
    return nullptr;
  }
  return iter->second;
}

std::shared_ptr<SessionId> OhosDrmAdapter::GetSessionIdByEmeId(
    const std::string& eme_id) {
  auto iter = eme_session_info_map_.find(eme_id);
  if (iter == eme_session_info_map_.end()) {
    return nullptr;
  }
  std::shared_ptr<SessionInfo> info = iter->second;
  if (info != nullptr) {
    return info->GetSessionId();
  }
  return nullptr;
}

void OhosDrmAdapter::RemoveSessionInfo(std::shared_ptr<SessionId> session_id) {
  /**
   * Remove session and related infomration from memory, but doesn't touch
   * persistent storage.
   */
  if (session_id == nullptr) {
    return;
  }
  std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
  if (info != nullptr) {
    eme_session_info_map_.erase(session_id->EmeId());
  }
}

void OhosDrmAdapter::LoadSessionInfo(const std::string& eme_id) {
  if (callback_) {
    callback_->OnStorageLoadInfo(eme_id);
  }
}

void OhosDrmAdapter::LoadSessionWithLoadedStorage(
    std::shared_ptr<SessionId> session_id,
    uint32_t promise_id) {
  if (session_id == nullptr) {
    return;
  }
  std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
  if (info == nullptr) {
    return;
  }
  if (info->KeyType() == MEDIA_KEY_TYPE_RELEASE) {
    if (callback_) {
      callback_->OnPromiseResolvedWithSession(promise_id, session_id->EmeId());
      std::vector<std::string> dummy_key_id;
      std::vector<uint32_t> dummy_status;
      dummy_key_id.push_back("");
      dummy_status.push_back(KEY_STATUS_INTERNAL_ERROR);
      callback_->OnSessionKeysChange(session_id->EmeId(), dummy_key_id,
                                     dummy_status, false, true);
    }
    return;
  }
  if (info->KeyType() != MEDIA_KEY_TYPE_OFFLINE) {
    return;
  }
  if (drm_key_session_ != nullptr) {
    Drm_ErrCode ret = OH_MediaKeySession_RestoreOfflineMediaKeys(
        drm_key_session_, session_id->KeySetId(), session_id->KeySetIdLen());
    if (ret != DRM_ERR_OK) {
      ClearPersistentSessionInfoForLoadFail(session_id);
      return;
    }
    if (callback_) {
      callback_->OnPromiseResolvedWithSession(promise_id, session_id->EmeId());
    }
  }
}

// remove && release
void OhosDrmAdapter::SetKeyType(std::shared_ptr<SessionId> session_id,
                                int32_t key_type) {
  std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
  if (info == nullptr) {
    return;
  }
  info->SetKeyType(key_type);

  if (info->GetSessionId() != nullptr &&
      info->GetSessionId()->KeySetId() != nullptr) {
    std::vector<uint8_t> key_set_id_vec;
    key_set_id_vec.insert(
        key_set_id_vec.begin(), info->GetSessionId()->KeySetId(),
        info->GetSessionId()->KeySetId() + info->GetSessionId()->KeySetIdLen());
    if (callback_) {
      callback_->OnStorageSaveInfo(key_set_id_vec, info->MimeType(),
                                   session_id->EmeId(), key_type);
    }
  }
}

// update
void OhosDrmAdapter::SetKeySetId(std::shared_ptr<SessionId> session_id,
                                 unsigned char* media_key_id,
                                 int32_t media_key_id_len) {
  if (session_id == nullptr) {
    HandleKeyUpdatedCallback(update_session_promiseId_, false);
    return;
  }
  session_id->SetKeySetId(media_key_id, media_key_id_len);
  if (callback_) {
    std::shared_ptr<SessionInfo> info = GetSessionInfo(session_id);
    if (info) {
      std::vector<uint8_t> key_set_id_vec;
      key_set_id_vec.insert(key_set_id_vec.begin(),
                            info->GetSessionId()->KeySetId(),
                            info->GetSessionId()->KeySetId() + info->GetSessionId()->KeySetIdLen());
      callback_->OnStorageSaveInfo(key_set_id_vec, info->MimeType(),
                                   session_id->EmeId(), info->KeyType());
    }
  } else {
    HandleKeyUpdatedCallback(update_session_promiseId_, false);
  }
}

void OhosDrmAdapter::ClearPersistentSessionInfoFroKeyRelease(
    std::shared_ptr<SessionId> session_id) {
  if (session_id != nullptr) {
    session_id->SetKeySetId(nullptr, 0);
    if (callback_) {
      callback_->OnStorageClearInfoForKeyRelease(session_id->EmeId());
    }
  }
}

void OhosDrmAdapter::ClearPersistentSessionInfoForLoadFail(
    std::shared_ptr<SessionId> session_id) {
  if (session_id != nullptr) {
    session_id->SetKeySetId(nullptr, 0);
    if (callback_) {
      callback_->OnStorageClearInfoForLoadFail(session_id->EmeId());
    }
  }
}

void OhosDrmAdapter::HandleKeyUpdatedCallback(uint32_t promise_id,
                                              bool result) {
  if (callback_) {
    if (!result) {
      callback_->OnPromiseRejected(
          promise_id, "failed to update key after response accepted");
      return;
    }
    callback_->OnPromiseResolved(promise_id);
  }
}

Drm_ErrCode OhosDrmAdapter::SystemCallBack(DRM_EventType event_type,
                                           uint8_t* info,
                                           int32_t info_len,
                                           char* extra) {
  return DRM_ERR_OK;
}
}  // namespace media

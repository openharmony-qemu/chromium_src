// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_DRM_CALLBACK_H_
#define MEDIA_BASE_OHOS_OHOS_DRM_CALLBACK_H_

#include <string>
#include <vector>
#include <cstdint>

#include "media/base/media_export.h"

namespace media {
class MEDIA_EXPORT OhosDrmCallback {
public:
  OhosDrmCallback() = default;
  virtual ~OhosDrmCallback() = default;

  virtual void OnSessionMessage(const std::string& session_id, uint32_t type, const std::vector<uint8_t>& message) = 0;

  virtual void OnProvisionRequest(const std::string& default_url, const std::string& request_data) = 0;

  virtual void OnProvisioningComplete(bool success) = 0;

  virtual void OnMediaKeySessionReady(void* session) = 0;

  virtual void OnPromiseRejected(uint32_t promise_id, const std::string& error_message) = 0;

  virtual void OnPromiseResolved(uint32_t promise_id) = 0;

  virtual void OnPromiseResolvedWithSession(uint32_t promise_id, const std::string& session_id) = 0;

  virtual void OnSessionClosed(const std::string& session_id) = 0;

  virtual void OnSessionKeysChange(const std::string& session_id, const std::vector<std::string>& key_id_array,
      const std::vector<uint32_t>& status_array, bool has_additional_usable_key, bool is_key_release) = 0;

  virtual void OnSessionExpirationUpdate(const std::string& session_id, uint64_t expiration_time) = 0;

  virtual void OnStorageProvisioned() = 0;

  virtual void OnStorageSaveInfo(const std::vector<uint8_t>& ket_set_id, const std::string& mime_type,
      const std::string& session_id, int32_t key_type) = 0;

  virtual void OnStorageLoadInfo(const std::string& session_id) = 0;

  virtual void OnStorageClearInfoForKeyRelease(const std::string& session_id) = 0;

  virtual void OnStorageClearInfoForLoadFail(const std::string& session_id) = 0;

  virtual void OnMediaLicenseReady(bool success) = 0;
};
} // namespace media

#endif // MEDIA_BASE_OHOS_OHOS_DRM_CALLBACK_H_
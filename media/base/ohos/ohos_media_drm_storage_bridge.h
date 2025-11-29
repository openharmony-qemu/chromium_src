// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_STORAGE_BRIDGE_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_STORAGE_BRIDGE_H_

#include <memory>
#include <string>

#include "base/functional/callback.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "media/base/media_drm_storage.h"
#include "url/origin.h"

namespace base {
class SingleThreadTaskRunner;
}  // namespace base

namespace media {

class OhosMediaDrmStorageBridge {
 public:
  using InitCB = base::OnceCallback<void(bool)>;
  using SaveResultCB = base::OnceCallback<void(bool, int32_t)>;
  using LoadResultCB = base::OnceCallback<void(const std::string&,
                                               const std::vector<uint8_t>&,
                                               const std::string&,
                                               uint32_t)>;

  OhosMediaDrmStorageBridge();

  OhosMediaDrmStorageBridge(const OhosMediaDrmStorageBridge&) = delete;
  OhosMediaDrmStorageBridge& operator=(const OhosMediaDrmStorageBridge&) =
      delete;

  ~OhosMediaDrmStorageBridge();

  void Initialize(const CreateStorageCB& create_storage_cb, InitCB init_cb);

  const std::string& origin_id() const { return origin_id_; }

  void OnProvisioned(MediaDrmStorage::ResultCB result_cb);
  void OnLoadInfo(std::string session_id, LoadResultCB load_result_cb);
  void OnSaveInfo(const std::vector<uint8_t>& ket_set_id,
                  const std::string& mime_type,
                  const std::string& session_id,
                  int32_t key_type,
                  MediaDrmStorage::ResultCB result_cb);
  void OnClearInfo(std::string session_id, MediaDrmStorage::ResultCB result_cb);

 private:
  void OnInitialized(InitCB init_cb,
                     bool success,
                     const MediaDrmStorage::MediaDrmOriginId& origin_id);
  void OnSessionDataLoaded(
      LoadResultCB load_result_cb,
      const std::string& session_id,
      std::unique_ptr<MediaDrmStorage::SessionData> session_data);

  std::unique_ptr<MediaDrmStorage> impl_;
  std::string origin_id_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtrFactory<OhosMediaDrmStorageBridge> weak_factory_{this};
};

}  // namespace media
#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_STORAGE_BRIDGE_H_
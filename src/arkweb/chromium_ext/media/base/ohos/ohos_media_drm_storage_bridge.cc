// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_storage_bridge.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "base/unguessable_token.h"
#include "media/base/media_drm_key_type.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"

namespace media {

OHOSMediaDrmStorageBridge::OHOSMediaDrmStorageBridge()
    : task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {}

OHOSMediaDrmStorageBridge::~OHOSMediaDrmStorageBridge() = default;

void OHOSMediaDrmStorageBridge::Initialize(
    const CreateStorageCB& create_storage_cb,
    InitCB init_cb) {
  DCHECK(create_storage_cb);
  impl_ = create_storage_cb.Run();

  impl_->Initialize(base::BindOnce(&OHOSMediaDrmStorageBridge::OnInitialized,
                                   weak_factory_.GetWeakPtr(),
                                   std::move(init_cb)));
}

void OHOSMediaDrmStorageBridge::OnProvisioned(
    MediaDrmStorage::ResultCB result_cb) {
  DCHECK(impl_);
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&MediaDrmStorage::OnProvisioned,
                                impl_->AsWeakPtr(), std::move(result_cb)));
}

void OHOSMediaDrmStorageBridge::OnLoadInfo(std::string session_id,
                                           LoadResultCB load_result_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(impl_);
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(
          &MediaDrmStorage::LoadPersistentSession, impl_->AsWeakPtr(),
          session_id,
          base::BindOnce(&OHOSMediaDrmStorageBridge::OnSessionDataLoaded,
                         weak_factory_.GetWeakPtr(), std::move(load_result_cb),
                         session_id)));
}

void OHOSMediaDrmStorageBridge::OnSaveInfo(
    const std::vector<uint8_t>& key_set_id,
    const std::string& mime,
    const std::string& session_id,
    int32_t key_type,
    MediaDrmStorage::ResultCB result_cb) {
  DCHECK(impl_);
  auto media_drm_key_type = static_cast<MediaDrmKeyType>(key_type);
  DCHECK(media_drm_key_type == MediaDrmKeyType::OFFLINE ||
         media_drm_key_type == MediaDrmKeyType::RELEASE);
  std::string mime_type = mime;
  std::vector<uint8_t> save_key_set_id;
  save_key_set_id.assign(key_set_id.begin(), key_set_id.end());
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&MediaDrmStorage::SavePersistentSession,
                                impl_->AsWeakPtr(), session_id,
                                MediaDrmStorage::SessionData(
                                    std::move(save_key_set_id),
                                    std::move(mime_type), media_drm_key_type),
                                std::move(result_cb)));
}

void OHOSMediaDrmStorageBridge::OnClearInfo(
    std::string session_id,
    MediaDrmStorage::ResultCB result_cb) {
  DCHECK(impl_);
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaDrmStorage::RemovePersistentSession,
                     impl_->AsWeakPtr(), session_id, std::move(result_cb)));
}

void OHOSMediaDrmStorageBridge::OnInitialized(
    InitCB init_cb,
    bool success,
    const MediaDrmStorage::MediaDrmOriginId& origin_id) {
  if (!success) {
    DCHECK(!origin_id);
    std::move(init_cb).Run(false);
    return;
  }

  if (origin_id && origin_id.value()) {
    origin_id_ = origin_id->ToString();
  } else {
    DCHECK(origin_id_.empty());
    if (OHOSMediaDrmBridge::IsPerApplicationProvisioningSupported()) {
      std::move(init_cb).Run(false);
      return;
    }
  }

  std::move(init_cb).Run(true);
}

void OHOSMediaDrmStorageBridge::OnSessionDataLoaded(
    LoadResultCB load_result_cb,
    const std::string& session_id,
    std::unique_ptr<MediaDrmStorage::SessionData> session_data) {
  LOG(INFO) << "[DRM]" << __func__;
  if (!session_data) {
    LOG(INFO) << "[DRM]" << __func__;
    std::move(load_result_cb).Run(session_id, std::vector<uint8_t>(), "", 0);
    return;
  }
  LOG(INFO) << "[DRM]" << __func__;
  std::move(load_result_cb)
      .Run(session_id, session_data->key_set_id, session_data->mime_type,
           static_cast<uint32_t>(session_data->key_type));
}

}  // namespace media

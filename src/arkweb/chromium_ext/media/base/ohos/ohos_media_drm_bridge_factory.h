// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_ANDROID_MEDIA_DRM_BRIDGE_FACTORY_H_
#define MEDIA_BASE_ANDROID_MEDIA_DRM_BRIDGE_FACTORY_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "media/base/cdm_factory.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/base/ohos/ohos_media_drm_storage_bridge.h"
#include "media/base/provision_fetcher.h"

namespace media {

struct CdmConfig;

class MEDIA_EXPORT OHOSMediaDrmBridgeFactory final : public CdmFactory {
 public:
  OHOSMediaDrmBridgeFactory(CreateFetcherCB create_fetcher_cb,
                            CreateStorageCB create_storage_cb);

  OHOSMediaDrmBridgeFactory(const OHOSMediaDrmBridgeFactory&) = delete;
  OHOSMediaDrmBridgeFactory& operator=(const OHOSMediaDrmBridgeFactory&) =
      delete;

  ~OHOSMediaDrmBridgeFactory() override;

  void Create(const CdmConfig& cdm_config,
              const SessionMessageCB& session_message_cb,
              const SessionClosedCB& session_closed_cb,
              const SessionKeysChangeCB& session_keys_change_cb,
              const SessionExpirationUpdateCB& session_expiration_update_cb,
              CdmCreatedCB cdm_created_cb) override;

 private:
  void OnStorageInitialized(bool success);
  void CreateMediaDrmBridge(const std::string& origin_id);
  void OnOHOSMediaCryptoReady(void* session, bool requires_secure_video_codec);

  CreateFetcherCB create_fetcher_cb_;
  CreateStorageCB create_storage_cb_;

  std::vector<uint8_t> scheme_uuid_;

  OHOSMediaDrmBridge::SecurityLevel security_level_ =
      OHOSMediaDrmBridge::SECURITY_LEVEL_DEFAULT;

  SessionMessageCB session_message_cb_;
  SessionClosedCB session_closed_cb_;
  SessionKeysChangeCB session_keys_change_cb_;
  SessionExpirationUpdateCB session_expiration_update_cb_;

  using CdmCreatedOnceCB = base::OnceCallback<CdmCreatedCB::RunType>;
  CdmCreatedOnceCB cdm_created_cb_;

  std::unique_ptr<OHOSMediaDrmStorageBridge> storage_;
  scoped_refptr<OHOSMediaDrmBridge> ohos_media_drm_bridge_;

  base::WeakPtrFactory<OHOSMediaDrmBridgeFactory> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_BASE_ANDROID_MEDIA_DRM_BRIDGE_FACTORY_H_

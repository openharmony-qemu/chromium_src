// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_bridge_factory.h"

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "media/base/cdm_config.h"
#include "media/base/content_decryption_module.h"
#include "media/base/key_system_names.h"
#include "media/cdm/clear_key_cdm_common.h"
#include "media/media_buildflags.h"
#include "third_party/widevine/cdm/widevine_cdm_common.h"
#include "third_party/wiseplay/cdm/wiseplay_cdm_common.h"

namespace media {

OhosMediaDrmBridgeFactory::OhosMediaDrmBridgeFactory(
    CreateFetcherCB create_fetcher_cb,
    CreateStorageCB create_storage_cb)
    : create_fetcher_cb_(std::move(create_fetcher_cb)),
      create_storage_cb_(std::move(create_storage_cb)) {
  DCHECK(create_fetcher_cb_);
  DCHECK(create_storage_cb_);
}

OhosMediaDrmBridgeFactory::~OhosMediaDrmBridgeFactory() {
  if (cdm_created_cb_) {
    std::move(cdm_created_cb_).Run(nullptr, media::CreateCdmStatus::kCdmCreationAborted);
  }
}

void OhosMediaDrmBridgeFactory::Create(
    const CdmConfig& cdm_config,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb,
    CdmCreatedCB cdm_created_cb) {
  DCHECK(OhosMediaDrmBridge::IsKeySystemSupported(cdm_config.key_system));
  DCHECK(scheme_uuid_.empty()) << "This factory can only be used once.";

  scheme_uuid_ = OhosMediaDrmBridge::GetUUID(cdm_config.key_system);
  DCHECK(!scheme_uuid_.empty());

  if (cdm_config.key_system == kWidevineKeySystem) {
    security_level_ = cdm_config.use_hw_secure_codecs
                          ? OhosMediaDrmBridge::SECURITY_LEVEL_1
                          : OhosMediaDrmBridge::SECURITY_LEVEL_3;
  } else if (cdm_config.key_system == kWisePlayKeySystem) {
    security_level_ = cdm_config.use_hw_secure_codecs
                          ? OhosMediaDrmBridge::SECURITY_LEVEL_1
                          : OhosMediaDrmBridge::SECURITY_LEVEL_3;
  } else if (media::IsExternalClearKey(cdm_config.key_system)) {
    security_level_ = OhosMediaDrmBridge::SECURITY_LEVEL_DEFAULT;
  } else if (!cdm_config.use_hw_secure_codecs) {
    std::move(cdm_created_cb).Run(nullptr, media::CreateCdmStatus::kUnsupportedKeySystem);
    return;
  }
  session_message_cb_ = session_message_cb;
  session_closed_cb_ = session_closed_cb;
  session_keys_change_cb_ = session_keys_change_cb;
  session_expiration_update_cb_ = session_expiration_update_cb;
  cdm_created_cb_ = std::move(cdm_created_cb);

#if BUILDFLAG(ENABLE_OHOS_MEDIA_DRM_STORAGE)
  storage_ = std::make_unique<OhosMediaDrmStorageBridge>();

  storage_->Initialize(
      create_storage_cb_,
      base::BindOnce(&OhosMediaDrmBridgeFactory::OnStorageInitialized,
                     weak_factory_.GetWeakPtr()));
#else
  CreateMediaDrmBridge();
#endif  // BUILDFLAG(ENABLE_OHOS_MEDIA_DRM_STORAGE)
}

void OhosMediaDrmBridgeFactory::OnStorageInitialized(bool success) {
  DCHECK(storage_);
  if (!success) {
    std::move(cdm_created_cb_).Run(nullptr,  media::CreateCdmStatus::kGetCdmOriginIdFailed);
    return;
  }

  CreateMediaDrmBridge();
}

void OhosMediaDrmBridgeFactory::CreateMediaDrmBridge() {
  DCHECK(!ohos_media_drm_bridge_);
  // Requires MediaCrypto so that it can be used by MediaCodec-based decoders.
  const bool requires_media_crypto = true;

  ohos_media_drm_bridge_ = OhosMediaDrmBridge::CreateInternal(
      scheme_uuid_, security_level_, requires_media_crypto,
      std::move(storage_), create_fetcher_cb_, session_message_cb_,
      session_closed_cb_, session_keys_change_cb_,
      session_expiration_update_cb_);
  if (!ohos_media_drm_bridge_) {
    LOG(WARNING) << __FUNCTION__ << "[WiseplayDRM] OhosMediaDrmBridge creation failed.";
    std::move(cdm_created_cb_)
        .Run(nullptr, media::CreateCdmStatus::kMediaDrmBridgeCreationFailed);
    return;
  }
  ohos_media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      base::BindOnce(&OhosMediaDrmBridgeFactory::OnOHOSMediaCryptoReady,
                     weak_factory_.GetWeakPtr()));
}

void OhosMediaDrmBridgeFactory::OnOHOSMediaCryptoReady(
    void* session,
    bool requires_secure_video_codec) {
  if (!session) {
    ohos_media_drm_bridge_ = nullptr;
    std::move(cdm_created_cb_).Run(nullptr, media::CreateCdmStatus::kMediaCryptoNotAvailable);
    LOG(WARNING) << "[WiseplayDRM] | OhosMediaDrmBridgeFactory::" << __FUNCTION__ << " | MediaCrypto not available.";
    return;
  }
  std::move(cdm_created_cb_).Run(ohos_media_drm_bridge_, media::CreateCdmStatus::kSuccess);
}

}  // namespace media
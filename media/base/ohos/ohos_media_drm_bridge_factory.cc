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
#include "third_party/widevine/cdm/widevine_cdm_common.h"
#if defined(OHOS_ENABLE_WISEPLAY)
#include "media/cdm/wiseplay_cdm_common.h"
#endif

namespace media {

OHOSMediaDrmBridgeFactory::OHOSMediaDrmBridgeFactory(
    CreateFetcherCB create_fetcher_cb,
    CreateStorageCB create_storage_cb)
    : create_fetcher_cb_(std::move(create_fetcher_cb)),
      create_storage_cb_(std::move(create_storage_cb)) {
  DCHECK(create_fetcher_cb_);
  DCHECK(create_storage_cb_);
}

OHOSMediaDrmBridgeFactory::~OHOSMediaDrmBridgeFactory() {
  if (cdm_created_cb_) {
    std::move(cdm_created_cb_).Run(nullptr, "CDM creation aborted");
  }
}

void OHOSMediaDrmBridgeFactory::Create(
    const CdmConfig& cdm_config,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb,
    CdmCreatedCB cdm_created_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(OHOSMediaDrmBridge::IsKeySystemSupported(cdm_config.key_system));
  DCHECK(scheme_uuid_.empty()) << "This factory can only be used once.";

  scheme_uuid_ = OHOSMediaDrmBridge::GetUUID(cdm_config.key_system);
  DCHECK(!scheme_uuid_.empty());

  if (cdm_config.key_system == kWidevineKeySystem) {
    security_level_ = cdm_config.use_hw_secure_codecs
                          ? OHOSMediaDrmBridge::SECURITY_LEVEL_1
                          : OHOSMediaDrmBridge::SECURITY_LEVEL_3;
    LOG(INFO) << "[DRM]" << __func__ << " security_level_: " << security_level_;
#if defined(OHOS_ENABLE_WISEPLAY)
  } else if (cdm_config.key_system == kWiseplayKeySystem) {
    security_level_ = cdm_config.use_hw_secure_codecs
                          ? OHOSMediaDrmBridge::SECURITY_LEVEL_1
                          : OHOSMediaDrmBridge::SECURITY_LEVEL_3;
    LOG(INFO) << "[DRM]" << __func__ << " security_level_: " << security_level_;
#endif
  } else {
    LOG(ERROR) << "[DRM]" << __func__ << ", invalid cdm config " << cdm_config.key_system;
    auto error_message =
        "Invalid cdm config for " + cdm_config.key_system;
    std::move(cdm_created_cb).Run(nullptr, error_message);
    return;
  }
  LOG(INFO) << "[DRM]" << __func__;
  session_message_cb_ = session_message_cb;
  session_closed_cb_ = session_closed_cb;
  session_keys_change_cb_ = session_keys_change_cb;
  session_expiration_update_cb_ = session_expiration_update_cb;
  cdm_created_cb_ = std::move(cdm_created_cb);
  LOG(INFO) << "[DRM]" << __func__;
  storage_ = std::make_unique<OHOSMediaDrmStorageBridge>();

  storage_->Initialize(
      create_storage_cb_,
      base::BindOnce(&OHOSMediaDrmBridgeFactory::OnStorageInitialized,
                     weak_factory_.GetWeakPtr()));
}

void OHOSMediaDrmBridgeFactory::OnStorageInitialized(bool success) {
  DCHECK(storage_);
  if (!success) {
    std::move(cdm_created_cb_).Run(nullptr, "Cannot fetch origin ID");
    return;
  }

  CreateMediaDrmBridge(storage_->origin_id());
}

void OHOSMediaDrmBridgeFactory::CreateMediaDrmBridge(
    const std::string& origin_id) {
  DCHECK(!ohos_media_drm_bridge_);
  LOG(INFO) << "[DRM]" << __func__;
  // Requires MediaCrypto so that it can be used by MediaCodec-based decoders.
  const bool requires_media_crypto = true;

  ohos_media_drm_bridge_ = OHOSMediaDrmBridge::CreateInternal(
      scheme_uuid_, origin_id, security_level_, requires_media_crypto,
      std::move(storage_), create_fetcher_cb_, session_message_cb_,
      session_closed_cb_, session_keys_change_cb_,
      session_expiration_update_cb_);
  LOG(INFO) << "[DRM]" << __func__;
  if (!ohos_media_drm_bridge_) {
    std::move(cdm_created_cb_)
        .Run(nullptr, "OHOSMediaDrmBridge creation failed");
    LOG(INFO) << "[DRM]" << __func__;
    return;
  }

  LOG(INFO) << "[DRM]" << __func__;
  ohos_media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      base::BindOnce(&OHOSMediaDrmBridgeFactory::OnOHOSMediaCryptoReady,
                     weak_factory_.GetWeakPtr()));
}

void OHOSMediaDrmBridgeFactory::OnOHOSMediaCryptoReady(
    void* session,
    bool requires_secure_video_codec) {
  LOG(INFO) << "[DRM]" << __func__;
  if (!session) {
    LOG(INFO) << "[DRM]" << __func__;
    ohos_media_drm_bridge_ = nullptr;
    std::move(cdm_created_cb_).Run(nullptr, "MediaCrypto not available");
    return;
  }
  std::move(cdm_created_cb_).Run(ohos_media_drm_bridge_, "");
  LOG(INFO) << "[DRM]" << __func__;
}

}  // namespace media

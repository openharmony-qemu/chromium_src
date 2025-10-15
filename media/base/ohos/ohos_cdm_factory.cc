// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_cdm_factory.h"

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/bind_post_task.h"
#include "media/base/cdm_config.h"
#include "media/base/content_decryption_module.h"
#include "media/base/key_system_names.h"
#include "media/base/media_switches.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/cdm/aes_decryptor.h"
#include "url/origin.h"

namespace media {

namespace {

void ReportMediaDrmBridgeKeySystemSupport(bool supported) {
  UMA_HISTOGRAM_BOOLEAN("Media.EME.OhosMediaDrmBridge.KeySystemSupport",
                        supported);
}

}  // namespace

OhosCdmFactory::OhosCdmFactory(CreateFetcherCB create_fetcher_cb,
                               CreateStorageCB create_storage_cb)
    : create_fetcher_cb_(std::move(create_fetcher_cb)),
      create_storage_cb_(std::move(create_storage_cb)) {}

OhosCdmFactory::~OhosCdmFactory() {
  weak_factory_.InvalidateWeakPtrs();
  for (auto& pending_creation : pending_creations_) {
    CdmCreatedCB cdm_created_cb = std::move(pending_creation.second.second);
    std::move(cdm_created_cb).Run(nullptr, media::CreateCdmStatus::kCdmFactoryCreationFailed);
  }
}

void OhosCdmFactory::Create(
    const CdmConfig& cdm_config,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb,
    CdmCreatedCB cdm_created_cb) {
  CdmCreatedCB bound_cdm_created_cb =
      base::BindPostTaskToCurrentDefault(std::move(cdm_created_cb));

  if (base::FeatureList::IsEnabled(media::kExternalClearKeyForTesting) &&
      IsExternalClearKey(cdm_config.key_system)) {
    scoped_refptr<ContentDecryptionModule> cdm(
        new AesDecryptor(session_message_cb, session_closed_cb,
                         session_keys_change_cb, session_expiration_update_cb));
    std::move(bound_cdm_created_cb).Run(cdm, media::CreateCdmStatus::kSuccess);
    return;
  }

  if (!OhosMediaDrmBridge::IsKeySystemSupported(cdm_config.key_system)) {
    ReportMediaDrmBridgeKeySystemSupport(false);
    std::move(bound_cdm_created_cb)
        .Run(nullptr, media::CreateCdmStatus::kUnsupportedKeySystem);
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] Key system not supported: " << cdm_config.key_system;
    return;
  }

  ReportMediaDrmBridgeKeySystemSupport(true);

  auto factory = std::make_unique<OhosMediaDrmBridgeFactory>(
      create_fetcher_cb_, create_storage_cb_);
  auto* raw_factory = factory.get();

  creation_id_++;
  auto result = pending_creations_.emplace(
      creation_id_,
      PendingCreation(std::move(factory), std::move(bound_cdm_created_cb)));
  CHECK(result.second);
  raw_factory->Create(cdm_config, session_message_cb, session_closed_cb,
                      session_keys_change_cb, session_expiration_update_cb,
                      base::BindOnce(&OhosCdmFactory::OnCdmCreated,
                                     weak_factory_.GetWeakPtr(), creation_id_));
}

void OhosCdmFactory::OnCdmCreated(
    uint32_t creation_id,
    const scoped_refptr<ContentDecryptionModule>& cdm,
    CreateCdmStatus status) {
  DCHECK(pending_creations_.contains(creation_id));
  CdmCreatedCB cdm_created_cb =
      std::move(pending_creations_[creation_id].second);
  pending_creations_.erase(creation_id);

  std::move(cdm_created_cb).Run(cdm, status);
}

}  // namespace media
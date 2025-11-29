// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_crypto_context_impl.h"

#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "third_party/wiseplay/cdm/wiseplay_cdm_common.h"


namespace media {

OhosMediaCryptoContextImpl::OhosMediaCryptoContextImpl(
    OhosMediaDrmBridge* media_drm_bridge)
    : media_drm_bridge_(media_drm_bridge) {
  DCHECK(media_drm_bridge_);
}

OhosMediaCryptoContextImpl::~OhosMediaCryptoContextImpl() = default;

void OhosMediaCryptoContextImpl::SetOHOSMediaCryptoReadyCB(
    OhosMediaCryptoReadyCB media_crypto_ready_cb) {
  if (!media_drm_bridge_) {
    LOG(ERROR) << "[WiseplayDRM] media_drm_bridge_ is nullptr.";
    return;
  }
#if defined(ENABLE_WISEPLAY)
  std::vector<uint8_t> scheme_uuid = media_drm_bridge_->GetSchemeUUID();
  if (base::ranges::equal(scheme_uuid, kWiseplayUuid)) {
    media_drm_bridge_->SetOHOSMediaCryptoAndLicenseReadyCB(
        std::move(media_crypto_ready_cb));
    return;
  }
#endif
  media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      std::move(media_crypto_ready_cb));
}
}  // namespace media
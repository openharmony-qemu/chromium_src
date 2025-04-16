// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_crypto_context_impl.h"

#include "media/base/ohos/ohos_media_drm_bridge.h"
#if defined(OHOS_ENABLE_WISEPLAY)
#include "media/cdm/wiseplay_cdm_common.h"
#endif

namespace media {

OHOSMediaCryptoContextImpl::OHOSMediaCryptoContextImpl(
    OHOSMediaDrmBridge* media_drm_bridge)
    : media_drm_bridge_(media_drm_bridge) {
  DCHECK(media_drm_bridge_);
}

OHOSMediaCryptoContextImpl::~OHOSMediaCryptoContextImpl() = default;

void OHOSMediaCryptoContextImpl::SetOHOSMediaCryptoReadyCB(
    OHOSMediaCryptoReadyCB media_crypto_ready_cb) {
  LOG(INFO) << "[DRM]" << __func__;
  if (!media_drm_bridge_) {
    LOG(ERROR) << "[DRM]media_drm_bridge_ is nullptr.";
    return;
  }
#if defined(OHOS_ENABLE_WISEPLAY)
  std::vector<uint8_t> scheme_uuid = media_drm_bridge_->GetSchemeUUID();
  if (base::ranges::equal(scheme_uuid, kWiseplayUuid)) {
    LOG(INFO) << "[DRM]" << __func__ << ", Wiseplay.";
    media_drm_bridge_->SetOHOSMediaCryptoAndLicenseReadyCB(
        std::move(media_crypto_ready_cb));
    return;
  }
#endif
  media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      std::move(media_crypto_ready_cb));
}
}  // namespace media

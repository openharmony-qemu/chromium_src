// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_crypto_context_impl.h"

#include "media/base/ohos/ohos_media_drm_bridge.h"

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
  media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      std::move(media_crypto_ready_cb));
}
}  // namespace media

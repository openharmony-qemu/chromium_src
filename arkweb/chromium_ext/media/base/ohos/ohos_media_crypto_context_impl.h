// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_IMPL_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_IMPL_H_

#include "base/memory/raw_ptr.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_crypto_context.h"

namespace media {

class OHOSMediaDrmBridge;

class MEDIA_EXPORT OHOSMediaCryptoContextImpl final
    : public OHOSMediaCryptoContext {
 public:
  explicit OHOSMediaCryptoContextImpl(OHOSMediaDrmBridge* media_drm_bridge);

  OHOSMediaCryptoContextImpl(const OHOSMediaCryptoContextImpl&) = delete;
  OHOSMediaCryptoContextImpl& operator=(const OHOSMediaCryptoContextImpl&) =
      delete;

  ~OHOSMediaCryptoContextImpl() override;

  void SetOHOSMediaCryptoReadyCB(
      OHOSMediaCryptoReadyCB media_crypto_ready_cb) override;

 private:
  const raw_ptr<OHOSMediaDrmBridge> media_drm_bridge_;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_IMPL_H_

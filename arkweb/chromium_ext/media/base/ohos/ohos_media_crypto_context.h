// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_H_

#include <memory>

#include "base/functional/callback.h"
#include "media/base/media_export.h"

namespace media {

class MEDIA_EXPORT OHOSMediaCryptoContext {
 public:
  OHOSMediaCryptoContext() = default;

  OHOSMediaCryptoContext(const OHOSMediaCryptoContext&) = delete;
  OHOSMediaCryptoContext& operator=(const OHOSMediaCryptoContext&) = delete;

  using OHOSMediaCryptoReadyCB =
      base::OnceCallback<void(void* session, bool requires_secure_video_codec)>;

  virtual ~OHOSMediaCryptoContext() = default;

  virtual void SetOHOSMediaCryptoReadyCB(
      OHOSMediaCryptoReadyCB media_crypto_ready_cb) = 0;

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
  virtual void ReleaseInnerResource() = 0;
#endif

  virtual std::vector<uint8_t> GetUUID() = 0;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_CRYPTO_CONTEXT_H_

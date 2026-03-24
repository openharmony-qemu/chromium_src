/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NET_CERT_INTERNAL_TRUST_STORE_OHOS_H_
#define NET_CERT_INTERNAL_TRUST_STORE_OHOS_H_

#include <atomic>

#include "base/containers/span.h"
#include "base/memory/ptr_util.h"
#include "base/memory/scoped_refptr.h"
#include "base/synchronization/lock.h"
#include "net/base/net_export.h"
#include "net/cert/cert_database.h"
#include "net/cert/internal/platform_trust_store.h"
#include "third_party/boringssl/src/pki/trust_store.h"
#include "third_party/boringssl/src/pki/trust_store_in_memory.h"
#include "ohos/adapter/cert_manager/cert_manager_adapter.h"

namespace net {

// TrustStoreOhos is an implementation of bssl::TrustStore which uses the
// OHOS cert systems to find user-added trust anchors for path building. It
// ignores the OHOS builtin trust anchors.
class NET_EXPORT TrustStoreOhos : public PlatformTrustStore {
 public:
  TrustStoreOhos();
  ~TrustStoreOhos();
  TrustStoreOhos(const TrustStoreOhos& other) = delete;
  TrustStoreOhos& operator=(const TrustStoreOhos& other) = delete;

  // Load settings from OHOS.
  void Initialize();

  // bssl::TrustStore:
  void SyncGetIssuersOf(const bssl::ParsedCertificate* cert,
                        bssl::ParsedCertificateList* issuers) override;
  bssl::CertificateTrust GetTrust(const bssl::ParsedCertificate* cert) override;

  // net::PlatformTrustStore implementation:
  std::vector<net::PlatformTrustStore::CertWithTrust> GetAllUserAddedCerts()
      override;
  void UpdateCerts() override;

 private:
  // Inner Impl class for use in initializing stores.
  class Impl;

  // returns pointer to the Impl.
  Impl* MaybeInitializeAndGetImpl();

  base::Lock init_lock_;
  std::unique_ptr<Impl> impl_ GUARDED_BY(init_lock_);
};

}  // namespace net

#endif  // NET_CERT_INTERNAL_TRUST_STORE_OHOS_H_

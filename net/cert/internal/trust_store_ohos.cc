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

#include "net/cert/internal/trust_store_ohos.h"

#include <openssl/span.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <utility>

#include "base/containers/span.h"
#include "base/containers/to_vector.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "net/cert/x509_certificate.h"
#include "net/cert/x509_util.h"
#include "third_party/boringssl/src/pki/cert_errors.h"
#include "third_party/boringssl/src/pki/parse_name.h"
#include "third_party/boringssl/src/pki/parsed_certificate.h"
#include "ui/base/l10n/l10n_util.h"

namespace net {

constexpr const char kRootCertDir[]      = "/system/etc/security/certificates/";
constexpr const char kUserCaCertPrefix[] = "/data/certificates/user_cacerts/";
constexpr const char kUserEdmCertDir[]   = "/data/certificates/user_cacerts/0/";

using ohos::adapter::CertManagerAdapter;

// Certificates in the OHOS roots store may be used as either trust
// anchors or trusted leafs (if self-signed).
constexpr bssl::CertificateTrust kRootCertTrust =
    bssl::CertificateTrust::ForTrustAnchorOrLeaf()
        .WithEnforceAnchorExpiry()
        .WithEnforceAnchorConstraints()
        .WithRequireLeafSelfSigned();

// Applicatioin added Private Certificates may be trusted leafs (if self-signed).
constexpr bssl::CertificateTrust kUserTrust =
    bssl::CertificateTrust::ForTrustedLeaf().WithRequireLeafSelfSigned();

class TrustStoreOhos::Impl {
 public:
  // Creates a TrustStoreOhos.
  Impl() {
    int32_t user_id =
        CertManagerAdapter::GetInstance().GetUserId();
    ca_cert_dir_ = base::StringPrintf("%s%d", kUserCaCertPrefix, user_id);
    GetAllCertsFromOS();
  }

  ~Impl() = default;

  Impl(const Impl& other) = delete;
  Impl& operator=(const Impl& other) = delete;

  void SyncGetIssuersOf(const bssl::ParsedCertificate* cert,
                        bssl::ParsedCertificateList* issuers) {
    base::AutoLock lock(cert_lock_);

    base::span<const uint8_t> issuer_span = cert->issuer_tlv();

    FindIssuersInCertList(issuer_span, issuers, system_ca_certs_);
    FindIssuersInCertList(issuer_span, issuers, user_trusted_certs_);
    FindIssuersInCertList(issuer_span, issuers, edm_certs_);
  }

  bssl::CertificateTrust GetTrust(const bssl::ParsedCertificate* cert) {
    base::AutoLock lock(cert_lock_);
    base::span<const uint8_t> cert_span = cert->der_cert();

    for (auto& system_ca_cert : system_ca_certs_) {
      if (std::ranges::equal(cert_span, system_ca_cert->der_cert())) {
        return kRootCertTrust;
      }
    }

    for (auto& user_trusted_cert : user_trusted_certs_) {
      if (std::ranges::equal(cert_span, user_trusted_cert->der_cert())) {
        return kUserTrust;
      }
    }

    for (auto& trusted_cert : edm_certs_) {
      if (std::ranges::equal(cert_span, trusted_cert->der_cert())) {
        return kRootCertTrust;
      }
    }

    return bssl::CertificateTrust::ForUnspecified();
  }

  std::vector<net::PlatformTrustStore::CertWithTrust> GetAllUserAddedCerts() {
    base::AutoLock lock(cert_lock_);
    std::vector<net::PlatformTrustStore::CertWithTrust> all_certs;

    for (auto& cert : system_ca_certs_) {
      all_certs.emplace_back(base::ToVector(cert->der_cert()), kRootCertTrust);
    }

    for (auto& cert : user_trusted_certs_) {
      all_certs.emplace_back(base::ToVector(cert->der_cert()), kUserTrust);
    }

    for (auto& cert : edm_certs_) {
      all_certs.emplace_back(base::ToVector(cert->der_cert()), kRootCertTrust);
    }

    return all_certs;
  }

void GetAllCertsFromOS() {
  base::AutoLock lock(cert_lock_);

  LoadCertsFromDir(ca_cert_dir_, user_trusted_certs_);
  LoadCertsFromDir(kRootCertDir, system_ca_certs_);
  LoadCertsFromDir(kUserEdmCertDir, edm_certs_);
}

 private:
   void FindIssuersInCertList(
      const base::span<const uint8_t> issuer_span,
      bssl::ParsedCertificateList* issuers,
      const std::vector<std::shared_ptr<const bssl::ParsedCertificate>>&
          cert_list) {
    for (const auto& trust_cert : cert_list) {
      base::span<const uint8_t> subject_span = trust_cert->subject_tlv();
      if (issuer_span.size() == subject_span.size() &&
          std::equal(issuer_span.begin(), issuer_span.end(),
                     subject_span.begin())) {
        issuers->push_back(trust_cert);
      }
    }
  }

  void LoadCertsFromDir(
      const std::string& dir,
      std::vector<std::shared_ptr<const bssl::ParsedCertificate>>& out_vec) {
    CertManagerAdapter::CertInfoList cert_infos =
        CertManagerAdapter::GetInstance().ListCACertsInfo(dir);
    LOG(INFO) << "[CertManager] get certs from " << dir
              << ", certs count: " << cert_infos.size();

    out_vec.clear();
    out_vec.reserve(cert_infos.size());

    for (auto& cert_info : cert_infos) {
      auto parsed_cert = CertOhostoChromium(cert_info);
      if (!parsed_cert) {
        LOG(WARNING) << "[CertManager] Cert translate failed, cert uri: "
                     << cert_info.uri;
        continue;
      }
      out_vec.push_back(std::move(parsed_cert));
    }
  }

  std::shared_ptr<const bssl::ParsedCertificate> CertOhostoChromium(
      const ohos::adapter::OhosCertInfo cert_info) {
    if (cert_info.cert == "") {
      LOG(WARNING) << "[CertManager] Cert doesn't exist, cert uri: "
                   << cert_info.uri;
      return nullptr;
    }
    std::string pem_cert = cert_info.cert;
    size_t len_cert = 0;
    bssl::UniquePtr<uint8_t> der_cert;
    if (!PemToDer(pem_cert, &der_cert, &len_cert) || !der_cert) {
      LOG(WARNING)
          << "[CertManager] Cert translate failed, PemToDer failed, uri: "
          << cert_info.uri;
      return nullptr;
    }

    bssl::UniquePtr<CRYPTO_BUFFER> der_buffer(
        CRYPTO_BUFFER_new(der_cert.get(), len_cert, nullptr));
    bssl::CertErrors errors;
    std::shared_ptr<const bssl::ParsedCertificate> parsed_cert(
        bssl::ParsedCertificate::Create(
            std::move(der_buffer), x509_util::DefaultParseCertificateOptions(),
            &errors));
    if (!parsed_cert) {
      LOG(WARNING) << "[CertManager] Cert translate failed, parsed cert is null, uri: "
                   << cert_info.uri;
    }
    return parsed_cert;
  }

  bool PemToDer(const std::string& pem,
                bssl::UniquePtr<uint8_t>* out,
                size_t* out_len) {
    if (out == nullptr) {
      LOG(ERROR) << "[CertManager] failed to get out pointer.";
      return false;
    }
    bssl::UniquePtr<BIO> bio(BIO_new_mem_buf(pem.data(), pem.size()));
    if (!bio) {
      LOG(ERROR) << "[CertManager] failed to get PEM data.";
      return false;
    }

    char* name = nullptr;
    char* header = nullptr;
    uint8_t* data = 0;
    long data_len = 0;
    if (!PEM_read_bio(bio.get(), &name, &header, &data, &data_len)) {
      LOG(ERROR) << "[CertManager] failed to read PEM data.";
      return false;
    }
    OPENSSL_free(name);
    OPENSSL_free(header);

    out->reset(data);
    *out_len = data_len;
    return true;
  }

  std::vector<std::shared_ptr<const bssl::ParsedCertificate>> system_ca_certs_;
  std::vector<std::shared_ptr<const bssl::ParsedCertificate>> user_trusted_certs_;
  std::vector<std::shared_ptr<const bssl::ParsedCertificate>> edm_certs_;

  std::string ca_cert_dir_;
  base::Lock cert_lock_;
};

TrustStoreOhos::TrustStoreOhos() = default;

TrustStoreOhos::~TrustStoreOhos() = default;

void TrustStoreOhos::Initialize() {
  // Don't need return value
  MaybeInitializeAndGetImpl();
}

TrustStoreOhos::Impl* TrustStoreOhos::MaybeInitializeAndGetImpl() {
  base::AutoLock lock(init_lock_);
  if (!impl_) {
    impl_ = std::make_unique<TrustStoreOhos::Impl>();
  }
  return impl_.get();
}

void TrustStoreOhos::UpdateCerts() {
  MaybeInitializeAndGetImpl()->GetAllCertsFromOS();
}

void TrustStoreOhos::SyncGetIssuersOf(const bssl::ParsedCertificate* cert,
                                      bssl::ParsedCertificateList* issuers) {
  MaybeInitializeAndGetImpl()->SyncGetIssuersOf(cert, issuers);
}

bssl::CertificateTrust TrustStoreOhos::GetTrust(
    const bssl::ParsedCertificate* cert) {
  return MaybeInitializeAndGetImpl()->GetTrust(cert);
}

std::vector<net::PlatformTrustStore::CertWithTrust>
TrustStoreOhos::GetAllUserAddedCerts() {
  return MaybeInitializeAndGetImpl()->GetAllUserAddedCerts();
}

}  // namespace net

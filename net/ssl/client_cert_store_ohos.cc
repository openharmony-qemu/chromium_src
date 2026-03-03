/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "net/ssl/client_cert_store_ohos.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/thread_pool.h"
#include "base/threading/scoped_blocking_call.h"
#include "net/cert/x509_certificate.h"
#include "net/cert/x509_util.h"
#include "net/ssl/ssl_cert_request_info.h"
#include "net/ssl/ssl_platform_key_ohos.h"
#include "net/ssl/threaded_ssl_private_key.h"
#include "ohos/adapter/cert_manager/cert_manager_adapter.h"
#include "third_party/boringssl/src/include/openssl/bio.h"
#include "third_party/boringssl/src/include/openssl/mem.h"
#include "third_party/boringssl/src/include/openssl/pem.h"
#include "third_party/boringssl/src/include/openssl/pool.h"
#include "third_party/boringssl/src/include/openssl/x509.h"
#include "third_party/boringssl/src/include/openssl/x509_vfy.h"
#include "third_party/boringssl/src/include/openssl/x509v3.h"

using namespace ohos::adapter;

namespace net {

// static
bool ClientCertStoreOHOS::is_huks_ = false;

namespace {

class ClientCertIdentityOHOS : public ClientCertIdentity {
 public:
  ClientCertIdentityOHOS(scoped_refptr<net::X509Certificate> cert,
                         bssl::UniquePtr<X509> bssl_cert)
      : ClientCertIdentity(std::move(cert)), bssl_cert_(std::move(bssl_cert)) {}
  ~ClientCertIdentityOHOS() override = default;

  void AcquirePrivateKey(base::OnceCallback<void(scoped_refptr<SSLPrivateKey>)>
                             private_key_callback) override {
    // Caller is responsible for keeping the ClientCertIdentity alive until
    // the |private_key_callback| is run, so it's safe to use Unretained here.
    base::ThreadPool::PostTaskAndReplyWithResult(
        FROM_HERE,
        {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN},
        base::BindOnce(&FetchClientCertPrivateKey,
                       base::Unretained(certificate()), bssl_cert_.get()),
        std::move(private_key_callback));
  }

 private:
  bssl::UniquePtr<X509> bssl_cert_;
};

}  // namespace

bool GetIssuerAndSubjectOHOS(X509* cert,
                             std::string& issuer,
                             std::string& subject) {
  uint8_t* issuer_buf = nullptr;
  int size = i2d_X509_NAME(X509_get_issuer_name(cert), &issuer_buf);
  if (!issuer_buf || size <= 0) {
    return false;
  }
  bssl::UniquePtr<uint8_t> free_issuer_buf(issuer_buf);
  issuer.assign(issuer_buf, issuer_buf + size);

  uint8_t* subject_buf = nullptr;
  size = i2d_X509_NAME(X509_get_subject_name(cert), &subject_buf);
  if (!subject_buf || size <= 0) {
    return false;
  }
  bssl::UniquePtr<uint8_t> free_subject_buf(subject_buf);
  subject.assign(subject_buf, subject_buf + size);

  return true;
}

bool MatchClientCertsIssuersOHOS(
    X509* cert,
    ClientCertStoreOHOS::CertificateStore& store,
    const std::vector<std::string>& cert_authorities,
    std::vector<bssl::UniquePtr<X509>>* intermediates) {
  // Bound how many iterations to try.
  static const int K_MAX_DEPTH = 20;

  intermediates->clear();

  // If no authorities are supplied, everything matches.
  if (cert_authorities.empty()) {
    return true;
  }

  // DER encoded issuer and subject name of current certificate.
  std::string issuer;
  std::string subject;
  if (!cert || !GetIssuerAndSubjectOHOS(cert, issuer, subject)) {
    return false;
  }

  // current cert is not issued by a valid CA, try to get issuer cert and check.
  while (intermediates->size() < K_MAX_DEPTH) {
    // Check if current cert is issued by a valid CA.
    for (const std::string& ca : cert_authorities) {
      if (issuer == ca) {
        return true;
      }
    }

    // Stop at self-issued certificates.
    if (issuer == subject) {
      return false;
    }

    // Look the parent up in the cert_stroe and keep searching.
    auto next_cert = store.find(issuer);
    if (next_cert == store.end()) {
      return false;
    }

    subject = next_cert->first;
    issuer = next_cert->second.second;

    intermediates->push_back(std::move(next_cert->second.first));
  }

  return false;
}

std::vector<bssl::UniquePtr<X509>> FindSSLCertsOHOS(bool only_client) {
  std::vector<bssl::UniquePtr<X509>> certs;

  CertManagerAdapter::CertInfoList ohos_cert_info_list =
      CertManagerAdapter::GetInstance().ListCertsInfo();
  for (auto& ohos_cert : ohos_cert_info_list) {
    std::string pem_cert = ohos_cert.cert;
    // pem cert file
    bssl::UniquePtr<BIO> cert_bio(
        BIO_new_mem_buf(pem_cert.c_str(), pem_cert.size()));
    if (!cert_bio) {
      return {};
    }
    X509* cert = nullptr;
    while (
        (cert = PEM_read_bio_X509(cert_bio.get(), nullptr, nullptr, nullptr))) {
      bssl::UniquePtr<X509> bssl_cert(cert);
      char* uri_cert = (char*)malloc(ohos_cert.uri.size() + 1);
      if (!uri_cert) {
        return {};
      }
      memset(uri_cert, 0x00, ohos_cert.uri.size() + 1);
      memcpy(uri_cert, ohos_cert.uri.data(), ohos_cert.uri.size());

      X509_set_ex_data(bssl_cert.get(), 0, uri_cert);
      if (!only_client) {
        certs.push_back(std::move(bssl_cert));
      } else if (X509_check_ca(cert) == 0 &&
                 X509_check_purpose(cert, X509_PURPOSE_SSL_CLIENT, 0) == 1) {
        certs.push_back(std::move(bssl_cert));
      }
    }
  }
  return certs;
}

std::vector<bssl::UniquePtr<X509>> GetSSLCertsOHOS(bool only_client) {
  std::vector<bssl::UniquePtr<X509>> certs;

  CertManagerAdapter::CertInfoList ohos_cert_info_list =
      CertManagerAdapter::GetInstance().EnumClientCerts();
  for (auto& ohos_cert : ohos_cert_info_list) {
    std::string pem_cert = ohos_cert.cert;
    // pem cert file
    bssl::UniquePtr<BIO> cert_bio(
        BIO_new_mem_buf(pem_cert.c_str(), pem_cert.size()));
    if (!cert_bio) {
      return {};
    }
    X509* cert = nullptr;
    while (
        (cert = PEM_read_bio_X509(cert_bio.get(), nullptr, nullptr, nullptr))) {
      bssl::UniquePtr<X509> bssl_cert(cert);
      char* uri_cert = (char*)malloc(ohos_cert.uri.size() + 1);
      if (!uri_cert) {
        return {};
      }
      memset(uri_cert, 0x00, ohos_cert.uri.size() + 1);
      memcpy(uri_cert, ohos_cert.uri.data(), ohos_cert.uri.size());

      X509_set_ex_data(bssl_cert.get(), 0, uri_cert);

      if (ohos_cert.type == ohos::adapter::CertType::USB_CERT) {
        X509_set_ex_data(bssl_cert.get(), 1, uri_cert);
      }
      if (!only_client) {
        certs.push_back(std::move(bssl_cert));
      } else if (X509_check_ca(cert) == 0 &&
                 X509_check_purpose(cert, X509_PURPOSE_SSL_CLIENT, 0) == 1) {
        certs.push_back(std::move(bssl_cert));
      }
    }
  }
  return certs;
}

ClientCertStoreOHOS::CertificateStore GetSSLCertStoreOHOS(
    const std::vector<std::string>& cert_files) {
  ClientCertStoreOHOS::CertificateStore cert_store;
  auto found_certs = FindSSLCertsOHOS(false);
  for (auto& bssl_cert : found_certs) {
    // get DER encoded issuer and subject of current certificate
    std::string issuer;
    std::string subject;
    if (!GetIssuerAndSubjectOHOS(bssl_cert.get(), issuer, subject)) {
      return {};
    }

    auto& cert_pair = cert_store[subject];
    cert_pair.first = std::move(bssl_cert);
    cert_pair.second = issuer;
  }
  return cert_store;
}

ClientCertStoreOHOS::ClientCertStoreOHOS() = default;

ClientCertStoreOHOS::~ClientCertStoreOHOS() = default;

void ClientCertStoreOHOS::GetClientCerts(
    scoped_refptr<const SSLCertRequestInfo> request,
    ClientCertListCallback callback) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE,
      {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN},
      base::BindOnce(&ClientCertStoreOHOS::GetAndFilterCertsOnWorkerThread,
                     // Caller is responsible for keeping the ClientCertStore
                     // alive until the callback is run.
                     base::Unretained(this), std::move(request)),
      base::BindOnce(&ClientCertStoreOHOS::OnClientCertsResponse,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void ClientCertStoreOHOS::GetSoftClientCerts(
                          ClientCertListCallback callback) {
  ClientCertIdentityList identities;
  auto found_certs = FindSSLCertsOHOS(true);

  for (auto itr = found_certs.begin(); itr != found_certs.end(); ++itr) {
    uint8_t* der_buf = nullptr;
    int size = i2d_X509(itr->get(), &der_buf);
    if (!der_buf || size <= 0) {
      LOG(ERROR) << "call i2d_X509 failed.";
      continue;
    }
    bssl::UniquePtr<uint8_t> free_der_buf(der_buf);
    std::vector<uint8_t> cert_buf(der_buf, der_buf + size);

    // Allow UTF-8 inside PrintableStrings in client certificates. See
    // crbug.com/770323.
    X509Certificate::UnsafeCreateOptions options;
    options.printable_string_is_utf8 = true;
    auto cert = X509Certificate::CreateFromBytesUnsafeOptions(
        base::make_span(cert_buf.data(), cert_buf.size()), options);
    if (!cert) {
      LOG(ERROR)
          << "x509_util::CreateX509CertificateFromCERTCertificate failed";
      continue;
    }
    char* cert_uri = (char*)X509_get_ex_data(itr->get(), 0);
    if (cert_uri ) {
      cert->SetCertUri(cert_uri);
    }
    identities.push_back(std::make_unique<ClientCertIdentityOHOS>(
        cert, std::move(*itr), nullptr));
  }
  std::move(callback).Run(std::move(identities));
}

void ClientCertStoreOHOS::OnClientCertsResponse(
    ClientCertListCallback callback,
    ClientCertIdentityList identities) {
  std::move(callback).Run(std::move(identities));
}

// static
void ClientCertStoreOHOS::FilterCertsOnWorkerThread(
    ClientCertIdentityList* identities,
    const SSLCertRequestInfo& request) {
  DCHECK(identities);
  
  size_t num_raw = 0;

  auto keep_iter = identities->begin();

  base::Time now = base::Time::Now();

  CertificateStore bssl_cert_store;

  for (auto examine_iter = identities->begin();
       examine_iter != identities->end(); ++examine_iter) {
    ++num_raw;

    X509Certificate* cert = (*examine_iter)->certificate();

    // Only offer unexpired certificates.
    if (now < cert->valid_start() || now > cert->valid_expiry()) {
      continue;
    }

    const uint8_t* cert_der_buf = CRYPTO_BUFFER_data(cert->cert_buffer());
    bssl::UniquePtr<X509> bssl_cert(d2i_X509(
        nullptr, &cert_der_buf, CRYPTO_BUFFER_len(cert->cert_buffer())));

    std::vector<bssl::UniquePtr<X509>> bssl_intermediates;
    if (!MatchClientCertsIssuersOHOS(bssl_cert.get(), bssl_cert_store,
                                     request.cert_authorities,
                                     &bssl_intermediates)) {
      continue;
    }

    std::vector<bssl::UniquePtr<CRYPTO_BUFFER>> intermediates;
    intermediates.reserve(bssl_intermediates.size());
    for (const bssl::UniquePtr<X509>& bssl_intermediate : bssl_intermediates) {
      uint8_t* der_buf = nullptr;
      int size = i2d_X509(bssl_intermediate.get(), &der_buf);
      if (!der_buf || size <= 0) {
        LOG(ERROR) << "call i2d_X509 failed.";
        continue;
      }
      bssl::UniquePtr<uint8_t> free_der_buf(der_buf);
      std::vector<uint8_t> buf(der_buf, der_buf + size);
      intermediates.push_back(x509_util::CreateCryptoBuffer(
          base::make_span(buf.data(), buf.size())));
    }

    // Retain a copy of the intermediates. Some deployments expect the client
    // to supply intermediates out of the local store. See
    // https://crbug.com/548631.
    (*examine_iter)->SetIntermediates(std::move(intermediates));

    if (examine_iter == keep_iter) {
      ++keep_iter;
    } else {
      *keep_iter++ = std::move(*examine_iter);
    }
  }
  identities->erase(keep_iter, identities->end());

  LOG(INFO) << "num_raw:" << num_raw << " num_filtered:" << identities->size();

  std::sort(identities->begin(), identities->end(), ClientCertIdentitySorter());
}

// static
void ClientCertStoreOHOS::GetPlatformCertsOnWorkerThread(
    ClientCertIdentityList* identities) {
  if (identities == nullptr) {
    LOG(ERROR) << "identities is nullptr.";
    return;
  }
  auto found_certs = FindSSLCertsOHOS(true);
  if (is_huks_)
    found_certs = GetSSLCertsOHOS(false);
  if (found_certs.empty()) {
    LOG(ERROR) << "No client certs found.";
    return;
  }

  for (auto itr = found_certs.begin(); itr != found_certs.end(); ++itr) {
    uint8_t* der_buf = nullptr;
    int size = i2d_X509(itr->get(), &der_buf);
    if (!der_buf || size <= 0) {
      LOG(ERROR) << "call i2d_X509 failed.";
      continue;
    }
    bssl::UniquePtr<uint8_t> free_der_buf(der_buf);
    std::vector<uint8_t> cert_buf(der_buf, der_buf + size);

    // Allow UTF-8 inside PrintableStrings in client certificates. See
    // crbug.com/770323.
    X509Certificate::UnsafeCreateOptions options;
    options.printable_string_is_utf8 = true;
    auto cert = X509Certificate::CreateFromBytesUnsafeOptions(
        base::make_span(cert_buf.data(), cert_buf.size()), options);
    if (!cert) {
      LOG(ERROR)
          << "x509_util::CreateX509CertificateFromCERTCertificate failed";
      continue;
    }
    char* cert_uri = (char*)X509_get_ex_data(itr->get(), 0);
    char* is_ukey = (char*)X509_get_ex_data(itr->get(), 1);
    if (cert_uri && !is_ukey) {
      cert->SetCertUri(cert_uri);
    }
    identities->push_back(std::make_unique<ClientCertIdentityOHOS>(
        cert, std::move(*itr)));
  }
}

ClientCertIdentityList ClientCertStoreOHOS::GetAndFilterCertsOnWorkerThread(
    scoped_refptr<const SSLCertRequestInfo> request) {
  // This method may acquire the BoringSSL lock or reenter this code via
  // extension hooks (such as smart card UI). To ensure threads are not starved
  // or deadlocked, the base::ScopedBlockingCall below increments the thread
  // pool capacity if this method takes too much time to run.
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  ClientCertIdentityList identities;
  GetPlatformCertsOnWorkerThread(&identities);
  FilterCertsOnWorkerThread(&identities, *request);

  return identities;
}

}  // namespace net

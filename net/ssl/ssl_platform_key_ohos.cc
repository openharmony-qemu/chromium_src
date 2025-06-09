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
 
#include "net/ssl/ssl_platform_key_ohos.h"
 
#include <memory>
#include <utility>
 
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/threading/scoped_blocking_call.h"
#include "net/cert/x509_certificate.h"
#include "net/ssl/ssl_platform_key_util.h"
#include "net/ssl/ssl_private_key.h"
#include "net/ssl/threaded_ssl_private_key.h"
#include "ohos/adapter/cert_manager/cert_manager_adapter.h"
#include "third_party/boringssl/src/include/openssl/bn.h"
#include "third_party/boringssl/src/include/openssl/bytestring.h"
#include "third_party/boringssl/src/include/openssl/ec.h"
#include "third_party/boringssl/src/include/openssl/ec_key.h"
#include "third_party/boringssl/src/include/openssl/ecdsa.h"
#include "third_party/boringssl/src/include/openssl/evp.h"
#include "third_party/boringssl/src/include/openssl/mem.h"
#include "third_party/boringssl/src/include/openssl/nid.h"
#include "third_party/boringssl/src/include/openssl/rsa.h"
#include "third_party/boringssl/src/include/openssl/ssl.h"
#include "third_party/boringssl/src/include/openssl/x509.h"
 
using ohos::adapter::CertManagerAdapter;
 
namespace net {
 
namespace {
 
class SSLPlatformKeyOHOS : public ThreadedSSLPrivateKey::Delegate {
 public:
  SSLPlatformKeyOHOS(int type,
                     std::string uri_cert)
      : type_(type) {
    uri_cert_ = uri_cert;
  }
 
  SSLPlatformKeyOHOS(const SSLPlatformKeyOHOS&) = delete;
  SSLPlatformKeyOHOS& operator=(const SSLPlatformKeyOHOS&) = delete;
 
  ~SSLPlatformKeyOHOS() override = default;
 
  std::string GetProviderName() override {
    // This logic accesses fields directly on the struct, so it may run on any
    // thread without caching.
    return base::StringPrintf("BoringSSL");
  }
 
  std::vector<uint16_t> GetAlgorithmPreferences() override {
    return SSLPrivateKey::DefaultAlgorithmPreferences(type_, true);
  }
 
  Error Sign(uint16_t algorithm,
             base::span<const uint8_t> input,
             std::vector<uint8_t>* signature) override {
    size_t size = 0;
    int ret = CertManagerAdapter::GetInstance().SignDataByUri(
        uri_cert_, (uint8_t*)input.data(), input.size(), nullptr, &size);
    if (ret) {
      return ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
    }
 
    std::vector<uint8_t> out_data(size, 0);
    ret = CertManagerAdapter::GetInstance().SignDataByUri(
        uri_cert_, (uint8_t*)input.data(), input.size(), out_data.data(), &size);
    if (ret) {
      return ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
    }
 
    signature->assign(out_data.data(), out_data.data() + size);
 
    return OK;
  }
 
 private:
  int type_;
  std::string uri_cert_;
};
 
}  // namespace
 
bssl::UniquePtr<EVP_PKEY> FindPrivateKeyByCertOHOS(
    X509* cert,
    const std::vector<std::string>& pkey_files) {
  if (!cert || pkey_files.empty()) {
    return nullptr;
  }
 
  for (auto& pkey_file : pkey_files) {
    // load pkey file
    auto pkey_path = base::FilePath::FromUTF8Unsafe(pkey_file.c_str());
    std::string pkey_content;
    if (!base::ReadFileToString(pkey_path, &pkey_content)) {
      return nullptr;
    }
 
    // check file-format is pem or der
    if (pkey_content.find("-----BEGIN PRIVATE KEY-----") != std::string::npos) {
      // pem pkey file
      bssl::UniquePtr<BIO> pkey_bio(
          BIO_new_mem_buf(pkey_content.c_str(), pkey_content.size()));
      if (!pkey_bio) {
        return nullptr;
      }
 
      EVP_PKEY* pkey = nullptr;
      while ((pkey = PEM_read_bio_PrivateKey(pkey_bio.get(), nullptr, nullptr,
                                             nullptr))) {
        bssl::UniquePtr<EVP_PKEY> bssl_pkey(pkey);
        if (X509_check_private_key(cert, pkey) == 1) {
          return bssl_pkey;
        }
      }
    } else {
      // der pkey file
      auto der_pkey = reinterpret_cast<const uint8_t*>(pkey_content.c_str());
      EVP_PKEY* pkey =
          d2i_AutoPrivateKey(nullptr, &der_pkey, pkey_content.size());
      if (pkey) {
        bssl::UniquePtr<EVP_PKEY> bssl_pkey(pkey);
        if (X509_check_private_key(cert, pkey) == 1) {
          return bssl_pkey;
        }
      }
    }
  }
 
  return nullptr;
}
 
scoped_refptr<SSLPrivateKey> FetchClientCertPrivateKey(
    const X509Certificate* certificate,
    X509* bssl_cert) {
  // This function may acquire the NSS lock or reenter this code via extension
  // hooks (such as smart card UI). To ensure threads are not starved or
  // deadlocked, the base::ScopedBlockingCall below increments the thread pool
  // capacity if this method takes too much time to run.
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
 
  if (!bssl_cert) {
    return nullptr;
  }
 
  char* uri_cert = (char*)X509_get_ex_data(bssl_cert, 0);
  std::string str_uri(uri_cert);
  free(uri_cert);
 
  int type;
  size_t max_length;
  if (!GetClientCertInfo(certificate, &type, &max_length)) {
    return nullptr;
  }
 
  return base::MakeRefCounted<ThreadedSSLPrivateKey>(
      std::make_unique<SSLPlatformKeyOHOS>(type, str_uri),
      GetSSLPlatformKeyTaskRunner());
}
 
}  // namespace net

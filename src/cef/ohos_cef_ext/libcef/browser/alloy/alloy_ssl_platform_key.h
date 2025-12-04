/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CEF_LIBCEF_BROWSER_ALLOY_ALLOY_SSL_PLATFORM_KEY_
#define CEF_LIBCEF_BROWSER_ALLOY_ALLOY_SSL_PLATFORM_KEY_
#pragma once

#include "include/cef_request_handler.h"
#include "net/base/net_errors.h"
#include "net/ssl/ssl_platform_key_util.h"
#include "net/ssl/threaded_ssl_private_key.h"
#include "third_party/boringssl/src/include/openssl/digest.h"
#include "third_party/boringssl/src/include/openssl/ec.h"
#include "third_party/boringssl/src/include/openssl/evp.h"
#include "third_party/boringssl/src/include/openssl/rsa.h"
#include "third_party/boringssl/src/include/openssl/ssl.h"

#if BUILDFLAG(IS_ARKWEB)
class SSLPlatformKey : public net::ThreadedSSLPrivateKey::Delegate {
 public:
  explicit SSLPlatformKey(bssl::UniquePtr<EVP_PKEY> key)
      : key_(std::move(key)) {}

  SSLPlatformKey(const SSLPlatformKey&) = delete;
  SSLPlatformKey& operator=(const SSLPlatformKey&) = delete;

  ~SSLPlatformKey() override = default;

  std::string GetProviderName() override { return "EVP_PKEY"; }

  std::vector<uint16_t> GetAlgorithmPreferences() override {
    return net::SSLPrivateKey::DefaultAlgorithmPreferences(
        EVP_PKEY_id(key_.get()), true /* supports PSS */);
  }

  net::Error Sign(uint16_t algorithm,
                  base::span<const uint8_t> input,
                  std::vector<uint8_t>* signature) override {
    bssl::ScopedEVP_MD_CTX ctx;
    EVP_PKEY_CTX* pctx;

    if (!EVP_DigestSignInit(ctx.get(), &pctx,
                            SSL_get_signature_algorithm_digest(algorithm),
                            nullptr, key_.get())) {
      LOG(ERROR) << "Sign: EVP digest sign init failed";
      return net::ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
    }

    if (SSL_is_signature_algorithm_rsa_pss(algorithm)) {
      if (!EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) ||
          !EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, -1 /* hash length */)) {
        LOG(ERROR) << "Sign: EVP pkey ctx set rsa padding failed";
        return net::ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
      }
    }

    size_t sig_len = 0;
    if (!EVP_DigestSign(ctx.get(), nullptr, &sig_len, input.data(),
                        input.size())) {
      LOG(ERROR) << "Sign: EVP digest sign failed";
      return net::ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
    }

    signature->resize(sig_len);
    if (!EVP_DigestSign(ctx.get(), signature->data(), &sig_len, input.data(),
                        input.size())) {
      LOG(ERROR) << "Sign: resize EVP digest sign failed";
      return net::ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED;
    }
    signature->resize(sig_len);

    return net::OK;
  }

 private:
  bssl::UniquePtr<EVP_PKEY> key_;
};

#endif  // IS_ARKWEB
#endif  // CEF_LIBCEF_BROWSER_ALLOY_ALLOY_SSL_PLATFORM_KEY_

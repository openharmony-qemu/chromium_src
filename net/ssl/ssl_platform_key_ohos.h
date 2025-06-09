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
 
#ifndef NET_SSL_SSL_PLATFORM_KEY_OHOS_H_
#define NET_SSL_SSL_PLATFORM_KEY_OHOS_H_
 
#include <string>
#include <vector>
 
#include "base/functional/callback.h"
#include "base/memory/scoped_refptr.h"
#include "net/base/net_export.h"
 
typedef struct x509_st X509;
typedef struct evp_pkey_st EVP_PKEY;
 
namespace net {
 
class SSLPrivateKey;
class X509Certificate;
 
using PrivateKeyFileDelegate =
    base::RepeatingCallback<std::vector<std::string>()>;
 
NET_EXPORT std::vector<uint16_t> GetAlgorithmPreferences(int type,
                                                         EVP_PKEY* key);
 
// Returns an SSLPrivateKey backed by the BoringSSL private key that corresponds
// to |certificate|'s public key. If |password_delegate| is non-null, it will be
// used to prompt for a password if necessary to unlock a slot or perform
// signing operations.
NET_EXPORT scoped_refptr<SSLPrivateKey> FetchClientCertPrivateKey(
    const X509Certificate* certificate,
    X509* bssl_cert);
 
}  // namespace net
 
#endif  // NET_SSL_SSL_PLATFORM_KEY_OHOS_H_

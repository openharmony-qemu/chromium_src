// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
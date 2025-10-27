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
#ifndef CEF_LIBCEF_BROWSER_ALLOY_ALLOY_CLIENT_CERT_IDENTITY_H_
#define CEF_LIBCEF_BROWSER_ALLOY_ALLOY_CLIENT_CERT_IDENTITY_H_
#pragma once

#include <string>

#include "net/ssl/client_cert_identity.h"
#include "net/ssl/ssl_private_key.h"

#if BUILDFLAG(IS_ARKWEB)
// Simple ClientCertIdentity implementation.
// Note: this implementation of AcquirePrivateKey will always call the callback
// synchronously.
class ClientCertIdentityOhos : public net::ClientCertIdentity {
 public:
  ClientCertIdentityOhos(scoped_refptr<net::X509Certificate> cert,
                         scoped_refptr<net::SSLPrivateKey> key)
      : net::ClientCertIdentity(std::move(cert)), key_(std::move(key)) {}

  ~ClientCertIdentityOhos() override {}

  // ClientCertIdentity implementation:
  void AcquirePrivateKey(
      base::OnceCallback<void(scoped_refptr<net::SSLPrivateKey>)>
          private_key_callback) override {}

 private:
  scoped_refptr<net::SSLPrivateKey> key_;
};

#endif  // IS_ARKWEB
#endif  // CEF_LIBCEF_BROWSER_ALLOY_ALLOY_CLIENT_CERT_IDENTITY_H_

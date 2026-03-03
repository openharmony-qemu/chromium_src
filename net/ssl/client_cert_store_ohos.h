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

#ifndef NET_SSL_CLIENT_CERT_STORE_OHOS_H_
#define NET_SSL_CLIENT_CERT_STORE_OHOS_H_

#include <map>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "net/base/net_export.h"
#include "net/ssl/client_cert_store.h"

namespace net {
class SSLCertRequestInfo;

class NET_EXPORT ClientCertStoreOHOS : public ClientCertStore {
 public:
  using CertificateStore =
      std::map<std::string, std::pair<bssl::UniquePtr<X509>, std::string>>;

  ClientCertStoreOHOS();

  ClientCertStoreOHOS(const ClientCertStoreOHOS&) = delete;
  ClientCertStoreOHOS& operator=(const ClientCertStoreOHOS&) = delete;

  ~ClientCertStoreOHOS() override;

  // ClientCertStore:
  void GetClientCerts(scoped_refptr<const SSLCertRequestInfo> cert_request_info,
                      ClientCertListCallback callback) override;
  void GetSoftClientCerts(
      ClientCertListCallback callback) override;

  // Examines the certificates in |identities| to find all certificates that
  // match the client certificate request in |request|, removing any that don't.
  // The remaining certs will be updated to include intermediates.
  // Must be called from a worker thread.
  static void FilterCertsOnWorkerThread(
      ClientCertIdentityList* identities,
      const SSLCertRequestInfo& request);

  // Retrieves all client certificates that are stored by BoringSSL and adds
  // them to |identities|. |password_delegate| is used to unlock slots if
  // required. If |cert_filter| is not null, only certificates that it returns
  // true on will be added. Must be called from a worker thread.
  static void GetPlatformCertsOnWorkerThread(
      ClientCertIdentityList* identities);

  
  static bool is_huks_;

 private:
  ClientCertIdentityList GetAndFilterCertsOnWorkerThread(
      scoped_refptr<const SSLCertRequestInfo> request);

  void OnClientCertsResponse(ClientCertListCallback callback,
                             ClientCertIdentityList identities);

  base::WeakPtrFactory<ClientCertStoreOHOS> weak_factory_{this};
};

}  // namespace net

#endif  // NET_SSL_CLIENT_CERT_STORE_OHOS_H_

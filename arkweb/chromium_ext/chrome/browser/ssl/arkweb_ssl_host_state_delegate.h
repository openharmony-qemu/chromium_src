/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#ifndef ARKWEB_SSL_HOST_STATE_DELEGATE_H_
#define ARKWEB_SSL_HOST_STATE_DELEGATE_H_
 
#include <map>
#include <string>
 
#include "components/security_interstitials/content/stateful_ssl_host_state_delegate.h"
#include "net/base/hash_value.h"
#include "net/cert/x509_certificate.h"
 
namespace chrome {
 
namespace internal {
// This class maintains the policy for storing actions on certificate errors.
class CertPolicy {
 public:
  CertPolicy();
  ~CertPolicy();
  // Returns true if the user has decided to proceed through the ssl error
  // before. For a certificate to be allowed, it must not have any
  // *additional* errors from when it was allowed.
  bool Check(const net::X509Certificate& cert, int error) const;
 
  // Causes the policy to allow this certificate for a given |error|. And
  // remember the user's choice.
  void Allow(const net::X509Certificate& cert, int error);
 
  // Returns true if and only if there exists a user allow exception for some
  // certificate.
  bool HasAllowException() const { return allowed_.size() > 0; }
 
 private:
  // The set of fingerprints of allowed certificates.
  std::map<net::SHA256HashValue, int> allowed_;
};
 
}  // namespace internal
 
class ArkWebSSLHostStateDelegate : public StatefulSSLHostStateDelegate {
 public:
  ArkWebSSLHostStateDelegate(content::BrowserContext* browser_context,
                             PrefService* pref_service,
                             HostContentSettingsMap* host_content_settings_map);
 
  ArkWebSSLHostStateDelegate(const ArkWebSSLHostStateDelegate&) = delete;
  ArkWebSSLHostStateDelegate& operator=(const ArkWebSSLHostStateDelegate&) =
      delete;
 
  ~ArkWebSSLHostStateDelegate() override;
 
  // Records that |cert| is permitted to be used for |host| in the future, for
  // a specified |error| type.
  void AllowCert(const std::string& host,
                 const net::X509Certificate& cert,
                 int error,
                 content::StoragePartition* storage_partition) override;
 
  void Clear(
      base::RepeatingCallback<bool(const std::string&)> host_filter) override;
 
  // Queries whether |cert| is allowed or denied for |host| and |error|.
  content::SSLHostStateDelegate::CertJudgment QueryPolicy(
      const std::string& host,
      const net::X509Certificate& cert,
      int error,
      content::StoragePartition* storage_partition) override;
 
  // Revokes all SSL certificate error allow exceptions made by the user for
  // |host|.
  void RevokeUserAllowExceptions(const std::string& host) override;
 
  // Returns whether the user has allowed a certificate error exception for
  // |host|. This does not mean that *all* certificate errors are allowed, just
  // that there exists an exception. To see if a particular certificate and
  // error combination exception is allowed, use QueryPolicy().
  bool HasAllowException(const std::string& host,
                         content::StoragePartition* storage_partition) override;
 
  // Returns whether the user has allowed any certificate error exception or
  // HTTP exception for any host in |storage_partition|.
  bool HasAllowExceptionForAnyHost(
      content::StoragePartition* storage_partition) override;
 
 private:
  // Certificate policies for each host.
  std::map<std::string, internal::CertPolicy> cert_policy_for_host_;
};
 
}  // namespace chrome
 
#endif  // ARKWEB_SSL_HOST_STATE_DELEGATE_H_

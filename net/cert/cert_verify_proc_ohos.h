// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_CERT_CERT_VERIFY_PROC_OHOS_H_
#define NET_CERT_CERT_VERIFY_PROC_OHOS_H_

#include "net/base/net_export.h"
#include "net/cert/cert_verify_proc.h"

typedef struct x509_lookup_st X509_LOOKUP;

namespace net {

class CertNetFetcher;
class VerifyProcOHOSInner;

class NET_EXPORT CertVerifyProcOHOS : public CertVerifyProc {
 public:
  explicit CertVerifyProcOHOS(scoped_refptr<CertNetFetcher> net_fetcher,
                              scoped_refptr<CRLSet> crl_set,
                              bool enable_cache = true);

  CertVerifyProcOHOS(const CertVerifyProcOHOS&) = delete;
  CertVerifyProcOHOS& operator=(const CertVerifyProcOHOS&) = delete;

 protected:
  ~CertVerifyProcOHOS() override;

 private:
  int VerifyInternal(X509Certificate* cert,
                     const std::string& hostname,
                     const std::string& ocsp_response,
                     const std::string& sct_list,
                     int flags,
                     CertVerifyResult* verify_result,
                     const NetLogWithSource& net_log) override;

  scoped_refptr<CertNetFetcher> cert_net_fetcher_;
  std::unique_ptr<VerifyProcOHOSInner> inner_;
};

}  // namespace net

#endif  // NET_CERT_CERT_VERIFY_PROC_OHOS_H_

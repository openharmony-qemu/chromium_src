// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_CERT_CERT_VERIFY_PROC_OHOS_H_
#define NET_CERT_CERT_VERIFY_PROC_OHOS_H_

#include "net/base/net_export.h"
#include "net/cert/cert_verify_proc.h"

namespace net {

class CertNetFetcher;

class NET_EXPORT CertVerifyProcOHOS : public CertVerifyProc {
 public:
  CertVerifyProcOHOS(const CertVerifyProcOHOS&) = delete;
  CertVerifyProcOHOS& operator=(const CertVerifyProcOHOS&) = delete;

  explicit CertVerifyProcOHOS(scoped_refptr<CertNetFetcher> net_fetcher);

  bool SupportsAdditionalTrustAnchors() const override;

 protected:
  ~CertVerifyProcOHOS() override;

 private:
  int VerifyInternal(X509Certificate* cert,
                     const std::string& hostname,
                     const std::string& ocsp_response,
                     const std::string& sct_list,
                     int flags,
                     const CertificateList& additional_trust_anchors,
                     CertVerifyResult* verify_result,
                     const NetLogWithSource& net_log) override;

  scoped_refptr<CertNetFetcher> cert_net_fetcher_;
};

}  // namespace net

#endif  // NET_CERT_CERT_VERIFY_PROC_OHOS_H_

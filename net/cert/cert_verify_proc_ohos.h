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

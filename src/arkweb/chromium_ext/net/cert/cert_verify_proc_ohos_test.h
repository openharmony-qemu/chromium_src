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
#include <network/netstack/net_ssl/net_ssl_c.h>

struct CertVerifyProcStub {
  BIO* (*BIO_new_mem_buf)(char* cert, int size);
  X509_STORE_CTX* (*X509_STORE_CTX_new)(void);
};

#ifdef __cplusplus
extern "C" {
#endif

CertVerifyProcStub* GetCertVerifyProcStub() {
  static CertVerifyProcStub stub;
  return &stub;
}

extern BIO* __real_BIO_new_mem_buf(char* cert, int size);
BIO* __wrap_BIO_new_mem_buf(char* cert, int size) {
  if (GetCertVerifyProcStub()->BIO_new_mem_buf) {
    return GetCertVerifyProcStub()->BIO_new_mem_buf(cert, size);
  }
  return __real_BIO_new_mem_buf(cert, size);
}

extern X509_STORE_CTX* __real_X509_STORE_CTX_new(void);
X509_STORE_CTX* __wrap_X509_STORE_CTX_new(void) {
  if (GetCertVerifyProcStub()->X509_STORE_CTX_new) {
    return GetCertVerifyProcStub()->X509_STORE_CTX_new();
  }
  return __real_X509_STORE_CTX_new();
}
#ifdef __cplusplus
}
#endif

namespace net {
namespace {
// Certficate with the subject CN set to "Chromium", the subject organisation
// set to "The Chromium Projects", and the subject organizational unit set to
// "Security."
static const char kSelfSignedWithCommonNamePEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDeTCCAmGgAwIBAgIJAKZbsC4gPYAUMA0GCSqGSIb3DQEBCwUAMFMxETAPBgNV\n"
    "BAMMCENocm9taXVtMR4wHAYDVQQKDBVUaGUgQ2hyb21pdW0gUHJvamVjdHMxETAP\n"
    "BgNVBAsMCFNlY3VyaXR5MQswCQYDVQQGEwJVUzAeFw0xNzAxMjkyMDU1NDFaFw0x\n"
    "ODAxMjkyMDU1NDFaMFMxETAPBgNVBAMMCENocm9taXVtMR4wHAYDVQQKDBVUaGUg\n"
    "Q2hyb21pdW0gUHJvamVjdHMxETAPBgNVBAsMCFNlY3VyaXR5MQswCQYDVQQGEwJV\n"
    "UzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMlir9M85QOvQ5ok+uvH\n"
    "XF7kmW21B22Ffdw+B2mXTV6NLGvINCdwocIlebQlAdWS2QY/WM08uAYJ3m0IGD+t\n"
    "6OG4zG3vOmWMdFQy4XkxMsDkbV11F9n4dsF5TXEvILlupOtOWu6Up8vfFkii/x+/\n"
    "bz4aGBDdFu6U8TdQ8ELSmHxJYi4LM0lUKTdLLte3T5Grv3UUXQW33Qs6RXZlH/ul\n"
    "jf7/v0HQefM3XdT9djG1XRv8Ga32c8tz+wtSw7PPIWjt0ZDJxZ2/fX7YLwAt2D6N\n"
    "zQgrNJtL0/I/j9sO6A0YQeHzmnlyoAd14VhBfEllZc51pFaut31wpbPPxtH0K0Ro\n"
    "2XUCAwEAAaNQME4wHQYDVR0OBBYEFD7eitJ8KlIaVS4J9w2Nz+5OE8H0MB8GA1Ud\n"
    "IwQYMBaAFD7eitJ8KlIaVS4J9w2Nz+5OE8H0MAwGA1UdEwQFMAMBAf8wDQYJKoZI\n"
    "hvcNAQELBQADggEBAFjuy0Jhj2E/ALOkOst53/nHIpT5suru4H6YEmmPye+KCQnC\n"
    "ws1msPyLQ8V10/kyQzJTSLbeehNyOaK99KJk+hZBVEKBa9uH3WXPpiwz1xr3STJO\n"
    "hhV2wXGTMqe5gryR7r+n88+2TpRiZ/mAVyJm4NQgev4HZbFsl3sT50AQrrEbHHiY\n"
    "Sh38NCR8JCVuzLBjcEEIWxjhDPkdNPJtx3cBkIDP+Cz1AUSPretGk7CQAGivq7Kq\n"
    "9y6A59guc1RFVPeEQAxUIUDZGDQlB3PtmrXrp1/LAaDYvQCstDBgiZoamy+xSROP\n"
    "BU2KIzRj2EUOWqtIURU4Q2QC1fbVqxVjfPowX/A=\n"
    "-----END CERTIFICATE-----\n";
}  // namespace

void GetChainDEREncodedBytes(X509Certificate* cert,
                             std::vector<std::string>* chain_bytes);
X509* p2i_X509(const char* pem);
void X509_d2i_free(X509* server_cert[], uint32_t server_cert_sum);
int32_t GetApplicationApiVersion();
void AddAppCert(const std::string_view& hostname, X509_STORE* ca_store);
int CertVerify(const std::vector<std::string>& cert_bytes,
               std::vector<std::string>* verified_chain,
               std::string_view hostname);
void SetCertStatus(int status, CertVerifyResult* verify_result);
bool PerformAIAFetchAndAddResultToVector(
    scoped_refptr<CertNetFetcher> fetcher,
    std::string_view uri,
    bssl::ParsedCertificateList* cert_list);
int TryVerifyWithAIAFetching(const std::vector<std::string>& cert_bytes,
                             const std::string& hostname,
                             scoped_refptr<CertNetFetcher> cert_net_fetcher,
                             CertVerifyResult* verify_result,
                             std::vector<std::string>* verified_chain);
std::shared_ptr<const bssl::ParsedCertificate> FindLastCertWithUnknownIssuer(
    const bssl::ParsedCertificateList& certs,
    const std::shared_ptr<const bssl::ParsedCertificate>& start);
int ConvertToParsedCertificates(const std::vector<std::string>& cert_bytes,
                                bssl::CertErrors& errors,
                                bssl::ParsedCertificateList& certs);
base::FilePath GetTmpCertDir();
int CertChainRootVerify(X509* server_cert[],
                        int32_t index,
                        X509_STORE* ca_store);
}  // namespace net

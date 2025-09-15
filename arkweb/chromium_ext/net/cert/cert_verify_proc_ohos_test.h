// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on cert_verify_proc_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <unistd.h>

#include <set>
#include <string>
#include <vector>

#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/base64.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/containers/adapters.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "crypto/sha2.h"
#include "net/cert/asn1_util.h"
#include "net/cert/cert_net_fetcher.h"
#include "net/cert/cert_verify_proc_ohos.h"
#include "net/cert/cert_verify_result.h"
#include "net/cert/crl_set.h"
#include "net/cert/known_roots.h"
#include "net/cert/x509_certificate.h"
#include "net/cert/x509_util.h"
#include "openssl/err.h"
#include "openssl/ossl_typ.h"
#include "openssl/x509.h"
#include "openssl/x509_vfy.h"
#include "third_party/boringssl/src/crypto/x509/internal.h"
#include "third_party/boringssl/src/pki/cert_errors.h"
#include "third_party/boringssl/src/pki/parsed_certificate.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "url/gurl.h"

struct CertVerifyProcStub {
  bool (*GetTrustAnchorsForHostName)(const std::string& hostname,
                                     std::vector<std::string>& certs);
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
// Valid PEM certificate headers but invalid BASE64 content.
static const char kInvalidCertificatePEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "This is invalid base64.\n"
    "It contains some (#$*) invalid characters.\n"
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
void ConvertToParsedCertificates(const std::vector<std::string>& cert_bytes,
                                 bssl::CertErrors& errors,
                                 bssl::ParsedCertificateList& certs);


class MockCertManagerAdapter final : public OHOS::NWeb::CertManagerAdapter {
 public:
  MockCertManagerAdapter() = default;
  ~MockCertManagerAdapter() override = default;

  uint32_t GetCertMaxSize() override { return 0; }
  uint32_t GetAppCertMaxSize() override { return 0; }
  int32_t GetSytemRootCertData(uint32_t certCount, uint8_t* certData) override {
    return 0;
  }
  uint32_t GetSytemRootCertSum() override { return 0; }
  int32_t GetUserRootCertData(uint32_t certCount, uint8_t* certData) override {
    return 0;
  }
  uint32_t GetUserRootCertSum() override { return 0; }
  int32_t GetAppCert(uint8_t* uriData,
                     uint8_t* certData,
                     uint32_t* len) override {
    return 0;
  }
  int32_t Sign(const uint8_t* uri,
               const uint8_t* certData,
               uint32_t certDataLen,
               uint8_t* signData,
               uint32_t signDataLen) override {
    return 0;
  }
  int32_t GetCertDataBySubject(const char* subjectName,
                               uint8_t* certData,
                               int32_t certType) override {
    return 0;
  }
  bool GetTrustAnchorsForHostName(const std::string& hostname,
                                  std::vector<std::string>& certs) override {
    if (GetCertVerifyProcStub()->GetTrustAnchorsForHostName) {
      return GetCertVerifyProcStub()->GetTrustAnchorsForHostName(hostname,
                                                                 certs);
    }
    return false;
  }
  bool GetPinSetForHostName(const std::string& hostname,
                            std::vector<std::string>& pins) override {
    return false;
  }
};

std::unique_ptr<OHOS::NWeb::CertManagerAdapter> MockGetRootCertDataAdapter() {
  return std::make_unique<MockCertManagerAdapter>();
}
}  // namespace net

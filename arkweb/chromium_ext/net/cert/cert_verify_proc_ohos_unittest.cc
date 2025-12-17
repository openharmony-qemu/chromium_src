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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include <memory>
#include <string>
#include <vector>

#include "base/base_paths.h"
#include "base/base_switches.h"
#include "base/files/file_util.h"
#include "base/memory/scoped_refptr.h"
#include "base/test/scoped_command_line.h"
#include "base/test/task_environment.h"
#include "net/cert/cert_net_fetcher.h"
#include "net/cert/cert_verify_result.h"
#include "net/cert/mock_cert_net_fetcher.h"
#include "net/cert/test_root_certs.h"
#include "net/log/net_log_with_source.h"
#include "net/test/cert_builder.h"
#include "net/test/cert_test_util.h"
#include "openssl/x509.h"
#include "openssl/x509_vfy.h"
#include "third_party/boringssl/src/pki/cert_errors.h"
#include "url/gurl.h"

#define protected public
#define private public
#include "arkweb/chromium_ext/net/cert/cert_verify_proc_ohos.h"
#undef private
#undef protected
#include "arkweb/chromium_ext/net/cert/cert_verify_proc_ohos_test.h"
#include "arkweb/ohos_adapter_ndk/hiviewdfx_adapter/hitrace_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "net/tools/transport_security_state_generator/cert_util.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/hitrace_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

using ::testing::_;
using ::testing::ByMove;
using ::testing::Return;

namespace net {
namespace {
const char kHostname[] = "www.example.com";
const char kUrl[] = "http://aia.test/root";
const GURL kRootURL(kUrl);
const GURL kIntermediateURL("http://aia.test/intermediate");
const char kInvalidPEM[] = "invalid pem data";
}  // namespace

class MockCertManagerAdapter : public OHOS::NWeb::CertManagerAdapter {
 public:
  MOCK_METHOD(uint32_t, GetCertMaxSize, (), (override));
  MOCK_METHOD(uint32_t, GetAppCertMaxSize, (), (override));
  MOCK_METHOD(int32_t,
              GetSytemRootCertData,
              (uint32_t certCount, uint8_t* certData),
              (override));
  MOCK_METHOD(uint32_t, GetSytemRootCertSum, (), (override));
  MOCK_METHOD(int32_t,
              GetUserRootCertData,
              (uint32_t certCount, uint8_t* certData),
              (override));
  MOCK_METHOD(uint32_t, GetUserRootCertSum, (), (override));
  MOCK_METHOD(int32_t,
              GetAppCert,
              (uint8_t * uriData, uint8_t* certData, uint32_t* len),
              (override));
  MOCK_METHOD(int32_t,
              Sign,
              (const uint8_t* uri,
               const uint8_t* certData,
               uint32_t certDataLen,
               uint8_t* signData,
               uint32_t signDataLen),
              (override));
  MOCK_METHOD(int32_t,
              GetCertDataBySubject,
              (const char* subjectName, uint8_t* certData, int32_t certType),
              (override));
  MOCK_METHOD(bool,
              GetTrustAnchorsForHostName,
              (const std::string& hostname, std::vector<std::string>& certs),
              (override));
  MOCK_METHOD(bool,
              GetPinSetForHostName,
              (const std::string& hostname, std::vector<std::string>& pins),
              (override));
  MOCK_METHOD(int32_t,
              SignV2,
              (const uint8_t* uri,
               const uint8_t* certData,
               uint32_t certDataLen,
               uint8_t* signData,
               uint32_t* signDataLen,
               uint16_t algorithm),
              (override));
};

class CertVerifyProcOHOSTest : public ::testing::Test {
 protected:
  void SetUp() override {
    instance_ = std::make_unique<OHOS::NWeb::MockOhosAdapterHelper>();
    OHOS::NWeb::OhosAdapterHelper::SetInstance(instance_.get());
    EXPECT_CALL(*instance_, GetHiTraceAdapterInstance())
        .WillRepeatedly(testing::Invoke([]() -> OHOS::NWeb::HiTraceAdapter& {
          return OHOS::NWeb::HiTraceAdapterImpl::GetInstance();
        }));

    fetcher_ = base::MakeRefCounted<MockCertNetFetcher>();
    certVerifyProc_ = base::MakeRefCounted<CertVerifyProcOHOS>(fetcher_);
    scoped_command_line_ = std::make_unique<base::test::ScopedCommandLine>();
  }

  void TearDown() override {
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(fetcher_.get()));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(instance_.get()));
    OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
    instance_.reset();
  }

  void SetupMockAdapter(std::unique_ptr<MockCertManagerAdapter> mock_adapter) {
    EXPECT_CALL(*instance_, GetRootCertDataAdapter())
        .WillOnce(testing::Return(std::move(mock_adapter)));
  }

  scoped_refptr<CertVerifyProcOHOS> certVerifyProc_;
  scoped_refptr<MockCertNetFetcher> fetcher_;
  std::unique_ptr<base::test::ScopedCommandLine> scoped_command_line_;
  std::unique_ptr<OHOS::NWeb::MockOhosAdapterHelper> instance_;
};

// Verify
TEST_F(CertVerifyProcOHOSTest, VerifyInternal_ValidCertificateChain) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  ScopedTestRoot test_root(root->GetX509Certificate());

  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  ASSERT_TRUE(mock_adapter);
  base::FilePath path = GetTmpCertDir();
  EXPECT_CALL(*mock_adapter,
              GetTrustAnchorsForHostName(::testing::_, ::testing::_))
      .WillRepeatedly(
          testing::Invoke([=](const std::string& hostname,
                              std::vector<std::string>& certs) -> bool {
            certs.push_back(path.value());
            return true;
          }));

  auto adapter = mock_adapter.get();
  SetupMockAdapter(std::move(mock_adapter));
  CertVerifyResult verify_result;
  int result = certVerifyProc_->VerifyInternal(
      leaf->GetX509CertificateChain().get(), kHostname, std::string(),
      std::string(), 0, &verify_result, NetLogWithSource());
  EXPECT_EQ(result, OK);
  EXPECT_FALSE(IsCertStatusError(verify_result.cert_status));
  EXPECT_TRUE(verify_result.verified_cert);
  EXPECT_FALSE(verify_result.public_key_hashes.empty());
  ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(adapter));
}

TEST_F(CertVerifyProcOHOSTest, VerifyInternal_Fail) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  // do not save root cert

  CertVerifyResult verify_result;
  int result = certVerifyProc_->VerifyInternal(
      leaf->GetX509CertificateChain().get(), kHostname, std::string(),
      std::string(), 0, &verify_result, NetLogWithSource());
  EXPECT_NE(result, OK);
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_AUTHORITY_INVALID);
}

TEST_F(CertVerifyProcOHOSTest, VerifyInternal_TryAIAFetch_Fail) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  // do not save root cert

  EXPECT_CALL(*fetcher_, FetchCaIssuers(kRootURL, _, _))
      .WillOnce(Return(ByMove(
          MockCertNetFetcherRequest::Create(intermediate->GetCertBuffer()))));
  CertVerifyResult verify_result;
  int result = certVerifyProc_->VerifyInternal(
      leaf->GetX509CertificateChain().get(), kHostname, std::string(),
      std::string(), 0, &verify_result, NetLogWithSource());
  EXPECT_NE(result, OK);
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_AUTHORITY_INVALID);
}

TEST_F(CertVerifyProcOHOSTest, VerifyInternal_TryAIAFetch_FetchFail) {
  auto [leaf1, intermediate1, root1] = CertBuilder::CreateSimpleChain3();
  leaf1->SetCaIssuersUrl(kRootURL);
  intermediate1->SetCaIssuersUrl(kRootURL);

  EXPECT_CALL(*fetcher_, FetchCaIssuers(kRootURL, _, _))
      .WillOnce(Return(ByMove(
          MockCertNetFetcherRequest::Create(ERR_DISALLOWED_URL_SCHEME))));

  // fail OH_NetStack_GetCertificatesForHostName
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  ASSERT_TRUE(mock_adapter);
  base::FilePath path = GetTmpCertDir();
  EXPECT_CALL(*mock_adapter,
              GetTrustAnchorsForHostName(::testing::_, ::testing::_))
      .WillRepeatedly(testing::Invoke(
          [=](const std::string& hostname,
              std::vector<std::string>& certs) -> bool { return false; }));

  auto adapter = mock_adapter.get();
  SetupMockAdapter(std::move(mock_adapter));
  CertVerifyResult verify_result;
  int result = certVerifyProc_->VerifyInternal(
      intermediate1->GetX509Certificate().get(), kHostname, std::string(),
      std::string(), 0, &verify_result, NetLogWithSource());
  EXPECT_NE(result, OK);
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_AUTHORITY_INVALID);
  ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(adapter));
}

TEST_F(CertVerifyProcOHOSTest, Constructor_ValidFetcher) {
  auto proc = base::MakeRefCounted<CertVerifyProcOHOS>(fetcher_);
  EXPECT_TRUE(proc);
  EXPECT_EQ(fetcher_.get(), proc->cert_net_fetcher_.get());
}

TEST_F(CertVerifyProcOHOSTest, Constructor_NullFetcher) {
  auto proc = base::MakeRefCounted<CertVerifyProcOHOS>(nullptr);
  EXPECT_TRUE(proc);
  EXPECT_EQ(nullptr, proc->cert_net_fetcher_.get());
  ASSERT_NO_FATAL_FAILURE(AddAppCert(kHostname, nullptr));
  SetupMockAdapter(nullptr);
  ASSERT_NO_FATAL_FAILURE(AddAppCert(kHostname, nullptr));
}

// GetChainDEREncodedBytes
TEST_F(CertVerifyProcOHOSTest, GetChainDEREncodedBytes_ValidCert) {
  std::vector<std::string> chain_bytes;
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  leaf->SetSubjectAltName(kHostname);
  GetChainDEREncodedBytes(leaf->GetX509CertificateChain().get(), &chain_bytes);
  EXPECT_GT(chain_bytes.size(), 0u);
  EXPECT_FALSE(chain_bytes[0].empty());
  EXPECT_EQ(chain_bytes.size(),
            1 + leaf->GetX509CertificateChain()->intermediate_buffers().size());
}

TEST_F(CertVerifyProcOHOSTest, GetChainDEREncodedBytes_SingleCert) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  leaf->SetSubjectAltName(kHostname);

  std::vector<std::string> chain_bytes;
  GetChainDEREncodedBytes(leaf->GetX509Certificate().get(), &chain_bytes);
  EXPECT_EQ(chain_bytes.size(), 1u);
  EXPECT_FALSE(chain_bytes[0].empty());
}

// p2i_X509
static BIO* BIO_new_mem_buf_stub(char* cert, int size) {
  return nullptr;
}
TEST_F(CertVerifyProcOHOSTest, p2i_X509) {
  X509* x509_cert = p2i_X509(kSelfSignedWithCommonNamePEM);
  EXPECT_NE(nullptr, x509_cert);
  if (x509_cert) {
    X509_free(x509_cert);
  }

  x509_cert = p2i_X509(kInvalidPEM);
  EXPECT_EQ(nullptr, x509_cert);

  // empty
  x509_cert = p2i_X509("");
  EXPECT_EQ(nullptr, x509_cert);

  // BIO_new_mem_buf fail
  GetCertVerifyProcStub()->BIO_new_mem_buf = BIO_new_mem_buf_stub;
  x509_cert = p2i_X509(kInvalidPEM);
  EXPECT_EQ(nullptr, x509_cert);
  GetCertVerifyProcStub()->BIO_new_mem_buf = nullptr;
}

// X509_d2i_free
TEST_F(CertVerifyProcOHOSTest, X509_d2i_free_ValidCerts) {
  std::vector<X509*> certs(3, nullptr);  // 3 test num
  ASSERT_NO_FATAL_FAILURE(X509_d2i_free(certs.data(), certs.size()));
  ASSERT_NO_FATAL_FAILURE(X509_d2i_free(certs.data(), 0));
}

// GetApplicationApiVersion
TEST_F(CertVerifyProcOHOSTest, GetApplicationApiVersion) {
  scoped_command_line_->GetProcessCommandLine()->RemoveSwitch(
      switches::kOhosAppApiVersion);
  int32_t version = GetApplicationApiVersion();
  EXPECT_EQ(-1, version);

  scoped_command_line_->GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "10");
  version = GetApplicationApiVersion();
  EXPECT_EQ(10, version);

  scoped_command_line_->GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "");
  version = GetApplicationApiVersion();
  EXPECT_EQ(-1, version);
  scoped_command_line_->GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "0xffffffffffffffffff");
  version = GetApplicationApiVersion();
  EXPECT_EQ(-1, version);
}

// SetCertStatus
TEST_F(CertVerifyProcOHOSTest, SetCertStatus) {
  CertVerifyResult verify_result;
  ASSERT_NO_FATAL_FAILURE(SetCertStatus(X509_V_OK, &verify_result));
  EXPECT_EQ(0u, verify_result.cert_status);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(
      SetCertStatus(X509_V_ERR_UNSPECIFIED, &verify_result));
  EXPECT_EQ(0u, verify_result.cert_status);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(SetCertStatus(
      X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN, &verify_result));
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_AUTHORITY_INVALID);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(SetCertStatus(
      X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY, &verify_result));
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_AUTHORITY_INVALID);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(
      SetCertStatus(X509_V_ERR_CERT_HAS_EXPIRED, &verify_result));
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_DATE_INVALID);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(
      SetCertStatus(X509_V_ERR_CERT_NOT_YET_VALID, &verify_result));
  EXPECT_TRUE(verify_result.cert_status & CERT_STATUS_DATE_INVALID);

  verify_result.cert_status = 0;
  ASSERT_NO_FATAL_FAILURE(
      SetCertStatus(X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT, &verify_result));
  EXPECT_TRUE(verify_result.cert_status &
              CERT_STATUS_DEPTH_ZERO_SELF_SIGNED_CERT);
}

// PerformAIAFetchAndAddResultToVector
TEST_F(CertVerifyProcOHOSTest, PerformAIAFetchAndAddResultToVector) {
  const GURL kTestURL("http://example.com/cert");
  std::vector<std::string> chain_bytes;
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();

  bssl::ParsedCertificateList cert_list;
  bool result =
      PerformAIAFetchAndAddResultToVector(fetcher_, "invalid_url", &cert_list);
  EXPECT_FALSE(result);

  EXPECT_CALL(*fetcher_, FetchCaIssuers(kTestURL, _, _))
      .WillOnce(Return(ByMove(
          MockCertNetFetcherRequest::Create(ERR_DISALLOWED_URL_SCHEME))));
  result = PerformAIAFetchAndAddResultToVector(fetcher_, kTestURL.spec(),
                                               &cert_list);
  EXPECT_FALSE(result);
  EXPECT_CALL(*fetcher_, FetchCaIssuers(kTestURL, _, _))
      .WillOnce(Return(ByMove(
          MockCertNetFetcherRequest::Create(intermediate->GetCertBuffer()))));
  result = PerformAIAFetchAndAddResultToVector(fetcher_, kTestURL.spec(),
                                               &cert_list);
  EXPECT_TRUE(result);
  EXPECT_FALSE(cert_list.empty());
}

TEST_F(CertVerifyProcOHOSTest,
       FindLastCertWithUnknownIssuer_SubjectEqualIssuer) {
  std::vector<std::string> chain_bytes;
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  ScopedTestRoot test_root(root->GetX509Certificate());
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  intermediate->SetSubjectCommonName("intermediate");
  leaf->SetSubjectCommonName("leaf");
  root->SetSubjectCommonName("root");
  bssl::CertErrors errors;
  bssl::ParsedCertificateList certs;

  // cert->normalized_subject() == last->normalized_issuer()
  leaf->SetSubjectCommonName(kUrl);
  GetChainDEREncodedBytes(leaf->GetX509CertificateChain().get(), &chain_bytes);
  int ret = ConvertToParsedCertificates(chain_bytes, errors, certs);
  EXPECT_EQ(ret, X509_V_OK);
  EXPECT_EQ(2u, certs.size());
  std::shared_ptr<const bssl::ParsedCertificate> parsedCerts =
      FindLastCertWithUnknownIssuer(certs, certs[0]);
  EXPECT_NE(parsedCerts, nullptr);

  certs.clear();
  chain_bytes.clear();
  parsedCerts.reset();
  // only one
  GetChainDEREncodedBytes(leaf->GetX509Certificate().get(), &chain_bytes);
  ConvertToParsedCertificates(chain_bytes, errors, certs);
  EXPECT_EQ(ret, X509_V_OK);
  EXPECT_EQ(1u, certs.size());
  parsedCerts = FindLastCertWithUnknownIssuer(certs, certs[0]);
  EXPECT_NE(parsedCerts, nullptr);

  certs.clear();
  ret = ConvertToParsedCertificates({}, errors, certs);
  EXPECT_EQ(ret, X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY);
  EXPECT_EQ(0u, certs.size());
}

TEST_F(CertVerifyProcOHOSTest, FindLastCertWithUnknownIssuer_SelfEqual) {
  std::vector<std::string> chain_bytes;
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  ScopedTestRoot test_root(root->GetX509Certificate());
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  intermediate->SetSubjectCommonName("leaf");
  leaf->SetSubjectCommonName("leaf");
  root->SetSubjectCommonName("root");
  bssl::CertErrors errors;
  bssl::ParsedCertificateList certs;

  GetChainDEREncodedBytes(leaf->GetX509CertificateFullChain().get(),
                          &chain_bytes);
  int ret = ConvertToParsedCertificates(chain_bytes, errors, certs);
  EXPECT_EQ(ret, X509_V_OK);
  EXPECT_EQ(3u, certs.size());
  auto parsedCerts = FindLastCertWithUnknownIssuer(certs, certs[0]);
  EXPECT_EQ(parsedCerts, nullptr);
}

X509_STORE_CTX* X509_STORE_CTX_new_stub() {
  return nullptr;
}
TEST_F(CertVerifyProcOHOSTest, VerifyInternal_X509_STORE_CTX_new_Fail) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  leaf->SetSubjectAltName(kHostname);
  ScopedTestRoot test_root(root->GetX509Certificate());

  GetCertVerifyProcStub()->X509_STORE_CTX_new = X509_STORE_CTX_new_stub;

  CertVerifyResult verify_result;
  int result = certVerifyProc_->VerifyInternal(
      leaf->GetX509CertificateChain().get(), kHostname, std::string(),
      std::string(), 0, &verify_result, NetLogWithSource());
  EXPECT_EQ(result, OK);
  EXPECT_FALSE(IsCertStatusError(verify_result.cert_status));
  EXPECT_FALSE(verify_result.verified_cert);
  EXPECT_TRUE(verify_result.public_key_hashes.empty());
  GetCertVerifyProcStub()->X509_STORE_CTX_new = nullptr;
}

TEST_F(CertVerifyProcOHOSTest, CertChainRootVerify_InvalidParam) {
  auto ret = CertChainRootVerify(nullptr, 0, nullptr);
  EXPECT_EQ(ret, X509_V_ERR_UNSPECIFIED);

  X509* server_cert[1];
  ret = CertChainRootVerify(server_cert, 0, nullptr);
  EXPECT_EQ(ret, X509_V_ERR_UNSPECIFIED);

  X509_STORE* ca_store = X509_STORE_new();
  ret = CertChainRootVerify(server_cert, 0, ca_store);
  EXPECT_EQ(ret, X509_V_ERR_UNSPECIFIED);

  GetCertVerifyProcStub()->X509_STORE_CTX_new = X509_STORE_CTX_new_stub;
  ret = CertChainRootVerify(server_cert, 0, ca_store);
  EXPECT_EQ(ret, X509_V_ERR_UNSPECIFIED);
  X509_STORE_free(ca_store);
  GetCertVerifyProcStub()->X509_STORE_CTX_new = nullptr;
}

TEST_F(CertVerifyProcOHOSTest, CertChainRootVerify) {
  auto [leaf, intermediate, root] = CertBuilder::CreateSimpleChain3();
  root->SetCaIssuersUrl(kRootURL);
  intermediate->SetCaIssuersUrl(kRootURL);
  leaf->SetCaIssuersUrl(kIntermediateURL);
  leaf->SetSubjectAltName(kHostname);
  ScopedTestRoot test_root(root->GetX509Certificate());

  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  ASSERT_TRUE(mock_adapter);
  base::FilePath path = GetTmpCertDir();
  EXPECT_CALL(*mock_adapter,
              GetTrustAnchorsForHostName(::testing::_, ::testing::_))
      .WillRepeatedly(
          testing::Invoke([=](const std::string& hostname,
                              std::vector<std::string>& certs) -> bool {
            certs.push_back(path.value());
            certs.push_back("");
            return true;
          }));

  auto adapter = mock_adapter.get();
  SetupMockAdapter(std::move(mock_adapter));

  std::vector<std::string> cert_bytes;
  GetChainDEREncodedBytes(leaf->GetX509CertificateChain().get(), &cert_bytes);

  int ret = X509_V_OK;
  uint32_t server_cert_sum = cert_bytes.size();
  X509* server_cert[4] = {0}; // 4 max cert
  for (uint32_t i = 0; i < server_cert_sum; i++) {
    const unsigned char* der_encoded_tmp =
        (unsigned char*)cert_bytes[i].c_str();
    server_cert[i] = d2i_X509(nullptr, &der_encoded_tmp, cert_bytes[i].size());
    if (server_cert[i] == nullptr) {
      ERR_clear_error();
      X509_d2i_free(server_cert, i);
      ret = X509_V_ERR_UNSPECIFIED;
      break;
    }
  }
  EXPECT_EQ(ret, X509_V_OK);
  {
    // not add lookdir
    X509_STORE* ca_store = X509_STORE_new();
    ret = CertChainRootVerify(server_cert, 1, ca_store);
    X509_STORE_free(ca_store);
    EXPECT_NE(ret, X509_V_OK);
  }
  {
    X509_STORE* ca_store = X509_STORE_new();
    AddAppCert("test.com", ca_store);
    ret = CertChainRootVerify(server_cert, 1, ca_store);
    X509_STORE_free(ca_store);
    EXPECT_EQ(ret, X509_V_OK);
  }
  ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(adapter));
}

}  // namespace net
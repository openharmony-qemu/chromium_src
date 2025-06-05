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

#include "net/cert/cert_verify_proc_ohos.h"

#include <set>
#include <string>
#include <vector>

#include "base/command_line.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "content/public/common/content_switches.h"
#include "crypto/sha2.h"
#include "net/cert/asn1_util.h"
#include "net/cert/cert_net_fetcher.h"
#include "net/cert/cert_verify_result.h"
#include "net/cert/crl_set.h"
#include "net/cert/known_roots.h"
#include "net/cert/test_root_certs.h"
#include "net/cert/x509_certificate.h"
#include "net/cert/x509_util.h"
#include "ohos/adapter/cert_manager/cert_manager_adapter.h"
#include "third_party/boringssl/src/crypto/x509/internal.h"
#include "third_party/boringssl/src/include/openssl/bio.h"
#include "third_party/boringssl/src/include/openssl/err.h"
#include "third_party/boringssl/src/include/openssl/ossl_typ.h"
#include "third_party/boringssl/src/include/openssl/pem.h"
#include "third_party/boringssl/src/include/openssl/x509.h"
#include "third_party/boringssl/src/include/openssl/x509_vfy.h"
#include "third_party/boringssl/src/pki/cert_errors.h"
#include "third_party/boringssl/src/pki/extended_key_usage.h"
#include "third_party/boringssl/src/pki/parsed_certificate.h"
#include "url/gurl.h"

using ohos::adapter::CertManagerAdapter;
#define ROOT_CERT "/etc/ssl/certs/cacert.pem"
#define USER_CACERT_PREFIX "/data/certificates/user_cacerts/"
#define EDM_CERT "/data/certificates/user_cacerts/0/"

#define MIN_CERT_NUM 1
#define DER_ENCODED 0x30

namespace net {
// OH ignores the authType parameter to
// X509TrustManager.checkServerTrusted, so pass in a dummy value. See
// https://crbug.com/627154.
const char kAuthType[] = "RSA";

// The maximum number of AIA fetches that TryVerifyWithAIAFetching() will
// attempt. If a valid chain cannot be built after this many fetches,
// TryVerifyWithAIAFetching() will give up and return
// X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY.
const unsigned int kMaxAIAFetches = 5;

class VerifyProcOHOSInner {
 public:
  explicit VerifyProcOHOSInner(bool enable_cache)
      : enable_cache_(enable_cache) {
    if (enable_cache_) {
      cert_crl_cache_ = bssl::UniquePtr<X509_STORE>(X509_STORE_new());
    }
  }

  ~VerifyProcOHOSInner() {}
  VerifyProcOHOSInner(const VerifyProcOHOSInner&) = delete;
  VerifyProcOHOSInner& operator=(const VerifyProcOHOSInner&) = delete;

  void GetChainDEREncodedBytes(X509Certificate* cert,
                               std::vector<std::string>* chain_bytes) {
    chain_bytes->reserve(1 + cert->intermediate_buffers().size());
    chain_bytes->emplace_back(
        net::x509_util::CryptoBufferAsStringPiece(cert->cert_buffer()));
    for (const auto& handle : cert->intermediate_buffers()) {
      chain_bytes->emplace_back(
          net::x509_util::CryptoBufferAsStringPiece(handle.get()));
    }
  }

  int CertVerifyByStore(X509* cert,
                        X509_STORE* ca_store,
                        std::vector<std::string>* verified_chain) {
    if (!cert || !ca_store) {
      LOG(ERROR) << "CertVerifyByStore() failed, cert or ca_store is NULL.";
      return X509_V_ERR_UNSPECIFIED;
    }

    bssl::UniquePtr<X509_STORE_CTX> ctx(X509_STORE_CTX_new());
    if (!ctx) {
      LOG(ERROR) << "CertVerifyByStore() failed, create ctx failed.";
      return X509_V_ERR_UNSPECIFIED;
    }
    X509_STORE_CTX_init(ctx.get(), ca_store, cert, nullptr);
    if (X509_verify_cert(ctx.get()) != 1) {
      LOG(ERROR) << "CertVerifyByStore() failed, verify cert failed";
      return ctx->error;
    }

    if (verified_chain && sk_X509_num(ctx->chain) != 0U) {
      verified_chain->reserve(1U + sk_X509_num(ctx->chain));
      for (size_t i = 0; i < sk_X509_num(ctx->chain); ++i) {
        X509* cert_in = sk_X509_value(ctx->chain, i);
        uint8_t* cert_buf = nullptr;
        int cert_len = i2d_X509(cert_in, &cert_buf);
        if (!cert_buf || cert_len <= 0) {
          LOG(ERROR)
              << "CertVerifyByStore() failed, convert X509 to DER failed";
          return X509_V_ERR_UNSPECIFIED;
        }
        bssl::UniquePtr<uint8_t> cert_free(cert_buf);
        verified_chain->emplace_back(
            std::string(cert_buf, cert_buf + cert_len));
      }
    }

    return X509_V_OK;
  }

  int CertChainVerify(const std::vector<bssl::UniquePtr<X509>>& server_certs,
                      X509_STORE* ca_store,
                      std::vector<std::string>* verified_chain) {
    if (server_certs.empty()) {
      LOG(ERROR) << "CertChainVerify() failed, certificate chain is empty";
      return X509_V_ERR_UNSPECIFIED;
    }

    // verify the intermediate certificates.
    // if OK, add it to the certificate store.
    // intermediate certificates can be unordered or invalid.
    std::vector<X509*> intermediate;
    for (size_t index = server_certs.size() - 1U; index > 0U; --index) {
      intermediate.push_back(server_certs[index].get());
    }
    int intermediate_result = X509_V_OK;
    for (size_t index = 0U; index < intermediate.size(); ++index) {
      int result = CertVerifyByStore(intermediate[index], ca_store, nullptr);
      if (result == X509_V_OK) {
        X509_STORE_add_cert(ca_store, intermediate[index]);
      } else {
        LOG(ERROR) << "CertChainVerify() failed, verify intermediate failed, "
                      "intermediate size: "
                   << intermediate.size() << ", index: " << index
                   << ", error code: " << result
                   << ", error info: " << X509_verify_cert_error_string(result);
        if (result != X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY) {
          intermediate_result = result;
        }
      }
    }

    // verify the certificate by CA certificates store
    int verify_result =
        CertVerifyByStore(server_certs[0U].get(), ca_store, verified_chain);
    // If verification fails, for detailed error information, see
    // X509_verify_cert_error_string
    if (verify_result != X509_V_OK) {
      LOG(ERROR) << "CertChainVerify() failed, verify cert failed, error code: "
                 << verify_result << ", error info: "
                 << X509_verify_cert_error_string(verify_result);
      if (verify_result == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY &&
          intermediate_result != X509_V_OK) {
        return intermediate_result;
      }
      return verify_result;
    }

    return X509_V_OK;
  }

  int X509_load_cert_crl(X509_LOOKUP* ctx,
                         const std::string& content,
                         int type) {
    bssl::UniquePtr<BIO> in(BIO_new_mem_buf(content.data(), content.size()));
    if (!in) {
      return 0;
    }

    bssl::UniquePtr<STACK_OF(X509_INFO)> inf(
        PEM_X509_INFO_read_bio(in.get(), nullptr, nullptr, nullptr));
    if (!inf) {
      return 0;
    }
    int count = 0;
    for (size_t i = 0; i < sk_X509_INFO_num(inf.get()); i++) {
      X509_INFO* itmp = sk_X509_INFO_value(inf.get(), i);
      if (itmp->x509 && X509_STORE_add_cert(ctx->store_ctx, itmp->x509)) {
        count++;
      }
      if (itmp->crl && X509_STORE_add_crl(ctx->store_ctx, itmp->crl)) {
        count++;
      }
    }
    return count;
  }

#if defined(NET_UNITTESTS_OHOS_ENABLE)
  // only for unittests
  bool GetTestCertFile(X509_LOOKUP* lookup) {
    if (!lookup) {
      return false;
    }

    base::FilePath test_cert_dir;
    if (!base::GetCurrentDirectory(&test_cert_dir)) {
      return false;
    }

    const std::string net_test_tmp_dir = "tmp/net_test_tmp_dir";
    test_cert_dir = test_cert_dir.Append(net_test_tmp_dir);
    if (!base::DirectoryExists(test_cert_dir)) {
      return true;
    }

    base::FileEnumerator test_enum(test_cert_dir, false,
                                   base::FileEnumerator::FILES);
    for (base::FilePath file_name = test_enum.Next(); !file_name.empty();
         file_name = test_enum.Next()) {
      // Load the test der-encode certificate file
      if (X509_load_cert_file(lookup, file_name.value().c_str(),
                              X509_FILETYPE_ASN1) != 1) {
        LOG(ERROR) << "Load the test certificate file failed, test certificate "
                      "file path: "
                   << file_name.value();
      }
    }

    return true;
  }
#endif

  void ParseUserCertCrl(X509_LOOKUP* look_up) {
    // read cert crl from arkts
    std::vector<std::string> cert_crl_list =
        CertManagerAdapter::GetInstance().GetCertCrl(
            std::string(USER_CACERT_PREFIX), true);
    for (size_t i = 0; i < cert_crl_list.size(); i++) {
      X509_load_cert_crl(look_up, cert_crl_list[i], X509_FILETYPE_PEM);
    }
    cert_crl_list.clear();
    cert_crl_list = CertManagerAdapter::GetInstance().GetCertCrl(
        std::string(EDM_CERT), false);
    for (size_t i = 0; i < cert_crl_list.size(); i++) {
      X509_load_cert_crl(look_up, cert_crl_list[i], X509_FILETYPE_PEM);
    }
  }

  int CertVerify(const std::vector<std::string>& cert_bytes,
                 std::vector<std::string>* verified_chain) {
    if (cert_bytes.size() < MIN_CERT_NUM) {
      LOG(ERROR)
          << "CertVerify() failed, Total number of server certificate is 0";
      return X509_V_ERR_UNSPECIFIED;
    }

    std::vector<bssl::UniquePtr<X509>> server_certs;
    // Convert the cert that to be verified to X509
    // If the conversion fails, return error
    auto verified_buf =
        reinterpret_cast<const uint8_t*>(cert_bytes[0U].c_str());
    bssl::UniquePtr<X509> verified_cert(
        d2i_X509(nullptr, &verified_buf, cert_bytes[0U].size()));
    if (!verified_cert) {
      int error = ERR_get_error();
      LOG(ERROR)
          << "CertVerify() failed, convert verified_cert to X509 failed. "
             "error code: "
          << error << ", error info: " << ERR_reason_error_string(error);
      return error;
    }
    server_certs.push_back(std::move(verified_cert));
    // Convert intermediate certificate to X509
    // If the conversion fails, prevent using it during verification.
    for (size_t index = 1U; index < cert_bytes.size(); ++index) {
      auto intermediate_buf =
          reinterpret_cast<const uint8_t*>(cert_bytes[index].c_str());
      bssl::UniquePtr<X509> intermediate_cert(
          d2i_X509(nullptr, &intermediate_buf, cert_bytes[index].size()));
      if (!intermediate_cert) {
        LOG(ERROR)
            << "CertVerify() failed, convert intermediate cert to X509 failed.";
      } else {
        server_certs.push_back(std::move(intermediate_cert));
      }
    }

    if (!enable_cache_) {
      cert_crl_cache_ = bssl::UniquePtr<X509_STORE>(X509_STORE_new());
    }

    if (!cert_crl_cache_) {
      LOG(ERROR) << "CertVerify() failed, Create X509 STORE failed";
      return X509_V_ERR_UNSPECIFIED;
    }

    if (enable_cache_ && has_cache_) {
      STACK_OF(X509_LOOKUP)* sk = cert_crl_cache_.get()->get_cert_methods;
      if (sk != nullptr) {
        // The user certificates support dynamic import
        ParseUserCertCrl(sk_X509_LOOKUP_value(sk, 0));
      }
      return CertChainVerify(server_certs, cert_crl_cache_.get(),
                             verified_chain);
    }

    // Allow partial chains if at least one certificate is in trusted store
    X509_STORE_set_flags(cert_crl_cache_.get(), X509_V_FLAG_PARTIAL_CHAIN);

    // Create X509_LOOKUP, the store_ctx member of this data structure is
    // associated with the newly created certificate store ca_store
    X509_LOOKUP* look_up =
        X509_STORE_add_lookup(cert_crl_cache_.get(), X509_LOOKUP_file());
    if (!look_up) {
      LOG(ERROR) << "CertVerify() failed, Create X509 LOOKUP failed";
      return X509_V_ERR_UNSPECIFIED;
    }

    // Parse the root certificate file
    if (X509_load_cert_crl_file(look_up, ROOT_CERT, X509_FILETYPE_PEM) == 0) {
      LOG(ERROR) << "CertVerify() failed, Root certificate number is 0";
      return X509_V_ERR_UNSPECIFIED;
    }

#if defined(NET_UNITTESTS_OHOS_ENABLE)
    // When running in test mode, also layer in the test-only root certificates.
    // Note that this integration requires TestRootCerts::HasInstance() to be
    // true by the time CertVerifyProcTrustStore is created - a limitation which
    // is acceptable for the test-only code that consumes this.
    if (TestRootCerts::HasInstance()) {
      // load test certificate file
      if (!GetTestCertFile(look_up)) {
        LOG(ERROR) << "CertVerify() failed, load test certificate failed";
      }
    }
#endif

    ParseUserCertCrl(look_up);
    has_cache_ = true;
    return CertChainVerify(server_certs, cert_crl_cache_.get(), verified_chain);
  }

  // Starting at certs[start], this function searches |certs| for an issuer of
  // certs[start], then for an issuer of that issuer, and so on until it finds a
  // certificate |cert| for which |certs| does not contain an issuer of
  // |cert|. Returns a pointer to this |cert|, or nullptr if all certificates
  // while path-building from |start| have an issuer in |certs| (including if
  // there is a loop). Note that the returned certificate will be equal to
  // |start| if |start| does not have an issuer in |certs|.
  //
  // TODO(estark): when searching for an issuer, this always uses the first
  // encountered issuer in |certs|, and does not handle the situation where
  // |certs| contains more than one issuer for a given certificate.
  std::shared_ptr<const bssl::ParsedCertificate> FindLastCertWithUnknownIssuer(
      const bssl::ParsedCertificateList& certs,
      const std::shared_ptr<const bssl::ParsedCertificate>& start) {
    DCHECK_GE(certs.size(), 1u);
    std::set<std::shared_ptr<const bssl::ParsedCertificate>> used_in_path;
    std::shared_ptr<const bssl::ParsedCertificate> last = start;
    while (true) {
      used_in_path.insert(last);
      std::shared_ptr<const bssl::ParsedCertificate> last_issuer;
      // Find an issuer for |last| (which might be |last| itself if
      // self-signed).
      for (const auto& cert : certs) {
        if (cert->normalized_subject() == last->normalized_issuer()) {
          last_issuer = cert;
          break;
        }
      }
      if (!last_issuer) {
        // There is no issuer for |last| in |certs|.
        return last;
      }
      if (last_issuer->normalized_subject() ==
          last_issuer->normalized_issuer()) {
        // A chain can be built from |start| to a self-signed certificate, so
        // return nullptr to indicate that there is no certificate with an
        // unknown issuer.
        return nullptr;
      }
      if (used_in_path.find(last_issuer) != used_in_path.end()) {
        // |certs| contains a loop.
        return nullptr;
      }
      // Continue the search for |last_issuer|'s issuer.
      last = last_issuer;
    }
    NOTREACHED();
    return nullptr;
  }

  // Uses |fetcher| to fetch issuers from |uri|. If the fetch succeeds, the
  // certificate is parsed and added to |cert_list|. Returns true if the fetch
  // was successful and the result could be parsed as a certificate, and false
  // otherwise.
  bool PerformAIAFetchAndAddResultToVector(
      scoped_refptr<CertNetFetcher> fetcher,
      std::string_view uri,
      bssl::ParsedCertificateList* cert_list) {
    GURL url(uri);
    if (!url.is_valid()) {
      LOG(ERROR)
          << "PerformAIAFetchAndAddResultToVector() failed, URL is invalied";
      return false;
    }

    std::unique_ptr<CertNetFetcher::Request> request(fetcher->FetchCaIssuers(
        url, CertNetFetcher::DEFAULT, CertNetFetcher::DEFAULT));
    Error error;
    std::vector<uint8_t> aia_fetch_bytes;
    request->WaitForResult(&error, &aia_fetch_bytes);

    if (error != OK) {
      LOG(ERROR) << "PerformAIAFetchAndAddResultToVector() failed, Wait for "
                    "result failed, uri: "
                 << uri;
      return false;
    }

    bssl::CertErrors errors;
    return bssl::ParsedCertificate::CreateAndAddToVector(
        x509_util::CreateCryptoBuffer(aia_fetch_bytes),
        x509_util::DefaultParseCertificateOptions(), cert_list, &errors);
  }

  void X509CertChainVerify(const std::vector<std::string>& cert_chain,
                           std::string_view auth_type,
                           std::string_view host,
                           int* status,
                           bool* is_issued_by_known_root,
                           std::vector<std::string>* verified_chain) {
    *is_issued_by_known_root = false;

    *status = CertVerify(cert_chain, verified_chain);
  }

  // Uses X509CertChainVerify() to verify the certificates in |certs| for
  // |hostname| and returns the verification status. If the verification was
  // successful, this function populates |verify_result| and |verified_chain|;
  // otherwise it leaves them untouched.
  int AttemptVerificationAfterAIAFetch(
      const bssl::ParsedCertificateList& certs,
      const std::string& hostname,
      CertVerifyResult* verify_result,
      std::vector<std::string>* verified_chain) {
    std::vector<std::string> cert_bytes;
    for (const auto& cert : certs) {
      cert_bytes.push_back(cert->der_cert().AsString());
    }

    bool is_issued_by_known_root = false;
    std::vector<std::string> candidate_verified_chain;
    int status = 0;
    X509CertChainVerify(cert_bytes, kAuthType, hostname, &status,
                        &is_issued_by_known_root, &candidate_verified_chain);

    if (status == X509_V_OK) {
      verify_result->is_issued_by_known_root = is_issued_by_known_root;
      *verified_chain = candidate_verified_chain;
    }
    return status;
  }

  int TryVerifyWithAIAFetching(const std::vector<std::string>& cert_bytes,
                               const std::string& hostname,
                               scoped_refptr<CertNetFetcher> cert_net_fetcher,
                               CertVerifyResult* verify_result,
                               std::vector<std::string>* verified_chain) {
    if (!cert_net_fetcher) {
      LOG(ERROR)
          << "TryVerifyWithAIAFetching() failed, cert_net_fetcher is NULL";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }

    // Convert the certificates into ParsedCertificates for ease of pulling out
    // AIA URLs.
    bssl::CertErrors errors;
    bssl::ParsedCertificateList certs;
    for (const auto& cert : cert_bytes) {
      if (!bssl::ParsedCertificate::CreateAndAddToVector(
              x509_util::CreateCryptoBuffer(cert),
              x509_util::DefaultParseCertificateOptions(), &certs, &errors)) {
        LOG(ERROR) << "TryVerifyWithAIAFetching() failed, Parse cert error";
        return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
      }
    }

    if (!certs.size()) {
      LOG(ERROR) << "TryVerifyWithAIAFetching() failed, Parse cert number is 0";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }

    // Build a chain as far as possible from the target certificate at index 0,
    // using the initially provided certificates.
    std::shared_ptr<const bssl::ParsedCertificate> last_cert_with_unknown_issuer =
        FindLastCertWithUnknownIssuer(certs, certs[0]);
    if (!last_cert_with_unknown_issuer) {
      // |certs| either contains a loop, or contains a full chain to a
      // self-signed certificate. Do not attempt AIA fetches for such a chain.
      LOG(ERROR)
          << "TryVerifyWithAIAFetching() failed, Cert chain is full, do not "
             "attempt AIA fetche";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }

    unsigned int num_aia_fetches = 0;
    while (true) {
      // If chain-building has terminated in a certificate that does not have an
      // AIA URL, give up.
      //
      // TODO(estark): Instead of giving up at this point, it would be more
      // robust to go back to the certificate before |last_cert| in the chain
      // and attempt an AIA fetch from that point (if one hasn't already been
      // done). This would accomodate chains where the server serves Leaf -> I1
      // signed by a root not in the client's trust store, but AIA fetching
      // would yield an intermediate I2 signed by a root that *is* in the
      // client's trust store.
      if (!last_cert_with_unknown_issuer->has_authority_info_access()) {
        LOG(ERROR)
            << "TryVerifyWithAIAFetching() failed, Cert do not have an AIA URL";
        return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
      }

      for (const auto& uri : last_cert_with_unknown_issuer->ca_issuers_uris()) {
        num_aia_fetches++;
        if (num_aia_fetches > kMaxAIAFetches) {
          LOG(ERROR) << "TryVerifyWithAIAFetching() failed, Reach the maximum "
                        "count of "
                        "AIA fetche cert";
          return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
        }

        if (!PerformAIAFetchAndAddResultToVector(cert_net_fetcher, uri,
                                                 &certs)) {
          continue;
        }
        int status = AttemptVerificationAfterAIAFetch(
            certs, hostname, verify_result, verified_chain);
        if (status == X509_V_OK) {
          return status;
        }
      }

      // If verification still failed but the path expanded, continue to attempt
      // AIA fetches.
      std::shared_ptr<const bssl::ParsedCertificate>
          new_last_cert_with_unknown_issuer = FindLastCertWithUnknownIssuer(
              certs, last_cert_with_unknown_issuer);
      if (!new_last_cert_with_unknown_issuer ||
          new_last_cert_with_unknown_issuer == last_cert_with_unknown_issuer) {
        // The last round of AIA fetches (if there were any) didn't expand the
        // path, or it did such that |certs| now contains a full path to an
        // (untrusted) root or a loop.
        //
        // TODO(estark): As above, it would be more robust to go back one
        // certificate and attempt an AIA fetch from that point.
        LOG(ERROR) << "TryVerifyWithAIAFetching() failed, No need continue to "
                      "attempt AIA "
                      "fetche cert";
        return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
      }
      last_cert_with_unknown_issuer = new_last_cert_with_unknown_issuer;
    }

    NOTREACHED();
    return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
  }

  // Returns true if the certificate verification call was successful
  // (regardless of its result), i.e. if |verify_result| was set. Otherwise
  // returns false.
  bool VerifyFromOhosTrustManager(
      const std::vector<std::string>& cert_bytes,
      const std::string& hostname,
      int flags,
      scoped_refptr<CertNetFetcher> cert_net_fetcher,
      CertVerifyResult* verify_result) {
    int status;
    std::vector<std::string> verified_chain;

    X509CertChainVerify(cert_bytes, kAuthType, hostname, &status,
                        &verify_result->is_issued_by_known_root,
                        &verified_chain);

    // If verification resulted in a
    // X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY error,
    //  then fetch intermediates and retry.
    if (status == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY &&
        !(flags & CertVerifyProc::VERIFY_DISABLE_NETWORK_FETCHES)) {
      LOG(INFO) << "VerifyFromOhosTrustManager() failed, Cert is not trusted, "
                   "try AIA fetche";
      status = TryVerifyWithAIAFetching(cert_bytes, hostname,
                                        std::move(cert_net_fetcher),
                                        verify_result, &verified_chain);
    }

    switch (status) {
      case X509_V_ERR_UNSPECIFIED:
        return false;
      case X509_V_OK:
        break;
      case X509_V_ERR_CERT_SIGNATURE_FAILURE:
      case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        verify_result->cert_status |= CERT_STATUS_AUTHORITY_INVALID;
        break;
      case X509_V_ERR_CERT_HAS_EXPIRED:
      case X509_V_ERR_CERT_NOT_YET_VALID:
        verify_result->cert_status |= CERT_STATUS_DATE_INVALID;
        break;
      case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        verify_result->cert_status |= CERT_STATUS_DEPTH_ZERO_SELF_SIGNED_CERT;
        break;
      default:
        NOTREACHED();
        verify_result->cert_status |= CERT_STATUS_INVALID;
        break;
    }

    // Save the verified chain.
    if (!verified_chain.empty()) {
      std::vector<std::string_view> verified_chain_pieces(
          verified_chain.size());
      for (size_t i = 0; i < verified_chain.size(); i++) {
        verified_chain_pieces[i] = std::string_view(verified_chain[i]);
      }
      scoped_refptr<X509Certificate> verified_cert =
          X509Certificate::CreateFromDERCertChain(verified_chain_pieces);
      if (verified_cert.get()) {
        verify_result->verified_cert = std::move(verified_cert);
      } else {
        LOG(ERROR)
            << "VerifyFromOhosTrustManager() failed, Verified cert create "
               "from der cert chain failed";
        verify_result->cert_status |= CERT_STATUS_INVALID;
      }
    }

    // Extract the public key hashes and check whether or not any are known
    // roots. Walk from the end of the chain (root) to leaf, to optimize for
    // known root checks.
    for (auto it = verified_chain.rbegin(); it != verified_chain.rend(); ++it) {
      std::string_view spki_bytes;
      if (!asn1::ExtractSPKIFromDERCert(*it, &spki_bytes)) {
        LOG(ERROR) << "VerifyFromOhosTrustManager() failed, Extract SPKI from "
                      "der cert failed";
        verify_result->cert_status |= CERT_STATUS_INVALID;
        continue;
      }

      HashValue sha256(HASH_VALUE_SHA256);
      crypto::SHA256HashString(spki_bytes, sha256.data(),
                               crypto::kSHA256Length);
      verify_result->public_key_hashes.push_back(sha256);

      if (!verify_result->is_issued_by_known_root) {
        verify_result->is_issued_by_known_root =
            GetNetTrustAnchorHistogramIdForSPKI(sha256) != 0;
      }
    }

    // Reverse the hash list, to maintain the leaf->root ordering.
    std::reverse(verify_result->public_key_hashes.begin(),
                 verify_result->public_key_hashes.end());

    return true;
  }

#if defined(NET_UNITTESTS_OHOS_ENABLE)
  bool CheckCertificateConstraints(X509Certificate* cert) {
    bssl::CertErrors errors;
    std::string prev_issuer;
    bool prev_is_ca = false;
    if (cert == nullptr) {
      LOG(ERROR) << "CheckCertificateConstraints() failed, cert "
                    "is nullptr";
      return false;
    }

    auto parsed_cert = bssl::ParsedCertificate::Create(
        bssl::UpRef(cert->cert_buffer()),
        x509_util::DefaultParseCertificateOptions(), &errors);
    if (!parsed_cert) {
      LOG(ERROR) << "CheckCertificateConstraints() failed, cert "
                    "ParsedCertificate failed";
      return false;
    }
    // if cert has no extended_key_usage, cert is invalid
    if (!parsed_cert->has_extended_key_usage()) {
      LOG(ERROR) << "CheckCertificateConstraints() failed, cert has no "
                    "extended_key_usage";
      return false;
    }
    auto& extended_key_usage = parsed_cert->extended_key_usage();
    // if kServerAuth is not in extended_key_usage, cert is invalid
    if (std::find(extended_key_usage.cbegin(), extended_key_usage.cend(),
                  bssl::der::Input(bssl::kServerAuth)) == extended_key_usage.cend()) {
      LOG(ERROR)
          << "CheckCertificateConstraints() failed, kServerAuth is not in "
             "extended_key_usage";
      return false;
    }
    prev_issuer = parsed_cert->normalized_issuer().AsString();
    // the cert to be verified can be no basic_constraints
    if (parsed_cert->has_basic_constraints()) {
      prev_is_ca = parsed_cert->basic_constraints().is_ca;
    }

    size_t index = 0U;
    for (auto& handle : cert->intermediate_buffers()) {
      auto parsed = bssl::ParsedCertificate::Create(
          bssl::UpRef(handle.get()),
          x509_util::DefaultParseCertificateOptions(), &errors);
      // intermediate cert is not necessary
      if (!parsed) {
        continue;
      }

      // if intermediate cert is self-signed, pass
      if (parsed->normalized_issuer() == parsed->normalized_subject()) {
        continue;
      }

      // if intermediate cert is not the issuer of prev cert, pass
      if (parsed->normalized_subject() != bssl::der::Input(std::string_view(prev_issuer))) {
        continue;
      }
      prev_issuer = parsed->normalized_issuer().AsString();

      // if intermediate cert has no basic_constraints, intermediate is invalid
      if (!parsed->has_basic_constraints()) {
        LOG(ERROR) << "CheckCertificateConstraints() failed, intermediate cert "
                      "has no basic_constraints";
        return false;
      }
      auto& basic_constraints = parsed->basic_constraints();
      // if intermediate cert is not CA cert, but prev cert is CA cert,
      // intermediate is invalid
      if (prev_is_ca && !basic_constraints.is_ca) {
        LOG(ERROR) << "CheckCertificateConstraints() failed, intermediate cert "
                      "is not CA cert, but prev cert is CA cert";
        return false;
      }
      prev_is_ca = basic_constraints.is_ca;
      // if intermediate cert has path len, but path len is wrong,
      // intermediate is invalid
      if (basic_constraints.has_path_len &&
          basic_constraints.path_len != index) {
        LOG(ERROR) << "CheckCertificateConstraints() failed, intermediate cert "
                      "has no path len, or path len is wrong";
        return false;
      }
      ++index;
    }

    return true;
  }
#endif

 private:
  bool enable_cache_ = true;
  bool has_cache_ = false;
  bssl::UniquePtr<X509_STORE> cert_crl_cache_;
};

CertVerifyProcOHOS::CertVerifyProcOHOS(
    scoped_refptr<CertNetFetcher> cert_net_fetcher,
    scoped_refptr<CRLSet> crl_set,
    bool enable_cache)
    : CertVerifyProc(std::move(crl_set)),
      cert_net_fetcher_(std::move(cert_net_fetcher)) {
  if (!std::move(cert_net_fetcher)) {
  }
  inner_ = std::make_unique<VerifyProcOHOSInner>(enable_cache);
}

CertVerifyProcOHOS::~CertVerifyProcOHOS() {}

int CertVerifyProcOHOS::VerifyInternal(
    X509Certificate* cert,
    const std::string& hostname,
    const std::string& ocsp_response,
    const std::string& sct_list,
    int flags,
    CertVerifyResult* verify_result,
    const NetLogWithSource& net_log) {
  if (inner_ == nullptr) {
    return ERR_FAILED;
  }
  std::vector<std::string> cert_bytes;
  inner_->GetChainDEREncodedBytes(cert, &cert_bytes);

  if (!inner_->VerifyFromOhosTrustManager(cert_bytes, hostname, flags,
                                          cert_net_fetcher_, verify_result)) {
    LOG(ERROR) << "VerifyInternal() failed, Trust manager verify cert failed";
    return ERR_FAILED;
  }

  if (IsCertStatusError(verify_result->cert_status)) {
    return MapCertStatusToNetError(verify_result->cert_status);
  }

#if defined(NET_UNITTESTS_OHOS_ENABLE)
  if (!inner_->CheckCertificateConstraints(cert)) {
    verify_result->cert_status |= CERT_STATUS_INVALID;
    return ERR_CERT_INVALID;
  }

  if (TestRootCerts::HasInstance() &&
      !verify_result->verified_cert->intermediate_buffers().empty() &&
      TestRootCerts::GetInstance()->IsKnownRoot(x509_util::CryptoBufferAsSpan(
          verify_result->verified_cert->intermediate_buffers().back().get()))) {
    verify_result->is_issued_by_known_root = true;
  }
#endif

  LogNameNormalizationMetrics(".OHOS", verify_result->verified_cert.get(),
                              verify_result->is_issued_by_known_root);

  return OK;
}

}  // namespace net

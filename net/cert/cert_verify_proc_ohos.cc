// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/cert/cert_verify_proc_ohos.h"
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <set>
#include <string>
#include <vector>

#include "base/command_line.h"
#include "base/containers/adapters.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "content/public/common/content_switches.h"
#include "crypto/sha2.h"
#include "net/cert/asn1_util.h"
#include "net/cert/cert_net_fetcher.h"
#include "net/cert/cert_verify_result.h"
#include "net/cert/crl_set.h"
#include "net/cert/known_roots.h"
#include "net/cert/pki/cert_errors.h"
#include "net/cert/pki/parsed_certificate.h"
#include "net/cert/x509_certificate.h"
#include "net/cert/x509_util.h"
#include "ohos_adapter_helper.h"
#include "openssl/err.h"
#include "openssl/ossl_typ.h"
#include "openssl/x509.h"
#include "openssl/x509_vfy.h"
#include "third_party/boringssl/src/crypto/x509/internal.h"
#include "url/gurl.h"

#include "base/files/file_util.h"
#include "base/files/file_enumerator.h"

#define ROOT_CERT_PATH "/system/etc/security/certificates/"
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

const char kUserCaPath1[] = "/data/certificates/user_cacerts/0";
const char kUserCaPath2[] = "/data/certificates/user_cacerts/100";

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

void X509_d2i_free(X509* server_cert[], uint32_t server_cert_sum) {
  uint32_t i;
  for (i = 0; i < server_cert_sum; i++) {
    X509_free(server_cert[i]);
  }
}

X509* p2i_X509(const char* pem) {
  BIO* bio = BIO_new_mem_buf(pem, strlen(pem));
  if (!bio) {
    LOG(ERROR) << "Create x509 from PEM, BIO new memory buffer failed";
    return nullptr;
  }
  auto x509 = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
  if (x509 == nullptr) {
    LOG(ERROR) << "Create x509 from PEM, x509 is null";
    BIO_free(bio);
    return nullptr;
  }
  BIO_free(bio);

  return x509;
}

int CertChainRootVerify(X509* server_cert[],
                        int32_t index,
                        X509_STORE* ca_store) {
  if (!server_cert || !ca_store || index <= 0) {
    return X509_V_ERR_UNSPECIFIED;
  }

  STACK_OF(X509)* ca_stack = nullptr;
  X509_STORE_CTX* ctx = nullptr;
  ctx = X509_STORE_CTX_new();
  if (!ctx) {
    return X509_V_ERR_UNSPECIFIED;
  }
  X509_STORE_CTX_init(ctx, ca_store, server_cert[index], ca_stack);
  if (!X509_verify_cert(ctx)) {
    auto error = ctx->error;
    LOG(ERROR) << "Certificate verify error: " << error
               << ", Certificate verify info: "
               << X509_verify_cert_error_string(ctx->error);
    X509_STORE_CTX_free(ctx);
    return error;
  }
  X509_STORE_CTX_free(ctx);
  return X509_V_OK;
}

int GetVerifiedChain(X509_STORE_CTX* ctx,
                     std::vector<std::string>* verified_chain) {
  uint8_t* cert_der = nullptr;
  uint8_t* buf = nullptr;
  X509* x509 = nullptr;
  int cert_len = 0;

  verified_chain->reserve(1 + sk_X509_num(ctx->chain));
  for (unsigned long i = 0; i < sk_X509_num(ctx->chain); i++) {
    x509 = sk_X509_value(ctx->chain, i);
    cert_len = i2d_X509(x509, nullptr);
    if (cert_len <= 0) {
      LOG(ERROR)
          << "I2d_X509 get cert length, cert length is less than or equal to 0";
      return X509_V_ERR_UNSPECIFIED;
    }

    buf = (uint8_t*)OPENSSL_malloc(cert_len);
    if (buf == nullptr) {
      LOG(ERROR) << "OPENSSL_malloc failed";
      return X509_V_ERR_UNSPECIFIED;
    }

    // The buf pointer of the i2d_X509 function changed during the conversion
    // process, and finally changed to buf = cert_der + cert_len;
    cert_der = buf;
    i2d_X509(x509, &buf);

    auto cert_der_span =
        base::make_span(cert_der, base::checked_cast<size_t>(cert_len));
    bssl::UniquePtr<CRYPTO_BUFFER> cert_buffer =
        net::x509_util::CreateCryptoBuffer(cert_der_span);
    if (cert_buffer == nullptr) {
      LOG(ERROR) << "Cert buffer is nullptr";
      OPENSSL_free(cert_der);
      return X509_V_ERR_UNSPECIFIED;
    }

    verified_chain->emplace_back(
        net::x509_util::CryptoBufferAsStringPiece(cert_buffer.get()));

    OPENSSL_free(cert_der);
    cert_der = nullptr;
    buf = nullptr;
  }

  return X509_V_OK;
}

int CertChainVerify(X509* server_cert[],
                    int32_t server_cert_sum,
                    X509_STORE* ca_store,
                    std::vector<std::string>* verified_chain) {
  int32_t server_cert_index;
  STACK_OF(X509)* ca_stack = nullptr;
  X509_STORE_CTX* ctx = nullptr;

  // Add the server certificate to the certificate store
  for (server_cert_index = server_cert_sum - 1; server_cert_index > 0;
       server_cert_index--) {
    int ret = CertChainRootVerify(server_cert, server_cert_index, ca_store);
    if (ret == X509_V_OK) {
      for (int cert_index = server_cert_index; cert_index > 0; cert_index--) {
        X509_STORE_add_cert(ca_store, server_cert[cert_index]);
      }
      break;
    }
  }

  // Create certificate store context function
  ctx = X509_STORE_CTX_new();
  if (ctx == nullptr) {
    LOG(ERROR) << "Create certificate store context function failed";
    X509_d2i_free(server_cert, server_cert_sum);
    X509_STORE_free(ca_store);
    return X509_V_ERR_UNSPECIFIED;
  }

  X509_STORE_CTX_init(ctx, ca_store, server_cert[0], ca_stack);

  // If verification fails, for detailed error information, see
  // X509_verify_cert_error_string
  if (!X509_verify_cert(ctx)) {
    auto error = ctx->error;
    LOG(ERROR) << "Certificate verify error: " << error
               << ", Certificate verify info: "
               << X509_verify_cert_error_string(ctx->error)
               << ", Total number of server certificate: " << server_cert_sum;
    X509_d2i_free(server_cert, server_cert_sum);
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(ca_store);
    return error;
  }

  if (GetVerifiedChain(ctx, verified_chain) != X509_V_OK) {
    LOG(ERROR) << "Get verified chain failed";
    X509_d2i_free(server_cert, server_cert_sum);
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(ca_store);
    return X509_V_ERR_UNSPECIFIED;
  }

  X509_STORE_CTX_free(ctx);
  X509_d2i_free(server_cert, server_cert_sum);
  X509_STORE_free(ca_store);

  return X509_V_OK;
}

int32_t GetApplicationApiVersion() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kOhosAppApiVersion)) {
    LOG(ERROR) << "kOhosAppApiVersion not exist";
    return -1;
  }
  std::string apiVersion =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kOhosAppApiVersion);
  if (apiVersion.empty()) {
    return -1;
  }
  return std::stoi(apiVersion);
}

void AddAppCert(const base::StringPiece &hostname, X509_STORE* ca_store) {
  auto RootCertDataAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetRootCertDataAdapter();
  if (!RootCertDataAdapter || !ca_store) {
    LOG(ERROR)
        << "Get cert info from cert manager, root cert data adapter is null";
    return;
  }

  X509_LOOKUP* ca_look_up = X509_STORE_add_lookup(ca_store, X509_LOOKUP_hash_dir());
  if (ca_look_up == nullptr) {
    LOG(ERROR) << "Create X509 LOOKUP failed";
    return;
  }

  X509_LOOKUP_add_dir(ca_look_up, kUserCaPath1, X509_FILETYPE_PEM);
  X509_LOOKUP_add_dir(ca_look_up, kUserCaPath2, X509_FILETYPE_PEM);

  // add app ca
  std::vector<std::string> app_certs_path;
  std::string host(hostname.data(), hostname.size());
  if (RootCertDataAdapter->GetTrustAnchorsForHostName(host, app_certs_path)) {
    for (size_t cert_index = 0; cert_index < app_certs_path.size(); cert_index++) {
      int ret = X509_LOOKUP_add_dir(ca_look_up, app_certs_path[cert_index].c_str(), X509_FILETYPE_PEM);
      if (ret == 0) {
        LOG(WARNING) << "Add app cert failed, path:" << app_certs_path[cert_index]
          << " ret:" << ret;
      }
    }
  } else {
    LOG(ERROR) << "GetTrustAnchorsForHostName host:" << host << " failed.";
  }

  return;
}

int CertVerify(const std::vector<std::string>& cert_bytes,
               std::vector<std::string>* verified_chain,
               base::StringPiece hostname) {
  uint32_t server_cert_sum;
  const unsigned char* der_encoded_tmp = nullptr;
  uint32_t i;
  X509_STORE* ca_store = nullptr;
  X509_LOOKUP* look_up = nullptr;

  server_cert_sum = cert_bytes.size();
  if (server_cert_sum < MIN_CERT_NUM) {
    LOG(ERROR) << "Total number of server certificate is error";
    return X509_V_ERR_UNSPECIFIED;
  }

  // Convert cert_bytes to internal X509 data structure(server_cert[]), If the
  // conversion fails, see ERR_reason_error_string()/kLibraryNames
  X509* server_cert[server_cert_sum];
  for (i = 0; i < server_cert_sum; i++) {
    der_encoded_tmp = (unsigned char*)cert_bytes[i].c_str();
    server_cert[i] = d2i_X509(nullptr, &der_encoded_tmp, cert_bytes[i].size());
    if (server_cert[i] == nullptr) {
      LOG(ERROR)
          << "Server certificate DEREncoded converted to X509 error, Reason: "
          << ERR_reason_error_string(ERR_get_error())
          << "\nTotal number of server certificate: " << server_cert_sum
          << "\nError certificate count:" << i;
      ERR_clear_error();
      X509_d2i_free(server_cert, i);
      return X509_V_ERR_UNSPECIFIED;
    }
  }

  // Create X509 certificate store
  ca_store = X509_STORE_new();
  if (ca_store == nullptr) {
    LOG(ERROR) << "Create X509 certificate store failed";
    X509_d2i_free(server_cert, server_cert_sum);
    return X509_V_ERR_UNSPECIFIED;
  }

  // Allow partial chains if at least one certificate is in trusted store
  X509_STORE_set_flags(ca_store, X509_V_FLAG_PARTIAL_CHAIN);

  // Create X509_LOOKUP, the store_ctx member of this data structure is
  // associated with the newly created certificate store ca_store
  look_up = X509_STORE_add_lookup(ca_store, X509_LOOKUP_hash_dir());
  if (look_up == nullptr) {
    LOG(ERROR) << "Create X509 LOOKUP failed";
    X509_d2i_free(server_cert, server_cert_sum);
    X509_STORE_free(ca_store);
    return X509_V_ERR_UNSPECIFIED;
  }

  // Parse the root certificate dir
  if (X509_LOOKUP_add_dir(look_up, ROOT_CERT_PATH, X509_FILETYPE_PEM) == 0) {
    LOG(ERROR) << "Add root certificate dir failed";
    X509_d2i_free(server_cert, server_cert_sum);
    X509_STORE_free(ca_store);
    return X509_V_ERR_UNSPECIFIED;
  }

  // Add user cert to ca store
  AddAppCert(hostname, ca_store);

  return CertChainVerify(server_cert, server_cert_sum, ca_store,
                         verified_chain);
}

// Starting at certs[start], this function searches |certs| for an issuer of
// certs[start], then for an issuer of that issuer, and so on until it finds a
// certificate |cert| for which |certs| does not contain an issuer of
// |cert|. Returns a pointer to this |cert|, or nullptr if all certificates
// while path-building from |start| have an issuer in |certs| (including if
// there is a loop). Note that the returned certificate will be equal to |start|
// if |start| does not have an issuer in |certs|.
//
// TODO(estark): when searching for an issuer, this always uses the first
// encountered issuer in |certs|, and does not handle the situation where
// |certs| contains more than one issuer for a given certificate.
std::shared_ptr<const ParsedCertificate> FindLastCertWithUnknownIssuer(
    const ParsedCertificateList& certs,
    const std::shared_ptr<const ParsedCertificate>& start) {
  DCHECK_GE(certs.size(), 1u);
  std::set<std::shared_ptr<const ParsedCertificate>> used_in_path;
  std::shared_ptr<const ParsedCertificate> last = start;
  while (true) {
    used_in_path.insert(last);
    std::shared_ptr<const ParsedCertificate> last_issuer;
    // Find an issuer for |last| (which might be |last| itself if self-signed).
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
    if (last_issuer->normalized_subject() == last_issuer->normalized_issuer()) {
      // A chain can be built from |start| to a self-signed certificate, so
      // return nullptr to indicate that there is no certificate with an unknown
      // issuer.
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
// certificate is parsed and added to |cert_list|. Returns true if the fetch was
// successful and the result could be parsed as a certificate, and false
// otherwise.
bool PerformAIAFetchAndAddResultToVector(scoped_refptr<CertNetFetcher> fetcher,
                                         std::string_view uri,
                                         ParsedCertificateList* cert_list) {
  GURL url(uri);
  if (!url.is_valid()) {
    LOG(ERROR) << "PerformAIAFetchAndAddResultToVector: URL is invalied";
    return false;
  }

  std::unique_ptr<CertNetFetcher::Request> request(fetcher->FetchCaIssuers(
      url, CertNetFetcher::DEFAULT, CertNetFetcher::DEFAULT));
  Error error;
  std::vector<uint8_t> aia_fetch_bytes;
  request->WaitForResult(&error, &aia_fetch_bytes);

  if (error != OK) {
    LOG(ERROR) << "PerformAIAFetchAndAddResultToVector: Wait for result failed, uri: " << uri;
    return false;
  }

  CertErrors errors;
  return ParsedCertificate::CreateAndAddToVector(
      x509_util::CreateCryptoBuffer(aia_fetch_bytes),
      x509_util::DefaultParseCertificateOptions(), cert_list, &errors);
}

void X509CertChainVerify(const std::vector<std::string>& cert_chain,
                         base::StringPiece auth_type,
                         base::StringPiece host,
                         int* status,
                         bool* is_issued_by_known_root,
                         std::vector<std::string>* verified_chain) {
  *is_issued_by_known_root = false;

  *status = CertVerify(cert_chain, verified_chain, host);
}

// Uses X509CertChainVerify() to verify the certificates in |certs| for
// |hostname| and returns the verification status. If the verification was
// successful, this function populates |verify_result| and |verified_chain|;
// otherwise it leaves them untouched.
int AttemptVerificationAfterAIAFetch(const ParsedCertificateList& certs,
                                     const std::string& hostname,
                                     CertVerifyResult* verify_result,
                                     std::vector<std::string>* verified_chain) {
  std::vector<std::string> cert_bytes;
  for (const auto& cert : certs) {
    cert_bytes.push_back(cert->der_cert().AsString());
  }

  bool is_issued_by_known_root;
  std::vector<std::string> candidate_verified_chain;
  int status;
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
    return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
  }

  // Convert the certificates into ParsedCertificates for ease of pulling out
  // AIA URLs.
  CertErrors errors;
  ParsedCertificateList certs;
  for (const auto& cert : cert_bytes) {
    if (!ParsedCertificate::CreateAndAddToVector(
            x509_util::CreateCryptoBuffer(cert),
            x509_util::DefaultParseCertificateOptions(), &certs, &errors)) {
      LOG(ERROR) << "TryVerifyWithAIAFetching: Parse cert error";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }
  }

  if (!certs.size()) {
    LOG(ERROR) << "TryVerifyWithAIAFetching: Parse cert number is 0";
    return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
  }

  // Build a chain as far as possible from the target certificate at index 0,
  // using the initially provided certificates.
  std::shared_ptr<const ParsedCertificate> last_cert_with_unknown_issuer =
      FindLastCertWithUnknownIssuer(certs, certs[0]);
  if (!last_cert_with_unknown_issuer) {
    // |certs| either contains a loop, or contains a full chain to a self-signed
    // certificate. Do not attempt AIA fetches for such a chain.
    LOG(ERROR) << "TryVerifyWithAIAFetching: Cert chain is full, do not attempt AIA fetche";
    return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
  }

  unsigned int num_aia_fetches = 0;
  while (true) {
    // If chain-building has terminated in a certificate that does not have an
    // AIA URL, give up.
    //
    // TODO(estark): Instead of giving up at this point, it would be more robust
    // to go back to the certificate before |last_cert| in the chain and attempt
    // an AIA fetch from that point (if one hasn't already been done). This
    // would accomodate chains where the server serves Leaf -> I1 signed by a
    // root not in the client's trust store, but AIA fetching would yield an
    // intermediate I2 signed by a root that *is* in the client's trust store.
    if (!last_cert_with_unknown_issuer->has_authority_info_access()) {
      LOG(ERROR) << "TryVerifyWithAIAFetching: Cert do not have an AIA URL";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }

    for (const auto& uri : last_cert_with_unknown_issuer->ca_issuers_uris()) {
      num_aia_fetches++;
      if (num_aia_fetches > kMaxAIAFetches) {
        LOG(ERROR) << "TryVerifyWithAIAFetching: Reach the maximum count of AIA fetche cert";
        return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
      }

      if (!PerformAIAFetchAndAddResultToVector(cert_net_fetcher, uri, &certs))
        continue;
      int status = AttemptVerificationAfterAIAFetch(
          certs, hostname, verify_result, verified_chain);
      if (status == X509_V_OK) {
        return status;
      }
    }

    // If verification still failed but the path expanded, continue to attempt
    // AIA fetches.
    std::shared_ptr<const ParsedCertificate> new_last_cert_with_unknown_issuer =
        FindLastCertWithUnknownIssuer(certs, last_cert_with_unknown_issuer);
    if (!new_last_cert_with_unknown_issuer ||
        new_last_cert_with_unknown_issuer == last_cert_with_unknown_issuer) {
      // The last round of AIA fetches (if there were any) didn't expand the
      // path, or it did such that |certs| now contains a full path to an
      // (untrusted) root or a loop.
      //
      // TODO(estark): As above, it would be more robust to go back one
      // certificate and attempt an AIA fetch from that point.
      LOG(ERROR) << "TryVerifyWithAIAFetching: No need continue to attempt AIA fetche cert";
      return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
    }
    last_cert_with_unknown_issuer = new_last_cert_with_unknown_issuer;
  }

  NOTREACHED();
  return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
}

// Returns true if the certificate verification call was successful (regardless
// of its result), i.e. if |verify_result| was set. Otherwise returns false.
bool VerifyFromOhosTrustManager(const std::vector<std::string>& cert_bytes,
                                const std::string& hostname,
                                scoped_refptr<CertNetFetcher> cert_net_fetcher,
                                CertVerifyResult* verify_result) {
  int status;
  std::vector<std::string> verified_chain;

  X509CertChainVerify(cert_bytes, kAuthType, hostname, &status,
                      &verify_result->is_issued_by_known_root, &verified_chain);

  // If verification resulted in a X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY
  // error,
  //  then fetch intermediates and retry.
  if (status == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY) {
    LOG(INFO) << "VerifyFromOhosTrustManager: Cert is not trusted, try AIA fetche";
    status = TryVerifyWithAIAFetching(cert_bytes, hostname,
                                      std::move(cert_net_fetcher),
                                      verify_result, &verified_chain);
  }

  switch (status) {
    case X509_V_ERR_UNSPECIFIED:
      return false;
    case X509_V_OK:
      break;
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
    std::vector<base::StringPiece> verified_chain_pieces(verified_chain.size());
    for (size_t i = 0; i < verified_chain.size(); i++) {
      verified_chain_pieces[i] = base::StringPiece(verified_chain[i]);
    }
    scoped_refptr<X509Certificate> verified_cert =
        X509Certificate::CreateFromDERCertChain(verified_chain_pieces);
    if (verified_cert.get())
      verify_result->verified_cert = std::move(verified_cert);
    else {
      LOG(ERROR) << "VerifyFromOhosTrustManager: Verified cert create from der cert chain failed";
      verify_result->cert_status |= CERT_STATUS_INVALID;
    }
  }

  // Extract the public key hashes and check whether or not any are known
  // roots. Walk from the end of the chain (root) to leaf, to optimize for
  // known root checks.
  for (const auto& cert : base::Reversed(verified_chain)) {
    base::StringPiece spki_bytes;
    if (!asn1::ExtractSPKIFromDERCert(cert, &spki_bytes)) {
      LOG(ERROR) << "VerifyFromOhosTrustManager: Extract SPKI from der cert failed";
      verify_result->cert_status |= CERT_STATUS_INVALID;
      continue;
    }

    HashValue sha256(HASH_VALUE_SHA256);
    crypto::SHA256HashString(spki_bytes, sha256.data(), crypto::kSHA256Length);
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

CertVerifyProcOHOS::CertVerifyProcOHOS(
    scoped_refptr<CertNetFetcher> cert_net_fetcher)
    : CertVerifyProc(CRLSet::BuiltinCRLSet()),
      cert_net_fetcher_(std::move(cert_net_fetcher)) {
  if (!std::move(cert_net_fetcher)) {
  }
}

CertVerifyProcOHOS::~CertVerifyProcOHOS() {}

bool CertVerifyProcOHOS::SupportsAdditionalTrustAnchors() const {
  return false;
}

int CertVerifyProcOHOS::VerifyInternal(
    X509Certificate* cert,
    const std::string& hostname,
    const std::string& ocsp_response,
    const std::string& sct_list,
    int flags,
    const CertificateList& additional_trust_anchors,
    CertVerifyResult* verify_result,
    const NetLogWithSource& net_log) {
  std::vector<std::string> cert_bytes;
  GetChainDEREncodedBytes(cert, &cert_bytes);

  if (!VerifyFromOhosTrustManager(cert_bytes, hostname, cert_net_fetcher_,
                                  verify_result)) {
    LOG(ERROR)
        << "VerifyInternal: Trust manager verify cert failed";
    NOTREACHED();
    return ERR_FAILED;
  }

  if (IsCertStatusError(verify_result->cert_status)) {
    return MapCertStatusToNetError(verify_result->cert_status);
  }

  return OK;
}

}  // namespace net

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"
#include "net/cert/cert_verify_proc_ohos.h"
#include "net/cert/test_root_certs.h"
#include "net/cert/x509_certificate.h"
#include "net/tools/transport_security_state_generator/cert_util.h"
#include "third_party/boringssl/src/include/openssl/pool.h"
#include "third_party/boringssl/src/include/openssl/x509.h"

namespace net {
namespace {
const char kUserCaBasePath[] = "/data/certificates/user_cacerts/test/";
}
base::FilePath GetTmpCertDir() {
  base::FilePath test_cert_dir(kUserCaBasePath);
  if (!base::DirectoryExists(test_cert_dir) &&
      !base::CreateDirectory(test_cert_dir)) {
    return base::FilePath();
  }
  return test_cert_dir;
}

bool TestRootCerts::AddImpl(X509Certificate* certificate) {
#if defined(NET_UNITTESTS_OHOS_ENABLE)
  base::FilePath test_cert_dir = GetTmpCertDir();
  // Convert DER to PEM format for OHOS compatibility
  std::string pem_encoded;
  if (!X509Certificate::GetPEMEncoded(certificate->cert_buffer(),
                                      &pem_encoded)) {
    return false;
  }
  bssl::UniquePtr<X509> x509 = GetX509CertificateFromPEM(pem_encoded);
  std::stringstream ss;
  ss << std::hex << X509_NAME_hash(X509_get_subject_name(x509.get())) << ".0";

  test_cert_dir = test_cert_dir.Append(ss.str());
  const int write_size = base::WriteFile(test_cert_dir, pem_encoded);
  return write_size == static_cast<int>(pem_encoded.size());
#else
  return true;
#endif
}

void TestRootCerts::ClearImpl() {
#if defined(NET_UNITTESTS_OHOS_ENABLE)
  base::FilePath test_cert_dir = GetTmpCertDir();
  if (base::DirectoryExists(test_cert_dir)) {
    base::DeletePathRecursively(test_cert_dir);
  }
#endif
}

TestRootCerts::~TestRootCerts() = default;

void TestRootCerts::Init() {}

}  // namespace net

#include "net/cert/test_root_certs.h"

#include "base/files/file_util.h"
#include "net/cert/cert_verify_proc_ohos.h"
#include "net/cert/x509_certificate.h"
#include "third_party/boringssl/src/include/openssl/pool.h"

namespace net {

bool TestRootCerts::AddImpl(X509Certificate* certificate) {
#if defined(NET_UNITTESTS_OHOS_ENABLE)
  base::FilePath test_cert_dir;
  if (!base::GetCurrentDirectory(&test_cert_dir) || test_cert_dir.empty()) {
    return false;
  }
  const std::string net_test_tmp_dir = "tmp/net_test_tmp_dir";
  test_cert_dir = test_cert_dir.Append(net_test_tmp_dir);
  if (!base::DirectoryExists(test_cert_dir) &&
      !base::CreateDirectory(test_cert_dir)) {
    return false;
  }

  base::FilePath test_cert_file;
  if (!base::CreateTemporaryFileInDir(test_cert_dir, &test_cert_file) ||
      test_cert_file.empty()) {
    return false;
  }

  auto data_buf = reinterpret_cast<const char*>(
      CRYPTO_BUFFER_data(certificate->cert_buffer()));
  const int data_size = CRYPTO_BUFFER_len(certificate->cert_buffer());
  std::string_view cert_data_view(data_buf, data_size);
  bool write_success = base::WriteFile(test_cert_file, cert_data_view);
  return write_success;
#else
  return true;
#endif
}

void TestRootCerts::ClearImpl() {
#if defined(NET_UNITTESTS_OHOS_ENABLE)
  base::FilePath test_cert_dir;
  if (!base::GetCurrentDirectory(&test_cert_dir) || test_cert_dir.empty()) {
    return;
  }
  const std::string net_test_tmp_dir = "tmp/net_test_tmp_dir";
  test_cert_dir = test_cert_dir.Append(net_test_tmp_dir);
  if (base::DirectoryExists(test_cert_dir)) {
    base::DeletePathRecursively(test_cert_dir);
  }
#endif
}

TestRootCerts::~TestRootCerts() = default;

void TestRootCerts::Init() {}

}  // namespace net

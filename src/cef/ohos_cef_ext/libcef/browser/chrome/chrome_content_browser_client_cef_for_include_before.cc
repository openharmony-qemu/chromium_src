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

public:
#if BUILDFLAG(IS_ARKWEB)
explicit CefSelectClientCertificateCallbackImpl(
      std::unique_ptr<content::ClientCertificateDelegate> delegate,
      const std::string& host,
      int port)
      : delegate_(std::move(delegate)),
        host_(host),
        port_(port) {}
 
~CefSelectClientCertificateCallbackImpl() override {
    // If Select has not been called, call it with NULL to continue without any
    // client certificate.
  if (!finsh_ && delegate_) {
    DoCancel();
  }
}
void Select(const CefString& private_key_file,
              const CefString& cert_chain_file) override {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::Select";
  if (!finsh_ && delegate_) {
    finsh_ = true;
    DoSelect(private_key_file, cert_chain_file);
  }
}
 
void Cancel() override {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::Cancel";
  if (!finsh_ && delegate_) {
    finsh_ = true;
    DoCancel();
  }
}
 
void Ignore() override {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::Ignore";
  if (!finsh_ && delegate_) {
    finsh_ = true;
    DoIgnore();
  }
}
#else
void Cancel() override {}
 
void Ignore() override {}
#endif

private:
void DoCancel() {
  if (CEF_CURRENTLY_ON_UIT()) {
    RunCancelNow(std::move(delegate_), host_, port_);
  } else {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&CefSelectClientCertificateCallbackImpl::RunCancelNow,
                       std::move(delegate_), host_, port_));
  }
}
 
void DoIgnore() {
  if (CEF_CURRENTLY_ON_UIT()) {
    RunIgnoreNow(std::move(delegate_), host_, port_);
  } else {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&CefSelectClientCertificateCallbackImpl::RunIgnoreNow,
                       std::move(delegate_), host_, port_));
  }
}
 
void DoSelect(const std::string& private_key_file,
              const std::string& cert_chain_file) {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::DoSelect";
  if (CEF_CURRENTLY_ON_UIT()) {
    RunSelectNow(std::move(delegate_), private_key_file, cert_chain_file,
                 host_, port_);
  } else {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&CefSelectClientCertificateCallbackImpl::RunSelectNow,
                       std::move(delegate_), private_key_file,
                       cert_chain_file, host_, port_));
  }
}

#if BUILDFLAG(IS_ARKWEB)
static scoped_refptr<net::SSLPrivateKey> WrapOpenSSLPrivateKey(
    bssl::UniquePtr<EVP_PKEY> key) {
  if (!key) {
    return nullptr;
  }

  return base::MakeRefCounted<net::ThreadedSSLPrivateKey>(
      std::make_unique<SSLPlatformKey>(std::move(key)),
      net::GetSSLPlatformKeyTaskRunner());
}

static net::ClientCertIdentityList ClientCertIdentityListFromCertificateList(
    const net::CertificateList& certs) {
  net::ClientCertIdentityList result;
  for (const auto& cert : certs) {
    result.push_back(std::make_unique<ClientCertIdentityOhos>(cert, nullptr));
  }

  return result;
}

static void AcquirePrivateKey(
    std::unique_ptr<content::ClientCertificateDelegate> delegate,
    CefRefPtr<CefX509Certificate> cert,
    const std::string& private_key_file,
    std::string& pkcs8) {
#if BUILDFLAG(ARKWEB_CA)
  scoped_refptr<net::SSLPrivateKey> ssl_private_key = nullptr;
  if (GetApplicationApiVersion() < APPLICATION_API_10) {
    CBS cbs;
    CBS_init(&cbs, reinterpret_cast<const uint8_t*>(pkcs8.data()),
             pkcs8.size());
    bssl::UniquePtr<EVP_PKEY> pkey(EVP_parse_private_key(&cbs));
    if (!pkey || CBS_len(&cbs) != 0) {
      LOG(ERROR) << "AcquirePrivateKey: EVP parse private key failed, pkey = "
                 << pkey << ", CBS length = " << CBS_len(&cbs);
      return;
    }

    ssl_private_key = WrapOpenSSLPrivateKey(std::move(pkey));
    if (!ssl_private_key) {
      LOG(ERROR) << "AcquirePrivateKey: ssl private key parse failed";
      return;
    }
  } else {
    ssl_private_key = WrapOpenSSLPrivateKeyOHOS(private_key_file);
    if (!ssl_private_key) {
      LOG(ERROR) << "AcquirePrivateKey: ssl private key parse failed";
      return;
    }
  }
#else
  CBS cbs;
  CBS_init(&cbs, reinterpret_cast<const uint8_t*>(pkcs8.data()), pkcs8.size());
  bssl::UniquePtr<EVP_PKEY> pkey(EVP_parse_private_key(&cbs));
  if (!pkey || CBS_len(&cbs) != 0) {
    LOG(ERROR) << "AcquirePrivateKey: EVP parse private key failed, pkey = "
               << pkey << ", CBS length = " << CBS_len(&cbs);
    return;
  }

  scoped_refptr<net::SSLPrivateKey> ssl_private_key =
      WrapOpenSSLPrivateKey(std::move(pkey));
  if (!ssl_private_key) {
    LOG(ERROR) << "AcquirePrivateKey: ssl private key parse failed";
    return;
  }
#endif  // BUILDFLAG(ARKWEB_CA)
  RunWithPrivateKey(std::move(delegate), cert, ssl_private_key);
}

static void RunSelectNow(
    std::unique_ptr<content::ClientCertificateDelegate> delegate,
    const std::string& private_key_file,
    const std::string& cert_chain_file,
    const std::string& host,
    int port) {
  LOG(INFO) << "CefSelectClientCertificateCallbackImpl::RunSelectNow";
  CEF_REQUIRE_UIT();
#if BUILDFLAG(ARKWEB_CA)
  net::CertificateList certsList;
  if (GetApplicationApiVersion() < APPLICATION_API_10) {
    // Client certificate file read
    std::string cert_data;
    base::FilePath src_root_cert;
    base::PathService::Get(base::DIR_SOURCE_ROOT, &src_root_cert);
    if (!base::ReadFileToString(src_root_cert.AppendASCII(cert_chain_file),
                                &cert_data)) {
      LOG(ERROR) << "RunSelectNow: read cert file to string failed";
      return;
    }

    // Convert the client certificates file to X509
    certsList = net::X509Certificate::CreateCertificateListFromBytes(
        base::as_bytes(base::make_span(cert_data)),
        net::X509Certificate::FORMAT_AUTO);
    if (certsList.empty()) {
      LOG(ERROR) << "RunSelectNow: certs list is empty";
      return;
    }
  } else {
    // Get client certificate from ohos cert manager
    auto RootCertDataAdapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetCertManagerAdapter();
    if (RootCertDataAdapter == nullptr) {
      LOG(ERROR) << "RunSelectNow: root cert data adapter is null";
      return;
    }
    char* uri = new char[private_key_file.length() + 1];
    if (uri == nullptr) {
      LOG(ERROR) << "RunSelectNow: new uri memory failed";
      return;
    }

    uint32_t i = 0;
    for (; i < private_key_file.length(); i++) {
      uri[i] = private_key_file[i];
    }
    uri[i] = '\0';

    auto certMaxSize = RootCertDataAdapter->GetAppCertMaxSize();
    uint8_t* certData = new uint8_t[certMaxSize];
    if (certData == nullptr) {
      LOG(ERROR) << "RunSelectNow: new cert data memory failed";
      delete[] uri;
      return;
    }

    if (memset_s(certData, certMaxSize, 0, certMaxSize) != EOK) {
      delete[] uri;
      delete[] certData;
      return;
    }
    uint32_t len = 0;
    RootCertDataAdapter->GetAppCert((uint8_t*)uri, certData, &len);
    if (len == 0) {
      LOG(ERROR) << "RunSelectNow: get app cert failed";
      delete[] uri;
      delete[] certData;
      return;
    }

    certsList = net::X509Certificate::CreateCertificateListFromBytes(
        base::as_bytes(
            base::make_span(static_cast<const uint8_t*>(certData), len)),
        net::X509Certificate::FORMAT_AUTO);
    if (certsList.empty()) {
      LOG(ERROR) << "RunSelectNow: certs list is empty";
      delete[] uri;
      delete[] certData;
      return;
    }

    delete[] uri;
    delete[] certData;
  }
#else
  // Client certificate file read
  std::string cert_data;
  base::FilePath src_root_cert;
  base::PathService::Get(base::DIR_SOURCE_ROOT, &src_root_cert);
  if (!base::ReadFileToString(src_root_cert.AppendASCII(cert_chain_file),
                              &cert_data)) {
    LOG(ERROR) << "RunSelectNow: read cert file to string failed";
    return;
  }

  // Convert the client certificates file to X509
  net::CertificateList certsList =
      net::X509Certificate::CreateCertificateListFromBytes(
          base::as_bytes(base::make_span(cert_data)),
          net::X509Certificate::FORMAT_AUTO);
  if (certsList.empty()) {
    LOG(ERROR) << "RunSelectNow: certs list is empty";
    return;
  }
#endif  // BUILDFLAG(ARKWEB_CA)

  auto client_certs = ClientCertIdentityListFromCertificateList(certsList);
  CefRequestHandler::X509CertificateList certs;
  for (net::ClientCertIdentityList::iterator iter = client_certs.begin();
       iter != client_certs.end(); iter++) {
    certs.push_back(new CefX509CertificateImpl(std::move(*iter)));
  }

  std::vector<bssl::UniquePtr<CRYPTO_BUFFER>> intermediates;
  for (size_t i = 1; i < certsList.size(); ++i) {
    intermediates.push_back(bssl::UpRef(certsList[i]->cert_buffer()));
  }

  scoped_refptr<net::X509Certificate> cert_X509(
      net::X509Certificate::CreateFromBuffer(
          bssl::UpRef(certsList[0]->cert_buffer()), std::move(intermediates)));

  // Save the converted X509 format certificate
  CefX509CertificateImpl* certImpl =
      static_cast<CefX509CertificateImpl*>(certs[0].get());
  certImpl->setClientCert(cert_X509);

  // Private key file read
  std::string prikey_data;
#if BUILDFLAG(ARKWEB_CA)
  if (GetApplicationApiVersion() < APPLICATION_API_10) {
    base::FilePath src_root_prikey;
    base::PathService::Get(base::DIR_SOURCE_ROOT, &src_root_prikey);
    if (!base::ReadFileToString(src_root_prikey.AppendASCII(private_key_file),
                                &prikey_data)) {
      LOG(ERROR) << "RunSelectNow: private key file to string failed";
      return;
    }
  }
#else
  base::FilePath src_root_prikey;
  base::PathService::Get(base::DIR_SOURCE_ROOT, &src_root_prikey);
  if (!base::ReadFileToString(src_root_prikey.AppendASCII(private_key_file),
                              &prikey_data)) {
    LOG(ERROR) << "RunSelectNow: private key file to string failed";
    return;
  }
#endif  // BUILDFLAG(ARKWEB_CA)

  AcquirePrivateKey(std::move(delegate), certs[0], private_key_file,
                    prikey_data);
  return;
}

#if BUILDFLAG(ARKWEB_CA)
static int32_t GetApplicationApiVersion() {
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

static scoped_refptr<net::SSLPrivateKey> WrapOpenSSLPrivateKeyOHOS(
    const std::string& private_key_file) {
  if (!private_key_file.c_str()) {
    LOG(ERROR) << "WrapOpenSSLPrivateKey, private key file is null";
    return nullptr;
  }

  return base::MakeRefCounted<net::ThreadedSSLPrivateKey>(
      std::make_unique<SSLPlatformKeyOHOS>(private_key_file),
      net::GetSSLPlatformKeyTaskRunner());
}
#endif  // BUILDFLAG(ARKWEB_CA)

static void RunCancelNow(
    std::unique_ptr<content::ClientCertificateDelegate> delegate,
    const std::string& host,
    int port) {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::RunCancelNow";
  CEF_REQUIRE_UIT();
  AlloyClientCertLookupTable::Deny(host, port);
  delegate->ContinueWithCertificate(nullptr, nullptr);
}
 
static void RunIgnoreNow(
    std::unique_ptr<content::ClientCertificateDelegate> delegate,
    const std::string& host,
    int port) {
  LOG(DEBUG) << "CefSelectClientCertificateCallbackImpl::RunIgnoreNow";
  CEF_REQUIRE_UIT();
  delegate->ContinueWithCertificate(nullptr, nullptr);
}

std::string host_;
int port_;
bool finsh_ = false;
#endif  // BUILDFLAG(IS_ARKWEB)

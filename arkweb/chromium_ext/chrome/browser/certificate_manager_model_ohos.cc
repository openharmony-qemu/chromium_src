// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/certificate_manager_model_ohos.h"

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/x509.h>

#include <utility>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_observation.h"
#include "base/sequence_checker.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "build/build_config.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/grit/generated_resources.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_context.h"
#include "net/base/net_errors.h"
#include "net/cert/cert_database.h"
#include "net/cert/x509_certificate.h"
#include "ui/base/l10n/l10n_util.h"

//using namespace ohos::adapter;

using x509_certificate_model::OptionalStringOrError;

using content::BrowserThread;

CertificateManagerModel::CertInfo::CertInfo(OhosCertInfo cert,
                                            net::CertType type,
                                            const std::u16string& name,
                                            const std::string& org_name,
                                            bool can_be_deleted,
                                            bool untrusted,
                                            Source source,
                                            bool web_trust_anchor,
                                            bool hardware_backed,
                                            bool device_wide)
    : cert_(cert),
      type_(type),
      name_(std::move(name)),
      org_name_(std::move(org_name)),
      can_be_deleted_(can_be_deleted),
      untrusted_(untrusted),
      source_(source),
      web_trust_anchor_(web_trust_anchor),
      hardware_backed_(hardware_backed),
      device_wide_(device_wide) {}

CertificateManagerModel::CertInfo::~CertInfo() {}

// static
std::unique_ptr<CertificateManagerModel::CertInfo>
CertificateManagerModel::CertInfo::Clone(const CertInfo* cert_info) {
  if (cert_info == nullptr) {
    return nullptr;
  }
  return std::make_unique<CertInfo>(
      cert_info->cert(), cert_info->type(), cert_info->name(),
      cert_info->org_name(), cert_info->can_be_deleted(),
      cert_info->untrusted(), cert_info->source(),
      cert_info->web_trust_anchor(), cert_info->hardware_backed(),
      cert_info->device_wide());
}

// static
void CertificateManagerModel::Create(
    content::BrowserContext* browser_context,
    CertificateManagerModel::Observer* observer,
    CreationCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  std::unique_ptr<Params> params = std::make_unique<Params>();

  std::unique_ptr<CertificateManagerModel> model =
      std::make_unique<CertificateManagerModel>(std::move(params), observer,
                                                nullptr);
  std::move(callback).Run(std::move(model));
}

CertificateManagerModel::CertificateManagerModel(
    std::unique_ptr<Params> params,
    Observer* observer,
    net::NSSCertDatabase* nss_cert_database)
    : observer_(observer) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
}

CertificateManagerModel::~CertificateManagerModel() {}

void CertificateManagerModel::OnCertsSourceUpdated() {
  if (hold_back_updates_) {
    return;
  }
  observer_->CertificatesRefreshed();
}

void CertificateManagerModel::Refresh() {
  hold_back_updates_ = true;

  CertManagerAdapter::CertInfoList list_certs =
      CertManagerAdapter::GetInstance().ListCertsInfo();
  if (ca_cert_dir_.empty()) {
    int32_t user_id = CertManagerAdapter::GetInstance().GetUserId();
    ca_cert_dir_ = USER_CACERT_PREFIX;
    ca_cert_dir_ += std::to_string(user_id);
  }
  CertManagerAdapter::CertInfoList list_certs_ca =
      CertManagerAdapter::GetInstance().ListCACertsInfo(ca_cert_dir_);
  list_certs.insert(list_certs.begin(), list_certs_ca.begin(),
                    list_certs_ca.end());

  CertManagerAdapter::CertInfoList list_certs_root =
      CertManagerAdapter::GetInstance().ListCACertsInfo(ROOT_CERT);
  list_certs.insert(list_certs.begin(), list_certs_root.begin(),
                    list_certs_root.end());

  CertManagerAdapter::CertInfoList list_certs_edm =
      CertManagerAdapter::GetInstance().ListCACertsInfo(USER_EDMCERT);
  list_certs.insert(list_certs.begin(), list_certs_edm.begin(),
                    list_certs_edm.end());

  ConvertCertInfos(list_certs);
  hold_back_updates_ = false;
  OnCertsSourceUpdated();
}

void CertificateManagerModel::ConvertCertInfos(
    CertManagerAdapter::CertInfoList ohos_cert_info_list) {
  std::vector<std::unique_ptr<CertificateManagerModel::CertInfo>> cert_infos;
  cert_infos.reserve(ohos_cert_info_list.size());

  for (auto& cert_info : ohos_cert_info_list) {
    net::CertType type = (net::CertType)cert_info.type;
    bool can_be_deleted = true;
    bool hardware_backed = false;
    std::u16string name;
    OptionalStringOrError common_name;
    OptionalStringOrError org_name;
    std::string pem_cert = cert_info.cert;

    size_t len_cert;
    bssl::UniquePtr<uint8_t> der_cert;
    if (!PemToDer(pem_cert, &der_cert, &len_cert)) {
      continue;
    }

    bssl::UniquePtr<CRYPTO_BUFFER> buf(
        CRYPTO_BUFFER_new(der_cert.get(), len_cert, nullptr));

    std::string nickname;
    x509_certificate_model::X509CertificateModel model(bssl::UpRef(buf),
                                                       std::move(nickname));

    common_name = model.GetSubjectCommonName();
    org_name = model.GetSubjectOrgName();
    if (absl::holds_alternative<x509_certificate_model::NotPresent>(
            common_name) ||
        absl::holds_alternative<x509_certificate_model::NotPresent>(org_name)) {
      continue;
    }
    name = base::UTF8ToUTF16(absl::get<std::string>(common_name));

    cert_models_.emplace_back(std::move(model));

    cert_infos.push_back(std::make_unique<CertificateManagerModel::CertInfo>(
        /*cert*/
        cert_info, type, name, absl::get<std::string>(org_name), can_be_deleted,
        /*untrusted=*/
        false,
        /*source=*/
        CertificateManagerModel::CertInfo::Source::kPlatform,
        /*web_trust_anchor=*/
        true, hardware_backed,
        /*device_wide=*/
        true));
  }
  cert_infos_.swap(cert_infos);
}

bool CertificateManagerModel::PemToDer(std::string pem,
                                       bssl::UniquePtr<uint8_t>* out,
                                       size_t* out_len) {
  bssl::UniquePtr<BIO> bio(BIO_new_mem_buf(pem.data(), pem.size()));
  if (!bio) {
    return false;
  }

  char* name = nullptr;
  char* header = nullptr;
  uint8_t* data = nullptr;
  long data_len;
  if (!PEM_read_bio(bio.get(), &name, &header, &data, &data_len)) {
    LOG(ERROR) << "failed to read PEM data.";
    return false;
  }
  OPENSSL_free(name);
  OPENSSL_free(header);
  if (out == nullptr) {
    return false;
  }
  out->reset(data);
  *out_len = data_len;
  return true;
}

std::string CertificateManagerModel::ParsePfxGetCommonName(
    const std::string& p12data,
    const std::string& pass) {
  std::string ret = "";
  auto der_buf = reinterpret_cast<const uint8_t*>(p12data.data());
  bssl::UniquePtr<PKCS12> p12(d2i_PKCS12(nullptr, &der_buf, p12data.size()));

  EVP_PKEY* pkey = nullptr;
  X509* cert = nullptr;
  // Parse the PKCS12 structure
  if (PKCS12_parse(p12.get(), pass.c_str(), &pkey, &cert, nullptr) != 1) {
    return "";
  }
  // if |p12| does not contain a private key,
  // both |pkey| and |cert| will be set to NULL
  // and all certificates will be returned via |ca|.
  bssl::UniquePtr<EVP_PKEY> pkey_free(pkey);
  bssl::UniquePtr<X509> cert_free(cert);
  if (!cert) {
    return "";
  }
  // Get the Common Name (CN) from the certificate
  X509_NAME* subjectName = X509_get_subject_name(cert);
  int commonNameIndex =
      X509_NAME_get_index_by_NID(subjectName, NID_commonName, -1);
  X509_NAME_ENTRY* commonNameEntry =
      X509_NAME_get_entry(subjectName, commonNameIndex);
  ASN1_STRING* commonNameASN1 = X509_NAME_ENTRY_get_data(commonNameEntry);
  ret = std::string(reinterpret_cast<char*>(ASN1_STRING_data(commonNameASN1)),
                    ASN1_STRING_length(commonNameASN1));
  return ret;
}

void CertificateManagerModel::FilterAndBuildOrgGroupingMap(
    net::CertType filter_type,
    CertificateManagerModel::OrgGroupingMap* out_org_grouping_map) const {
  for (auto& cert_info : cert_infos_) {
    if (cert_info->type() != filter_type) {
      continue;
    }
    std::string org = cert_info->org_name();
    std::unique_ptr<CertificateManagerModel::CertInfo> cert =
        CertInfo::Clone(cert_info.get());
    if (cert == nullptr) {
      continue;
    }
    (*out_org_grouping_map)[org].push_back(std::move(cert));
  }
}

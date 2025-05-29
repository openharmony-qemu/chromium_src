// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CERTIFICATE_MANAGER_MODEL_OHOS_H_
#define CHROME_BROWSER_CERTIFICATE_MANAGER_MODEL_OHOS_H_

#include <map>
#include <memory>
#include <string>

#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/net/nss_service.h"
#include "chrome/common/net/x509_certificate_model.h"
#include "net/cert/cert_type.h"
#include "net/ssl/client_cert_identity.h"

static const char* ROOT_CERT = "/system/etc/security/certificates/";
static const char* USER_CACERT_PREFIX = "/data/certificates/user_cacerts/";
static const char* USER_EDMCERT = "/data/certificates/user_cacerts/0/";

namespace content {
class BrowserContext;
}  // namespace content

// CertificateManagerModel provides the data to be displayed in the certificate
// manager dialog, and processes changes from the view.
class CertificateManagerModel {
 public:
  // Holds information about a certificate, along with the certificate itself.
  class CertInfo {
   public:
    enum class Source {
      // This certificate is installed in the platform certificate database.
      kPlatform,
      // This certificate is provided by enterprise policy.
      kPolicy,
      // This certificate is provided by an extension.
      kExtension
    };

    CertInfo(ohos::adapter::OhosCertInfo cert,
             net::CertType type,
             const std::u16string& name,
             const std::string& org_name,
             bool can_be_deleted,
             bool untrusted,
             Source source,
             bool web_trust_anchor,
             bool hardware_backed,
             bool device_wide);

    CertInfo(const CertInfo&) = delete;
    CertInfo& operator=(const CertInfo&) = delete;

    ~CertInfo();

    ohos::adapter::OhosCertInfo cert() const { return cert_; }
    net::CertType type() const { return type_; }
    const std::u16string& name() const { return name_; }
    const std::string& org_name() const { return org_name_; }
    bool can_be_deleted() const { return can_be_deleted_; }
    bool untrusted() const { return untrusted_; }
    Source source() const { return source_; }
    bool web_trust_anchor() const { return web_trust_anchor_; }
    bool hardware_backed() const { return hardware_backed_; }
    bool device_wide() const { return device_wide_; }

    // Clones a CertInfo, duplicating the contained NSS certificate.
    static std::unique_ptr<CertInfo> Clone(const CertInfo* cert_info);

   private:
    // The certificate itself.
    ohos::adapter::OhosCertInfo cert_;

    // The type of the certificate. Used to filter certificates to be displayed
    // on the tabs of the certificate manager UI.
    net::CertType type_;

    // A user readable certificate name.
    std::u16string name_;

    std::string org_name_;

    // false if the certificate is stored on a read-only slot or provided by
    // enterprise policy or an extension, otherwise true.
    bool can_be_deleted_;

    // true if the certificate is untrusted.
    bool untrusted_;

    // Describes where this certificate originates from.
    Source source_;

    // true if the certificate is given web trust (either by its platform trust
    // settings, or by enterprise policy).
    bool web_trust_anchor_;

    // true if the certificate is hardware-backed. Note that extension-provided
    // certificates are not regarded as hardware-backed.
    bool hardware_backed_;

    // true if the certificate is device-wide.
    // Note: can be true only on Chrome OS.
    bool device_wide_;

    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest,
                             CanDeleteCertificateCommonTest);
    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest,
                             CanDeleteUserCertificateTest);
    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest,
                             CanDeleteCACertificateTest);
    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest,
                             CanEditCertificateCommonTest);
    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest,
                             CanEditUserCertificateTest);
    FRIEND_TEST_ALL_PREFIXES(CertificateHandlerTest, CanEditCACertificateTest);
  };

  using CertInfoList = std::vector<CertInfo>;

  class CertsSource;

  // Holds parameters during construction.
  struct Params {
    Params() = default;
    Params(const Params&) = delete;
    Params& operator=(const Params&) = delete;
    Params(Params&& other) = default;
    ~Params() = default;
  };

  // Map from the subject organization name to the list of certs from that
  // organization.  If a cert does not have an organization name, the
  // subject's CertPrincipal::GetDisplayName() value is used instead.
  using OrgGroupingMap =
      std::map<std::string, std::vector<std::unique_ptr<CertInfo>>>;

  using CreationCallback =
      base::OnceCallback<void(std::unique_ptr<CertificateManagerModel>)>;

  class Observer {
   public:
    // Called to notify the view that the certificate list has been refreshed.
    virtual void CertificatesRefreshed() = 0;

   protected:
    virtual ~Observer() = default;
  };

  // Creates a CertificateManagerModel. The model will be passed to the callback
  // when it is ready. The caller must ensure the model does not outlive the
  // |browser_context|.
  static void Create(content::BrowserContext* browser_context,
                     Observer* observer,
                     CreationCallback callback);

  // Use |Create| instead to create a |CertificateManagerModel| for a
  // |BrowserContext|.
  CertificateManagerModel(std::unique_ptr<Params> params,
                          Observer* observer,
                          net::NSSCertDatabase* nss_cert_database);

  CertificateManagerModel(const CertificateManagerModel&) = delete;
  CertificateManagerModel& operator=(const CertificateManagerModel&) = delete;

  ~CertificateManagerModel();

  // Trigger a refresh of the list of certs, unlock any slots if necessary.
  // Following this call, the observer CertificatesRefreshed method will be
  // called so the view can call FilterAndBuildOrgGroupingMap as necessary to
  // refresh its tree views.
  void Refresh();

  void ConvertCertInfos(
      ohos::adapter::CertManagerAdapter::CertInfoList cert_info_list);

  // Fill |*out_org_grouping_map| with the certificates matching |filter_type|.
  void FilterAndBuildOrgGroupingMap(net::CertType filter_type,
                                    OrgGroupingMap* out_org_grouping_map) const;

  bool PemToDer(const std::string pem,
                bssl::UniquePtr<uint8_t>* out,
                size_t* out_len);

  std::string ParsePfxGetCommonName(const std::string& p12data,
                                    const std::string& pass);

 private:
  // |observer_| that the certificate list has been refreshed.
  void OnCertsSourceUpdated();

  std::vector<x509_certificate_model::X509CertificateModel> cert_models_;
  ohos::adapter::CertManagerAdapter::CertInfoList ohos_cert_infos_;
  std::vector<std::unique_ptr<CertificateManagerModel::CertInfo>> cert_infos_;

  bool hold_back_updates_ = false;
  std::string ca_cert_dir_ = "";

  // The observer to notify when certificate list is refreshed.
  raw_ptr<Observer> observer_;
};

#endif  // CHROME_BROWSER_CERTIFICATE_MANAGER_MODEL_OHOS_H_

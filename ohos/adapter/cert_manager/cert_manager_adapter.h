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

#ifndef OHOS_ADAPTER_CERT_MANAGER_CERT_MANAGER_ADAPTER_H_
#define OHOS_ADAPTER_CERT_MANAGER_CERT_MANAGER_ADAPTER_H_

#include <napi/native_api.h>

#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter {

enum CertType {
  OTHER_CERT,
  CA_CERT,
  USER_CERT,
  SERVER_CERT,
  USB_CERT,
  NUM_CERT_TYPES
};

// Holds a certificate along with additional information.
struct OhosCertInfo {
  std::string cert;
  std::string uri;
  CertType type;
};

class ADAPTER_EXPORT_API CertManagerAdapter {
 public:
  static CertManagerAdapter& GetInstance();

  CertManagerAdapter() = default;
  CertManagerAdapter(const CertManagerAdapter&) = delete;
  CertManagerAdapter& operator=(const CertManagerAdapter&) = delete;

  virtual ~CertManagerAdapter() = default;

  using CertInfoList = std::vector<OhosCertInfo>;

  virtual CertInfoList ListCertsInfo();
  CertInfoList ListCACertsInfo(const std::string& pathDir);
  int32_t GetUserId();
  virtual int InstallPersonalCert(std::vector<uint8_t> cert_data,
                          const std::string& cert_pass,
                          const std::string& alias);
  int UninstallPersonalCert(const std::string& uri);
  int SignDataByUri(const std::string& uri,
                    uint8_t* input,
                    size_t inputlen,
                    uint8_t* out,
                    size_t* outlen);

  CertInfoList EnumClientCerts();
  int SignByHuks(const std::string& uri,
                  uint8_t* input,
                  size_t inputlen,
                  uint8_t* out,
                  size_t* outlen);
                  

  void ConvertCertInfo(aki::Value cert_info_value, OhosCertInfo* cert_info);

  std::vector<std::string> GetCertCrl(const std::string& pathDirPrefix, bool getUserId);
  void ShowCertificateManagerDialog();
  void ShowCertificateManagerDialog(CertType certType);
  bool IsSdk22();

 private:
  int32_t user_id_ = 0;
};

}  // namespace ohos::adapter

#endif  // OHOS_ADAPTER_CERT_MANAGER_CERT_MANAGER_ADAPTER_H_

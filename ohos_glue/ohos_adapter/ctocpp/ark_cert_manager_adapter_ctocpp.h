/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_CERT_MANAGER_ADAPTER_CTOCPP_H_
#define ARK_CERT_MANAGER_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_cert_manager_adapter_capi.h"
#include "ohos_adapter/include/ark_cert_manager_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkCertManagerAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkCertManagerAdapterCToCpp,
                                    ArkCertManagerAdapter,
                                    ark_cert_manager_adapter_t> {
public:
  ArkCertManagerAdapterCToCpp();
  virtual ~ArkCertManagerAdapterCToCpp();

  // ArkCertManagerAdapter methods.
  uint32_t GetCertMaxSize() override;

  uint32_t GetAppCertMaxSize() override;

  int32_t GetSytemRootCertData(uint32_t certCount, uint8_t *certData) override;

  uint32_t GetSytemRootCertSum() override;

  int32_t GetUserRootCertData(uint32_t certCount, uint8_t *certData) override;

  uint32_t GetUserRootCertSum() override;

  int32_t GetAppCert(uint8_t *uriData, uint8_t *certData,
                     uint32_t *len) override;

  int32_t Sign(const uint8_t *uri, const uint8_t *certData,
               uint32_t certDataLen, uint8_t *signData,
               uint32_t signDataLen) override;

  int32_t GetCertDataBySubject(const char *subjectName, uint8_t *certData,
                               int32_t certType) override;

  int VerifyCertFromNetSsl(uint8_t *certData, uint32_t certSize) override;

  bool GetTrustAnchorsForHostName(const ArkWebString &hostname,
                                  ArkWebStringVector &certs) override;

  bool GetPinSetForHostName(const ArkWebString &hostname,
                            ArkWebStringVector &pins) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_CERT_MANAGER_ADAPTER_CTOCPP_H_

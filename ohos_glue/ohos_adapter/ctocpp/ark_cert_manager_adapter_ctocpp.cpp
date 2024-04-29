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

#include "ohos_adapter/ctocpp/ark_cert_manager_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
uint32_t ArkCertManagerAdapterCToCpp::GetCertMaxSize() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_cert_max_size, 0);

  // Execute
  return _struct->get_cert_max_size(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkCertManagerAdapterCToCpp::GetAppCertMaxSize() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_app_cert_max_size, 0);

  // Execute
  return _struct->get_app_cert_max_size(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCertManagerAdapterCToCpp::GetSytemRootCertData(uint32_t certCount,
                                                          uint8_t *certData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_sytem_root_cert_data, 0);

  // Execute
  return _struct->get_sytem_root_cert_data(_struct, certCount, certData);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkCertManagerAdapterCToCpp::GetSytemRootCertSum() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_sytem_root_cert_sum, 0);

  // Execute
  return _struct->get_sytem_root_cert_sum(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCertManagerAdapterCToCpp::GetUserRootCertData(uint32_t certCount,
                                                         uint8_t *certData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_user_root_cert_data, 0);

  // Execute
  return _struct->get_user_root_cert_data(_struct, certCount, certData);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkCertManagerAdapterCToCpp::GetUserRootCertSum() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_user_root_cert_sum, 0);

  // Execute
  return _struct->get_user_root_cert_sum(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCertManagerAdapterCToCpp::GetAppCert(uint8_t *uriData,
                                                uint8_t *certData,
                                                uint32_t *len) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_app_cert, 0);

  // Execute
  return _struct->get_app_cert(_struct, uriData, certData, len);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCertManagerAdapterCToCpp::Sign(const uint8_t *uri,
                                          const uint8_t *certData,
                                          uint32_t certDataLen,
                                          uint8_t *signData,
                                          uint32_t signDataLen) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, sign, 0);

  // Execute
  return _struct->sign(_struct, uri, certData, certDataLen, signData,
                       signDataLen);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCertManagerAdapterCToCpp::GetCertDataBySubject(
    const char *subjectName, uint8_t *certData, int32_t certType) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_cert_data_by_subject, 0);

  // Execute
  return _struct->get_cert_data_by_subject(_struct, subjectName, certData,
                                           certType);
}

ARK_WEB_NO_SANITIZE
int ArkCertManagerAdapterCToCpp::VerifyCertFromNetSsl(uint8_t *certData,
                                                      uint32_t certSize) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, verify_cert_from_net_ssl, 0);

  // Execute
  return _struct->verify_cert_from_net_ssl(_struct, certData, certSize);
}

ARK_WEB_NO_SANITIZE
bool ArkCertManagerAdapterCToCpp::GetTrustAnchorsForHostName(
    const ArkWebString &hostname, ArkWebStringVector &certs) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_trust_anchors_for_host_name,
                                   false);

  // Execute
  return _struct->get_trust_anchors_for_host_name(_struct, &hostname, &certs);
}

ARK_WEB_NO_SANITIZE
bool ArkCertManagerAdapterCToCpp::GetPinSetForHostName(
    const ArkWebString &hostname, ArkWebStringVector &pins) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_cert_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_pin_set_for_host_name, false);

  // Execute
  return _struct->get_pin_set_for_host_name(_struct, &hostname, &pins);
}

ArkCertManagerAdapterCToCpp::ArkCertManagerAdapterCToCpp() {
}

ArkCertManagerAdapterCToCpp::~ArkCertManagerAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkCertManagerAdapterCToCpp, ArkCertManagerAdapter,
                           ark_cert_manager_adapter_t>::kBridgeType =
        ARK_CERT_MANAGER_ADAPTER;

} // namespace OHOS::ArkWeb

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_OHOS_TYPE_CONVERSIONS_H_
#define DEVICE_FIDO_OHOS_TYPE_CONVERSIONS_H_

#include <string>
#include <vector>

#include "base/logging.h"
#include "device/fido/authenticator_get_assertion_response.h"
#include "device/fido/authenticator_make_credential_response.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/ohos/fido2_api.h"

namespace device {

template<typename R, typename T>
R Convert(const T& t)
{
    return t;
}

template<>
std::string Convert(const FIDO2_ClientCapability& capability);

template<>
FIDO2_PublicKeyCredentialType Convert(const CredentialType& type);

template<>
FIDO2_PublicKeyCredentialParameters Convert(
    const PublicKeyCredentialParams::CredentialInfo& info);

template<>
std::vector<FIDO2_PublicKeyCredentialParameters> Convert(
    const PublicKeyCredentialParams& params);

template<>
FIDO2_AuthenticatorTransport Convert(const FidoTransportProtocol& transport);

template<>
FidoTransportProtocol Convert(const FIDO2_AuthenticatorTransport& transport);

template<>
std::optional<base::flat_set<FidoTransportProtocol>> Convert(
    const FIDO2_AuthenticatorTransportArray& transports);

template<>
std::vector<FIDO2_AuthenticatorTransport> Convert(
    const base::flat_set<FidoTransportProtocol>& transport);

template<>
FIDO2_UserVerificationRequirement Convert(
    const UserVerificationRequirement& uv);

template<>
FIDO2_AuthenticatorAttachment Convert(
    const AuthenticatorAttachment& attachment);

template<>
AuthenticatorAttachment Convert(
    const FIDO2_AuthenticatorAttachment& attachment);

template<>
FIDO2_PublicKeyCredentialRpEntity Convert(
    const PublicKeyCredentialRpEntity& rp);

template<>
FIDO2_PublicKeyCredentialUserEntity Convert(
    const PublicKeyCredentialUserEntity& user);

template<>
FIDO2_AttestationConveyancePreference Convert(
    const AttestationConveyancePreference& preference);

template<>
std::vector<uint8_t> Convert(const Uint8Buff& buffer);

template<>
std::string Convert(const std::vector<uint8_t>& data);

template<>
FIDO2_PublicKeyCredentialHint Convert(const CredentialHint& hint);

template<>
FIDO2_CredentialMediationRequirement Convert(
    const CredentialMediationRequirement& mediation);

template<>
std::string Convert(const FIDO2_PublicKeyAttestationCredential& credential);

template<>
AuthenticatorMakeCredentialResponse Convert(
    const FIDO2_PublicKeyAttestationCredential& credential);

template<>
std::string Convert(const FIDO2_PublicKeyAssertionCredential& credential);

template<>
AuthenticatorGetAssertionResponse Convert(
    const FIDO2_PublicKeyAssertionCredential& credential);

template<>
std::string Convert(const FIDO2_CredentialCreationOptions& options);

template<>
std::string Convert(const FIDO2_CredentialRequestOptions& options);

template<>
std::string Convert(const FIDO2_CapabilityArray& capabilities);

std::vector<uint8_t> ConvertHexStringToBytes(const std::string& hex_string);

struct CredentialOptionsDataHolder {
    CredentialOptionsDataHolder(const CtapMakeCredentialRequest& request);
    CredentialOptionsDataHolder(const CtapGetAssertionRequest& request);

    std::vector<FIDO2_PublicKeyCredentialParameters> public_keys;
    std::vector<FIDO2_PublicKeyCredentialDescriptor> descriptor_list;
    std::vector<std::vector<FIDO2_AuthenticatorTransport>> transports_list;
    std::vector<std::vector<uint8_t>> id_list;
    std::vector<FIDO2_PublicKeyCredentialHint> hint_list;
    std::vector<char*> formats_c_ptrs;
};

void Initialize(FIDO2_CredentialCreationOptions* options);
void Initialize(CredentialOptionsDataHolder& data_holder,
                CtapMakeCredentialRequest& request,
                MakeCredentialOptions& request_options,
                FIDO2_CredentialCreationOptions* options);
void Initialize(FIDO2_CredentialRequestOptions* options);
void Initialize(CredentialOptionsDataHolder& data_holder,
                CtapGetAssertionRequest& request,
                CtapGetAssertionOptions& request_options,
                FIDO2_CredentialRequestOptions* options);
void Initialize(FIDO2_TokenBinding* token_binding);

} // namespace device

#endif  // DEVICE_FIDO_OHOS_TYPE_CONVERSIONS_H_

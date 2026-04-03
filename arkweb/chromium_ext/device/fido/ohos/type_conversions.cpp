// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/fido/ohos/type_conversions.h"

#include <algorithm>
#include <sstream>
#include <string_view>

#include "base/strings/string_number_conversions.h"
#include "device/fido/attestation_statement.h"

namespace device {

namespace {

const size_t g_max_length = 3;

} // namespace

template<>
std::string Convert(const FIDO2_ClientCapability& capability)
{
    switch (capability) {
        case FIDO2_CONDITIONAL_CREATE:
            return "conditionalCreate";
        case FIDO2_CONDITIONAL_GET:
            return "conditionalGet";
        case FIDO2_HYBRID_TRANSPORT:
            return "hybridTransport";
        case FIDO2_PASSKEY_PLATFORM_AUTHENTICATOR:
            return "passkeyPlatformAuthenticator";
        case FIDO2_USER_VERIFYING_PLATFORM_AUTHENTICATOR:
            return "userVerifyingPlatformAuthenticator";
        case FIDO2_RELATED_ORIGINS:
            return "relatedOrigins";
        case FIDO2_SIGNAL_ALL_ACCEPTED_CREDENTIALS:
            return "signalAllAcceptedCredentials";
        case FIDO2_SIGNAL_CURRENT_USER_DETAILS:
            return "signalCurrentUserDetails";
        case FIDO2_SIGNAL_UNKNOWN_CREDENTIAL:
            return "signalUnknownCredential";
        case FIDO2_EXTENSION_UVI:
            return "extension:uvi";
  }
  return {};
}

template<>
FIDO2_PublicKeyCredentialType Convert(const CredentialType& type)
{
    switch (type) {
        case CredentialType::kPublicKey:
            return FIDO2_PUBLIC_KEY;
    }
    return FIDO2_PUBLIC_KEY;
}

template<>
FIDO2_PublicKeyCredentialParameters Convert(
    const PublicKeyCredentialParams::CredentialInfo& info)
{
    FIDO2_PublicKeyCredentialType type =
        Convert<FIDO2_PublicKeyCredentialType>(info.type);
    FIDO2_Algorithm algorithm = static_cast<FIDO2_Algorithm>(info.algorithm);
    return {type, algorithm};
}

template<>
std::vector<FIDO2_PublicKeyCredentialParameters> Convert(
    const PublicKeyCredentialParams& params)
{
    std::vector<FIDO2_PublicKeyCredentialParameters> ret;
    ret.reserve(params.public_key_credential_params().size());
    for (const auto param : params.public_key_credential_params()) {
        FIDO2_PublicKeyCredentialParameters fido_param =
            Convert<FIDO2_PublicKeyCredentialParameters>(param);
        ret.push_back(fido_param);
    }
    return ret;
}

template<>
FIDO2_AuthenticatorTransport Convert(const FidoTransportProtocol& transport)
{
    switch (transport) {
        case FidoTransportProtocol::kUsbHumanInterfaceDevice:
            return FIDO2_USB;
        case FidoTransportProtocol::kNearFieldCommunication:
            return FIDO2_NFC;
        case FidoTransportProtocol::kBluetoothLowEnergy:
            return FIDO2_BLE;
        case FidoTransportProtocol::kHybrid:
            return FIDO2_HYBRID;
        case FidoTransportProtocol::kInternal:
            return FIDO2_INTERNAL;
        case FidoTransportProtocol::kDeprecatedAoa:
            LOG(INFO) << "Unsupported transport";
    }
    // how to handle FIDO_SMART_CARD case ?
    return FIDO2_INTERNAL;
}

template<>
FidoTransportProtocol Convert(const FIDO2_AuthenticatorTransport& transport)
{
    switch (transport) {
        case FIDO2_USB:
            return FidoTransportProtocol::kUsbHumanInterfaceDevice;
        case FIDO2_NFC:
            return FidoTransportProtocol::kNearFieldCommunication;
        case FIDO2_BLE:
            return FidoTransportProtocol::kBluetoothLowEnergy;
        case FIDO2_SMART_CARD: // how to handle FIDO2_SMART_CARD ?
            return FidoTransportProtocol::kHybrid;
        case FIDO2_HYBRID:
            return FidoTransportProtocol::kHybrid;
        case FIDO2_INTERNAL:
            return FidoTransportProtocol::kInternal;
    }
    return FidoTransportProtocol::kInternal;
}

template<>
std::optional<base::flat_set<FidoTransportProtocol>> Convert(
    const FIDO2_AuthenticatorTransportArray& transports)
{
    if (transports.transportNum == 0 || !transports.transports) {
        return std::nullopt;
    }
    base::flat_set<FidoTransportProtocol> ret;
    for (uint32_t i = 0; i < transports.transportNum; i++) {
        ret.insert(Convert<FidoTransportProtocol>(*(transports.transports + i)));
    }
    return ret;
}

template<>
std::vector<FIDO2_AuthenticatorTransport> Convert(
    const base::flat_set<FidoTransportProtocol>& transport)
{
    std::vector<FIDO2_AuthenticatorTransport> ret;
    ret.reserve(transport.size());
    for (const auto& item : transport) {
        FIDO2_AuthenticatorTransport fido2_transport =
            Convert<FIDO2_AuthenticatorTransport>(item);
        ret.push_back(fido2_transport);
    }
    return ret;
}

template<>
FIDO2_UserVerificationRequirement Convert(const UserVerificationRequirement& uv)
{
    switch (uv) {
        case UserVerificationRequirement::kRequired:
            return FIDO2_REQUIRED;
        case UserVerificationRequirement::kPreferred:
            return FIDO2_PREFERRED;
        case UserVerificationRequirement::kDiscouraged:
            return FIDO2_DISCOURAGED;
    }
    return FIDO2_DISCOURAGED;
}

template<>
FIDO2_AuthenticatorAttachment Convert(const AuthenticatorAttachment& attachment)
{
    switch (attachment) {
        case AuthenticatorAttachment::kAny:
            return FIDO2_PLATFORM;
        case AuthenticatorAttachment::kPlatform:
            return FIDO2_PLATFORM;
        case AuthenticatorAttachment::kCrossPlatform:
            return FIDO2_CROSS_PLATFORM;
    }
    return FIDO2_PLATFORM;
}

template<>
AuthenticatorAttachment Convert(const FIDO2_AuthenticatorAttachment& attachment)
{
    switch (attachment) {
        case FIDO2_PLATFORM:
            return AuthenticatorAttachment::kPlatform;
        case FIDO2_CROSS_PLATFORM:
            return AuthenticatorAttachment::kCrossPlatform;
    }
    return AuthenticatorAttachment::kAny;
}

template<>
FIDO2_PublicKeyCredentialRpEntity Convert(
    const PublicKeyCredentialRpEntity& rp)
{
    FIDO2_PublicKeyCredentialRpEntity ret;
    ret.id = const_cast<char*>(rp.id.c_str());
    ret.name = rp.name ? const_cast<char*>(rp.name->c_str()) : nullptr;
    return ret;
}

template<>
FIDO2_PublicKeyCredentialUserEntity Convert(
    const PublicKeyCredentialUserEntity& user)
{
    FIDO2_PublicKeyCredentialUserEntity ret;
    ret.id.length = user.id.size();
    ret.id.val = const_cast<uint8_t*>(user.id.data());
    ret.displayName = user.display_name ?
        const_cast<char*>(user.display_name->c_str()) : nullptr;
    ret.name = user.name ?
        const_cast<char*>(user.name->c_str()) : nullptr;
    return ret;
}

template<>
FIDO2_AttestationConveyancePreference Convert(
    const AttestationConveyancePreference& preference)
{
    switch (preference) {
        case AttestationConveyancePreference::kNone:
            return FIDO2_NONE;
        case AttestationConveyancePreference::kIndirect:
            return FIDO2_INDIRECT;
        case AttestationConveyancePreference::kDirect:
            return FIDO2_DIRECT;
        case AttestationConveyancePreference::kEnterpriseIfRPListedOnAuthenticator:
            return FIDO2_ENTERPRISE;
        case AttestationConveyancePreference::kEnterpriseApprovedByBrowser:
            return FIDO2_ENTERPRISE;
    }
    return FIDO2_NONE;
}

template<>
std::vector<uint8_t> Convert(const Uint8Buff& buffer)
{
    if (buffer.val == nullptr || buffer.length == 0) {
      return {};
    }
    std::vector<uint8_t> ret;
    ret.reserve(buffer.length);
    ret.assign(buffer.val, buffer.val + buffer.length);
    return ret;
}

template<>
std::string Convert(const std::vector<uint8_t>& data)
{
    if (data.empty()) {
      return {};
    }
    std::string ret;
    ret.reserve(data.size());
    ret.assign(reinterpret_cast<char*>(const_cast<uint8_t*>(data.data())), data.size());
    return ret;
}

template<>
FIDO2_PublicKeyCredentialHint Convert(const CredentialHint& hint)
{
    switch (hint) {
        case CredentialHint::kSecurityKey:
            return FIDO2_SECURITY_KEY;
        case CredentialHint::kClientDevice:
            return FIDO2_CLIENT_DEVICE;
        case CredentialHint::kHintHybrid:
            return FIDO2_HINT_HYBRID;
    }
    return FIDO2_HINT_HYBRID;
}

template<>
FIDO2_CredentialMediationRequirement Convert(
    const CredentialMediationRequirement& mediation)
{
    switch (mediation) {
        case CredentialMediationRequirement::kSilent:
            return FIDO2_SILENT;
        case CredentialMediationRequirement::kOptional:
            return FIDO2_OPTIONAL;
        case CredentialMediationRequirement::kConditional:
            return FIDO2_CONDITIONAL;
        case CredentialMediationRequirement::kRequired:
            return FIDO2_MEDIATION_REQUIRED;
    }
    return FIDO2_OPTIONAL;
}

template<>
std::string Convert(const Uint8Buff& buffer)
{
    return base::HexEncode(buffer.val, buffer.length);
}

std::string ToString(const char* c_str)
{
   if (c_str == nullptr) {
       return {};
   }
   return c_str;
}

std::string_view TruncateString(const char* s)
{
    if (s == nullptr) {
        return "<null>";
    }
    const size_t count = std::min(std::strlen(s), g_max_length);
    return {s, count};
}

template<>
std::string Convert(const FIDO2_PublicKeyCredentialHintArray& hints)
{
    std::stringstream ss;
    ss << "[";
    for (uint32_t i = 0; i < hints.hintNum; i++) {
        if (i > 0) {
            ss << ", ";
        }
        ss << static_cast<int>(hints.hints[i]);
    }
    ss << "]";
    return ss.str();
}

template<>
std::string Convert(const FIDO2_AuthenticatorTransportArray& transports)
{
    std::stringstream ss;
    ss << "[";
    for (uint32_t i = 0; i < transports.transportNum; i++) {
        if (i > 0) {
            ss << ", ";
        }
        ss << static_cast<int>(transports.transports[i]);
    }
    ss << "]";
    return ss.str();
}

template<>
std::string Convert(const FIDO2_PublicKeyAttestationCredential& credential)
{
    const size_t max_length = g_max_length * 2;
    const FIDO2_AuthenticatorAttestationResponse& response = credential.response;
    std::stringstream ss;
    ss << "{\n";
    ss << "  rawId: 0x"
       << Convert<std::string>(credential.rawId).substr(0, max_length) << ",\n";
    ss << "  response: {\n";
    ss << "    attestationObject: 0x"
       << Convert<std::string>(response.attestationObject).substr(0, max_length) << ",\n";
    ss << "    clientDataJson: 0x"
       << Convert<std::string>(response.clientDataJson).substr(0, max_length) << ",\n";
    ss << "    publicKey: 0x"
       << Convert<std::string>(response.publicKey).substr(0, max_length) << ",\n";
    ss << "    authenticatorData: 0x"
       << Convert<std::string>(response.authenticatorData).substr(0, max_length) << ",\n";
    ss << "    publicKeyAlgorithm: "
       << static_cast<int>(response.publicKeyAlgorithm) << ",\n";
    ss << "    transports: " << Convert<std::string>(response.transports) << ",\n";
    ss << "  },\n";
    ss << "  authenticatorAttachment: "
       << static_cast<int>(credential.authenticatorAttachment) << ",\n";
    ss << "  id: " << TruncateString(credential.id) << ",\n";
    ss << "  type: " << TruncateString(credential.type) << ",\n";
    ss << "  clientExtensionResults: "
       << TruncateString(credential.clientExtensionResults.placeholder) << ",\n";
    ss << "}";
    return ss.str();
}

template<>
AuthenticatorMakeCredentialResponse Convert(
    const FIDO2_PublicKeyAttestationCredential& credential)
{
    std::array<uint8_t, kRpIdHashLength> rp_id_hash;
    AuthenticatorData auth_data(
        rp_id_hash, false, false, false, false, 0, std::nullopt, std::nullopt);
    AttestationObject attestation_object(
        std::move(auth_data), nullptr);
    std::optional<FidoTransportProtocol> transport_used;
    AuthenticatorMakeCredentialResponse ret(
        transport_used, std::move(attestation_object));

    ret.transports =
        Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(
            credential.response.transports);
    ret.response_extra = AuthenticatorMakeCredentialResponseExtra();
    ret.response_extra->common.raw_id =
        Convert<std::vector<uint8_t>>(credential.rawId);
    ret.response_extra->common.client_data_json =
        Convert<std::vector<uint8_t>>(credential.response.clientDataJson);
    ret.response_extra->common.authenticator_data =
        Convert<std::vector<uint8_t>>(credential.response.authenticatorData);
    ret.response_extra->common.autenticator_attachment =
        Convert<AuthenticatorAttachment>(credential.authenticatorAttachment);
    ret.response_extra->common.id = ToString(credential.id);
    ret.response_extra->common.type = ToString(credential.type);
    ret.response_extra->common.client_extension_results =
        ToString(credential.clientExtensionResults.placeholder);
    ret.response_extra->attestation_object =
        Convert<std::vector<uint8_t>>(credential.response.attestationObject);
    ret.response_extra->public_key =
        Convert<std::vector<uint8_t>>(credential.response.publicKey);
    ret.response_extra->public_key_algorithm = credential.response.publicKeyAlgorithm;

    return ret;
}

template<>
std::string Convert(const FIDO2_PublicKeyAssertionCredential& credential)
{
    const size_t max_length = g_max_length * 2;
    const FIDO2_AuthenticatorResponse& response = credential.response;
    std::stringstream ss;
    ss << "{\n";
    ss << "  rawId: 0x"
       << Convert<std::string>(credential.rawId).substr(0, max_length) << ",\n";
    ss << "  response: {\n";
    ss << "    authenticatorData: 0x"
       << Convert<std::string>(response.authenticatorData).substr(0, max_length) << ",\n";
    ss << "    signature: 0x"
       << Convert<std::string>(response.signature).substr(0, max_length) << ",\n";
    ss << "    userHandle: 0x"
       << Convert<std::string>(response.userHandle).substr(0, max_length) << ",\n";
    ss << "    clientDataJson: 0x"
       << Convert<std::string>(response.clientDataJson).substr(0, max_length) << ",\n";
    ss << "  },\n";
    ss << "  authenticatorAttachment: "
       << static_cast<int>(credential.authenticatorAttachment) << ",\n";
    ss << "  id: " << TruncateString(credential.id) << ",\n";
    ss << "  type: " << TruncateString(credential.type) << ",\n";
    ss << "  clientExtensionResults: "
       << TruncateString(credential.clientExtensionResults.placeholder) << ",\n";
    ss << "}";
    return ss.str();
}

template<>
AuthenticatorGetAssertionResponse Convert(
    const FIDO2_PublicKeyAssertionCredential& credential)
{
    std::array<uint8_t, kRpIdHashLength> rp_id_hash;
    std::optional<FidoTransportProtocol> transport_used;
    AuthenticatorData auth_data(
        rp_id_hash, false, false, false, false, 0, std::nullopt, std::nullopt);
    std::vector<uint8_t> signature;

    AuthenticatorGetAssertionResponse ret(
        std::move(auth_data), std::move(signature), transport_used);

    ret.response_extra = AuthenticatorGetAssertionResponseExtra();
    ret.response_extra->common.raw_id =
        Convert<std::vector<uint8_t>>(credential.rawId);
    ret.response_extra->common.client_data_json =
        Convert<std::vector<uint8_t>>(credential.response.clientDataJson);
    ret.response_extra->common.authenticator_data =
        Convert<std::vector<uint8_t>>(credential.response.authenticatorData);
    ret.response_extra->common.autenticator_attachment =
        Convert<AuthenticatorAttachment>(credential.authenticatorAttachment);
    ret.response_extra->common.id = ToString(credential.id);
    ret.response_extra->common.type = ToString(credential.type);
    ret.response_extra->common.client_extension_results =
        ToString(credential.clientExtensionResults.placeholder);

    ret.response_extra->signature =
        Convert<std::vector<uint8_t>>(credential.response.signature);
    ret.response_extra->user_handle =
        Convert<std::vector<uint8_t>>(credential.response.userHandle);
    return ret;
}

template<>
std::string Convert(const FIDO2_CredentialCreationOptions& options)
{
    const size_t max_length = g_max_length * 2;
    const FIDO2_PublicKeyCredentialCreationOptions& public_key = options.publicKey;
    const FIDO2_PublicKeyCredentialRpEntity& rp = public_key.rp;
    const FIDO2_PublicKeyCredentialUserEntity& user = public_key.user;
    const FIDO2_AuthenticatorSelectionCriteria& selection =
        public_key.authenticatorSelection;
    std::stringstream ss;
    ss << "{\n";
    ss << "  mediation: " << static_cast<int>(options.mediation) << ",\n";
    ss << "  publicKey: {\n";
    ss << "    rp: {\n";
    ss << "      id: " << TruncateString(rp.id) << ",\n";
    ss << "      name: " << TruncateString(rp.name) << ",\n";
    ss << "    }\n";
    ss << "    user: {\n";
    ss << "      id: " << Convert<std::string>(user.id).substr(0, max_length) << ",\n";
    ss << "      displayName: " << TruncateString(user.displayName) << ",\n";
    ss << "      name: " << TruncateString(user.name) << ",\n";
    ss << "    }\n";
    ss << "    challenge: "
       << Convert<std::string>(public_key.challenge).substr(0, max_length) << ",\n";
    ss << "    authenticatorSelection: {\n";
    ss << "      authenticatorAttachment: "
       << static_cast<int>(selection.authenticatorAttachment) << ",\n";
    ss << "      residentKey: " << TruncateString(selection.residentKey) << ",\n";
    ss << "      requireResidentKey: " << selection.requireResidentKey << ",\n";
    ss << "      userVerification: "
       << static_cast<int>(selection.userVerification) << ",\n";
    ss << "    }\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

template<>
std::string Convert(const FIDO2_CredentialRequestOptions& options)
{
    const size_t max_length = g_max_length * 2;
    const FIDO2_PublicKeyCredentialRequestOptions& public_key = options.publicKey;
    const FIDO2_PublicKeyCredentialDescriptorArray& descriptors =
        options.publicKey.allowCredentials;
    std::stringstream ss;
    ss << "{\n";
    ss << "  mediation: " << static_cast<int>(options.mediation) << ",\n";
    ss << "  publicKey: {\n";
    ss << "    challenge: "
       << Convert<std::string>(public_key.challenge).substr(0, max_length) << ",\n";
    ss << "    timeout: " << options.publicKey.timeout << ",\n";
    ss << "    rpId: " << TruncateString(public_key.rpId) << ",\n";
    ss << "    allowCredentials: [\n";
    for (uint32_t i = 0; i < descriptors.allowCredentiallNum; i++) {
        const auto& descriptor = descriptors.allowCredentials[i];
        ss << "      {\n";
        ss << "        type: " << static_cast<int>(descriptor.type) << ",\n";
        ss << "        id: " << Convert<std::string>(descriptor.id).substr(0, max_length) << ",\n";
        ss << "        transports: " << Convert<std::string>(descriptor.transports) << ",\n";
        ss << "      },\n";
    }
    ss << "    ],\n";
    ss << "    userVerification: " << static_cast<int>(public_key.userVerification) << ",\n";
    ss << "    hints: " << Convert<std::string>(public_key.hints) << ",\n";
    ss << "    extensions: " << TruncateString(public_key.extensions) << ",\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

template<>
std::string Convert(const FIDO2_CapabilityArray& capabilities)
{
    std::stringstream ss;
    ss << "[";
    for (uint32_t i = 0; i < capabilities.number; i++) {
      const auto& capability = capabilities.capability[i];
      if (i > 0) {
        ss << ", ";
      }
      ss << "{" << static_cast<int>(capability.capability)
         << ":" << capability.isSupported << "}";
    }
    ss << "]";
    return ss.str();
}

std::vector<uint8_t> ConvertHexStringToBytes(const std::string& hex_string)
{
    const std::string prefix("(bytestring)");
    std::string_view hex_stream = std::string_view(hex_string);
    if (hex_stream.length() > prefix.length()) {
        std::string_view stream_header = hex_stream.substr(0, prefix.length());
        if (stream_header == prefix) {
            hex_stream = hex_stream.substr(prefix.length());
        }
    }
    std::vector<uint8_t> ret;
    if (base::HexStringToBytes(hex_stream, &ret)) {
        return ret;
    } else {
        return {};
    }
}

void Initialize(FIDO2_CredentialCreationOptions* options)
{
    options->mediation = FIDO2_SILENT;

    options->publicKey.rp.id = nullptr;
    options->publicKey.rp.name = nullptr;

    options->publicKey.user.id.length = 0;
    options->publicKey.user.id.val = nullptr;
    options->publicKey.user.displayName = nullptr;
    options->publicKey.user.name = nullptr;

    options->publicKey.challenge.length = 0;
    options->publicKey.challenge.val = nullptr;

    options->publicKey.pubKeyCredParams.pubKeyCredParamNum = 0;
    options->publicKey.pubKeyCredParams.pubKeyCredParams = nullptr;
    options->publicKey.timeout = 0;

    options->publicKey.excludeCredentials.allowCredentiallNum = 0;
    options->publicKey.excludeCredentials.allowCredentials = nullptr;

    options->publicKey.authenticatorSelection.authenticatorAttachment = FIDO2_PLATFORM;
    options->publicKey.authenticatorSelection.residentKey = nullptr;
    options->publicKey.authenticatorSelection.requireResidentKey = false;
    options->publicKey.authenticatorSelection.userVerification = FIDO2_DISCOURAGED;

    options->publicKey.hints.hintNum = 0;
    options->publicKey.hints.hints = nullptr;

    options->publicKey.attestation = FIDO2_NONE;
    options->publicKey.attestationFormats.attestationFormatsNum = 0;
    options->publicKey.attestationFormats.attestationFormats = nullptr;
    options->publicKey.extensions = nullptr;
}


template<>
const char* Convert(const ResidentKeyRequirement& resident_key)
{
    switch (resident_key) {
        case ResidentKeyRequirement::kDiscouraged:
            return "discouraged";
        case ResidentKeyRequirement::kPreferred:
            return "preferred";
        case ResidentKeyRequirement::kRequired:
            return "required";
    }
    return "discouraged";
}

void Initialize(CredentialOptionsDataHolder& data_holder,
                CtapMakeCredentialRequest& request,
                MakeCredentialOptions& request_options,
                FIDO2_CredentialCreationOptions* options)
{
    options->publicKey.rp =
        Convert<FIDO2_PublicKeyCredentialRpEntity>(request.rp);
    options->publicKey.user =
        Convert<FIDO2_PublicKeyCredentialUserEntity>(request.user);

    options->publicKey.pubKeyCredParams.pubKeyCredParamNum =
        data_holder.public_keys.size();
    options->publicKey.pubKeyCredParams.pubKeyCredParams =
        data_holder.public_keys.data();

    options->publicKey.excludeCredentials.allowCredentiallNum =
        data_holder.descriptor_list.size();
    options->publicKey.excludeCredentials.allowCredentials =
        data_holder.descriptor_list.data();

    options->publicKey.authenticatorSelection.authenticatorAttachment =
        Convert<FIDO2_AuthenticatorAttachment>(
            request_options.authenticator_attachment);
    options->publicKey.authenticatorSelection.residentKey =
        Convert<const char*>(request_options.resident_key);
    options->publicKey.authenticatorSelection.requireResidentKey =
        request.resident_key_required;
    options->publicKey.authenticatorSelection.userVerification =
        Convert<FIDO2_UserVerificationRequirement>(request_options.user_verification);

    options->publicKey.hints.hintNum = data_holder.hint_list.size();
    options->publicKey.hints.hints = data_holder.hint_list.data();

    options->publicKey.attestation =
        Convert<FIDO2_AttestationConveyancePreference>(request.attestation_preference);

    options->publicKey.attestationFormats.attestationFormatsNum =
        data_holder.formats_c_ptrs.size();
    options->publicKey.attestationFormats.attestationFormats =
        data_holder.formats_c_ptrs.data();

    if (request.extra) {
        options->mediation =
           Convert<FIDO2_CredentialMediationRequirement>(request.extra->common.mediation);
        options->publicKey.challenge.length = request.extra->common.challenge.size();
        options->publicKey.challenge.val = request.extra->common.challenge.data();
        int64_t timeout_in_ms = request.extra->common.timeout.InMilliseconds();
        options->publicKey.timeout =
            timeout_in_ms > std::numeric_limits<uint32_t>::max()
                ? std::numeric_limits<uint32_t>::max()
                : static_cast<uint32_t>(request.extra->common.timeout.InMilliseconds());
        if (request.extra->common.extensions) {
            options->publicKey.extensions =
                const_cast<char*>(request.extra->common.extensions->c_str());
        }
    }
}

void Initialize(FIDO2_CredentialRequestOptions* options)
{
    options->mediation = FIDO2_SILENT;

    options->publicKey.challenge.length = 0;
    options->publicKey.challenge.val = nullptr;

    options->publicKey.timeout = 0;
    options->publicKey.rpId = nullptr;

    options->publicKey.allowCredentials.allowCredentiallNum = 0;
    options->publicKey.allowCredentials.allowCredentials = nullptr;

    options->publicKey.userVerification = FIDO2_DISCOURAGED;

    options->publicKey.hints.hintNum = 0;
    options->publicKey.hints.hints = nullptr;

    options->publicKey.extensions = nullptr;
}

void Initialize(CredentialOptionsDataHolder& data_holder,
                CtapGetAssertionRequest& request,
                CtapGetAssertionOptions& request_options,
                FIDO2_CredentialRequestOptions* options)
{
    options->publicKey.rpId = const_cast<char*>(request.rp_id.c_str());

    options->publicKey.allowCredentials.allowCredentiallNum =
        data_holder.descriptor_list.size();
    options->publicKey.allowCredentials.allowCredentials =
        data_holder.descriptor_list.data();

    options->publicKey.userVerification =
        Convert<FIDO2_UserVerificationRequirement>(request.user_verification);

    options->publicKey.hints.hintNum = data_holder.hint_list.size();
    options->publicKey.hints.hints = data_holder.hint_list.data();

    if (request.extra) {
        options->mediation =
             Convert<FIDO2_CredentialMediationRequirement>(request.extra->common.mediation);
        options->publicKey.challenge.length = request.extra->common.challenge.size();
        options->publicKey.challenge.val = request.extra->common.challenge.data();
        int64_t timeout_in_ms = request.extra->common.timeout.InMilliseconds();
        options->publicKey.timeout =
            timeout_in_ms > std::numeric_limits<uint32_t>::max()
                ? std::numeric_limits<uint32_t>::max()
                : static_cast<uint32_t>(request.extra->common.timeout.InMilliseconds());
        if (request.extra->common.extensions) {
            options->publicKey.extensions =
                const_cast<char*>(request.extra->common.extensions->c_str());
        }
    }
}

CredentialOptionsDataHolder::CredentialOptionsDataHolder(
    const CtapMakeCredentialRequest& request)
    : public_keys(Convert<std::vector<FIDO2_PublicKeyCredentialParameters>>(
        request.public_key_credential_params))
{
    descriptor_list.reserve(request.exclude_list.size());
    transports_list.reserve(request.exclude_list.size());
    id_list.reserve(request.exclude_list.size());

    if (request.extra) {
        formats_c_ptrs.reserve(request.extra->attestation_formats.size());
        for (const auto& format : request.extra->attestation_formats) {
            formats_c_ptrs.push_back(const_cast<char*>(format.c_str()));
        }

        hint_list.reserve(request.extra->common.hints.size());
        for (const auto& hint : request.extra->common.hints) {
            hint_list.push_back(Convert<FIDO2_PublicKeyCredentialHint>(hint));
        }
    }

    for (const auto& item : request.exclude_list) {
        FIDO2_PublicKeyCredentialDescriptor descriptor;
        descriptor.type =
            Convert<FIDO2_PublicKeyCredentialType>(item.credential_type);
        id_list.push_back(Convert<std::vector<uint8_t>>(item.id));
        descriptor.id.length = id_list.back().size();
        descriptor.id.val = id_list.back().data();
        std::vector<FIDO2_AuthenticatorTransport> transports =
            Convert<std::vector<FIDO2_AuthenticatorTransport>>(item.transports);
        transports_list.push_back(std::move(transports));
        descriptor.transports.transportNum = transports_list.back().size();
        descriptor.transports.transports = transports_list.back().data();

        descriptor_list.push_back(descriptor);
    }
}

CredentialOptionsDataHolder::CredentialOptionsDataHolder(
    const CtapGetAssertionRequest& request)
{
    descriptor_list.reserve(request.allow_list.size());
    transports_list.reserve(request.allow_list.size());
    id_list.reserve(request.allow_list.size());

    if (request.extra) {
        hint_list.reserve(request.extra->common.hints.size());
        for (const auto& hint : request.extra->common.hints) {
            hint_list.push_back(Convert<FIDO2_PublicKeyCredentialHint>(hint));
        }
    }

    for (const auto& item : request.allow_list) {
        FIDO2_PublicKeyCredentialDescriptor descriptor;
        descriptor.type =
            Convert<FIDO2_PublicKeyCredentialType>(item.credential_type);
        id_list.push_back(Convert<std::vector<uint8_t>>(item.id));
        descriptor.id.length = id_list.back().size();
        descriptor.id.val = id_list.back().data();
        std::vector<FIDO2_AuthenticatorTransport> transports =
            Convert<std::vector<FIDO2_AuthenticatorTransport>>(item.transports);
        transports_list.push_back(std::move(transports));
        descriptor.transports.transportNum = transports_list.back().size();
        descriptor.transports.transports = transports_list.back().data();

        descriptor_list.push_back(descriptor);
    }
}

void Initialize(FIDO2_TokenBinding* token_binding)
{
  token_binding->status = FIDO2_PRESENT;
  token_binding->id = nullptr;
}

} // namespace device

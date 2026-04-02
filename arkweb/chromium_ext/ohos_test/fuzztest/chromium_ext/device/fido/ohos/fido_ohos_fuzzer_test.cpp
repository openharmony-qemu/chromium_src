/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#include "base/time/time.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_get_assertion_request_extra.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/ctap_make_credential_request_extra.h"
#include "device/fido/ctap_request_extra_common.h"
#include "device/fido/fido_types.h"
#include "device/fido/ohos/fido2_api_utils.h"
#include "device/fido/ohos/type_conversions.h"
#include "device/fido/public_key_credential_descriptor.h"
#include "device/fido/public_key_credential_params.h"
#include "device/fido/public_key_credential_rp_entity.h"
#include "device/fido/public_key_credential_user_entity.h"

using namespace device;

static const char kValidAuthDataHex[] =
    "0000000000000000000000000000000000000000000000000000000000000000"
    "4500000001";

static const char kValidPackedJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"sig":"AABB","x5c":["CCDD"]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kMissingFmtJson[] =
    R"({"attStmt":{"alg":-7,"sig":"AABB","x5c":["CCDD"]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kMissingAttStmtJson[] =
    R"({"fmt":"packed",)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kMissingAuthDataJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"sig":"AABB","x5c":["CCDD"]}})";

static const char kNonPackedFmtJson[] =
    R"({"fmt":"none","attStmt":{"alg":-7,"sig":"AABB","x5c":["CCDD"]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kPackedMissingAlgJson[] =
    R"({"fmt":"packed","attStmt":{"sig":"AABB","x5c":["CCDD"]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kPackedMissingSigJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"x5c":["CCDD"]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kPackedMissingX5cJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"sig":"AABB"},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kPackedInvalidX5cJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"sig":"AABB","x5c":[123]},)"
    R"("authData":"0000000000000000000000000000000000000000000000000000000000000000)"
    R"(4500000001"})";

static const char kShortAuthDataJson[] =
    R"({"fmt":"packed","attStmt":{"alg":-7,"sig":"AABB","x5c":["CCDD"]},)"
    R"("authData":"0011"})";

static void FuzzParseNullptr() { Parse(nullptr); }

static void FuzzParseWithJson(const std::string& json_str) {
  FIDO2_PublicKeyAttestationCredential cred;
  cred.response.attestationObject.val =
      reinterpret_cast<uint8_t*>(const_cast<char*>(json_str.data()));
  cred.response.attestationObject.length = json_str.size();
  Parse(&cred);
}

static void FuzzParseFixedInputs() {
  FuzzParseNullptr();
  FuzzParseWithJson(kValidPackedJson);
  FuzzParseWithJson(kMissingFmtJson);
  FuzzParseWithJson(kMissingAttStmtJson);
  FuzzParseWithJson(kMissingAuthDataJson);
  FuzzParseWithJson(kNonPackedFmtJson);
  FuzzParseWithJson(kPackedMissingAlgJson);
  FuzzParseWithJson(kPackedMissingSigJson);
  FuzzParseWithJson(kPackedMissingX5cJson);
  FuzzParseWithJson(kPackedInvalidX5cJson);
  FuzzParseWithJson(kShortAuthDataJson);
  FuzzParseWithJson("");
  FuzzParseWithJson("{");
  FuzzParseWithJson("not json at all");
  FuzzParseWithJson("[]");
  FuzzParseWithJson("123");
}

static void FuzzParseFuzzedJson(FuzzedDataProvider& provider) {
  auto json = provider.ConsumeRemainingBytesAsString();
  FuzzParseWithJson(json);
}

static void FuzzConvertAllEnums() {
  Convert<std::string>(FIDO2_CONDITIONAL_CREATE);
  Convert<std::string>(FIDO2_CONDITIONAL_GET);
  Convert<std::string>(FIDO2_HYBRID_TRANSPORT);
  Convert<std::string>(FIDO2_PASSKEY_PLATFORM_AUTHENTICATOR);
  Convert<std::string>(FIDO2_USER_VERIFYING_PLATFORM_AUTHENTICATOR);
  Convert<std::string>(FIDO2_RELATED_ORIGINS);
  Convert<std::string>(FIDO2_SIGNAL_ALL_ACCEPTED_CREDENTIALS);
  Convert<std::string>(FIDO2_SIGNAL_CURRENT_USER_DETAILS);
  Convert<std::string>(FIDO2_SIGNAL_UNKNOWN_CREDENTIAL);
  Convert<std::string>(FIDO2_EXTENSION_UVI);

  Convert<FIDO2_AuthenticatorTransport>(
      FidoTransportProtocol::kUsbHumanInterfaceDevice);
  Convert<FIDO2_AuthenticatorTransport>(
      FidoTransportProtocol::kNearFieldCommunication);
  Convert<FIDO2_AuthenticatorTransport>(
      FidoTransportProtocol::kBluetoothLowEnergy);
  Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kHybrid);
  Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kInternal);
  Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kDeprecatedAoa);

  Convert<FidoTransportProtocol>(FIDO2_USB);
  Convert<FidoTransportProtocol>(FIDO2_NFC);
  Convert<FidoTransportProtocol>(FIDO2_BLE);
  Convert<FidoTransportProtocol>(FIDO2_SMART_CARD);
  Convert<FidoTransportProtocol>(FIDO2_HYBRID);
  Convert<FidoTransportProtocol>(FIDO2_INTERNAL);

  Convert<FIDO2_UserVerificationRequirement>(
      UserVerificationRequirement::kRequired);
  Convert<FIDO2_UserVerificationRequirement>(
      UserVerificationRequirement::kPreferred);
  Convert<FIDO2_UserVerificationRequirement>(
      UserVerificationRequirement::kDiscouraged);

  Convert<FIDO2_AuthenticatorAttachment>(AuthenticatorAttachment::kAny);
  Convert<FIDO2_AuthenticatorAttachment>(AuthenticatorAttachment::kPlatform);
  Convert<FIDO2_AuthenticatorAttachment>(
      AuthenticatorAttachment::kCrossPlatform);

  Convert<AuthenticatorAttachment>(FIDO2_PLATFORM);
  Convert<AuthenticatorAttachment>(FIDO2_CROSS_PLATFORM);

  Convert<FIDO2_AttestationConveyancePreference>(
      AttestationConveyancePreference::kNone);
  Convert<FIDO2_AttestationConveyancePreference>(
      AttestationConveyancePreference::kIndirect);
  Convert<FIDO2_AttestationConveyancePreference>(
      AttestationConveyancePreference::kDirect);
  Convert<FIDO2_AttestationConveyancePreference>(
      AttestationConveyancePreference::kEnterpriseIfRPListedOnAuthenticator);
  Convert<FIDO2_AttestationConveyancePreference>(
      AttestationConveyancePreference::kEnterpriseApprovedByBrowser);

  Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kSecurityKey);
  Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kClientDevice);
  Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kHintHybrid);

  Convert<FIDO2_CredentialMediationRequirement>(
      CredentialMediationRequirement::kSilent);
  Convert<FIDO2_CredentialMediationRequirement>(
      CredentialMediationRequirement::kOptional);
  Convert<FIDO2_CredentialMediationRequirement>(
      CredentialMediationRequirement::kConditional);
  Convert<FIDO2_CredentialMediationRequirement>(
      CredentialMediationRequirement::kRequired);

  Convert<const char*>(ResidentKeyRequirement::kDiscouraged);
  Convert<const char*>(ResidentKeyRequirement::kPreferred);
  Convert<const char*>(ResidentKeyRequirement::kRequired);

  Convert<FIDO2_PublicKeyCredentialType>(CredentialType::kPublicKey);
}

static void FuzzConvertUint8Buff(FuzzedDataProvider& provider) {
  auto bytes =
      provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 256));

  Uint8Buff null_empty{0, nullptr};
  auto vec1 = Convert<std::vector<uint8_t>>(null_empty);
  auto str1 = Convert<std::string>(null_empty);

  Uint8Buff buff;
  if (bytes.empty()) {
    buff.val = nullptr;
    buff.length = 0;
  } else {
    buff.val = bytes.data();
    buff.length = static_cast<uint32_t>(bytes.size());
  }
  auto vec2 = Convert<std::vector<uint8_t>>(buff);
  auto str2 = Convert<std::string>(buff);

  Uint8Buff nonnull_zero{0, reinterpret_cast<uint8_t*>(const_cast<char*>(""))};
  auto vec3 = Convert<std::vector<uint8_t>>(nonnull_zero);
  auto str3 = Convert<std::string>(nonnull_zero);
}

static void FuzzConvertVectorToString(FuzzedDataProvider& provider) {
  auto bytes =
      provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 256));
  auto result = Convert<std::string>(bytes);

  std::vector<uint8_t> empty;
  auto empty_result = Convert<std::string>(empty);
}

static void FuzzConvertHexStringToBytes(FuzzedDataProvider& provider) {
  ConvertHexStringToBytes("(bytestring)AABBCCDD11223344");
  ConvertHexStringToBytes("AABBCCDD11223344");
  ConvertHexStringToBytes("aabbccdd");
  ConvertHexStringToBytes("ZZ");
  ConvertHexStringToBytes("");
  ConvertHexStringToBytes("(bytestring)");
  ConvertHexStringToBytes("(bytestring)ZZ");
  ConvertHexStringToBytes("(bytestr)");

  auto fuzzed = provider.ConsumeRemainingBytesAsString();
  ConvertHexStringToBytes(fuzzed);
  ConvertHexStringToBytes(std::string("(bytestring)") + fuzzed);
}

static void FuzzConvertAttestationCredentialToString(FuzzedDataProvider& provider) {
  FIDO2_PublicKeyAttestationCredential cred;

  auto raw_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto att_obj = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto client_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto pub_key = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto auth_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));

  cred.rawId.val = raw_id.empty() ? nullptr : raw_id.data();
  cred.rawId.length = static_cast<uint32_t>(raw_id.size());

  cred.response.attestationObject.val = att_obj.empty() ? nullptr : att_obj.data();
  cred.response.attestationObject.length = static_cast<uint32_t>(att_obj.size());

  cred.response.clientDataJson.val = client_data.empty() ? nullptr : client_data.data();
  cred.response.clientDataJson.length = static_cast<uint32_t>(client_data.size());

  cred.response.publicKey.val = pub_key.empty() ? nullptr : pub_key.data();
  cred.response.publicKey.length = static_cast<uint32_t>(pub_key.size());

  cred.response.authenticatorData.val = auth_data.empty() ? nullptr : auth_data.data();
  cred.response.authenticatorData.length = static_cast<uint32_t>(auth_data.size());

  cred.response.publicKeyAlgorithm =
      static_cast<FIDO2_Algorithm>(provider.ConsumeIntegralInRange<int32_t>(-260, 0));
  cred.authenticatorAttachment =
      static_cast<FIDO2_AuthenticatorAttachment>(provider.ConsumeIntegralInRange<int32_t>(0, 1));
  cred.response.transports.transportNum = 0;
  cred.response.transports.transports = nullptr;
  cred.id = nullptr;
  cred.type = nullptr;
  cred.clientExtensionResults.placeholder = nullptr;

  auto result = Convert<std::string>(cred);
}

static void FuzzConvertAssertionCredentialToString(FuzzedDataProvider& provider) {
  FIDO2_PublicKeyAssertionCredential cred;

  auto raw_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto auth_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto signature = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto user_handle = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto client_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));

  cred.rawId.val = raw_id.empty() ? nullptr : raw_id.data();
  cred.rawId.length = static_cast<uint32_t>(raw_id.size());

  cred.response.authenticatorData.val = auth_data.empty() ? nullptr : auth_data.data();
  cred.response.authenticatorData.length = static_cast<uint32_t>(auth_data.size());

  cred.response.signature.val = signature.empty() ? nullptr : signature.data();
  cred.response.signature.length = static_cast<uint32_t>(signature.size());

  cred.response.userHandle.val = user_handle.empty() ? nullptr : user_handle.data();
  cred.response.userHandle.length = static_cast<uint32_t>(user_handle.size());

  cred.response.clientDataJson.val = client_data.empty() ? nullptr : client_data.data();
  cred.response.clientDataJson.length = static_cast<uint32_t>(client_data.size());

  cred.authenticatorAttachment =
      static_cast<FIDO2_AuthenticatorAttachment>(provider.ConsumeIntegralInRange<int32_t>(0, 1));
  cred.id = nullptr;
  cred.type = nullptr;
  cred.clientExtensionResults.placeholder = nullptr;

  auto result = Convert<std::string>(cred);
}

static void FuzzConvertAttestationCredentialToResponse(FuzzedDataProvider& provider) {
  FIDO2_PublicKeyAttestationCredential cred;

  auto raw_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto att_obj = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto client_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto pub_key = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto auth_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));

  cred.rawId.val = raw_id.empty() ? nullptr : raw_id.data();
  cred.rawId.length = static_cast<uint32_t>(raw_id.size());

  cred.response.attestationObject.val = att_obj.empty() ? nullptr : att_obj.data();
  cred.response.attestationObject.length = static_cast<uint32_t>(att_obj.size());

  cred.response.clientDataJson.val = client_data.empty() ? nullptr : client_data.data();
  cred.response.clientDataJson.length = static_cast<uint32_t>(client_data.size());

  cred.response.publicKey.val = pub_key.empty() ? nullptr : pub_key.data();
  cred.response.publicKey.length = static_cast<uint32_t>(pub_key.size());

  cred.response.authenticatorData.val = auth_data.empty() ? nullptr : auth_data.data();
  cred.response.authenticatorData.length = static_cast<uint32_t>(auth_data.size());

  cred.response.publicKeyAlgorithm =
      static_cast<FIDO2_Algorithm>(provider.ConsumeIntegralInRange<int32_t>(-260, 0));
  cred.authenticatorAttachment =
      static_cast<FIDO2_AuthenticatorAttachment>(provider.ConsumeIntegralInRange<int32_t>(0, 1));
  cred.response.transports.transportNum = 0;
  cred.response.transports.transports = nullptr;
  cred.id = nullptr;
  cred.type = nullptr;
  cred.clientExtensionResults.placeholder = nullptr;

  auto response = Convert<AuthenticatorMakeCredentialResponse>(cred);
}

static void FuzzConvertAssertionCredentialToResponse(FuzzedDataProvider& provider) {
  FIDO2_PublicKeyAssertionCredential cred;

  auto raw_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto auth_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto signature = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));
  auto user_handle = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto client_data = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 128));

  cred.rawId.val = raw_id.empty() ? nullptr : raw_id.data();
  cred.rawId.length = static_cast<uint32_t>(raw_id.size());

  cred.response.authenticatorData.val = auth_data.empty() ? nullptr : auth_data.data();
  cred.response.authenticatorData.length = static_cast<uint32_t>(auth_data.size());

  cred.response.signature.val = signature.empty() ? nullptr : signature.data();
  cred.response.signature.length = static_cast<uint32_t>(signature.size());

  cred.response.userHandle.val = user_handle.empty() ? nullptr : user_handle.data();
  cred.response.userHandle.length = static_cast<uint32_t>(user_handle.size());

  cred.response.clientDataJson.val = client_data.empty() ? nullptr : client_data.data();
  cred.response.clientDataJson.length = static_cast<uint32_t>(client_data.size());

  cred.authenticatorAttachment =
      static_cast<FIDO2_AuthenticatorAttachment>(provider.ConsumeIntegralInRange<int32_t>(0, 1));
  cred.id = nullptr;
  cred.type = nullptr;
  cred.clientExtensionResults.placeholder = nullptr;

  auto response = Convert<AuthenticatorGetAssertionResponse>(cred);
}

static void FuzzConvertCreationOptionsToString(FuzzedDataProvider& provider) {
  FIDO2_CredentialCreationOptions options;

  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  auto user_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));

  options.mediation = static_cast<FIDO2_CredentialMediationRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 3));

  options.publicKey.rp.id = nullptr;
  options.publicKey.rp.name = nullptr;

  options.publicKey.user.id.val = user_id.empty() ? nullptr : user_id.data();
  options.publicKey.user.id.length = static_cast<uint32_t>(user_id.size());
  options.publicKey.user.displayName = nullptr;
  options.publicKey.user.name = nullptr;

  options.publicKey.challenge.val = challenge.empty() ? nullptr : challenge.data();
  options.publicKey.challenge.length = static_cast<uint32_t>(challenge.size());

  options.publicKey.authenticatorSelection.authenticatorAttachment =
      static_cast<FIDO2_AuthenticatorAttachment>(provider.ConsumeIntegralInRange<int32_t>(0, 1));
  options.publicKey.authenticatorSelection.residentKey = nullptr;
  options.publicKey.authenticatorSelection.requireResidentKey = false;
  options.publicKey.authenticatorSelection.userVerification =
      static_cast<FIDO2_UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));

  auto result = Convert<std::string>(options);
}

static void FuzzConvertRequestOptionsToString(FuzzedDataProvider& provider) {
  FIDO2_CredentialRequestOptions options;

  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));

  options.mediation = static_cast<FIDO2_CredentialMediationRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 3));

  options.publicKey.challenge.val = challenge.empty() ? nullptr : challenge.data();
  options.publicKey.challenge.length = static_cast<uint32_t>(challenge.size());
  options.publicKey.timeout = provider.ConsumeIntegralInRange<uint32_t>(0, 600000);
  options.publicKey.rpId = nullptr;

  options.publicKey.allowCredentials.allowCredentiallNum = 0;
  options.publicKey.allowCredentials.allowCredentials = nullptr;

  options.publicKey.userVerification =
      static_cast<FIDO2_UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));

  options.publicKey.hints.hintNum = 0;
  options.publicKey.hints.hints = nullptr;

  options.publicKey.extensions = nullptr;

  auto result = Convert<std::string>(options);
}

static void FuzzConvertRequestOptionsWithDescriptors(FuzzedDataProvider& provider) {
  FIDO2_CredentialRequestOptions options;

  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  options.publicKey.challenge.val = challenge.empty() ? nullptr : challenge.data();
  options.publicKey.challenge.length = static_cast<uint32_t>(challenge.size());

  auto desc_count = provider.ConsumeIntegralInRange<uint32_t>(1, 4);
  std::vector<FIDO2_PublicKeyCredentialDescriptor> descriptors(desc_count);
  std::vector<std::vector<uint8_t>> id_storage(desc_count);
  std::vector<std::vector<FIDO2_AuthenticatorTransport>> transport_storage(desc_count);

  for (uint32_t i = 0; i < desc_count; ++i) {
    id_storage[i] = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
    descriptors[i].type = FIDO2_PUBLIC_KEY;
    descriptors[i].id.val = id_storage[i].empty() ? nullptr : id_storage[i].data();
    descriptors[i].id.length = static_cast<uint32_t>(id_storage[i].size());
    descriptors[i].transports.transportNum = 0;
    descriptors[i].transports.transports = nullptr;
  }

  options.publicKey.allowCredentials.allowCredentiallNum = desc_count;
  options.publicKey.allowCredentials.allowCredentials = descriptors.data();

  options.publicKey.userVerification =
      static_cast<FIDO2_UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));

  auto result = Convert<std::string>(options);
}

static void FuzzConvertCapabilityArray(FuzzedDataProvider& provider) {
  auto count = provider.ConsumeIntegralInRange<uint32_t>(0, 10);
  std::vector<FIDO2_Capability> caps(count);
  for (uint32_t i = 0; i < count; ++i) {
    caps[i].capability = static_cast<FIDO2_ClientCapability>(
        provider.ConsumeIntegralInRange<int32_t>(0, 9));
    caps[i].isSupported = provider.ConsumeBool();
  }

  FIDO2_CapabilityArray arr;
  arr.number = count;
  arr.capability = caps.empty() ? nullptr : caps.data();

  auto result = Convert<std::string>(arr);
}

static void FuzzConvertTransportArray(FuzzedDataProvider& provider) {
  FIDO2_AuthenticatorTransportArray null_arr{0, nullptr};
  auto opt1 = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(null_arr);

  FIDO2_AuthenticatorTransportArray zero_num_arr{0, nullptr};
  auto opt2 = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(zero_num_arr);

  auto count = provider.ConsumeIntegralInRange<uint32_t>(1, 6);
  std::vector<FIDO2_AuthenticatorTransport> transports(count);
  for (uint32_t i = 0; i < count; ++i) {
    transports[i] = static_cast<FIDO2_AuthenticatorTransport>(
        provider.ConsumeIntegralInRange<int32_t>(0, 5));
  }

  FIDO2_AuthenticatorTransportArray valid_arr;
  valid_arr.transportNum = count;
  valid_arr.transports = transports.data();
  auto opt3 = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(valid_arr);

  auto str1 = Convert<std::string>(null_arr);
  auto str2 = Convert<std::string>(valid_arr);

  base::flat_set<FidoTransportProtocol> proto_set;
  proto_set.insert(FidoTransportProtocol::kUsbHumanInterfaceDevice);
  proto_set.insert(FidoTransportProtocol::kInternal);
  proto_set.insert(FidoTransportProtocol::kHybrid);
  auto vec = Convert<std::vector<FIDO2_AuthenticatorTransport>>(proto_set);
}

static void FuzzConvertHintArray(FuzzedDataProvider& provider) {
  FIDO2_PublicKeyCredentialHintArray null_arr{0, nullptr};
  auto str1 = Convert<std::string>(null_arr);

  auto count = provider.ConsumeIntegralInRange<uint32_t>(0, 3);
  std::vector<FIDO2_PublicKeyCredentialHint> hints(count);
  for (uint32_t i = 0; i < count; ++i) {
    hints[i] = static_cast<FIDO2_PublicKeyCredentialHint>(
        provider.ConsumeIntegralInRange<int32_t>(0, 2));
  }

  FIDO2_PublicKeyCredentialHintArray valid_arr;
  valid_arr.hintNum = count;
  valid_arr.hints = hints.empty() ? nullptr : hints.data();
  auto str2 = Convert<std::string>(valid_arr);
}

static void FuzzConvertCredentialParams(FuzzedDataProvider& provider) {
  PublicKeyCredentialParams::CredentialInfo info;
  info.type = CredentialType::kPublicKey;
  info.algorithm = provider.ConsumeIntegralInRange<int32_t>(-260, 0);
  auto param = Convert<FIDO2_PublicKeyCredentialParameters>(info);

  PublicKeyCredentialParams::CredentialInfo info2;
  info2.type = CredentialType::kPublicKey;
  info2.algorithm = -7;
  auto param2 = Convert<FIDO2_PublicKeyCredentialParameters>(info2);

  std::vector<PublicKeyCredentialParams::CredentialInfo> infos;
  auto count = provider.ConsumeIntegralInRange<size_t>(0, 5);
  for (size_t i = 0; i < count; ++i) {
    PublicKeyCredentialParams::CredentialInfo ci;
    ci.type = CredentialType::kPublicKey;
    ci.algorithm = provider.ConsumeIntegralInRange<int32_t>(-260, 0);
    infos.push_back(ci);
  }

  PublicKeyCredentialParams params(infos);
  auto result = Convert<std::vector<FIDO2_PublicKeyCredentialParameters>>(params);
}

static void FuzzConvertEntityTypes(FuzzedDataProvider& provider) {
  {
    PublicKeyCredentialRpEntity rp("example.com", "Example RP");
    auto result = Convert<FIDO2_PublicKeyCredentialRpEntity>(rp);

    PublicKeyCredentialRpEntity rp_no_name("example.com");
    auto result2 = Convert<FIDO2_PublicKeyCredentialRpEntity>(rp_no_name);
  }

  {
    auto id_bytes =
        provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
    PublicKeyCredentialUserEntity user(id_bytes, "testuser", "Test User");
    auto result = Convert<FIDO2_PublicKeyCredentialUserEntity>(user);

    PublicKeyCredentialUserEntity user_minimal(id_bytes);
    auto result2 = Convert<FIDO2_PublicKeyCredentialUserEntity>(user_minimal);

    std::vector<uint8_t> empty_id;
    PublicKeyCredentialUserEntity user_empty(empty_id);
    auto result3 = Convert<FIDO2_PublicKeyCredentialUserEntity>(user_empty);
  }
}

static void FuzzInitializeAll() {
  {
    FIDO2_CredentialCreationOptions options;
    Initialize(&options);
  }
  {
    FIDO2_CredentialRequestOptions options;
    Initialize(&options);
  }
  {
    FIDO2_TokenBinding token_binding;
    Initialize(&token_binding);
  }
}

static void FuzzInitializeAndConvertOptions(FuzzedDataProvider& provider) {
  {
    FIDO2_CredentialCreationOptions options;
    Initialize(&options);
    auto str = Convert<std::string>(options);
  }
  {
    FIDO2_CredentialRequestOptions options;
    Initialize(&options);
    auto str = Convert<std::string>(options);
  }
  {
    FIDO2_TokenBinding token_binding;
    Initialize(&token_binding);
  }
}

static void FuzzCredentialOptionsDataHolderMakeCredential(FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto user_name = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));

  PublicKeyCredentialParams::CredentialInfo ci;
  ci.type = CredentialType::kPublicKey;
  ci.algorithm = -7;
  PublicKeyCredentialParams params({ci});

  auto user_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  PublicKeyCredentialUserEntity user(user_id, user_name, user_name);
  PublicKeyCredentialRpEntity rp(rp_id, rp_id);

  CtapMakeCredentialRequest request("", rp, user, params);
  request.attestation_preference = AttestationConveyancePreference::kNone;
  request.user_verification = UserVerificationRequirement::kDiscouraged;

  auto exclude_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < exclude_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
    PublicKeyCredentialDescriptor desc(CredentialType::kPublicKey, cred_id);
    request.exclude_list.push_back(std::move(desc));
  }

  CredentialOptionsDataHolder data_holder(request);
  MakeCredentialOptions options;
  FIDO2_CredentialCreationOptions fido_options;
  Initialize(&fido_options);
  Initialize(data_holder, request, options, &fido_options);
}

static void FuzzCredentialOptionsDataHolderGetAssertion(FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  CtapGetAssertionRequest request(rp_id, "");

  auto allow_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < allow_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
    PublicKeyCredentialDescriptor desc(CredentialType::kPublicKey, cred_id);
    request.allow_list.push_back(std::move(desc));
  }

  CredentialOptionsDataHolder data_holder(request);
  CtapGetAssertionOptions options;
  FIDO2_CredentialRequestOptions fido_options;
  Initialize(&fido_options);
  Initialize(data_holder, request, options, &fido_options);
}

static void FuzzInitializeMakeCredentialWithExtra(FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto user_name = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
  auto origin = provider.ConsumeRandomLengthString(128);

  PublicKeyCredentialParams::CredentialInfo ci;
  ci.type = CredentialType::kPublicKey;
  ci.algorithm = -7;
  PublicKeyCredentialParams params({ci});

  auto user_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 64));
  PublicKeyCredentialUserEntity user(user_id, user_name, user_name);
  PublicKeyCredentialRpEntity rp(rp_id, rp_id);

  CtapMakeCredentialRequest request("", rp, user, params);

  CtapMakeCredentialRequestExtra extra;
  extra.common.origin = origin;
  extra.common.challenge = challenge;
  extra.common.timeout = base::Seconds(provider.ConsumeIntegralInRange<int64_t>(0, 600));
  extra.common.mediation = static_cast<CredentialMediationRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 3));
  if (provider.ConsumeBool()) {
    extra.common.extensions = provider.ConsumeRandomLengthString(64);
  }
  if (provider.ConsumeBool()) {
    extra.common.hints.push_back(CredentialHint::kSecurityKey);
  }
  extra.attestation_formats.push_back("packed");
  extra.attestation_formats.push_back("none");
  request.extra = extra;

  CredentialOptionsDataHolder data_holder(request);
  MakeCredentialOptions options;
  options.authenticator_attachment = AuthenticatorAttachment::kPlatform;
  options.resident_key = ResidentKeyRequirement::kPreferred;
  options.user_verification = UserVerificationRequirement::kRequired;

  FIDO2_CredentialCreationOptions fido_options;
  Initialize(&fido_options);
  Initialize(data_holder, request, options, &fido_options);
}

static void FuzzInitializeGetAssertionWithExtra(FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
  auto origin = provider.ConsumeRandomLengthString(128);

  CtapGetAssertionRequest request(rp_id, "");

  auto allow_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < allow_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
    PublicKeyCredentialDescriptor desc(CredentialType::kPublicKey, cred_id);
    request.allow_list.push_back(std::move(desc));
  }

  CtapGetAssertionRequestExtra extra;
  extra.common.origin = origin;
  extra.common.challenge = challenge;
  extra.common.timeout = base::Seconds(provider.ConsumeIntegralInRange<int64_t>(0, 600));
  extra.common.mediation = static_cast<CredentialMediationRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 3));
  if (provider.ConsumeBool()) {
    extra.common.extensions = provider.ConsumeRandomLengthString(64);
  }
  request.extra = extra;

  CredentialOptionsDataHolder data_holder(request);
  CtapGetAssertionOptions options;
  FIDO2_CredentialRequestOptions fido_options;
  Initialize(&fido_options);
  Initialize(data_holder, request, options, &fido_options);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FuzzedDataProvider provider(data, size);
  uint8_t choice = provider.ConsumeIntegralInRange<uint8_t>(0, 28);

  switch (choice) {
    case 0:
      FuzzParseFixedInputs();
      break;
    case 1:
      FuzzParseFuzzedJson(provider);
      break;
    case 2:
      FuzzConvertAllEnums();
      break;
    case 3:
      FuzzConvertUint8Buff(provider);
      break;
    case 4:
      FuzzConvertVectorToString(provider);
      break;
    case 5:
      FuzzConvertHexStringToBytes(provider);
      break;
    case 6:
      FuzzConvertAttestationCredentialToString(provider);
      break;
    case 7:
      FuzzConvertAssertionCredentialToString(provider);
      break;
    case 8:
      FuzzConvertAttestationCredentialToResponse(provider);
      break;
    case 9:
      FuzzConvertAssertionCredentialToResponse(provider);
      break;
    case 10:
      FuzzConvertCreationOptionsToString(provider);
      break;
    case 11:
      FuzzConvertRequestOptionsToString(provider);
      break;
    case 12:
      FuzzConvertRequestOptionsWithDescriptors(provider);
      break;
    case 13:
      FuzzConvertCapabilityArray(provider);
      break;
    case 14:
      FuzzConvertTransportArray(provider);
      break;
    case 15:
      FuzzConvertHintArray(provider);
      break;
    case 16:
      FuzzConvertCredentialParams(provider);
      break;
    case 17:
      FuzzConvertEntityTypes(provider);
      break;
    case 18:
      FuzzInitializeAll();
      break;
    case 19:
      FuzzInitializeAndConvertOptions(provider);
      break;
    case 20: {
      FIDO2_PublicKeyAttestationCredential cred;
      cred.response.transports.transportNum = 0;
      cred.response.transports.transports = nullptr;
      auto with_transports =
          Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(
              cred.response.transports);
      break;
    }
    case 21: {
      std::vector<FIDO2_AuthenticatorTransport> all = {
          FIDO2_USB, FIDO2_NFC, FIDO2_BLE,
          FIDO2_SMART_CARD, FIDO2_HYBRID, FIDO2_INTERNAL};
      FIDO2_AuthenticatorTransportArray arr;
      arr.transportNum = static_cast<uint32_t>(all.size());
      arr.transports = all.data();
      auto opt = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(arr);
      auto str = Convert<std::string>(arr);
      break;
    }
    case 22: {
      FIDO2_PublicKeyCredentialHintArray hints;
      FIDO2_PublicKeyCredentialHint h[] = {FIDO2_SECURITY_KEY, FIDO2_CLIENT_DEVICE, FIDO2_HINT_HYBRID};
      hints.hintNum = 3;
      hints.hints = h;
      auto str = Convert<std::string>(hints);
      break;
    }
    case 23: {
      auto fuzzed_hex = provider.ConsumeRemainingBytesAsString();
      auto bytes = ConvertHexStringToBytes(fuzzed_hex);
      if (!bytes.empty()) {
        auto str = Convert<std::string>(bytes);
      }
      break;
    }
    case 24: {
      FIDO2_CapabilityArray caps_zero{0, nullptr};
      auto str = Convert<std::string>(caps_zero);
      break;
    }
    case 25:
      FuzzCredentialOptionsDataHolderMakeCredential(provider);
      break;
    case 26:
      FuzzCredentialOptionsDataHolderGetAssertion(provider);
      break;
    case 27:
      FuzzInitializeMakeCredentialWithExtra(provider);
      break;
    case 28:
      FuzzInitializeGetAssertionWithExtra(provider);
      break;
    default:
      break;
  }

  return 0;
}

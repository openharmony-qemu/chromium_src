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
#include <optional>
#include <string>
#include <vector>

#include "base/time/time.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_get_assertion_request_extra.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/ctap_make_credential_request_extra.h"
#include "device/fido/ctap_request_extra_common.h"
#include "device/fido/ohos/ohos_webauthn_api.h"
#include "device/fido/public_key_credential_descriptor.h"
#include "device/fido/public_key_credential_params.h"
#include "device/fido/public_key_credential_rp_entity.h"
#include "device/fido/public_key_credential_user_entity.h"

namespace {

void FuzzIsAvailable(FuzzedDataProvider& provider) {
  auto* api = device::OhosWebAuthnApi::Instance();
  api->IsAvailable();
}

void FuzzGetClientCapabilities(FuzzedDataProvider& provider) {
  auto* api = device::OhosWebAuthnApi::Instance();
  auto result = api->GetClientCapabilities();
  for (const auto& cap : result) {
    (void)cap.first;
    (void)cap.second;
  }
}

void FuzzIsUserVerifyingPlatformAuthenticatorAvailable(FuzzedDataProvider& provider) {
  auto* api = device::OhosWebAuthnApi::Instance();
  api->IsUserVerifyingPlatformAuthenticatorAvailable();
}

device::CtapMakeCredentialRequest BuildMakeCredentialRequest(
    FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto user_name = provider.ConsumeRandomLengthString(64);
  auto user_display = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 32));
  auto user_id = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 64));

  device::PublicKeyCredentialParams::CredentialInfo ci;
  ci.type = device::CredentialType::kPublicKey;
  ci.algorithm = provider.ConsumeIntegralInRange<int32_t>(-260, 0);
  device::PublicKeyCredentialParams params({ci});

  device::PublicKeyCredentialUserEntity user(user_id, user_name, user_display);
  device::PublicKeyCredentialRpEntity rp(rp_id, rp_id);

  device::CtapMakeCredentialRequest request("", rp, user, params);
  request.attestation_preference =
      static_cast<device::AttestationConveyancePreference>(
          provider.ConsumeIntegralInRange<int32_t>(0, 4));
  request.user_verification =
      static_cast<device::UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));
  request.authenticator_attachment =
      static_cast<device::AuthenticatorAttachment>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));
  request.resident_key_required = provider.ConsumeBool();

  auto exclude_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < exclude_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(
        provider.ConsumeIntegralInRange<size_t>(0, 32));
    device::PublicKeyCredentialDescriptor desc(
        device::CredentialType::kPublicKey, cred_id);
    request.exclude_list.push_back(std::move(desc));
  }

  return request;
}

device::MakeCredentialOptions BuildMakeCredentialOptions(
    FuzzedDataProvider& provider) {
  device::MakeCredentialOptions options;
  options.authenticator_attachment =
      static_cast<device::AuthenticatorAttachment>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));
  options.resident_key = static_cast<device::ResidentKeyRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 2));
  options.user_verification =
      static_cast<device::UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));
  return options;
}

void FuzzRegister(FuzzedDataProvider& provider) {
  auto request = BuildMakeCredentialRequest(provider);
  auto options = BuildMakeCredentialOptions(provider);

  auto* api = device::OhosWebAuthnApi::Instance();
  auto result = api->Register(std::move(request), std::move(options));
  (void)result.first;
  (void)result.second;
}

void FuzzRegisterWithExtra(FuzzedDataProvider& provider) {
  auto request = BuildMakeCredentialRequest(provider);

  device::CtapMakeCredentialRequestExtra extra;
  extra.common.origin = provider.ConsumeRandomLengthString(128);
  extra.common.challenge = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 32));
  extra.common.timeout =
      base::Seconds(provider.ConsumeIntegralInRange<int64_t>(0, 600));
  extra.common.mediation =
      static_cast<device::CredentialMediationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 3));
  if (provider.ConsumeBool()) {
    extra.common.extensions = provider.ConsumeRandomLengthString(64);
  }
  if (provider.ConsumeBool()) {
    extra.common.hints.push_back(device::CredentialHint::kSecurityKey);
  }
  if (provider.ConsumeBool()) {
    extra.common.hints.push_back(device::CredentialHint::kClientDevice);
  }
  auto format_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < format_count; ++i) {
    extra.attestation_formats.push_back(
        provider.ConsumeRandomLengthString(16));
  }
  request.extra = extra;

  auto options = BuildMakeCredentialOptions(provider);
  auto requesttmp = BuildMakeCredentialRequest(provider);

  auto* api = device::OhosWebAuthnApi::Instance();
  auto result = api->Register(std::move(requesttmp), std::move(options));
  (void)result.first;
  (void)result.second;
}

device::CtapGetAssertionRequest BuildGetAssertionRequest(
    FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  device::CtapGetAssertionRequest request(rp_id, "");
  request.user_verification =
      static_cast<device::UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));

  auto allow_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < allow_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(
        provider.ConsumeIntegralInRange<size_t>(0, 32));
    device::PublicKeyCredentialDescriptor desc(
        device::CredentialType::kPublicKey, cred_id);
    request.allow_list.push_back(std::move(desc));
  }

  return request;
}

void FuzzGetAssertion(FuzzedDataProvider& provider) {
  auto request = BuildGetAssertionRequest(provider);
  device::CtapGetAssertionOptions options;

  auto* api = device::OhosWebAuthnApi::Instance();
  auto result = api->GetAssertion(std::move(request), std::move(options));
  (void)result.first;
  (void)result.second;
}

void FuzzGetAssertionWithExtra(FuzzedDataProvider& provider) {
  auto request = BuildGetAssertionRequest(provider);

  device::CtapGetAssertionRequestExtra extra;
  extra.common.origin = provider.ConsumeRandomLengthString(128);
  extra.common.challenge = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 32));
  extra.common.timeout =
      base::Seconds(provider.ConsumeIntegralInRange<int64_t>(0, 600));
  extra.common.mediation =
      static_cast<device::CredentialMediationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 3));
  if (provider.ConsumeBool()) {
    extra.common.extensions = provider.ConsumeRandomLengthString(64);
  }
  if (provider.ConsumeBool()) {
    extra.common.hints.push_back(device::CredentialHint::kSecurityKey);
  }
  request.extra = extra;

  auto requesttemp = BuildGetAssertionRequest(provider);
  device::CtapGetAssertionOptions options;

  auto* api = device::OhosWebAuthnApi::Instance();
  auto result = api->GetAssertion(std::move(requesttemp), std::move(options));
  (void)result.first;
  (void)result.second;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FuzzedDataProvider provider(data, size);

  uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, 6);
  switch (action) {
    case 0:
      FuzzIsAvailable(provider);
      break;
    case 1:
      FuzzGetClientCapabilities(provider);
      break;
    case 2:
      FuzzIsUserVerifyingPlatformAuthenticatorAvailable(provider);
      break;
    case 3:
      FuzzRegister(provider);
      break;
    case 4:
      FuzzRegisterWithExtra(provider);
      break;
    case 5:
      FuzzGetAssertion(provider);
      break;
    case 6:
      FuzzGetAssertionWithExtra(provider);
      break;
    default:
      break;
  }

  return 0;
}

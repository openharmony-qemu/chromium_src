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
#include <string>
#include <vector>

#include "base/at_exit.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#define private public
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/ohos/ohos_authenticator.h"
#include "device/fido/ohos/ohos_webauthn_api.h"
#include "device/fido/public_key_credential_descriptor.h"
#include "device/fido/public_key_credential_params.h"
#include "device/fido/public_key_credential_rp_entity.h"
#include "device/fido/public_key_credential_user_entity.h"
#undef private

namespace {

using device::OhosAuthenticator;

void FuzzConstructorAndGetters(FuzzedDataProvider& provider) {
  OhosAuthenticator auth;

  auto type = auth.GetType();
  (void)type;

  auto id = auth.GetId();
  (void)id;

  auto display = auth.GetDisplayName();
  (void)display;

  auto transport = auth.AuthenticatorTransport();
  (void)transport;

  const auto& options = auth.Options();
  (void)options.is_platform_device;
  (void)options.supports_resident_key;
  (void)options.user_verification_availability;

  auto weak = auth.GetWeakPtr();
  (void)weak;

  auto algorithms = auth.GetAlgorithms();
  (void)algorithms;

  bool storage_full = auth.DiscoverableCredentialStorageFull();
  (void)storage_full;

  auto tunnel = auth.GetTunnelDevice();
  (void)tunnel;
}

void FuzzStaticStubs(FuzzedDataProvider& provider) {
  std::vector<uint8_t> cred_id =
      provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
  std::string rp_id = provider.ConsumeRandomLengthString(64);
  OhosAuthenticator::SignalUnknownCredential(cred_id, rp_id);

  std::vector<std::vector<uint8_t>> all_creds;
  auto count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < count; ++i) {
    all_creds.push_back(
        provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 16)));
  }
  std::vector<uint8_t> user_id =
      provider.ConsumeBytes<uint8_t>(provider.ConsumeIntegralInRange<size_t>(0, 32));
  OhosAuthenticator::SignalAllAcceptedCredentials(rp_id, user_id, all_creds);
}

void FuzzInitializeAuthenticator(FuzzedDataProvider& provider) {
  OhosAuthenticator auth;
  bool callback_called = false;
  base::OnceClosure callback = base::BindOnce(
      [](bool* called) { *called = true; }, &callback_called);
  auth.InitializeAuthenticator(std::move(callback));
  (void)callback_called;
}

void FuzzGetPlatformCredentialInfoForRequest(FuzzedDataProvider& provider) {
  OhosAuthenticator auth;
  auto rp_id = provider.ConsumeRandomLengthString(64);
  device::CtapGetAssertionRequest request(rp_id, "");

  auto allow_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < allow_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(
        provider.ConsumeIntegralInRange<size_t>(0, 32));
    device::PublicKeyCredentialDescriptor desc(
        device::CredentialType::kPublicKey, cred_id);
    request.allow_list.push_back(std::move(desc));
  }

  device::CtapGetAssertionOptions options;
  bool callback_called = false;
  auto callback = base::BindOnce(
      [](bool* called,
         std::vector<device::DiscoverableCredentialMetadata> creds,
         device::FidoRequestHandlerBase::RecognizedCredential has) {
        *called = true;
      },
      &callback_called);

  auth.GetPlatformCredentialInfoForRequest(request, options, std::move(callback));
  (void)callback_called;
}

void FuzzCancelState(FuzzedDataProvider& provider) {
  OhosAuthenticator auth;

  auth.Cancel();

  auth.is_pending_ = true;
  auth.Cancel();
  if (auth.waiting_for_cancellation_) {
    auth.Cancel();
  }

  auth.is_pending_ = false;
  auth.waiting_for_cancellation_ = false;
  auth.Cancel();
}

void FuzzMakeCredential(FuzzedDataProvider& provider) {
  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto user_name = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 32));
  auto user_id = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 64));

  device::PublicKeyCredentialParams::CredentialInfo ci;
  ci.type = device::CredentialType::kPublicKey;
  ci.algorithm = provider.ConsumeIntegralInRange<int32_t>(-260, 0);
  device::PublicKeyCredentialParams params({ci});

  device::PublicKeyCredentialUserEntity user(user_id, user_name, user_name);
  device::PublicKeyCredentialRpEntity rp(rp_id, rp_id);

  device::CtapMakeCredentialRequest request("", rp, user, params);
  request.attestation_preference =
      static_cast<device::AttestationConveyancePreference>(
          provider.ConsumeIntegralInRange<int32_t>(0, 4));

  auto exclude_count = provider.ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < exclude_count; ++i) {
    auto cred_id = provider.ConsumeBytes<uint8_t>(
        provider.ConsumeIntegralInRange<size_t>(0, 32));
    device::PublicKeyCredentialDescriptor desc(
        device::CredentialType::kPublicKey, cred_id);
    request.exclude_list.push_back(std::move(desc));
  }

  device::MakeCredentialOptions options;
  options.authenticator_attachment =
      static_cast<device::AuthenticatorAttachment>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));
  options.resident_key = static_cast<device::ResidentKeyRequirement>(
      provider.ConsumeIntegralInRange<int32_t>(0, 2));
  options.user_verification =
      static_cast<device::UserVerificationRequirement>(
          provider.ConsumeIntegralInRange<int32_t>(0, 2));

  OhosAuthenticator auth;
  auto callback = base::BindOnce(
      [](device::MakeCredentialStatus status,
         std::optional<device::AuthenticatorMakeCredentialResponse> response) {});

  auth.MakeCredential(std::move(request), std::move(options),
                      std::move(callback));
}

void FuzzGetAssertion(FuzzedDataProvider& provider) {
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

  device::CtapGetAssertionOptions options;

  OhosAuthenticator auth;
  auto callback = base::BindOnce(
      [](device::GetAssertionStatus status,
         std::vector<device::AuthenticatorGetAssertionResponse> responses) {});

  auth.GetAssertion(std::move(request), std::move(options),
                    std::move(callback));
}

void FuzzPINUVDisposition(FuzzedDataProvider& provider) {
  OhosAuthenticator auth;

  auto rp_id = provider.ConsumeRandomLengthString(64);
  auto user_name = provider.ConsumeRandomLengthString(64);
  auto challenge = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 32));
  auto user_id = provider.ConsumeBytes<uint8_t>(
      provider.ConsumeIntegralInRange<size_t>(0, 64));

  device::PublicKeyCredentialParams::CredentialInfo ci;
  ci.type = device::CredentialType::kPublicKey;
  ci.algorithm = -7;
  device::PublicKeyCredentialParams params({ci});
  device::PublicKeyCredentialUserEntity user(user_id, user_name, user_name);
  device::PublicKeyCredentialRpEntity rp(rp_id, rp_id);

  device::CtapMakeCredentialRequest make_req("", rp, user, params);
  auto disposition = auth.PINUVDispositionForMakeCredential(make_req, nullptr);
  (void)disposition;

  device::CtapGetAssertionRequest get_req(rp_id, "");
  auto get_disposition = auth.PINUVDispositionForGetAssertion(get_req, nullptr);
  (void)get_disposition;
}

void FuzzStaticApiMethods(FuzzedDataProvider& provider) {
  bool uvpa_called = false;
  auto uvpa_callback = base::BindOnce(
      [](bool* called, bool result) { *called = true; }, &uvpa_called);
  OhosAuthenticator::IsUserVerifyingPlatformAuthenticatorAvailable(
      std::move(uvpa_callback));

  bool caps_called = false;
  auto caps_callback = base::BindOnce(
      [](bool* called,
         const OhosAuthenticator::ClientCapabilities& caps) { *called = true; },
      &caps_called);
  OhosAuthenticator::GetClientCapabilities(std::move(caps_callback));
}

}  // namespace

static base::AtExitManager* g_at_exit = nullptr;
static std::optional<base::SingleThreadTaskExecutor> g_executor;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  static bool initialized = false;
  if (!initialized) {
    g_at_exit = new base::AtExitManager();
    g_executor.emplace(base::MessagePumpType::DEFAULT);
    base::ThreadPoolInstance::CreateAndStartWithDefaultParams("fuzzer");
    initialized = true;
  }

  FuzzedDataProvider provider(data, size);
  base::RunLoop run_loop;

  uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, 8);
  switch (action) {
    case 0:
      FuzzConstructorAndGetters(provider);
      break;
    case 1:
      FuzzStaticStubs(provider);
      break;
    case 2:
      FuzzStaticApiMethods(provider);
      break;
    case 3:
      FuzzInitializeAuthenticator(provider);
      break;
    case 4:
      FuzzGetPlatformCredentialInfoForRequest(provider);
      break;
    case 5:
      FuzzCancelState(provider);
      break;
    case 6:
      FuzzMakeCredential(provider);
      break;
    case 7:
      FuzzGetAssertion(provider);
      break;
    case 8:
      FuzzPINUVDisposition(provider);
      break;
    default:
      break;
  }

  run_loop.RunUntilIdle();
  return 0;
}

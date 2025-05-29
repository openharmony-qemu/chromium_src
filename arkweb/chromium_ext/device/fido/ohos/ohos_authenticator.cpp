// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/fido/ohos/ohos_authenticator.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "base/functional/bind.h"
#include "base/native_library.h"
#include "base/notreached.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "components/device_event_log/device_event_log.h"
#include "device/fido/authenticator_supported_options.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/discoverable_credential_metadata.h"
#include "device/fido/features.h"
#include "device/fido/fido_constants.h"
#include "device/fido/fido_request_handler_base.h"
#include "device/fido/fido_transport_protocol.h"
#include "device/fido/ohos/ohos_webauthn_api.h"
#include "device/fido/public_key_credential_descriptor.h"

namespace device {

namespace {

AuthenticatorSupportedOptions OhosWebAuthnApiOptions()
{
  AuthenticatorSupportedOptions options;
  options.is_platform_device =
      AuthenticatorSupportedOptions::PlatformDevice::kBoth;
  options.supports_resident_key = true;
  options.user_verification_availability = AuthenticatorSupportedOptions::
      UserVerificationAvailability::kSupportedAndConfigured;
  options.supports_user_presence = true;
  options.supports_cred_protect = true;
  options.enterprise_attestation = true;
  options.large_blob_type = LargeBlobSupportType::kBespoke;
  options.supports_min_pin_length_extension = true;
  options.max_cred_blob_length = 256;
  options.supports_hmac_secret = true;
  return options;
}

bool MayHaveWindowsHelloCredentials(
    std::vector<PublicKeyCredentialDescriptor> allow_list)
{
  return allow_list.empty() ||
         base::ranges::any_of(allow_list, [](const auto& credential) {
           return credential.transports.empty() ||
                  base::Contains(credential.transports,
                                 FidoTransportProtocol::kInternal);
         });
}

// Filters credentials from |found_creds| that are not present in
// |allow_list_creds|.
void FilterFoundCredentials(
    std::vector<DiscoverableCredentialMetadata>* found_creds,
    const std::vector<PublicKeyCredentialDescriptor>& allow_list_creds)
{
  auto remove_it = base::ranges::remove_if(
      *found_creds, [&allow_list_creds](const auto& found_cred) {
        return base::ranges::none_of(
            allow_list_creds, [&found_cred](const auto& allow_list_cred) {
              return allow_list_cred.id == found_cred.cred_id;
            });
      });
  found_creds->erase(remove_it, found_creds->end());
}

}  // namespace

// static
void OhosAuthenticator::IsUserVerifyingPlatformAuthenticatorAvailable(
    base::OnceCallback<void(bool is_available)> callback)
{
  std::move(callback).Run(true);
}

// static
void OhosAuthenticator::SignalUnknownCredential(
    const std::vector<uint8_t>& credential_id,
    const std::string& relying_party_id)
{
}

// static
void OhosAuthenticator::SignalAllAcceptedCredentials(
    const std::string& relying_party_id,
    const std::vector<uint8_t>& user_id,
    const std::vector<std::vector<uint8_t>>& all_accepted_credential_ids)
{
}

// static
std::vector<std::pair<std::string, bool>> OhosAuthenticator::GetClientCapabilities()
{
  return OhosWebAuthnApi::Instance()->GetClientCapabilities();
}

OhosAuthenticator::OhosAuthenticator()
    : options_(OhosWebAuthnApiOptions()),
      ohos_api_(OhosWebAuthnApi::Instance())
{
  CHECK(ohos_api_->IsAvailable());
}

OhosAuthenticator::~OhosAuthenticator()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  Cancel();
}

void OhosAuthenticator::InitializeAuthenticator(
    base::OnceClosure callback)
{
  std::move(callback).Run();
}

void OhosAuthenticator::MakeCredential(
    CtapMakeCredentialRequest request,
    MakeCredentialOptions options,
    MakeCredentialCallback callback)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (is_pending_) {
    NOTREACHED();
  }
  is_pending_ = true;

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::TaskPriority::USER_BLOCKING, base::MayBlock()},
      base::BindOnce(&OhosWebAuthnApi::Register, base::Unretained(ohos_api_),
                     std::move(request),
                     std::move(options)),
      base::BindOnce(&OhosAuthenticator::MakeCredentialDone,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void OhosAuthenticator::MakeCredentialDone(
    MakeCredentialCallback callback,
    std::pair<MakeCredentialStatus,
              std::optional<AuthenticatorMakeCredentialResponse>> result)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(is_pending_);
  is_pending_ = false;
  if (waiting_for_cancellation_) {
    // Don't bother invoking the reply callback if the caller has already
    // cancelled the operation.
    waiting_for_cancellation_ = false;
    return;
  }
  if (result.first != MakeCredentialStatus::kSuccess) {
    std::move(callback).Run(result.first, std::nullopt);
    return;
  }
  CHECK(result.second);
  std::move(callback).Run(result.first, std::move(result.second));
}

void OhosAuthenticator::GetAssertion(CtapGetAssertionRequest request,
                                     CtapGetAssertionOptions options,
                                     GetAssertionCallback callback)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(!is_pending_);
  if (is_pending_) {
    return;
  }

  is_pending_ = true;

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::TaskPriority::USER_BLOCKING, base::MayBlock()},
      base::BindOnce(&OhosWebAuthnApi::GetAssertion, base::Unretained(ohos_api_),
                     std::move(request),
                     std::move(options)),
      base::BindOnce(&OhosAuthenticator::GetAssertionDone,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void OhosAuthenticator::GetAssertionDone(
    GetAssertionCallback callback,
    std::pair<GetAssertionStatus,
              std::optional<AuthenticatorGetAssertionResponse>> result)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(is_pending_);
  is_pending_ = false;
  if (waiting_for_cancellation_) {
    waiting_for_cancellation_ = false;
    return;
  }
  if (result.first != GetAssertionStatus::kSuccess) {
    std::move(callback).Run(result.first, {});
    return;
  }
  CHECK(result.second);
  std::vector<AuthenticatorGetAssertionResponse> responses;
  responses.emplace_back(std::move(*result.second));
  std::move(callback).Run(result.first, std::move(responses));
}

void OhosAuthenticator::GetPlatformCredentialInfoForRequest(
    const CtapGetAssertionRequest& request,
    const CtapGetAssertionOptions& request_options,
    GetPlatformCredentialInfoForRequestCallback callback)
{
  std::move(callback).Run({},
      FidoRequestHandlerBase::RecognizedCredential::kNoRecognizedCredential);
}

void OhosAuthenticator::GetTouch(base::OnceClosure callback)
{
  NOTREACHED();
}

void OhosAuthenticator::Cancel()
{
  if (!is_pending_ || waiting_for_cancellation_) {
    return;
  }

  waiting_for_cancellation_ = true;
}

AuthenticatorType OhosAuthenticator::GetType() const
{
  return AuthenticatorType::kOhosNative;
}

std::string OhosAuthenticator::GetId() const
{
  return "OhosAuthenticator";
}

std::optional<FidoTransportProtocol> OhosAuthenticator::AuthenticatorTransport() const
{
  return std::nullopt;
}

const AuthenticatorSupportedOptions& OhosAuthenticator::Options() const
{
  return options_;
}

base::WeakPtr<FidoAuthenticator> OhosAuthenticator::GetWeakPtr()
{
  return weak_factory_.GetWeakPtr();
}

}  // namespace device

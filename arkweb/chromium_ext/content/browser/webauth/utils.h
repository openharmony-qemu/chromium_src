// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_WEBAUTH_UTILS_H_
#define CONTENT_BROWSER_WEBAUTH_UTILS_H_

#include "device/fido/authenticator_get_assertion_response.h"
#include "device/fido/authenticator_make_credential_response.h"
#include "device/fido/ctap_get_assertion_request_extra.h"
#include "device/fido/ctap_make_credential_request_extra.h"
#include "third_party/blink/public/mojom/webauthn/authenticator.mojom.h"

namespace content {

device::CredentialMediationRequirement Convert(
    const blink::mojom::PublicKeyCredentialMediationRequirement& mediation);

device::CredentialHint Convert(const blink::mojom::Hint&);

device::CtapRequestExtraCommon CreateCtapRequestExtraCommon(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialCreationOptionsPtr& options);

device::CtapRequestExtraCommon CreateCtapRequestExtraCommon(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialRequestOptionsPtr& options);

device::CtapMakeCredentialRequestExtra CreateCtapMakeCredentialRequestExtra(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialCreationOptionsPtr& options);

device::CtapGetAssertionRequestExtra CreateCtapGetAssertionRequestExtra(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialRequestOptionsPtr& options);

blink::mojom::MakeCredentialAuthenticatorResponsePtr CreateMakeCredentialResponse(
    const device::AuthenticatorMakeCredentialResponse& response_data);

blink::mojom::GetAssertionAuthenticatorResponsePtr CreateGetAssertionResponse(
    const device::AuthenticatorGetAssertionResponse& response_data);

void GetClientCapabilitiesFromOhosWebAuthnApi(
    blink::mojom::Authenticator::GetClientCapabilitiesCallback callback);

} //namespace content

#endif  // CONTENT_BROWSER_WEBAUTH_UTILS_H_

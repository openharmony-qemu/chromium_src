// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/webauth/utils.h"

namespace content {

device::CredentialMediationRequirement Convert(
    const blink::mojom::PublicKeyCredentialMediationRequirement& mediation)
{
    switch (mediation) {
        case blink::mojom::PublicKeyCredentialMediationRequirement::SILENT:
            return device::CredentialMediationRequirement::kSilent;
        case blink::mojom::PublicKeyCredentialMediationRequirement::OPTIONAL:
            return device::CredentialMediationRequirement::kOptional;
        case blink::mojom::PublicKeyCredentialMediationRequirement::REQUIRED:
            return device::CredentialMediationRequirement::kRequired;
        case blink::mojom::PublicKeyCredentialMediationRequirement::CONDITIONAL:
            return device::CredentialMediationRequirement::kConditional;
    }
    return device::CredentialMediationRequirement::kSilent;
}

device::CredentialHint Convert(const blink::mojom::Hint& hint)
{
    switch (hint) {
        case blink::mojom::Hint::SECURITY_KEY:
            return device::CredentialHint::kSecurityKey;
        case blink::mojom::Hint::CLIENT_DEVICE:
            return device::CredentialHint::kClientDevice;
        case blink::mojom::Hint::HYBRID:
            return device::CredentialHint::kHintHybrid;
    }
    return device::CredentialHint::kHintHybrid;
}

device::CtapRequestExtraCommon CreateCtapRequestExtraCommon(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialCreationOptionsPtr& options)
{
    device::CtapRequestExtraCommon ret;
    ret.origin = caller_origin.Serialize();
    ret.challenge = options->challenge;
    ret.mediation = Convert(options->mediation);
    if (options->timeout) {
        ret.timeout = *options->timeout;
    }
    ret.hints.reserve(options->hints.size());
    for (const auto& hint : options->hints) {
        ret.hints.push_back(Convert(hint));
    }
    return ret;
}

device::CtapRequestExtraCommon CreateCtapRequestExtraCommon(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialRequestOptionsPtr& options)
{
    device::CtapRequestExtraCommon ret;
    ret.origin = caller_origin.Serialize();
    ret.challenge = options->challenge;
    ret.mediation = Convert(options->mediation);
    if (options->timeout) {
        ret.timeout = *options->timeout;
    }
    ret.hints.reserve(options->hints.size());
    for (const auto& hint : options->hints) {
        ret.hints.push_back(Convert(hint));
    }
    // Not support extensions.
    return ret;
}

device::CtapMakeCredentialRequestExtra CreateCtapMakeCredentialRequestExtra(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialCreationOptionsPtr& options)
{
    device::CtapMakeCredentialRequestExtra ret;
    ret.common = CreateCtapRequestExtraCommon(caller_origin, options);
    ret.attestation_formats = options->attestation_formats;
    return ret;
}

device::CtapGetAssertionRequestExtra CreateCtapGetAssertionRequestExtra(
    const url::Origin& caller_origin,
    const blink::mojom::PublicKeyCredentialRequestOptionsPtr& options)
{
    device::CtapGetAssertionRequestExtra ret;
    ret.common = CreateCtapRequestExtraCommon(caller_origin, options);
    return ret;
}

} // namespace content

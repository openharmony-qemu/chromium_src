// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/webauth/utils.h"

#include "base/logging.h"
#include "base/values.h"
#include "base/strings/string_number_conversions.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "content/browser/webauth/common_utils.h"
#include "device/fido/ohos/ohos_authenticator.h"

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
    if (options->extensions) {
        base::Value::Dict dict;
        if (options->extensions->large_blob_write.has_value()) {
            std::string encoded = base::HexEncode(options->extensions->large_blob_write->data(),
                options->extensions->large_blob_write->size());
            dict.Set("write", encoded);
        }
        dict.Set("read", options->extensions->large_blob_read);
        base::Value::Dict dict_large_blob;
        if (options->extensions->large_blob_read || options->extensions->large_blob_write) {
            dict_large_blob.Set("largeBlob", std::move(dict));
        }
        std::string str;
        if (base::JSONWriter::Write(dict_large_blob, &str)) {
            LOG(INFO) << "convert largeBlob successfully";
        }
        ret.extensions = str;
        return ret;
    }
    ret.extensions = std::nullopt;
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

blink::mojom::MakeCredentialAuthenticatorResponsePtr CreateMakeCredentialResponse(
    const device::AuthenticatorMakeCredentialResponse& response_data)
{
    LOG(INFO) << "CreateMakeCredentialResponse response_extra";
    auto response = blink::mojom::MakeCredentialAuthenticatorResponse::New();
    auto common_info = blink::mojom::CommonCredentialInfo::New();

    common_info->client_data_json =
        response_data.response_extra->common.client_data_json;
    common_info->raw_id = response_data.response_extra->common.raw_id;
    common_info->id = Base64UrlEncodeChallenge(common_info->raw_id);
    common_info->authenticator_data =
        response_data.response_extra->common.authenticator_data;
    response->info = std::move(common_info);
    response->attestation_object =
        response_data.response_extra->attestation_object;
    if (response_data.transports) {
        response->transports.assign(
            response_data.transports->begin(),
            response_data.transports->end());
    }
    response->public_key_der = response_data.response_extra->public_key;
    response->public_key_algo =
        response_data.response_extra->public_key_algorithm;

    return response;
}

blink::mojom::GetAssertionAuthenticatorResponsePtr CreateGetAssertionResponse(
    const device::AuthenticatorGetAssertionResponse& response_data)
{
    LOG(INFO) << "CreateGetAssertionResponse response_extra";
    auto response = blink::mojom::GetAssertionAuthenticatorResponse::New();
    auto common_info = blink::mojom::CommonCredentialInfo::New();

    common_info->client_data_json =
        response_data.response_extra->common.client_data_json;
    common_info->raw_id = response_data.response_extra->common.raw_id;
    common_info->id = Base64UrlEncodeChallenge(common_info->raw_id);
    common_info->authenticator_data =
        response_data.response_extra->common.authenticator_data;
    response->info = std::move(common_info);

    response->authenticator_attachment =
        response_data.response_extra->common.autenticator_attachment;
    response->signature = response_data.response_extra->signature;
    response->user_handle = response_data.response_extra->user_handle;
    response->extensions =
        blink::mojom::AuthenticationExtensionsClientOutputs::New();

    std::optional<base::Value> json_val = base::JSONReader::Read(
        response_data.response_extra->common.client_extension_results);
    if (!json_val.has_value() || !json_val->is_dict()) {
        return response;
    }
    const base::Value::Dict& outer_dict = json_val->GetDict();
    // largeBlob
    const base::Value* large_blob_val = outer_dict.Find("largeBlob");
    if (large_blob_val != nullptr && large_blob_val->is_dict()) {
        response->extensions->echo_large_blob = true;
        const base::Value::Dict& inner_dict = large_blob_val->GetDict();
        const base::Value* written = inner_dict.Find("written");
        if (written != nullptr && written->is_bool()) {
            response->extensions->echo_large_blob_written = true;
            response->extensions->large_blob_written = written->GetBool();
        }
        const std::string* blob = inner_dict.FindString("blob");
        if (blob != nullptr) {
            std::vector<uint8_t> temp_vec;
            base::HexStringToBytes(*blob, &temp_vec);
            response->extensions->large_blob = std::move(temp_vec);
        }
    }
    // digital shield data
    const std::string* auth_data_hex = outer_dict.FindString("authData");
    if (auth_data_hex != nullptr) {
        std::vector<uint8_t> temp_vec;
        base::HexStringToBytes(*auth_data_hex, &temp_vec);
        response->extensions->digital_shield_data = std::move(temp_vec);
    }

    return response;
}

void GetClientCapabilitiesFromOhosWebAuthnApi(
    blink::mojom::Authenticator::GetClientCapabilitiesCallback callback)
{
    device::OhosAuthenticator::GetClientCapabilities(base::BindOnce(
        [](const std::vector<std::pair<std::string, bool>>& capabilities) {
            std::vector<blink::mojom::WebAuthnClientCapabilityPtr> result;
            result.reserve(capabilities.size());
            for (const auto& capability : capabilities) {
                result.push_back(blink::mojom::WebAuthnClientCapability::New(
                    capability.first, capability.second));
            }
            return result;
        }).Then(std::move(callback)));
}

} // namespace content

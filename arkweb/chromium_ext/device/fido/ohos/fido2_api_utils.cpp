// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/fido/ohos/fido2_api_utils.h"

#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "device/fido/attestation_statement_formats.h"
#include "device/fido/ohos/type_conversions.h"

namespace device {

std::optional<base::Value::Dict> ExtractAttestationObjectAsJson(
    const FIDO2_PublicKeyAttestationCredential* credential_ptr)
{
    if (!credential_ptr) {
        LOG(INFO) << "exit ExtractAttestationObjectAsJson, !credential_ptr";
        return std::nullopt;
    }
    const Uint8Buff& buffer = credential_ptr->response.attestationObject;
    std::string attestation_json_data(buffer.val, buffer.val + buffer.length);
    std::optional<base::Value> json_response =
        base::JSONReader::Read(attestation_json_data);
    if (!json_response || !json_response->is_dict()) {
        LOG(INFO) << "exit ExtractAttestationObjectAsJson, !json_response";
        return std::nullopt;
    }
    return std::move(json_response->GetDict());
}

std::unique_ptr<PackedAttestationStatement> BuildPackedAttestationStatement(
    const base::Value::Dict* att_stmt)
{
    std::optional<int> alg = att_stmt->FindInt("alg");
    const std::string* sig = att_stmt->FindString("sig");
    const base::Value::List* x5c = att_stmt->FindList("x5c");
    if (!alg || !sig || !x5c) {
        LOG(INFO) << "exit Build, !alg || !sig || !x5c";
        return nullptr;
    }
    CoseAlgorithmIdentifier algorithm = static_cast<CoseAlgorithmIdentifier>(*alg);
    std::vector<uint8_t> signature = ConvertHexStringToBytes(*sig);
    std::vector<std::vector<uint8_t>> x509_certificates;
    for (const auto& x5c_item : *x5c) {
        if (!x5c_item.is_string()) {
            LOG(INFO) << "exit Build, !x5c_item.is_string()";
            return nullptr;
        }
        x509_certificates.push_back(ConvertHexStringToBytes(x5c_item.GetString()));
    }
    return std::make_unique<PackedAttestationStatement>(
        algorithm, std::move(signature), std::move(x509_certificates));
}

std::optional<AuthenticatorMakeCredentialResponse> Parse(
    FIDO2_PublicKeyAttestationCredential* credential_ptr)
{
    std::optional<AuthenticatorMakeCredentialResponse> ret;
    LOG(INFO) << "enter Parse";
    std::optional<base::Value::Dict> response_root =
        ExtractAttestationObjectAsJson(credential_ptr);
    if (!response_root) {
        LOG(INFO) << "exit Parse, !response_root";
        return ret;
    }
    const std::string* fmt = response_root->FindString("fmt");
    const base::Value::Dict* att_stmt = response_root->FindDict("attStmt");
    std::unique_ptr<AttestationStatement> attestation_statement;
    std::optional<FidoTransportProtocol> transport_used;
    if (!fmt || !att_stmt) {
        LOG(INFO) << "exit Parse, fmt[" << fmt << "], att_stmt[" << att_stmt << "]";
        return ret;
    }
    if (*fmt == "packed") {
        attestation_statement = BuildPackedAttestationStatement(att_stmt);
        if (!attestation_statement) {
            LOG(INFO) << "exit Parse, !attestation_statement";
            return ret;
        }
    }
    const std::string* auth_data_str = response_root->FindString("authData");
    if (!auth_data_str) {
        LOG(INFO) << "exit Parse, !auth_data_str";
        return ret;
    }
    auto auth_data_bytes = ConvertHexStringToBytes(*auth_data_str);
    auto auth_data = AuthenticatorData::DecodeAuthenticatorData(auth_data_bytes);
    if (!auth_data) {
        LOG(INFO) << "exit Parse, !auth_data";
        return ret;
    }
    AttestationObject attestation_object(
        std::move(*auth_data), std::move(attestation_statement));
    ret = AuthenticatorMakeCredentialResponse(
        transport_used, std::move(attestation_object));
    LOG(INFO) << "exit Parse";
    return ret;
}

} // namespace device

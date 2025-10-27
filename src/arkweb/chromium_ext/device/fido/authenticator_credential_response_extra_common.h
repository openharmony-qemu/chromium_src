// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_AUTHENTICATOR_CREDENTIAL_RESPONSE_EXTRA_COMMON_H_
#define DEVICE_FIDO_AUTHENTICATOR_CREDENTIAL_RESPONSE_EXTRA_COMMON_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "device/fido/fido_types.h"

struct AuthenticatorCredentialResponseExtraCommon {
  std::vector<uint8_t> raw_id;
  std::vector<uint8_t> client_data_json;
  std::vector<uint8_t> authenticator_data;
  device::AuthenticatorAttachment autenticator_attachment =
      device::AuthenticatorAttachment::kAny;
  std::optional<std::string> id;
  std::optional<std::string> type;
  std::string client_extension_results;
};

struct AuthenticatorMakeCredentialResponseExtra {
  AuthenticatorCredentialResponseExtraCommon common;
  std::vector<uint8_t> attestation_object;
  std::vector<uint8_t> public_key;
  int32_t public_key_algorithm = 0;
};

struct AuthenticatorGetAssertionResponseExtra {
  AuthenticatorCredentialResponseExtraCommon common;
  std::vector<uint8_t> signature;
  std::vector<uint8_t> user_handle;
};

#endif  // DEVICE_FIDO_AUTHENTICATOR_CREDENTIAL_RESPONSE_EXTRA_COMMON_H_

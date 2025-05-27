// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_OHOS_OHOS_WEBAUTHN_API_H_
#define DEVICE_FIDO_OHOS_OHOS_WEBAUTHN_API_H_

#include "device/fido/authenticator_get_assertion_response.h"
#include "device/fido/authenticator_make_credential_response.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/discoverable_credential_metadata.h"
#include "device/fido/fido_authenticator.h"
#include "device/fido/fido_constants.h"
#include "device/fido/ohos/fido2_api.h"

namespace device {

class OhosWebAuthnApi {
 public:
  static OhosWebAuthnApi* Instance();
  virtual ~OhosWebAuthnApi();

  virtual bool IsAvailable();

  using GetClientCapabilitiesResult = std::vector<std::pair<std::string, bool>>;
  virtual GetClientCapabilitiesResult GetClientCapabilities();

  using RegisterResult = std::pair<MakeCredentialStatus,
      std::optional<AuthenticatorMakeCredentialResponse>>;
  virtual RegisterResult Register(
      CtapMakeCredentialRequest request,
      MakeCredentialOptions request_options);

  using GetAssertionResult = std::pair<GetAssertionStatus,
      std::optional<AuthenticatorGetAssertionResponse>>;
  virtual GetAssertionResult GetAssertion(CtapGetAssertionRequest request,
               CtapGetAssertionOptions options);
};

} // namespace device

#endif  // DEVICE_FIDO_OHOS_OHOS_WEBAUTHN_API_H_

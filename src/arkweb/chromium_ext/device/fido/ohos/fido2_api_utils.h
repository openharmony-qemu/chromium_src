// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_OHOS_UTILS_H_
#define DEVICE_FIDO_OHOS_UTILS_H_

#include <optional>

#include "device/fido/authenticator_make_credential_response.h"
#include "device/fido/ohos/fido2_api.h"

namespace device {

std::optional<AuthenticatorMakeCredentialResponse> Parse(
    FIDO2_PublicKeyAttestationCredential* credential_ptr);

} // namespace device

#endif  // DEVICE_FIDO_OHOS_UTILS_H_

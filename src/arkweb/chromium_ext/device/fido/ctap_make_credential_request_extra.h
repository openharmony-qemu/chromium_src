// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_CTAP_MAKE_CREDENTIAL_REQUEST_EXTRA_H_
#define DEVICE_FIDO_CTAP_MAKE_CREDENTIAL_REQUEST_EXTRA_H_

#include <stdint.h>
#include <optional>
#include <string>
#include <vector>

#include "device/fido/ctap_request_extra_common.h"

namespace device {

struct CtapMakeCredentialRequestExtra {
  CtapRequestExtraCommon common;
  std::vector<std::string> attestation_formats;
};

} // namespace device
#endif  // DEVICE_FIDO_CTAP_MAKE_CREDENTIAL_REQUEST_EXTRA_H_

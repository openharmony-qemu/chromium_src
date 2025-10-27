// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_CTAP_REQUEST_EXTRA_COMMON_H_
#define DEVICE_FIDO_CTAP_REQUEST_EXTRA_COMMON_H_

#include <stdint.h>
#include <optional>
#include <string>
#include <vector>

#include "base/time/time.h"

namespace device {

enum class CredentialMediationRequirement {
  kSilent = 0,
  kOptional,
  kConditional,
  kRequired,
};

enum class CredentialHint {
  kSecurityKey = 0,
  kClientDevice,
  kHintHybrid,
};

struct CtapRequestExtraCommon {
  std::string origin;
  std::vector<uint8_t> challenge;
  CredentialMediationRequirement mediation =
      CredentialMediationRequirement::kSilent;
  base::TimeDelta timeout;
  std::vector<CredentialHint> hints;
  std::optional<std::string> extensions;
};

} // namespace device

#endif  // DEVICE_FIDO_CTAP_REQUEST_EXTRA_COMMON_H_

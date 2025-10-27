// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_THREAT_TYPE_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_THREAT_TYPE_H_

namespace ohos_safe_browsing {

// The type of block page
enum OHSBThreatType {
  THREAT_DEFAULT = -1,
  THREAT_ILLEGAL = 0,
  THREAT_FRAUD = 1,
  THREAT_RISK = 2,
  THREAT_WARNING = 3,
  THREAT_URL_TRUST_LIST = 4,
  THREAT_UNPROCESSED = 5,
};

enum OHSBPolicyType {
  POLICY_NO_PROMPT = 0,
  POLICY_DANGER_LABEL = 1,
  POLICY_POPUP_AND_DANGER = 2,
  POLICY_FORBIDDEN_PROHIBIT_ACCESS = 3,
  POLICY_CHILD_MODE_PROHIBIT_ACCESS = 4,
  POLICY_CUSTOMIZE_JUMP = 5,
  POLICY_URL_TRUST_LIST = 6,
  POLICY_HALF_POPUP = 10,
};

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_THREAT_TYPE_H_

// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/ohos_safe_browsing/ohos_sb_prefs.h"

#include "components/prefs/pref_registry_simple.h"

namespace ohos_safe_browsing {

const char kMaliciousAllowList[] = "oh.safe_browsing.malicious_allowlist";

const char kIncognitoMaliciousAllowList[] =
    "oh.safe_browsing.incognito_malicious_allowlist";

void RegisterProfilePrefs(PrefRegistrySimple* registry) {
  registry->RegisterListPref(kMaliciousAllowList);
  registry->RegisterListPref(kIncognitoMaliciousAllowList);
}

}  // namespace ohos_safe_browsing

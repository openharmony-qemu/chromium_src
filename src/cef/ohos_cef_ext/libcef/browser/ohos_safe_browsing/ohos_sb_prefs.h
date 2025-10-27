// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_PREFS_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_PREFS_H_

#include "components/prefs/pref_registry_simple.h"

namespace ohos_safe_browsing {

extern const char kMaliciousAllowList[];

extern const char kIncognitoMaliciousAllowList[];

void RegisterProfilePrefs(PrefRegistrySimple* registry);

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_PREFS_H_

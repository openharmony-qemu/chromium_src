// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/ohos_safe_browsing/ohos_sb_malicious_allowlist.h"

namespace ohos_safe_browsing {

MaliciousAllowlist& MaliciousAllowlist::GetInstance() {
  static MaliciousAllowlist maliciousAllowlist;
  return maliciousAllowlist;
}

void MaliciousAllowlist::AddToAllowlist(const std::string& url,
                                        bool incognito_mode) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (incognito_mode) {
    incognito_allowlist_.Append(url);
  } else {
    allowlist_.Append(url);
  }
}

bool MaliciousAllowlist::IsInAllowlist(const std::string& url,
                                       bool incognito_mode) const {
  std::lock_guard<std::mutex> lock(mutex_);
  const base::Value::List& list =
      incognito_mode ? incognito_allowlist_ : allowlist_;
  for (const base::Value& value : list) {
    if (value.GetString() == url) {
      return true;
    }
  }
  return false;
}

}  // namespace ohos_safe_browsing

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_MALICIOUS_ALLOWLIST_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_MALICIOUS_ALLOWLIST_H_

#include <mutex>
#include <string>

#include "base/values.h"

namespace ohos_safe_browsing {

class MaliciousAllowlist {
 public:
  static MaliciousAllowlist& GetInstance();

  void AddToAllowlist(const std::string& url, bool incognito_mode);
  bool IsInAllowlist(const std::string& url, bool incognito_mode) const;

 private:
  MaliciousAllowlist() = default;
  ~MaliciousAllowlist() = default;

  base::Value::List incognito_allowlist_;
  base::Value::List allowlist_;
  mutable std::mutex mutex_;
};

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_MALICIOUS_ALLOWLIST_H_

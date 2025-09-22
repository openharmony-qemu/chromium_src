// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_BROWSER_SSL_HTTPS_UPGRADES_UTIL_H_
#define OHOS_BROWSER_SSL_HTTPS_UPGRADES_UTIL_H_

class OhosHttpsUpgradesUtil {
 public:
  static bool IsHttpsUpgradeEnabled();
  static void EnableHttpsUpgrades(bool enable);

 private:
  static bool is_arkweb_https_upgrade_enabled_;
};

#endif  // OHOS_BROWSER_SSL_HTTPS_UPGRADES_UTIL_H_

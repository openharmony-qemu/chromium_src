// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_util.h"
#include "base/logging.h"
#include "base/values.h"

bool OhosHttpsUpgradesUtil::is_arkweb_https_upgrade_enabled_ = false;

bool OhosHttpsUpgradesUtil::IsHttpsUpgradeEnabled() {
  return is_arkweb_https_upgrade_enabled_;
}

void OhosHttpsUpgradesUtil::EnableHttpsUpgrades(bool enable) {
  LOG(INFO) << "before EnableHttpsUpgrades is_arkweb_https_upgrade_enabled_ is: "
            << is_arkweb_https_upgrade_enabled_
            << ", after EnableHttpsUpgrades is: " << enable;
  is_arkweb_https_upgrade_enabled_ = enable;
}
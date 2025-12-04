// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_
#define OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_

#include "base/containers/contains.h"
#include "content/public/browser/web_contents_user_data.h"
#include "base/logging.h"

namespace content {
class WebContents;
}  // namespace content

class OhosHttpsUpgradesHelper
    : public content::WebContentsUserData<OhosHttpsUpgradesHelper> {
 public:
  OhosHttpsUpgradesHelper(const OhosHttpsUpgradesHelper&) = delete;
  OhosHttpsUpgradesHelper& operator=(const OhosHttpsUpgradesHelper&) = delete;
  ~OhosHttpsUpgradesHelper() override;

  void set_is_arkweb_https_upgrades_enable(bool enable) {
    LOG(INFO) << "OhosHttpsUpgradesHelper::set_is_arkweb_https_upgrades_enable, " << is_arkweb_https_upgrades_enable_
              << " to " << enable;
    is_arkweb_https_upgrades_enable_ = enable;
  }
  bool is_arkweb_https_upgrades_enable() const { return is_arkweb_https_upgrades_enable_; }


 private:
  explicit OhosHttpsUpgradesHelper(content::WebContents* web_contents);
  friend class content::WebContentsUserData<OhosHttpsUpgradesHelper>;

  bool is_arkweb_https_upgrades_enable_ = false;


  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

#endif  // OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_

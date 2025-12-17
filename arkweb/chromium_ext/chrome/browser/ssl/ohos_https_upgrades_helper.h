// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_
#define OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_

#include "base/containers/contains.h"
#include "base/logging.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}  // namespace content

class OhosHttpsUpgradesHelper
    : public content::WebContentsUserData<OhosHttpsUpgradesHelper> {
 public:
  OhosHttpsUpgradesHelper(const OhosHttpsUpgradesHelper&) = delete;
  OhosHttpsUpgradesHelper& operator=(const OhosHttpsUpgradesHelper&) = delete;
  ~OhosHttpsUpgradesHelper() override;

  void set_is_arkweb_https_upgrades_enable(bool enable);
  bool is_arkweb_https_upgrades_enable() const {
    return is_arkweb_https_upgrades_enable_;
  }
  bool should_upgrade_to_https() const;

  static void NullOpenLoginTabCallback(content::WebContents* web_contents,
                                       bool focus);

 private:
  explicit OhosHttpsUpgradesHelper(content::WebContents* web_contents);
  friend class content::WebContentsUserData<OhosHttpsUpgradesHelper>;

  bool is_arkweb_https_upgrades_enable_ = false;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

#endif  // OHOS_BROWSER_SSL_HTTPS_UPGRADES_HELPER_H_

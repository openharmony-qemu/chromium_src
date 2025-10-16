// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_helper.h"

#include "arkweb/chromium_ext/components/captive_portal/content/captive_portal_tab_helper_ohos.h"
#include "chrome/browser/captive_portal/captive_portal_service_factory.h"
#include "chrome/browser/profiles/profile.h"

OhosHttpsUpgradesHelper::~OhosHttpsUpgradesHelper() = default;

// static
void OhosHttpsUpgradesHelper::NullOpenLoginTabCallback(
    content::WebContents* web_contents,
    bool focus) {}

OhosHttpsUpgradesHelper::OhosHttpsUpgradesHelper(
    content::WebContents* web_contents)
    : content::WebContentsUserData<OhosHttpsUpgradesHelper>(*web_contents) {}

void OhosHttpsUpgradesHelper::set_is_arkweb_https_upgrades_enable(bool enable) {
  if (is_arkweb_https_upgrades_enable_ == enable) {
    return;
  }
  auto* helper = captive_portal::CaptivePortalTabHelperOhos::FromWebContents(
      &GetWebContents());
  if (!helper) {
    Profile* profile =
        Profile::FromBrowserContext(GetWebContents().GetBrowserContext());
    captive_portal::CaptivePortalTabHelperOhos::CreateForWebContents(
        &GetWebContents(), CaptivePortalServiceFactory::GetForProfile(profile),
        base::BindRepeating(&OhosHttpsUpgradesHelper::NullOpenLoginTabCallback,
                            &GetWebContents(), false));
    helper = captive_portal::CaptivePortalTabHelperOhos::FromWebContents(
        &GetWebContents());
  }
  if (helper) {
    helper->EnableArkWebHttpsUpgrade(enable);
  }
  LOG(INFO) << " https upgrade enabled changed:"
            << is_arkweb_https_upgrades_enable_ << "->" << enable;
  is_arkweb_https_upgrades_enable_ = enable;
}

bool OhosHttpsUpgradesHelper::should_upgrade_to_https() const {
  return captive_portal::CaptivePortalTabHelperOhos::IsInternetConnected() &&
         is_arkweb_https_upgrades_enable_;
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OhosHttpsUpgradesHelper);

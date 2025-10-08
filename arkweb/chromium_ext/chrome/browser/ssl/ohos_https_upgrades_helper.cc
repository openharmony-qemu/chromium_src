// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_helper.h"


OhosHttpsUpgradesHelper::~OhosHttpsUpgradesHelper() = default;


OhosHttpsUpgradesHelper::OhosHttpsUpgradesHelper(
    content::WebContents* web_contents)
    : content::WebContentsUserData<OhosHttpsUpgradesHelper>(*web_contents) {}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OhosHttpsUpgradesHelper);

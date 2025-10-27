// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/net/ohos_url_rewrite_controller.h"
#include "base/logging.h"

bool OhosUrlRewriteController::is_arkweb_rewrite_url_enable_ = false;

bool OhosUrlRewriteController::IsRewriteUrlEnabled() {
  return is_arkweb_rewrite_url_enable_;
}

void OhosUrlRewriteController::EnableRewriteUrl(bool enable) {
  LOG(DEBUG) << "before EnableRewriteUrl is_arkweb_rewrite_url_enable_ is: "
             << is_arkweb_rewrite_url_enable_ << ", after EnableRewriteUrl is: "
             << enable;
  is_arkweb_rewrite_url_enable_ = enable;
}
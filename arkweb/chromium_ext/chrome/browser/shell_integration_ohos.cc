// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/notreached.h"
#include "chrome/browser/shell_integration.h"

namespace shell_integration {

bool SetAsDefaultBrowser() {
  return false;
}

bool SetAsDefaultClientForScheme(const std::string& scheme) {
  NOTIMPLEMENTED_LOG_ONCE();
  return false;
}

std::u16string GetApplicationNameForScheme(const GURL& url) {
  NOTIMPLEMENTED_LOG_ONCE();
  return {};
}

DefaultWebClientState GetDefaultBrowser() {
  bool ret = 0;
  VLOG(1) << "GetDefaultBrowser ret: " << ret;
  if (ret) {
    return DefaultWebClientState::IS_DEFAULT;
  }
  return DefaultWebClientState::NOT_DEFAULT;
}

bool IsFirefoxDefaultBrowser() {
  NOTIMPLEMENTED_LOG_ONCE();
  return false;
}

DefaultWebClientState IsDefaultClientForScheme(const std::string& scheme) {
  return GetDefaultBrowser();
}

namespace internal {

DefaultWebClientSetPermission GetPlatformSpecificDefaultWebClientSetPermission(
    WebClientSetMethod method) {
  NOTIMPLEMENTED_LOG_ONCE();
  return SET_DEFAULT_UNATTENDED;
}

}  // namespace internal

}  // namespace shell_integration

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CHROME_BROWSER_BROWSER_PROCESS_EXT_H_
#define CHROME_BROWSER_BROWSER_PROCESS_EXT_H_

#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"

namespace subresource_filter {
class UserRulesetService;
}

class BrowserProcessExt {
 public:
  BrowserProcessExt() = default;

  BrowserProcessExt(const BrowserProcessExt&) = delete;
  BrowserProcessExt& operator=(const BrowserProcessExt&) = delete;

  virtual ~BrowserProcessExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  // Returns the service providing versioned storage for user rules used by the
  // Safe Browsing subresource filter.
  virtual subresource_filter::UserRulesetService*
  subresource_filter_user_ruleset_service() = 0;
#endif
};

#endif  // CHROME_BROWSER_BROWSER_PROCESS_EXT_H_

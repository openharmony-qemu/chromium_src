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

#ifndef CHROME_BROWSER_BROWSER_PROCESS_IMPL_EXT_H_
#define CHROME_BROWSER_BROWSER_PROCESS_IMPL_EXT_H_

#include "arkweb/chromium_ext/chrome/browser/browser_process_ext.h"
#include "chrome/browser/browser_process_impl.h"

class BrowserProcessImpl;

class BrowserProcessImplExt : public BrowserProcessImpl,
                              public BrowserProcessExt {
 public:
  explicit BrowserProcessImplExt(StartupData* startup_data);

  BrowserProcessImplExt(const BrowserProcessImplExt&) = delete;
  BrowserProcessImplExt& operator=(const BrowserProcessImplExt&) = delete;

  ~BrowserProcessImplExt() override;

  BrowserProcessImplExt* AsBrowserProcessImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  subresource_filter::UserRulesetService*
  subresource_filter_user_ruleset_service() override;
#endif

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void CreateSubresourceFilterUserRulesetService();

  bool created_subresource_filter_user_ruleset_service_ = false;
  std::unique_ptr<subresource_filter::UserRulesetService>
      subresource_filter_user_ruleset_service_;

  SEQUENCE_CHECKER(user_sequence_checker_);
#endif
};

#endif  // CHROME_BROWSER_BROWSER_PROCESS_IMPL_EXT_H_

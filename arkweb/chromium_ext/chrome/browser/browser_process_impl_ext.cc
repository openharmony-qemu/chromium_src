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

#include "arkweb/chromium_ext/chrome/browser/browser_process_impl_ext.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "components/subresource_filter/content/browser/safe_browsing_user_ruleset_publisher.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"
#include "components/subresource_filter/core/browser/subresource_filter_constants.h"
#include "components/subresource_filter/core/browser/subresource_filter_features.h"
#include "components/subresource_filter/core/common/constants.h"
#include "ohos_cef_ext/libcef/browser/subresource_filter/adblock_ruleset_manager.h"
#include "ohos_cef_ext/libcef/browser/subresource_filter/user_adblock_ruleset_manager.h"
#endif

BrowserProcessImplExt::BrowserProcessImplExt(StartupData* startup_data)
    : BrowserProcessImpl(startup_data) {}

BrowserProcessImplExt::~BrowserProcessImplExt() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  g_browser_process = nullptr;
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
::subresource_filter::UserRulesetService*
BrowserProcessImplExt::subresource_filter_user_ruleset_service() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(user_sequence_checker_);
  if (!created_subresource_filter_user_ruleset_service_) {
    CreateSubresourceFilterUserRulesetService();
  }

  return subresource_filter_user_ruleset_service_.get();
}

void BrowserProcessImplExt::CreateSubresourceFilterUserRulesetService() {
  DCHECK(!subresource_filter_user_ruleset_service_);
  created_subresource_filter_user_ruleset_service_ = true;

  base::FilePath user_data_dir;

  base::PathService::Get(base::DIR_CACHE, &user_data_dir);
  LOG(INFO) << "adblock path: " << user_data_dir.value();
  subresource_filter_user_ruleset_service_ =
      subresource_filter::UserRulesetService::Create(
          subresource_filter::kSafeBrowsingUserRulesetConfig, local_state(),
          user_data_dir,
          subresource_filter::UserAdblockRulesetManager::GetInstance(),
          subresource_filter::SafeBrowsingUserRulesetPublisher::Factory());
}
#endif

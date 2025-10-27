/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "libcef/browser/subresource_filter/adblock_content_subresource_filter_web_contents_helper_factory.h"

#include "chrome/browser/browser_process.h"
#include "components/subresource_filter/content/browser/content_subresource_filter_web_contents_helper.h"
#include "components/subresource_filter/content/shared/browser/ruleset_service.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"
#include "libcef/browser/subresource_filter/adblock_safe_browsing_database_manager_impl.h"

void CreateSubresourceFilterWebContentsHelper(
    content::NavigationHandle* navigation_handle) {
  if (g_browser_process) {
    subresource_filter::RulesetService* ruleset_service =
        g_browser_process->subresource_filter_ruleset_service();
    subresource_filter::VerifiedRulesetDealer::Handle* dealer =
        ruleset_service ? ruleset_service->GetRulesetDealer() : nullptr;

    scoped_refptr<safe_browsing::SafeBrowsingDatabaseManagerImpl>
        database_manager = base::MakeRefCounted<
            safe_browsing::SafeBrowsingDatabaseManagerImpl>(
            content::GetUIThreadTaskRunner({}));

    subresource_filter::ContentSubresourceFilterWebContentsHelper::
        CreateForWebContents(navigation_handle->GetWebContents(), nullptr,
                             std::move(database_manager), dealer);
    if (auto* help =
            subresource_filter::ContentSubresourceFilterWebContentsHelper::
                FromWebContents(navigation_handle->GetWebContents())) {
      help->CreateThrottleManager(navigation_handle);
    }
  }
}

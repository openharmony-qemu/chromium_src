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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_RULESET_PUBLISHER_IMPL_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_RULESET_PUBLISHER_IMPL_H_

#include <memory>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "components/subresource_filter/content/browser/ruleset_publisher.h"
#include "components/subresource_filter/content/browser/verified_ruleset_dealer.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_process_host_creation_observer.h"

#include "components/subresource_filter/content/browser/user_ruleset_version.h"

namespace subresource_filter {

class UserRulesetService;

// The implementation of RulesetPublisher. Owned by the underlying
// UserRulesetService. Its main responsibility is receiving new versions of
// subresource filtering rules from the UserRulesetService, and distributing
// them to renderer processes, where they will be memory-mapped as-needed by the
// UnverifiedRulesetDealer.
class UserRulesetPublisherImpl
    : public RulesetPublisher,
      public content::RenderProcessHostCreationObserver {
 public:
  UserRulesetPublisherImpl(
      UserRulesetService* ruleset_service,
      scoped_refptr<base::SequencedTaskRunner> blocking_task_runner);

  UserRulesetPublisherImpl(const UserRulesetPublisherImpl&) = delete;
  UserRulesetPublisherImpl& operator=(const UserRulesetPublisherImpl&) = delete;

  ~UserRulesetPublisherImpl() override;

  // RulesetPublisher:
  void TryOpenAndSetRulesetFile(
      const base::FilePath& file_path,
      int expected_checksum,
      base::OnceCallback<void(RulesetFilePtr)> callback) override;
  void PublishNewRulesetVersion(RulesetFilePtr ruleset_data) override;
  scoped_refptr<base::SingleThreadTaskRunner> BestEffortTaskRunner() override;
  VerifiedRulesetDealer::Handle* GetRulesetDealer() override;
  void SetRulesetPublishedCallbackForTesting(
      base::OnceClosure callback) override;

  // Forwards calls to the underlying ruleset_service_.
  void IndexAndStoreAndPublishRulesetIfNeeded(
      const UserUnindexedRulesetInfo& unindex_ruleset_info);

 protected:
  virtual void SendRulesetToRenderProcess(base::File* file,
                                          content::RenderProcessHost* rph);

 private:
  // content::RenderProcessHostCreationObserver:
  void OnRenderProcessHostCreated(content::RenderProcessHost* rph) override;

  RulesetFilePtr ruleset_data_{nullptr, base::OnTaskRunnerDeleter{nullptr}};
  base::OnceClosure ruleset_published_callback_;

  // The service owns the publisher, and therefore outlives it.
  raw_ptr<UserRulesetService> ruleset_service_;
  std::unique_ptr<VerifiedRulesetDealer::Handle> ruleset_dealer_;
  scoped_refptr<base::SingleThreadTaskRunner> best_effort_task_runner_;
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_RULESET_PUBLISHER_IMPL_H_

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

#include "components/subresource_filter/content/browser/user_ruleset_publisher_impl.h"

#include <utility>

#include "base/check_op.h"
#include "base/feature_list.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "components/subresource_filter/core/common/common_features.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_process_host.h"
#include "components/subresource_filter/content/browser/user_ruleset_service.h"
#include "components/subresource_filter/core/browser/user_subresource_filter_constants.h"
#include "ipc/ipc_channel_proxy.h"

namespace subresource_filter {

UserRulesetPublisherImpl::UserRulesetPublisherImpl(
    UserRulesetService* ruleset_service,
    scoped_refptr<base::SequencedTaskRunner> blocking_task_runner)
    : ruleset_service_(ruleset_service),
      ruleset_dealer_(std::make_unique<VerifiedRulesetDealer::Handle>(
          std::move(blocking_task_runner))) {
  best_effort_task_runner_ =
      content::GetUIThreadTaskRunner({base::TaskPriority::BEST_EFFORT});
  DCHECK(best_effort_task_runner_->BelongsToCurrentThread());
}

UserRulesetPublisherImpl::~UserRulesetPublisherImpl() = default;

void UserRulesetPublisherImpl::SetRulesetPublishedCallbackForTesting(
    base::OnceClosure callback) {
  ruleset_published_callback_ = std::move(callback);
}

void UserRulesetPublisherImpl::TryOpenAndSetRulesetFile(
    const base::FilePath& file_path,
    int expected_checksum,
    base::OnceCallback<void(RulesetFilePtr)> callback) {
  GetRulesetDealer()->TryOpenAndSetRulesetFile(file_path, expected_checksum,
                                               std::move(callback));
}

void UserRulesetPublisherImpl::PublishNewRulesetVersion(
    RulesetFilePtr ruleset_data) {
  DCHECK(ruleset_data);
  DCHECK(ruleset_data->IsValid());
  ruleset_data_.reset();

  // If Ad Tagging is running, then every request does a lookup and it's
  // important that we verify the ruleset early on.
  if (base::FeatureList::IsEnabled(kAdTagging)) {
    // Even though the handle will immediately be destroyed, it will still
    // validate the ruleset on its task runner.
    VerifiedRuleset::Handle ruleset_handle(GetRulesetDealer());
  }

  ruleset_data_ = std::move(ruleset_data);
  for (auto it = content::RenderProcessHost::AllHostsIterator(); !it.IsAtEnd();
       it.Advance()) {
    SendRulesetToRenderProcess(ruleset_data_.get(), it.GetCurrentValue());
  }

  if (!ruleset_published_callback_.is_null()) {
    std::move(ruleset_published_callback_).Run();
  }
}

scoped_refptr<base::SingleThreadTaskRunner>
UserRulesetPublisherImpl::BestEffortTaskRunner() {
  return best_effort_task_runner_;
}

VerifiedRulesetDealer::Handle* UserRulesetPublisherImpl::GetRulesetDealer() {
  return ruleset_dealer_.get();
}

void UserRulesetPublisherImpl::IndexAndStoreAndPublishRulesetIfNeeded(
    const UserUnindexedRulesetInfo& unindexed_ruleset_info) {
  DCHECK(ruleset_service_);
  ruleset_service_->IndexAndStoreAndPublishRulesetIfNeeded(
      unindexed_ruleset_info);
}

void UserRulesetPublisherImpl::OnRenderProcessHostCreated(
    content::RenderProcessHost* rph) {
  if (!ruleset_data_ || !ruleset_data_->IsValid()) {
    return;
  }
  SendRulesetToRenderProcess(ruleset_data_.get(), rph);
}

void UserRulesetPublisherImpl::SendRulesetToRenderProcess(
    base::File* file,
    content::RenderProcessHost* rph) {
  DCHECK(rph);
  DCHECK(file);
  DCHECK(file->IsValid());
  if (!rph->GetChannel()) {
    return;
  }
  mojo::AssociatedRemote<mojom::UserSubresourceFilterRulesetObserver>
      subresource_filter;
  rph->GetChannel()->GetRemoteAssociatedInterface(&subresource_filter);
  subresource_filter->UserSetRulesetForProcess(file->Duplicate());
}

}  // namespace subresource_filter

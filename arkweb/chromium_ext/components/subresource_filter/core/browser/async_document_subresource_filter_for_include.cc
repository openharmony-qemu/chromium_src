/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_ADBLOCK)
AsyncDocumentSubresourceFilter::AsyncDocumentSubresourceFilter(
    VerifiedRuleset::Handle* ruleset_handle,
    VerifiedRuleset::Handle* user_ruleset_handle,
    InitializationParams params,
    base::OnceCallback<void(mojom::ActivationState)> activation_state_callback,
    std::string_view uma_tag)
    : task_runner_(ruleset_handle->task_runner()),
      core_(new Core(), base::OnTaskRunnerDeleter(task_runner_.get())) {
  // Note: It is safe to post |ruleset_handle|'s VerifiedRuleset pointer,
  // because a task to delete it can only be posted to (and, therefore,
  // processed by) |task_runner| after this method returns, hence after the
  // below task is posted.
  task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(
          &Core::InitializeForUserRuleSet, base::Unretained(core_.get()),
          std::move(params),
          ruleset_handle ? ruleset_handle->ruleset_.get() : nullptr,
          user_ruleset_handle ? user_ruleset_handle->ruleset_.get() : nullptr,
          uma_tag),
      base::BindOnce(&AsyncDocumentSubresourceFilter::OnActivateStateCalculated,
                     weak_ptr_factory_.GetWeakPtr(),
                     std::move(activation_state_callback)));
}

mojom::ActivationState
AsyncDocumentSubresourceFilter::Core::InitializeForUserRuleSet(
    InitializationParams params,
    VerifiedRuleset* verified_ruleset,
    VerifiedRuleset* user_verified_ruleset,
    std::string_view uma_tag) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  if ((!verified_ruleset || !verified_ruleset->Get()) &&
      (!user_verified_ruleset || !user_verified_ruleset->Get())) {
    LOG(WARNING) << "ruleset and user ruleset null";
    return mojom::ActivationState();
  }

  mojom::ActivationState activation_state =
      ComputeActivationState(params.document_url, params.parent_document_origin,
                             params.parent_activation_state,
                             (verified_ruleset && verified_ruleset->Get())
                                 ? verified_ruleset->Get()
                                 : user_verified_ruleset->Get());

  CHECK_NE(mojom::ActivationLevel::kDisabled, activation_state.activation_level,
           base::NotFatalUntil::M129);
  filter_.emplace(url::Origin::Create(params.document_url), activation_state,
                  (verified_ruleset && verified_ruleset->Get())
                      ? verified_ruleset->Get()
                      : user_verified_ruleset->Get(),
                  uma_tag);

  return activation_state;
}
#endif
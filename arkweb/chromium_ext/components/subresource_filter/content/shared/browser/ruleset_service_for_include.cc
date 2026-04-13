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

#include "arkweb/chromium_ext/components/subresource_filter/content/shared/browser/ruleset_service_for_include.h"

#include "base/path_service.h"
#include "chrome/common/chrome_paths.h"

namespace subresource_filter {

#if BUILDFLAG(ARKWEB_ADBLOCK)
RulesetService::RulesetService(
    const RulesetConfig& config,
    PrefService* local_state,
    scoped_refptr<base::SequencedTaskRunner> background_task_runner,
    const base::FilePath& indexed_ruleset_base_dir,
    const base::FilePath& unindexed_ruleset_base_dir,
    RulesetServiceClient* client,
    scoped_refptr<base::SequencedTaskRunner> blocking_task_runner,
    const RulesetPublisher::Factory& publisher_factory)
    : config_(config),
      local_state_(local_state),
      background_task_runner_(std::move(background_task_runner)),
      is_initialized_(false),
      indexed_ruleset_base_dir_(indexed_ruleset_base_dir),
      unindexed_ruleset_base_dir_(unindexed_ruleset_base_dir),
      ruleset_service_client_(client) {
  if (local_state_->GetInitializationStatus() == PrefService::INITIALIZATION_STATUS_WAITING) {
    base::FilePath local_state_file;
    bool result = base::PathService::Get(chrome::FILE_LOCAL_STATE, &local_state_file);
    LOG(ERROR) << "[Adblock] RulesetService::RulesetService file:" << local_state_file
              << " .PathExists: " << base::PathExists(local_state_file.DirName());
  }
  CHECK_NE(local_state_->GetInitializationStatus(),
           PrefService::INITIALIZATION_STATUS_WAITING,
           base::NotFatalUntil::M129);
  publisher_ = publisher_factory.Create(this, std::move(blocking_task_runner));
  IndexedRulesetVersion most_recently_indexed_version(config.filter_tag);
  most_recently_indexed_version.ReadFromPrefs(local_state_);
  TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("loading"),
               "RulesetService::RulesetService", "prefs_version",
               most_recently_indexed_version.ToTracedValue());
  if (most_recently_indexed_version.IsValid() &&
      most_recently_indexed_version.IsCurrentFormatVersion()) {
    OpenAndPublishRuleset(most_recently_indexed_version);
  } else {
    IndexedRulesetVersion(config.filter_tag).SaveToPrefs(local_state_);
  }

  CHECK(publisher_->BestEffortTaskRunner()->BelongsToCurrentThread(),
        base::NotFatalUntil::M129);
#if BUILDFLAG(ARKWEB_ADBLOCK)
  FinishInitialization();
#else
  publisher_->BestEffortTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&RulesetService::FinishInitialization,
                                weak_ptr_factory_.GetWeakPtr()));
#endif
}
#endif

void DeleteObsoleteRulesetsExt(bool& has_different_format,
                               base::FilePath& format_dir,
                               base::FilePath& current_format_dir) {
  if (format_dir != current_format_dir) {
    base::DeletePathRecursively(format_dir);
    LOG(INFO) << "[Adblock] Delete obsolete indexed rulesets:"
              << format_dir.value();
    has_different_format = true;
  }
}

void DeleteObsoleteRulesetsClientExt(
    bool& has_different_format,
    bool& has_different_version,
    const base::FilePath& unindexed_ruleset_base_dir,
    RulesetServiceClient* client) {
  // Upgrading case, should also remove unindexed file when format or version
  // change.
  if (has_different_format || has_different_version) {
    LOG(INFO) << "[AdBlock] Delete obsolete unindexed rulesets:"
              << unindexed_ruleset_base_dir.value();
    base::DeletePathRecursively(unindexed_ruleset_base_dir);
  }
  if (has_different_format) {
    if (client) {
      client->OnDeleteRulesetFile();
    }
  }
}

}  // namespace subresource_filter

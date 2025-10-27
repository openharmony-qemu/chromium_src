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

#include "libcef/browser/subresource_filter/adblock_ruleset_manager.h"

#include <memory>

#include "base/feature_list.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/path_service.h"
#include "base/uuid.h"
#include "chrome/browser/browser_process.h"
#include "components/subresource_filter/core/browser/ruleset_version.h"
#include "components/subresource_filter/core/browser/subresource_filter_constants.h"
#include "components/subresource_filter/core/browser/subresource_filter_features.h"
#include "components/subresource_filter/tools/ruleset_converter/ruleset_converter.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"

using subresource_filter::kSafeBrowsingSubresourceFilter;
using subresource_filter::kTopLevelDirectoryName;
using subresource_filter::kUnindexedRulesetBaseDirectoryName;
using subresource_filter::kUnindexedRulesetDataFileName;
using subresource_filter::RulesetConverter;
using subresource_filter::RulesetService;
using subresource_filter::UnindexedRulesetInfo;

namespace subresource_filter {

const char kSwitchInputFormat[] = "filter-list";
const char kSwitchOutputFormat[] = "unindexed-ruleset";
const char kLogTag[] = "[AdBLock]";

namespace {
base::FilePath GetOhosAppDataDir() {
  base::FilePath app_data_dir;
  base::PathService::Get(base::DIR_CACHE, &app_data_dir);

  return app_data_dir;
}

bool GetTmpDir(base::FilePath& tmp_dir) {
  tmp_dir =
      GetOhosAppDataDir().Append(FILE_PATH_LITERAL(kTopLevelDirectoryName));
  if (!base::PathExists(tmp_dir)) {
    if (!base::CreateDirectory(tmp_dir)) {
      LOG(ERROR) << kLogTag << "Create directory failed:" << tmp_dir.value();
      return false;
    }
  }
  tmp_dir = tmp_dir.Append(
      FILE_PATH_LITERAL(base::Uuid::GenerateRandomV4().AsLowercaseString()));
  if (!base::PathExists(tmp_dir)) {
    if (!base::CreateDirectory(tmp_dir)) {
      LOG(ERROR) << kLogTag
                 << "Create temp directory failed:" << tmp_dir.value();
      return false;
    }
  }
  return true;
}

base::FilePath GetUnindexedRulesetDir() {
  return GetOhosAppDataDir()
      .Append(FILE_PATH_LITERAL(kTopLevelDirectoryName))
      .Append(FILE_PATH_LITERAL(kUnindexedRulesetBaseDirectoryName));
}

base::FilePath GetUnindexedRulesetFile() {
  return GetOhosAppDataDir()
      .Append(FILE_PATH_LITERAL(kTopLevelDirectoryName))
      .Append(FILE_PATH_LITERAL(kUnindexedRulesetBaseDirectoryName))
      .Append(FILE_PATH_LITERAL(kUnindexedRulesetDataFileName));
}

bool GetEasylistsWithCommaSplit(const std::vector<base::FilePath>& easylists,
                                std::string& easylists_with_comma_split) {
  for (auto easylist : easylists) {
    if (!base::PathExists(easylist)) {
      LOG(ERROR) << kLogTag
                 << "Easylist file does not exist:" << easylist.value();
      return false;
    }
    if (!easylists_with_comma_split.empty()) {
      easylists_with_comma_split.append(",");
    }
    easylists_with_comma_split.append(easylist.value());
  }
  if (easylists_with_comma_split.empty()) {
    LOG(ERROR) << kLogTag << "Easylist file path is empty";
    return false;
  }
  return true;
}

void UnindexedRulesetToIndexedRulesetInternal(const base::FilePath unindexed_file,
                                              long long version) {
  UnindexedRulesetInfo ruleset_info;
  ruleset_info.content_version = std::to_string(version);
  ruleset_info.ruleset_path = GetUnindexedRulesetFile();                                              

  if (!g_browser_process) {
    return;
  }

  RulesetService* ruleset_service =
      g_browser_process->subresource_filter_ruleset_service();
  if (ruleset_service) {
    ruleset_service->IndexAndStoreAndPublishRulesetIfNeeded(ruleset_info);
  } else {
    LOG(WARNING) << kLogTag << "ruleset_service is null";
    return;
  }
}

bool UnindexedRulesetToIndexedRuleset(const base::FilePath& unindexed_file,
                                      long long version) {
  if (!g_browser_process) {
    return false;
  }

  content::GetUIThreadTaskRunner({base::TaskPriority::HIGHEST})
      ->PostTask(FROM_HERE, base::BindOnce(&UnindexedRulesetToIndexedRulesetInternal,
                 unindexed_file, version));

  LOG(INFO) << kLogTag << "Unindexed ruleset will be indexed";
  return true;
}

// ruleset converter -input format=filter-list
// -output format=unindexed-ruleset
//    --input files=easylist+easylistchina.txt,easylist manual.txt
//    -output file=easylist unindexed
bool EasylistToUnindexedRuleset(const std::string& easylists_with_comma_split,
                                const base::FilePath& unindexed_file) {
  RulesetConverter converter;
  if (!converter.SetInputFiles(easylists_with_comma_split)) {
    LOG(ERROR) << kLogTag
               << "Set input file failed:" << easylists_with_comma_split;
    return false;
  }

  if (!converter.SetOutputFile(unindexed_file)) {
    LOG(ERROR) << kLogTag
               << "Set output file failed:" << unindexed_file.value();
    return false;
  }

  if (!converter.SetInputFormat(kSwitchInputFormat)) {
    LOG(ERROR) << kLogTag
               << "Set input format failed:" << easylists_with_comma_split;
    return false;
  }

  if (!converter.SetOutputFormat(kSwitchOutputFormat)) {
    LOG(ERROR) << kLogTag << "Set output format failed"
               << unindexed_file.value();
    return false;
  }

  if (!converter.Convert()) {
    LOG(ERROR) << kLogTag << "Convert easylist to unindexed_file failed:"
               << easylists_with_comma_split;
    return false;
  }

  return true;
}

bool UpdateUnindexedRuleset(const base::FilePath& unindexed_file,
                            base::FilePath& target_unindexed_file) {
  base::FilePath unindexed_dir = GetUnindexedRulesetDir();

  if (base::DirectoryExists(unindexed_dir)) {
    base::DeletePathRecursively(unindexed_dir);
  }
  if (!base::CreateDirectory(unindexed_dir)) {
    LOG(ERROR) << kLogTag
               << "Create directory failed:" << unindexed_dir.value();
    return false;
  }

  target_unindexed_file =
      unindexed_dir.Append(FILE_PATH_LITERAL(kUnindexedRulesetDataFileName));

  // copy unindexed_file to UnindexedRulesetDir
  if (!CopyFile(unindexed_file, target_unindexed_file)) {
    LOG(ERROR) << kLogTag << "Copy " << unindexed_file.value() << "to "
               << target_unindexed_file.value() << " failed";
    return false;
  }

  LOG(INFO) << kLogTag << "Unindexed ruleset updates success";
  return true;
}

bool UpdateRulesetInner(const std::string& easylists_with_comma_split,
                        const base::FilePath& tmp_dir,
                        long long version) {
  base::FilePath tmp_unindexed_file;
  tmp_unindexed_file =
      tmp_dir.Append(FILE_PATH_LITERAL(kUnindexedRulesetDataFileName));
  if (!EasylistToUnindexedRuleset(easylists_with_comma_split,
                                  tmp_unindexed_file)) {
    return false;
  }

  // replacing the current used unindexed ruleset in
  // Subresource Filter/Unindexed Rules/Filtering Rules
  base::FilePath target_unindexed_file;
  if (!UpdateUnindexedRuleset(tmp_unindexed_file, target_unindexed_file)) {
    return false;
  }

  if (!UnindexedRulesetToIndexedRuleset(target_unindexed_file, version)) {
    return false;
  }

  return true;
}

bool UpdateRuleset(const std::vector<base::FilePath>& easylists,
                   long long version) {
  std::string easylists_with_comma_split;
  if (!GetEasylistsWithCommaSplit(easylists, easylists_with_comma_split)) {
    return false;
  }

  base::FilePath tmp_dir;
  if (!GetTmpDir(tmp_dir)) {
    LOG(ERROR) << kLogTag << "Get temp directory failed";
    return false;
  }

  if (!UpdateRulesetInner(easylists_with_comma_split, tmp_dir, version)) {
    base::DeletePathRecursively(tmp_dir);
    return false;
  }

  base::DeletePathRecursively(tmp_dir);
  return true;
}

void OnUpdateRulesetFinished(bool success) {
  LOG(INFO) << kLogTag << "OnUpdateRulesetFinished:"
            << (success == true ? "success" : "fail");
}

void SetAdBlockEasylistVersion(int64_t version) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}
}  // namespace

AdblockRulesetManager::AdblockRulesetManager() {}
AdblockRulesetManager::~AdblockRulesetManager() {}

base::FilePath AdblockRulesetManager::GetOhosAdblockEasylistFilePath() {
  return GetOhosAppDataDir()
      .Append(FILE_PATH_LITERAL(kTopLevelDirectoryName))
      .Append(FILE_PATH_LITERAL(kAdblockEasylistFileName));
}

// implement RulesetServiceclient interface
void AdblockRulesetManager::OnDeleteRulesetFile() {
  if (!base::FeatureList::IsEnabled(kSafeBrowsingSubresourceFilter)) {
    return;
  }

  base::FilePath easylist_file = GetOhosAdblockEasylistFilePath();

  // Rewrite easylist version to default in SharedPreference.
  // Rewrite sharedpreference only can be called in UI thread.
  content::GetUIThreadTaskRunner({base::TaskPriority::HIGHEST})
      ->PostTask(FROM_HERE, base::BindOnce(&SetAdBlockEasylistVersion, 0L));

  LOG(INFO) << kLogTag << "On delete ruleset file and try to regenerate from"
            << easylist_file.value();

  if (!base::PathExists(easylist_file)) {
    LOG(INFO) << kLogTag
              << "Easylist file does not exist:" << easylist_file.value();
    return;
  }

  std::vector<base::FilePath> easylists;
  easylists.push_back(easylist_file);

  sequenced_task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE, base::BindOnce(&UpdateRuleset, easylists, 0L),
      base::BindOnce(&OnUpdateRulesetFinished));
}

// In order to facilitate subsequent manual addition of easylist rules,
// supports simultaneous conversion of multiple easylist_files to indexed file
// in each update;
void AdblockRulesetManager::EasyListFileUpdated(
    const std::vector<base::FilePath>& easylists,
    long long version) {
  if (!base::FeatureList::IsEnabled(kSafeBrowsingSubresourceFilter)) {
    return;
  }

  if (version < current_easylist_version) {
    LOG(ERROR) << kLogTag << "Easylist updated failed, easylist version "
               << version << " <<current" << current_easylist_version;
    return;
  }

  sequenced_task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE, base::BindOnce(&UpdateRuleset, easylists, version),
      base::BindOnce(&OnUpdateRulesetFinished));
}

// public static
AdblockRulesetManager* AdblockRulesetManager::GetInstance() {
  return base::Singleton<AdblockRulesetManager>::get();
}

}  // namespace subresource_filter
                                  
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

#ifndef CEF_LIBCEF_BROWSER_USER_ADBLOCK_RULESET_MANAGER_H_
#define CEF_LIBCEF_BROWSER_USER_ADBLOCK_RULESET_MANAGER_H_

#include "base/files/file_path.h"
#include "base/task/thread_pool.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}

namespace subresource_filter {

// Responsible for updating easylist and converting to indexed files
class UserAdblockRulesetManager
    : public ::subresource_filter::UserRulesetServiceClient {
 public:
  static UserAdblockRulesetManager* GetInstance();

  base::FilePath GetOhosAdblockEasylistFilePath();

  void UserEasyListFileUpdated(const std::vector<base::FilePath>& easylists);

  // implement RulesetServiceclient interface
  void OnDeleteUserRulesetFile() override;

 private:
  UserAdblockRulesetManager();
  ~UserAdblockRulesetManager() override;
  friend struct base::DefaultSingletonTraits<UserAdblockRulesetManager>;

  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner({base::MayBlock()});
};

}  // namespace subresource_filter

#endif  // CEF_LIBCEF_BROWSER_USER_ADBLOCK_RULESET_MANAGER_H_

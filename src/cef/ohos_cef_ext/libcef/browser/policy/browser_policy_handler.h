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

#ifndef BROWSER_POLICY_HANDLER_H
#define BROWSER_POLICY_HANDLER_H

#include <string>

#include "base/observer_list.h"
#include "components/policy/core/common/policy_bundle.h"

namespace policy {

class BrowserPolicyHandler {
 public:
  class Observer {
   public:
    virtual void OnPolicyChanged() {}

   protected:
    virtual ~Observer() = default;
  };
  static BrowserPolicyHandler* GetInstance();
  void MaybeInitFromPersistentPrefs();
  void InitPolicyFromFile(const base::FilePath& cache_path);
  void SetPolicyAndNotify(const std::string& policy, int version);
  PolicyBundle GetPolicyBundle();
  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

 private:
  const int kInvalidPolicyVersion = -1;
  bool SetPolicy(const std::string& policy, int version);
  PolicyBundle bundle_;
  base::FilePath policy_file_path_;
  int current_version_ = kInvalidPolicyVersion;
  base::ObserverList<Observer>::Unchecked observers_;
};
}  // namespace policy

#endif  // BROWSER_POLICY_HANDLER_H

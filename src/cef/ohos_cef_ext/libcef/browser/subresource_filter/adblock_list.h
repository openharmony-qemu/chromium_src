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

#ifndef CEF_LIBCEF_BROWSER_ADBLOCK_LIST_H_
#define CEF_LIBCEF_BROWSER_ADBLOCK_LIST_H_

#include "base/files/file.h"

namespace adblock {

class AdBlockList {
 public:
  class Observer {
   public:
    virtual ~Observer() {}
    virtual void OnAdBlockEasyListUpdated() = 0;

   protected:
    Observer() {}
  };

  static AdBlockList* GetInstance();

  virtual void AddObserver(Observer* observer) = 0;
  virtual void RemoveObserver(Observer* observer) = 0;

  static void UpdateUserEasyListRules(const std::string& path);

 protected:
  virtual ~AdBlockList() {}
  AdBlockList() {}

 private:
};

void UpdateAdblockEasyListRules(long adBlockEasyListVersion);

void UpdateAdblockEasyListRules(base::FilePath path);
}  // namespace adblock

#endif  // CEF_LIBCEF_BROWSER_ADBLOCK_LIST_H_

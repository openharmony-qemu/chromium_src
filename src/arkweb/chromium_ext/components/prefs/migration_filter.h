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

#ifndef ARKWEB_CHROMIUM_EXT_COMPONENTS_PREFS_MIGRATION_FILTER_H_
#define ARKWEB_CHROMIUM_EXT_COMPONENTS_PREFS_MIGRATION_FILTER_H_

#include <set>
#include <string>

#include "base/memory/raw_ptr.h"
#include "components/prefs/pref_filter.h"

class JsonPrefStore;

class COMPONENTS_PREFS_EXPORT MigrationFilter : public PrefFilter {
 public:
  MigrationFilter();
  explicit MigrationFilter(JsonPrefStore* store);
  ~MigrationFilter() override;

  void FilterOnLoad(
      PostFilterOnLoadCallback post_filter_on_load_callback,
      base::Value::Dict pref_store_contents) override;

  void FilterUpdate(std::string_view path) override;
  OnWriteCallbackPair FilterSerializeData(
      base::Value::Dict& pref_store_contents) override;
  void OnStoreDeletionFromDisk() override;

 private:
  void MigrationFinished(
      PostFilterOnLoadCallback post_filter_on_load_callback,
      base::Value::Dict pref,
      bool should_write);

  raw_ptr<JsonPrefStore> store_;
  std::set<std::string> changed_keys_;
  bool migration_finished_{false};
};

#endif // ARKWEB_CHROMIUM_EXT_COMPONENTS_PREFS_MIGRATION_FILTER_H_

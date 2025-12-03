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

#include "arkweb/chromium_ext/components/prefs/migration_filter.h"

#include "base/files/file_path.h"
#include "base/json/json_file_value_serializer.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "components/prefs/json_pref_store.h"
#include "components/prefs/pref_service.h"

namespace {

const char* version_114_pref_file = "LocalPrefs.json";
const char* root_cache_dir_suffix = "global";
const char* migration_key = "arkweb_local_pref_migration";

base::Value::Dict ReadFromOldVersionPrefFile() {
  base::FilePath cache_path;
  bool success = base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (!success) {
    LOG(ERROR) << "get cache path failed";
    return {};
  }
  base::FilePath root_cache_path =
      cache_path.AppendASCII(root_cache_dir_suffix);
  base::FilePath old_pref_file_path =
      root_cache_path.AppendASCII(version_114_pref_file);
  JSONFileValueDeserializer deserializer(old_pref_file_path);
  int error_code = 0;
  std::string error_msg;
  std::unique_ptr<base::Value> result =
      deserializer.Deserialize(&error_code, &error_msg);
  if (!result) {
    LOG(INFO) << "old version pref read failed, code[" << error_code
              << "], message[" << error_msg
              << "], old_pref_file_path[" << old_pref_file_path << "]";
    return {};
  }
  auto* root = result->GetIfDict();
  if (!root) {
    LOG(ERROR) << "old version pref is not a dict"
               << ", old_pref_file_path[" << old_pref_file_path << "]";
    return {};
  }
  return std::move(*root);
}

} // namespace

MigrationFilter::MigrationFilter() = default;

MigrationFilter::MigrationFilter(JsonPrefStore* store)
    : store_(store) {}

MigrationFilter::~MigrationFilter() = default;

void MigrationFilter::FilterOnLoad(
    PostFilterOnLoadCallback post_filter_on_load_callback,
    base::Value::Dict pref_store_contents) {
  LOG(INFO) << "enter FilterOnLoad";
  bool write = false;

  // Check migration successed once.
  std::optional<bool> migration = pref_store_contents.FindBool(migration_key);
  if (migration && *migration) {
    // Migration has been successed.
    MigrationFinished(std::move(post_filter_on_load_callback),
        std::move(pref_store_contents), write);
    LOG(INFO) << "exit FilterOnLoad, already success";
    return;
  }

  write = true;

  // Read old version pref.
  auto old_pref = ReadFromOldVersionPrefFile();
  if (!old_pref.empty()) {
    old_pref.Merge(std::move(pref_store_contents));
    pref_store_contents = std::move(old_pref);
  }

  // Update changed key-value.
  if (store_) {
    for (const auto& key : changed_keys_) {
      const base::Value* new_value = nullptr;
      bool success = store_->GetValue(key, &new_value);
      if (!success) {
        continue;
      }
      pref_store_contents.SetByDottedPath(key, new_value->Clone());
    }
  }

  // Migration finished.
  pref_store_contents.Set(migration_key, true);
  MigrationFinished(std::move(post_filter_on_load_callback),
      std::move(pref_store_contents), write);
  LOG(INFO) << "exit FilterOnLoad";
}

void MigrationFilter::FilterUpdate(std::string_view path) {
  if (migration_finished_) {
    return;
  }
  changed_keys_.emplace(path);
}

MigrationFilter::OnWriteCallbackPair MigrationFilter::FilterSerializeData(
    base::Value::Dict& pref_store_contents) {
  return {};
}

void MigrationFilter::OnStoreDeletionFromDisk() {}

void MigrationFilter::MigrationFinished(
    PostFilterOnLoadCallback post_filter_on_load_callback,
    base::Value::Dict pref,
    bool should_write) {
  LOG(INFO) << "MigrationFinished(" << should_write << ")";

  DCHECK(!migration_finished_);
  migration_finished_ = true;
  changed_keys_.clear();
  std::move(post_filter_on_load_callback).Run(std::move(pref), should_write);
}

void MigrationFilter::SetPrefService(PrefService* pref_service) {
  pref_service_ = pref_service;
}
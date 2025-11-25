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

#include "base/stl_util.h"

namespace extensions {

namespace {

void NotifyOnInstalledExtensionsLoadedInFileTask() {
  content::GetUIThreadTaskRunner({})
      ->PostTask(FROM_HERE,base::BindOnce(&ExtensionRegistryInfoManager::StopInitialLoad));
}
 
void NotifyOnInstalledExtensionsLoadedInUITask() {
  GetExtensionFileTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&NotifyOnInstalledExtensionsLoadedInFileTask));
}
 
void NotifyOnInstalledExtensionsLoaded() {
  content::GetUIThreadTaskRunner({base::TaskPriority::USER_VISIBLE})
      ->PostTask(FROM_HERE, base::BindOnce(&NotifyOnInstalledExtensionsLoadedInUITask));
} 

}  // namespace

void ExtensionService::SetForbidDisplayInSettings(
    const ExtensionIdSet& extension_ids) {
  ExtensionIdSet valid_ids;
  for (const auto& id : extension_ids) {
    if (crx_file::id_util::IdIsValid(id)) {
      valid_ids.insert(id);
    } else {
      LOG(ERROR) << "Invalid extension_id: " << id;
    }
  }

  ExtensionIdSet no_longer_forbid = base::STLSetDifference<ExtensionIdSet>(
      forbid_display_in_settings_, valid_ids);
  ExtensionIdSet newly_forbid = base::STLSetDifference<ExtensionIdSet>(
      valid_ids, forbid_display_in_settings_);
  for (const ExtensionId& id : no_longer_forbid) {
    forbid_display_in_settings_.erase(id);
    extension_prefs_->SetNotDisplayInSettings(id, false);
  }

  for (const ExtensionId& id : newly_forbid) {
    forbid_display_in_settings_.insert(id);
    extension_prefs_->SetNotDisplayInSettings(id, true);
  }
}

void ExtensionService::LoadForbidDisplayInSettingsExtensions() {
  forbid_display_in_settings_.clear();
  ExtensionIdSet ids = registry_->GenerateInstalledExtensionsSet().GetIDs();
  for (const auto& id : ids) {
    if (extension_prefs_->IsNotDisplayInSettings(id)) {
      forbid_display_in_settings_.insert(id);
    }
  }
}

void ExtensionService::RemoveForbidDisplayInSettings(
    const ExtensionId& extension_id) {
  if (forbid_display_in_settings_.find(extension_id) !=
      forbid_display_in_settings_.end()) {
    forbid_display_in_settings_.erase(extension_id);
    extension_prefs_->SetNotDisplayInSettings(extension_id, false);
  }
}
}  // namespace extensions
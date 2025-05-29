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

#include "arkweb/chromium_ext/base/feature_list_utils.h"

#include "base/logging.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/persistent_memory_allocator.h"
#include "base/pickle.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace base {
FeatureListUtils::FeatureListUtils(FeatureList* feature_list)
    : feature_list_(feature_list) {}

#if BUILDFLAG(ARKWEB_SCROLLBAR)
// static
void FeatureListUtils::SetScrollbarEnable(bool enable) {
  if (FeatureList::GetInstance()) {
    FeatureList::OverrideState state =
        FeatureList::OverrideState::OVERRIDE_ENABLE_FEATURE;
    if (enable) {
      state = FeatureList::OverrideState::OVERRIDE_DISABLE_FEATURE;
    }
    LOG(INFO) << "set Scrollbar:" << enable << " state:" << state;
    FeatureList::GetInstance()->GetUtils()->SetOverrideStateByFeatureName(
        "OverlayScrollbar", state);
    FeatureList::GetInstance()->GetUtils()->SetOverrideStateByFeatureName(
        "ForceScrollbar", state);
  } else {
    LOG(ERROR) << "set Scrollbar error";
  }
}

void FeatureListUtils::SetOverrideStateByFeatureName(
    std::string_view feature_name,
    FeatureList::OverrideState state) {
  DCHECK(FeatureList::GetInstance()->initialized_);
  DCHECK(
      FeatureList::GetInstance()->IsValidFeatureOrFieldTrialName(feature_name))
      << feature_name;
  auto it = FeatureList::GetInstance()->overrides_.find(feature_name);
  if (it == FeatureList::GetInstance()->overrides_.end()) {
    LOG(INFO) << "scroll add feature: " << feature_name << " state:" << state;
    FeatureList::GetInstance()->overrides_.emplace(
        std::string(feature_name), FeatureList::OverrideEntry(state, nullptr));
  }
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
void FeatureListUtils::ModifyFeaturesToAllocator(
    PersistentMemoryAllocator* allocator) {
  DCHECK(FeatureList::GetInstance()->initialized_);
  LOG(INFO) << "scrollbar modify features";
  PersistentMemoryAllocator::Iterator iter(allocator);
  const FeatureEntry* entry;
  while ((entry = iter.GetNextOfObject<FeatureEntry>()) != nullptr) {
    std::string_view feature_name;
    std::string_view trial_name;
    if (!entry->GetFeatureAndTrialName(&feature_name, &trial_name)) {
      continue;
    }
    if (feature_name == "OverlayScrollbar" ||
        feature_name == "ForceScrollbar") {
      allocator->Delete(entry);
    }
  }
  AddFeatureToField(allocator, "");
}

void FeatureListUtils::AddFeatureToField(PersistentMemoryAllocator* allocator,
                                         std::string feature_name) {
  for (const auto& override : FeatureList::GetInstance()->overrides_) {
    if (override.first != "OverlayScrollbar" &&
        override.first != "ForceScrollbar") {
      continue;
    }
    Pickle pickle;
    pickle.WriteString(override.first);
    if (override.second.field_trial) {
      pickle.WriteString(override.second.field_trial->trial_name());
    }

    size_t total_size = sizeof(FeatureEntry) + pickle.size();
    FeatureEntry* entry = allocator->New<FeatureEntry>(total_size);
    if (!entry) {
      LOG(ERROR) << "AddFeatureToField allocator error";
      return;
    }

    entry->override_state = override.second.overridden_state;
    entry->pickle_size = pickle.size();

    char* dst = reinterpret_cast<char*>(entry) + sizeof(FeatureEntry);
    memcpy_s(dst, pickle.size(), pickle.data(), pickle.size());

    allocator->MakeIterable(entry);
  }
}
#endif

}  // namespace base

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

#ifndef ARKWEB_CHROMIUM_EXT_BASE_FEATURE_LIST_UTILS_H
#define ARKWEB_CHROMIUM_EXT_BASE_FEATURE_LIST_UTILS_H

#include "base/feature_list.h"
#include "arkweb/build/features/features.h"

namespace base {
class FeatureList;
class PersistentMemoryAllocator;

class FeatureListUtils {
 public:
  FeatureListUtils(FeatureList* feature_list);

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  // set feature by name.
  void SetOverrideStateByFeatureName(std::string_view feature_name,
                                     FeatureList::OverrideState state);
  void ModifyFeaturesToAllocator(PersistentMemoryAllocator* allocator);
  void AddFeatureToField(PersistentMemoryAllocator* allocator,
                         std::string feature_name);
  // set scroll bar resident.
  static void SetScrollbarEnable(bool enable);
#endif

  FeatureList* feature_list_;
};
}  // namespace base
#endif  // ARKWEB_CHROMIUM_EXT_BASE_FEATURE_LIST_UTILS_H

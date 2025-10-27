/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/base/metrics/field_trial_utils.h"

namespace base {

#if BUILDFLAG(IS_ARKWEB)
// static
int FieldTrialListUtils::GetFieldTrialDescriptor() {
  FieldTrialList::InstantiateFieldTrialAllocatorIfNeeded();
  if (!FieldTrialList::global_ || !FieldTrialList::global_->readonly_allocator_region_.IsValid()) {
    return -1;
  }

#if BUILDFLAG(IS_ARKWEB)
  return FieldTrialList::global_->readonly_allocator_region_.GetPlatformHandle();
#else
  return FieldTrialList::global_->readonly_allocator_region_.GetPlatformHandle().fd;
#endif
}

#endif  // BUILDFLAG(IS_ARKWEB)
}  // namespace base

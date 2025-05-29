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

#ifndef BASE_METRICS_FIELD_TRIAL_UTILS_H_
#define BASE_METRICS_FIELD_TRIAL_UTILS_H_

#include "base/metrics/field_trial.h"

namespace base {
class BASE_EXPORT FieldTrialListUtils {
 public:
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  // On POSIX, we also need to explicitly pass down this file descriptor that
  // should be shared with the child process. Returns -1 if it was not
  // initialized properly. The current process remains the onwer of the passed
  // descriptor.
  static int GetFieldTrialDescriptor();
#endif  // BUILDFLAG(IS_ARKWEB)
};
}

#endif  // BASE_METRICS_FIELD_TRIAL_UTILS_H_
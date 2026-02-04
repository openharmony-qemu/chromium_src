/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_EXTENSION_SAFE_BROWSING_PARAMS_H_
#define OHOS_NWEB_SRC_NWEB_EXTENSION_SAFE_BROWSING_PARAMS_H_

#include <stddef.h>

#include <string>

struct SafeBrowsingParams {
  int32_t code;
  int32_t policy;
  int32_t threat;

  std::string ToString() const {
    return "code: " + std::to_string(code) +
           ", policy: " + std::to_string(policy) +
           ", threat: " + std::to_string(threat);
  }
};

#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_SAFE_BROWSING_PARAMS_H_
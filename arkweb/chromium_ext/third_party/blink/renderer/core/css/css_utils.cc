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

#include "css_utils.h"

#include "base/command_line.h"
#include "content/public/common/content_switches_ext.h"
#include "ohos_nweb/src/nweb_advanced_security.h"

namespace blink {

// LCOV_EXCL_START
bool Cssutils::IsMathFormulaDisabledMode() {
  return OHOS::NWeb::NWebAdvancedSecurityHelper::Inst().
        IsSecFeatureEnabled(OHOS::NWeb::NWebAdvancedSecurityHelper::Feature::ENABLE_MATHML)
      || base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kDisableMathml);
}
// LCOV_EXCL_STOP

}  // namespace

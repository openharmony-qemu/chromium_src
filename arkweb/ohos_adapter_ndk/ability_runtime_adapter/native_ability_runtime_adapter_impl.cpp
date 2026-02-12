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

#include "native_ability_runtime_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <AbilityKit/ability_runtime/ability_runtime_common.h>

namespace OHOS::NWeb {
bool NativeAbilityRuntimeAdapterImpl::NotifyPageChanged(
    const char* targetPageName, int32_t targetPageNameLength, int32_t windowId)
{
    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    code = OH_AbilityRuntime_ApplicationContextNotifyPageChanged(targetPageName, targetPageNameLength, windowId);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextNotifyPageChanged failed: err=%{public}d", code);
        return false;
    }
    WVLOG_D("NativeAbilityRuntimeAdapterImpl NotifyPageChanged success.");
    return true;
}
}
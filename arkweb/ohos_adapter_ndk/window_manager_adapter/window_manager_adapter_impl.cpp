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
#include "window_manager_adapter_impl.h"

#include <window_manager/oh_window.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {

WindowManagerAdapterImpl& WindowManagerAdapterImpl::GetInstance()
{
    WVLOG_D("Window manager adapter impl get instance.");
    static WindowManagerAdapterImpl instance;
    return instance;
}

bool WindowManagerAdapterImpl::GetWindowPrivacyMode(uint32_t windowId)
{
    WindowManager_WindowProperties windowProperties;
    int32_t ret = OH_WindowManager_GetWindowProperties(windowId, &windowProperties);
    if (ret != 0) {
        WVLOG_E("Window manager get window properties failed ret = %{public}d.", ret);
        return false;
    }
    return windowProperties.isPrivacyMode;
}
} // namespace OHOS::NWeb
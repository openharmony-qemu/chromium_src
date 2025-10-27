/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_AAFWK_RENDER_SCHEDULER_HOST_ADAPTER_H
#define ARK_AAFWK_RENDER_SCHEDULER_HOST_ADAPTER_H
#pragma once

#include "ark_aafwk_browser_client_adapter.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAafwkRenderSchedulerHostAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd, int32_t crashFd) = 0;

    /*--ark web()--*/
    virtual void NotifyBrowser(
        int32_t ipcFd, int32_t sharedFd, int32_t crashFd, ArkWebRefPtr<ArkAafwkBrowserClientAdapter> adapter) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AAFWK_RENDER_SCHEDULER_HOST_ADAPTER_H

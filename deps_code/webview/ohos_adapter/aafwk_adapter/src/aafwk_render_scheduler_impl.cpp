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

#include "aafwk_render_scheduler_impl.h"

#include "ibrowser.h"
#include "nweb_log.h"
#include "system_properties_adapter_impl.h"
#include "vsync_adapter_impl.h"

namespace OHOS::NWeb {
AafwkRenderSchedulerImpl::AafwkRenderSchedulerImpl(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter)
    : renderSchedulerHostAdapter_(adapter)
{}

void AafwkRenderSchedulerImpl::NotifyBrowserFd(
    int32_t ipcFd, int32_t sharedFd, int32_t crashFd, sptr<IRemoteObject> browser)
{
    WVLOG_D("received browser fd.");
    if (renderSchedulerHostAdapter_ == nullptr) {
        WVLOG_E("renderSchedulerHostAdapter_ is nullptr.");
        return;
    }
    if (browser == nullptr) {
        WVLOG_D("NotifyBrowserFd for render process.");
        renderSchedulerHostAdapter_->NotifyBrowser(ipcFd, sharedFd, crashFd, nullptr);
    } else {
        sptr<IBrowser> browserHost = iface_cast<IBrowser>(browser);
        browserClientAdapter_ = std::make_shared<AafwkBrowserClientAdapterImpl>();
        AafwkBrowserClientAdapterImpl::GetInstance().browserHost_ = browserHost;
        renderSchedulerHostAdapter_->NotifyBrowser(ipcFd, sharedFd, crashFd, browserClientAdapter_);
        VSyncAdapterImpl::GetInstance().SetIsGPUProcess(true);
    }
}
} // namespace OHOS::NWeb

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_aafwk_render_scheduler_host_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_aafwk_browser_client_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAafwkRenderSchedulerHostAdapterWrapper::ArkAafwkRenderSchedulerHostAdapterWrapper(
    ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter> ref)
    : ctocpp_(ref)
{}

void ArkAafwkRenderSchedulerHostAdapterWrapper::NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd, int32_t crashFd)
{
    ctocpp_->NotifyBrowserFd(ipcFd, sharedFd, crashFd);
}

void ArkAafwkRenderSchedulerHostAdapterWrapper::NotifyBrowser(
    int32_t ipcFd, int32_t sharedFd, int32_t crashFd, std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> adapter)
{
    ctocpp_->NotifyBrowser(ipcFd, sharedFd, crashFd, new ArkAafwkBrowserClientAdapterImpl(adapter));
}

} // namespace OHOS::ArkWeb

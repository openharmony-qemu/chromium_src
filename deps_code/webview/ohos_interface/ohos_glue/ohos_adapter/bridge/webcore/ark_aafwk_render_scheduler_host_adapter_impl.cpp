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

#include "ohos_adapter/bridge/ark_aafwk_render_scheduler_host_adapter_impl.h"

#include "ohos_adapter/bridge/ark_aafwk_browser_client_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkAafwkRenderSchedulerHostAdapterImpl::ArkAafwkRenderSchedulerHostAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AafwkRenderSchedulerHostAdapter> ref)
    : real_(ref)
{}

void ArkAafwkRenderSchedulerHostAdapterImpl::NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd, int32_t crashFd)
{
    real_->NotifyBrowserFd(ipcFd, sharedFd, crashFd);
}

void ArkAafwkRenderSchedulerHostAdapterImpl::NotifyBrowser(
    int32_t ipcFd, int32_t sharedFd, int32_t crashFd, ArkWebRefPtr<ArkAafwkBrowserClientAdapter> adapter)
{
    real_->NotifyBrowser(ipcFd, sharedFd, crashFd, std::make_shared<ArkAafwkBrowserClientAdapterWrapper>(adapter));
}

} // namespace OHOS::ArkWeb

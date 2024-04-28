/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/cpptoc/ark_aafwk_render_scheduler_host_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_aafwk_render_scheduler_host_adapter_notify_browser_fd(
    struct _ark_aafwk_render_scheduler_host_adapter_t *self, int32_t ipcFd,
    int32_t sharedFd, int32_t crashFd) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkAafwkRenderSchedulerHostAdapterCppToC::Get(self)->NotifyBrowserFd(
      ipcFd, sharedFd, crashFd);
}

} // namespace

ArkAafwkRenderSchedulerHostAdapterCppToC::
    ArkAafwkRenderSchedulerHostAdapterCppToC() {
  GetStruct()->notify_browser_fd =
      ark_aafwk_render_scheduler_host_adapter_notify_browser_fd;
}

ArkAafwkRenderSchedulerHostAdapterCppToC::
    ~ArkAafwkRenderSchedulerHostAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkAafwkRenderSchedulerHostAdapterCppToC,
    ArkAafwkRenderSchedulerHostAdapter,
    ark_aafwk_render_scheduler_host_adapter_t>::kBridgeType =
    ARK_AAFWK_RENDER_SCHEDULER_HOST_ADAPTER;

} // namespace OHOS::ArkWeb

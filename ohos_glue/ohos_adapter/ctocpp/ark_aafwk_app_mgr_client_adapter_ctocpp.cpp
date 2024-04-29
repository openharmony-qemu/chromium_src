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

#include "ohos_adapter/ctocpp/ark_aafwk_app_mgr_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_aafwk_render_scheduler_host_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int ArkAafwkAppMgrClientAdapterCToCpp::StartRenderProcess(
    const ArkWebString &renderParam, int32_t ipcFd, int32_t sharedFd,
    int32_t crashFd, pid_t &renderPid) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_aafwk_app_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_render_process, 0);

  // Execute
  return _struct->start_render_process(_struct, &renderParam, ipcFd, sharedFd,
                                       crashFd, &renderPid);
}

ARK_WEB_NO_SANITIZE
void ArkAafwkAppMgrClientAdapterCToCpp::AttachRenderProcess(
    ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter> adapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_aafwk_app_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, attach_render_process, );

  // Execute
  _struct->attach_render_process(
      _struct, ArkAafwkRenderSchedulerHostAdapterCppToC::Invert(adapter));
}

ARK_WEB_NO_SANITIZE
int ArkAafwkAppMgrClientAdapterCToCpp::GetRenderProcessTerminationStatus(
    pid_t renderPid, int &status) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_aafwk_app_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   get_render_process_termination_status, 0);

  // Execute
  return _struct->get_render_process_termination_status(_struct, renderPid,
                                                        &status);
}

ArkAafwkAppMgrClientAdapterCToCpp::ArkAafwkAppMgrClientAdapterCToCpp() {
}

ArkAafwkAppMgrClientAdapterCToCpp::~ArkAafwkAppMgrClientAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkAafwkAppMgrClientAdapterCToCpp, ArkAafwkAppMgrClientAdapter,
    ark_aafwk_app_mgr_client_adapter_t>::kBridgeType =
    ARK_AAFWK_APP_MGR_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb

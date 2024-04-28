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

#include "ohos_adapter/ctocpp/ark_background_task_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"

namespace OHOS::ArkWeb {

using ArkBackgroundTaskAdapterRequestBackgroundRunningFunc =
    bool (*)(bool running, int32_t bgMode);
static ArkBackgroundTaskAdapterRequestBackgroundRunningFunc
    ark_background_task_adapter_request_background_running = nullptr;

ARK_WEB_NO_SANITIZE
bool ArkBackgroundTaskAdapter::RequestBackgroundRunning(bool running,
                                                        int32_t bgMode) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_background_task_adapter_request_background_running) {
    ark_background_task_adapter_request_background_running = reinterpret_cast<
        ArkBackgroundTaskAdapterRequestBackgroundRunningFunc>(
        ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
            "ark_background_task_adapter_request_background_running_static"));
    if (!ark_background_task_adapter_request_background_running) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_background_task_adapter_request_background_running(running,
                                                                bgMode);
}

ArkBackgroundTaskAdapterCToCpp::ArkBackgroundTaskAdapterCToCpp() {
}

ArkBackgroundTaskAdapterCToCpp::~ArkBackgroundTaskAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkBackgroundTaskAdapterCToCpp, ArkBackgroundTaskAdapter,
    ark_background_task_adapter_t>::kBridgeType = ARK_BACKGROUND_TASK_ADAPTER;

} // namespace OHOS::ArkWeb

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

#include "arkweb/chromium_ext/content/browser/renderer_host/agent_scheduling_group_host_utils.h"

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

namespace content {

AgentSchedulingGroupHostUtils::AgentSchedulingGroupHostUtils(AgentSchedulingGroupHost* impl) {
  this->agentSchedulingGroupHost = impl;
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
void AgentSchedulingGroupHostUtils::ReportCreateView(int32_t process_id) {
  OHOS::NWeb::ResSchedClientAdapter::ReportProcessInUse(process_id);
  if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
    OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread(
        OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED, process_id,
        process_id, OHOS::NWeb::ResSchedRoleAdapter::IMPORTANT_DISPLAY);
  }
}
}
#endif
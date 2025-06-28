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
#ifndef CONTENT_CHILD_CHILD_THREAD_IMPL_H_
#error "must be in include form CONTENT_CHILD_CHILD_THREAD_IMPL_H_"
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
#include "content/renderer/anr_dumper.h"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void ChildThreadImpl::ReportKeyThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t roleAdapter) {
  using namespace OHOS::NWeb;
  if (child_process_host_)
    child_process_host_->ReportKeyThread(status, process_id, thread_id, roleAdapter);
}
#endif

#if BUILDFLAG(ARKWEB_DFX_TRACING)
void ChildThreadImpl::ReportHisyevent(int64_t block_time, const std::string& mode) {
  using namespace OHOS::NWeb;
  if (child_process_host_)
    child_process_host_->ReportHisyevent(block_time, mode);
}
#endif

} // namespace content

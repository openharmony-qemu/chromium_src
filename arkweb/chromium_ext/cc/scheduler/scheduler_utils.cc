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

#include "arkweb/chromium_ext/cc/scheduler/scheduler_utils.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "cc/scheduler/scheduler.h"

namespace cc {

SchedulerUtils::SchedulerUtils(Scheduler* scheduler)
    : scheduler_(scheduler) {
  DCHECK(scheduler);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void SchedulerUtils::HandleScrollUpdateForInternalBeginFrame(
    const viz::BeginFrameArgs& args) {
  if (args.internal_frame) {
    TRACE_EVENT0("cc,benchmark", "Scheduler::internal_frame::scrollupdate");
    scheduler_->client_->HandleScrollUpdateForInternalBeginFrame(args);
  }
}
#endif

#if BUILDFLAG(ARKWEB_WEBGL)
void SchedulerUtils::SetShouldDeferInvalidation(
    bool main_thread_response_expected_soon) {
  scheduler_->state_machine_.set_should_defer_invalidation_for_fast_main_frame(
      scheduler_->state_machine_.should_defer_invalidation_for_fast_main_frame()
          ? true
          : main_thread_response_expected_soon);
}
#endif

#if BUILDFLAG(ARKWEB_WEBGL)
void SchedulerUtils::SetDeferInvalidationForFastMainFrame(
    bool defer_invalidation_for_fast_main_frame) {
  scheduler_->state_machine_.set_should_defer_invalidation_for_fast_main_frame(
      defer_invalidation_for_fast_main_frame);
}
#endif
}
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

#include "arkweb/chromium_ext/components/viz/service/display/display_scheduler_utils.h"

#include "base/trace_event/trace_event.h"
#include "components/viz/service/display/display_scheduler.h"

namespace viz {
DisplaySchedulerUtils::DisplaySchedulerUtils(
    DisplayScheduler* display_scheduler)
    : display_scheduler_(display_scheduler) {}

void DisplaySchedulerUtils::OnDisplayDamaged(SurfaceId surface_id) {
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  TRACE_EVENT1("viz", "DisplayScheduler::OnDisplayDamaged", "surface_id",
               surface_id.ToString());
  if (wait_render_frame_submission_before_draw_ &&
      surface_id.frame_sink_id().client_id() != 0) {
    TRACE_EVENT1("viz",
                 "DisplayScheduler::OnDisplayDamaged received render frame",
                 "surface_id", surface_id.ToString());
    wait_render_frame_submission_deadline_callback_.Cancel();
    wait_render_frame_submission_before_draw_ = false;
  }
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
constexpr int WAIT_RENDER_FRAME_DEADLINE_INTERVAL_MILLISECONDS = 200;
void DisplaySchedulerUtils::SetShouldFrameSubmissionBeforeDraw(bool should) {
  wait_render_frame_submission_before_draw_ = should;
  wait_render_frame_submission_deadline_callback_.Reset(base::BindOnce(
      &DisplaySchedulerUtils::ResetShouldFrameSubmissionBeforeDraw,
      weak_ptr_factory_.GetWeakPtr()));

  TRACE_EVENT0("viz", "DisplayScheduler::SetShouldFrameSubmissionBeforeDraw");
  display_scheduler_->task_runner_->PostDelayedTask(
      FROM_HERE, wait_render_frame_submission_deadline_callback_.callback(),
      base::Milliseconds(WAIT_RENDER_FRAME_DEADLINE_INTERVAL_MILLISECONDS));
}

void DisplaySchedulerUtils::ResetShouldFrameSubmissionBeforeDraw() {
  TRACE_EVENT0("viz", "DisplayScheduler::ResetShouldFrameSubmissionBeforeDraw");
  wait_render_frame_submission_before_draw_ = false;
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

}  // namespace viz
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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_DISPLAY_SCHEDULER_UTILS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_DISPLAY_SCHEDULER_UTILS_H_

#include <string>

#include "base/cancelable_callback.h"
#include "base/memory/raw_ptr.h"
#include "components/viz/common/surfaces/surface_id.h"

#define DESIRED_BEGIN_FRAME_DEADLINE_MODED()                             \
  if (display_scheduler_utils_->wait_before_draw()) {                    \
    TRACE_EVENT0("viz", "Wait for render frame submission before draw"); \
    return BeginFrameDeadlineMode::kNone;                                \
  }

namespace viz {
class DisplayScheduler;

class DisplaySchedulerUtils {
 public:
  DisplaySchedulerUtils(DisplayScheduler* display);

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should);
  void ResetShouldFrameSubmissionBeforeDraw();
#endif
  void OnDisplayDamaged(SurfaceId surface_id);
  bool wait_before_draw() { return wait_render_frame_submission_before_draw_; }

 private:
  raw_ptr<DisplayScheduler> display_scheduler_;

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  bool wait_render_frame_submission_before_draw_ = false;
  base::CancelableOnceClosure wait_render_frame_submission_deadline_callback_;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

base::WeakPtrFactory<DisplaySchedulerUtils> weak_ptr_factory_{this};
};
}  // namespace viz
#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_DISPLAY_SCHEDULER_UTILS_H_
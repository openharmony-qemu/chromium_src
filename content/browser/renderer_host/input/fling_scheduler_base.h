// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_INPUT_FLING_SCHEDULER_BASE_H_
#define CONTENT_BROWSER_RENDERER_HOST_INPUT_FLING_SCHEDULER_BASE_H_

#include "content/browser/renderer_host/input/fling_controller.h"

namespace ui {
class Compositor;
}

namespace content {

class FlingSchedulerBase : public FlingControllerSchedulerClient {
 public:
  virtual void ProgressFlingOnBeginFrameIfneeded(
      base::TimeTicks current_time) = 0;

  void SetCompositor(ui::Compositor* compositor) {
    compositor_ = compositor;
  }

#if BUILDFLAG(IS_OHOS)
  ui::Compositor* GetCompositor() { return compositor_; }
#endif

 protected:
  ui::Compositor* compositor_ = nullptr;
};

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_INPUT_FLING_SCHEDULER_BASE_H_

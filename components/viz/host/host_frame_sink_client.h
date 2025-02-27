// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_CLIENT_H_
#define COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_CLIENT_H_

#include <stdint.h>

#include "base/time/time.h"
#if BUILDFLAG(IS_OHOS)
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"
#endif

namespace viz {

class SurfaceInfo;

class HostFrameSinkClient {
 public:
  // Called when a CompositorFrame with a new SurfaceId activates for the first
  // time.
  virtual void OnFirstSurfaceActivation(const SurfaceInfo& surface_info) = 0;

  // Called when a CompositorFrame with a new frame token is provided.
  virtual void OnFrameTokenChanged(uint32_t frame_token,
                                   base::TimeTicks activation_time) = 0;

#if BUILDFLAG(IS_OHOS) && defined(OHOS_PERFORMANCE_JITTER)
  virtual void OnVsync() {}
  virtual void OnVsyncReceived() {}
#endif
#if BUILDFLAG(IS_OHOS)
  virtual void RestoreRenderFit() {}
#endif

 protected:
  virtual ~HostFrameSinkClient() {}
};

}  // namespace viz

#endif  // COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_CLIENT_H_

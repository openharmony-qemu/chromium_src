// Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_

#include <algorithm>
#include <cstdint>
#include <memory>

#include "base/time/time.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/service/viz_service_export.h"
#include "graphic_adapter.h"

namespace viz {
class VIZ_SERVICE_EXPORT ExternalBeginFrameSourceOHOS
    : public ExternalBeginFrameSource,
      public ExternalBeginFrameSourceClient {
 public:
  explicit ExternalBeginFrameSourceOHOS(
      uint32_t restart_id,
      FrameSinkManagerImpl* frame_sink_manager);
  ~ExternalBeginFrameSourceOHOS() override;
  ExternalBeginFrameSourceOHOS(const ExternalBeginFrameSourceOHOS&) = delete;
  ExternalBeginFrameSourceOHOS& operator=(const ExternalBeginFrameSourceOHOS&) =
      delete;

  // BeginFrameSource:
  void SetDynamicBeginFrameDeadlineOffsetSource(
      DynamicBeginFrameDeadlineOffsetSource*
          dynamic_begin_frame_deadline_offset_source) override;
  void SendInternalBeginFrame() override;

  static void OnVSync(int64_t timestamp, void* data);
  class VSyncUserData;
  void OnVSyncImpl(int64_t timestamp, VSyncUserData* user_data);

  // ExternalBeginFrameSource implementation.
  void SetCurrentFrameSinkId(const FrameSinkId& frame_sink_id) override {
    frame_sink_id_ = frame_sink_id;
  };

  void SetEnableLowerFrameRate(bool enabled) override {
    lower_frame_rate_enabled_ = enabled;
  }

 private:
  // ExternalBeginFrameSourceClient implementation.
  void OnNeedsBeginFrames(bool needs_begin_frames) override;

  void SetEnabled(bool enabled);

  BeginFrameArgsGenerator begin_frame_args_generator_;
  bool vsync_notification_enabled_;
  bool first_vsync_since_notify_enabled_;

  std::unique_ptr<VSyncUserData> user_data_;
  OHOS::NWeb::VSyncAdapter& vsync_adapter_;

  FrameSinkId frame_sink_id_;
  const raw_ptr<FrameSinkManagerImpl> frame_sink_manager_;
  int64_t vsync_period_ = 0;
  int64_t pre_vsync_period_ = 0;
  int64_t last_vsync_period_ = 0;
  base::TimeTicks last_dead_line_ = base::TimeTicks();
  bool lower_frame_rate_enabled_ = false;
  base::WeakPtrFactory<ExternalBeginFrameSourceOHOS> weak_factory_{this};
};
}  // namespace viz

#endif
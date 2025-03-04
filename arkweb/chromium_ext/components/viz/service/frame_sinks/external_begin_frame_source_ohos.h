#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_

#include <algorithm>
#include <cstdint>
#include <memory>

#include "arkweb/build/features/features.h"
#include "base/containers/circular_deque.h"
#include "base/time/time.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/service/viz_service_export.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/graphic_adapter.h"

namespace viz {
class VIZ_SERVICE_EXPORT ExternalBeginFrameSourceOHOS
    : public ExternalBeginFrameSource,
      public ExternalBeginFrameSourceClient {
 public:
  explicit ExternalBeginFrameSourceOHOS(
      uint32_t restart_id,
      FrameSinkManagerImpl* frame_sink_manager);

  explicit ExternalBeginFrameSourceOHOS(uint32_t restart_id);

  ~ExternalBeginFrameSourceOHOS() override;
  ExternalBeginFrameSourceOHOS(const ExternalBeginFrameSourceOHOS&) = delete;
  ExternalBeginFrameSourceOHOS& operator=(const ExternalBeginFrameSourceOHOS&) =
      delete;
  void SendInternalBeginFrame() override;
 
  static void OnVSync(int64_t timestamp, void* data);
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  static void OnVSyncCallback();
#endif
  class VSyncUserData;
  void OnVSyncImpl(int64_t timestamp, VSyncUserData* user_data);
 
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  // ExternalBeginFrameSource implementation.
  void SetCurrentFrameSinkId(const FrameSinkId& frame_sink_id) override {
    frame_sink_id_ = frame_sink_id;
  }
#endif
  void SetDrawRect(const gfx::Rect& new_rect) override {
    draw_rect_ = new_rect;
  }

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void UpdateVSyncFrequency(int frame_rate) override;
  void ResetVSyncFrequency() override;
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void SetEnableLowerFrameRate(bool enabled) override {
    lower_frame_rate_enabled_ = enabled;
  }
#endif

 private:
  // ExternalBeginFrameSourceClient implementation.
  void OnNeedsBeginFrames(bool needs_begin_frames) override;

  void SetEnabled(bool enabled);

  BeginFrameArgsGenerator begin_frame_args_generator_;
  bool vsync_notification_enabled_;
  bool first_vsync_since_notify_enabled_;

  std::unique_ptr<VSyncUserData> user_data_;
  OHOS::NWeb::VSyncAdapter& vsync_adapter_;

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  FrameSinkId frame_sink_id_;
  const raw_ptr<FrameSinkManagerImpl> frame_sink_manager_;
#endif
  int64_t vsync_period_ = 0;
  int64_t pre_vsync_period_ = 0;
  int64_t last_vsync_period_ = 0;
  base::TimeTicks last_dead_line_ = base::TimeTicks();
  bool lower_frame_rate_enabled_ = false;
  gfx::Rect draw_rect_;
  base::WeakPtrFactory<ExternalBeginFrameSourceOHOS> weak_factory_{this};
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  int64_t vsync_frequency_to_reset_ = 0;
  int64_t vsync_frequency_to_update_ = 30; // vsync_to_update_ >= 30 for user experience
  bool update_vsync_frequency_ = false;
  bool reset_vsync_frequency_ = false;
#endif
};
}  // namespace viz

#endif

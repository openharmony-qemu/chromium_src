// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_EXT_H_

#include "third_party/blink/renderer/platform/media/web_media_player_impl.h"
#include <stdint.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/cancelable_callback.h"
#include "base/compiler_specific.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/threading/thread.h"
#include "base/time/default_tick_clock.h"
#include "base/time/time.h"
#include "base/timer/elapsed_timer.h"
#include "base/timer/timer.h"
#include "build/build_config.h"
#include "media/base/cdm_config.h"
#include "media/base/data_source.h"
#include "media/base/demuxer.h"
#include "media/base/eme_constants.h"
#include "media/base/encryption_scheme.h"
#include "media/base/media_observer.h"
#include "media/base/media_tracks.h"
#include "media/base/overlay_info.h"
#include "media/base/pipeline_impl.h"
#include "media/base/renderer_factory_selector.h"
#include "media/base/routing_token_callback.h"
#include "media/base/simple_watch_timer.h"
#include "media/filters/demuxer_manager.h"
#include "media/mojo/mojom/media_metrics_provider.mojom.h"
#include "media/mojo/mojom/playback_events_recorder.mojom.h"
#include "media/renderers/paint_canvas_video_renderer.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/media_session/public/cpp/media_position.h"
#include "third_party/blink/public/platform/media/video_frame_compositor.h"
#include "third_party/blink/public/platform/media/web_media_player_builder.h"
#include "third_party/blink/public/platform/media/web_media_player_delegate.h"
#include "third_party/blink/public/platform/web_audio_source_provider.h"
#include "third_party/blink/public/platform/web_content_decryption_module_result.h"
#include "third_party/blink/public/platform/web_media_player.h"
#include "third_party/blink/public/platform/web_surface_layer_bridge.h"
#include "third_party/blink/renderer/platform/allow_discouraged_type.h"
#include "third_party/blink/renderer/platform/bindings/v8_external_memory_accounter.h"
#include "third_party/blink/renderer/platform/media/learning_experiment_helper.h"
#include "third_party/blink/renderer/platform/media/media_player_client.h"
#include "third_party/blink/renderer/platform/media/multi_buffer_data_source.h"
#include "third_party/blink/renderer/platform/media/smoothness_helper.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "url/gurl.h"

namespace base {
class SingleThreadTaskRunner;
class TaskRunner;
}  // namespace base

namespace cc {
class VideoLayer;
}

namespace gfx {
class Size;
}

namespace media {
class CdmContextRef;
class ChunkDemuxer;
class Demuxer;
class MediaLog;
class MemoryDumpProviderProxy;
class PipelineController;
class SwitchableAudioRendererSink;

namespace learning {
class LearningTaskController;
}
}  // namespace media

namespace viz {
class RasterContextProvider;
}

namespace blink {
class BufferedDataSourceHostImpl;
class PowerStatusHelper;
class ThreadSafeBrowserInterfaceBrokerProxy;
class UrlIndex;
class VideoDecodeStatsReporter;
class VideoFrameCompositor;
class WatchTimeReporter;
class WebAudioSourceProviderImpl;
class WebContentDecryptionModule;
class WebLocalFrame;
class WebMediaPlayerEncryptedMediaClient;
class WebMediaPlayerImpl;

class WebMediaPlayerImplExt : public WebMediaPlayerImpl {
public:
  WebMediaPlayerImplExt(
      WebLocalFrame* frame,
      MediaPlayerClient* client,
      WebMediaPlayerEncryptedMediaClient* encrypted_client,
      WebMediaPlayerDelegate* delegate,
      std::unique_ptr<media::RendererFactorySelector> renderer_factory_selector,
      UrlIndex* url_index,
      std::unique_ptr<VideoFrameCompositor> compositor,
      std::unique_ptr<media::MediaLog> media_log,
      media::MediaPlayerLoggingID player_id,
      WebMediaPlayerBuilder::DeferLoadCB defer_load_cb,
      scoped_refptr<media::SwitchableAudioRendererSink> audio_renderer_sink,
      scoped_refptr<base::SequencedTaskRunner> media_task_runner,
      scoped_refptr<base::TaskRunner> worker_task_runner,
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      scoped_refptr<base::SingleThreadTaskRunner>
          video_frame_compositor_task_runner,
      WebContentDecryptionModule* initial_cdm,
      media::RequestRoutingTokenCallback request_routing_token_cb,
      base::WeakPtr<media::MediaObserver> media_observer,
      bool enable_instant_source_buffer_gc,
      bool embedded_media_experience_enabled,
      mojo::PendingRemote<media::mojom::MediaMetricsProvider> metrics_provider,
      CreateSurfaceLayerBridgeCB create_bridge_callback,
      scoped_refptr<viz::RasterContextProvider> raster_context_provider,
      bool use_surface_layer,
      bool is_background_suspend_enabled,
      bool is_background_video_play_enabled,
      bool is_background_video_track_optimization_supported,
      std::unique_ptr<media::Demuxer> demuxer_override,
      scoped_refptr<ThreadSafeBrowserInterfaceBrokerProxy> remote_interfaces);

  blink::WebMediaPlayerImplExt* AsWebMediaPlayerImplExt() override {
    return this;
  }

  base::WeakPtr<WebMediaPlayerImplExt> WebMediaPlayerImplExtWeakThis() override {
    return weak_this_;
  }

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void DoReloadForPrimitive();
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void RestartForPrimitive() override;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void OnNativeTextureCreated(int native_texture_id,
                              media::Renderer::OnGetRectCallback callback);
  void UpdatePlaybackStatus(uint32_t status);
  void UpdateVolume(double volume);
  void UpdateMuted(bool muted);
  void UpdatePlaybackRate(double playback_rate);

  void PlayWithReason(media::ActionReason reason) override;
  void PauseWithReason(media::ActionReason reason) override;
  bool IsMediaPlayerShown() const override;
  bool IsUsingCustomRenderer() const override;
  void SetInitialPreload(uint32_t preload) override;

  void OnLayerRectChange(const gfx::Rect& rect);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnLayerBoundsChange(const gfx::Rect& bounds);
  void OnLayerOpacityChange(float opacity);
  void SaveLastFrameTimeStamp();
  void HandleSurfaceSwitchWhenPaused();
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnSurfaceRequested(media::SurfaceCreatedCB surface_created_cb,
                          bool support_video_surface,
                          std::string decoder_name);
  void OnVideoDecoderChanaged(bool support_video_surface,
                              std::string decoder_name);
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurface(int32_t widget_id) override;
  bool SupportVideoSurface() override;
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  bool IsFrameHidden() override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  void RecycleDmaBuffer() override;
  void ResumeDmaBuffer() override;
  bool IsDmaBufferRecycleEnabled() override;
  void SetDmaBufferSeekState(bool state) override;

  base::Lock lock_;
  DmaBufferState dma_state_;
  bool dmabuf_seeking_enabled_ = false;
  std::optional<bool> dmabuf_recycled;
#endif  // ARKWEB_MEDIA_DMABUF
#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
  void NotifyMemoryLevel(
    base::MemoryPressureListener::MemoryPressureLevel memory_pressure_level) override;

  base::MemoryPressureListener::MemoryPressureLevel memory_pressure_level_ =
    base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE;
#endif  // ARKWEB_MEDIA_MEMORY_PRESSURE

#if BUILDFLAG(ARKWEB_BFCACHE)
  void MediaResumeFromBFCachePage(bool restoring_in_bfcache) override;
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  int64_t GetFreezeTime() const override;
  int64_t GetPlayedTime() override;
  void SetPipelineStatus(media::PipelineStatusCodes status);
  media::PipelineStatusCodes GetPipelineStatus() const override;
  void OnWebURLError(int reason);
  int GetWebURLErrorReason() const override;
  WebString GetMimeType() const override;
  bool UsingMediaPlayer() const override;
  void OnHiddenVideoReport(bool storing_in_bfcache) override;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  int64_t start_play_time_ = 0;
  int64_t total_play_time_ = 0;
  media::PipelineStatusCodes pipeline_status_ = media::PIPELINE_OK;
  int web_url_error_reason_ = 0;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

  base::WeakPtr<WebMediaPlayerImplExt> weak_this_;
  base::WeakPtrFactory<WebMediaPlayerImplExt> weak_factory_{this};
};

}

#endif //THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_EXT_H_
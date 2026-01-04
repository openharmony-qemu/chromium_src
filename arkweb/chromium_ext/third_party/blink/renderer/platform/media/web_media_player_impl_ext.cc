// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/check.h"
#include "base/command_line.h"
#include "base/debug/alias.h"
#include "base/debug/crash_logging.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "cc/layers/video_layer.h"
#include "components/viz/common/gpu/raster_context_provider.h"
#include "media/audio/null_audio_sink.h"
#include "media/base/audio_renderer_sink.h"
#include "media/base/cdm_context.h"
#include "media/base/demuxer.h"
#include "media/base/encryption_scheme.h"
#include "media/base/key_systems.h"
#include "media/base/limits.h"
#include "media/base/media_content_type.h"
#include "media/base/media_log.h"
#include "media/base/media_player_logging_id.h"
#include "media/base/media_switches.h"
#include "media/base/media_url_demuxer.h"
#include "media/base/memory_dump_provider_proxy.h"
#include "media/base/remoting_constants.h"
#include "media/base/renderer.h"
#include "media/base/routing_token_callback.h"
#include "media/base/supported_types.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_frame.h"
#include "media/filters/chunk_demuxer.h"
#include "media/filters/ffmpeg_demuxer.h"
#include "media/filters/memory_data_source.h"
#include "media/filters/pipeline_controller.h"
#include "media/learning/common/learning_task_controller.h"
#include "media/learning/common/media_learning_tasks.h"
#include "media/learning/mojo/public/cpp/mojo_learning_task_controller.h"
#include "media/media_buildflags.h"
#include "media/mojo/mojom/media_metrics_provider.mojom-blink.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "net/base/data_url.h"
#include "net/http/http_request_headers.h"
#include "net/url_request/url_request_job.h"
#include "services/device/public/mojom/battery_monitor.mojom-blink.h"
#include "third_party/blink/public/common/media/display_type.h"
#include "third_party/blink/public/common/media/watch_time_reporter.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/web_audio_source_provider_impl.h"
#include "third_party/blink/public/platform/web_content_decryption_module.h"
#include "third_party/blink/public/platform/web_encrypted_media_types.h"
#include "third_party/blink/public/platform/web_fullscreen_video_status.h"
#include "third_party/blink/public/platform/web_media_player_encrypted_media_client.h"
#include "third_party/blink/public/platform/web_media_player_source.h"
#include "third_party/blink/public/platform/web_media_source.h"
#include "third_party/blink/public/platform/web_runtime_features.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_surface_layer_bridge.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/strings/grit/blink_strings.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/renderer/platform/media/buffered_data_source_host_impl.h"
#include "third_party/blink/renderer/platform/media/media_player_util.h"
#include "third_party/blink/renderer/platform/media/power_status_helper.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/blink/renderer/platform/media/video_decode_stats_reporter.h"
#include "third_party/blink/renderer/platform/media/web_content_decryption_module_impl.h"
#include "third_party/blink/renderer/platform/media/web_media_source_impl.h"
#include "ui/gfx/geometry/size.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/web_media_player_impl_ext.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"

namespace blink {

namespace {
  std::ostream& operator<<(std::ostream& stream,
                         WebMediaPlayerImplExt const& media_player) {
  return stream << static_cast<void const*>(&media_player);
}
}

WebMediaPlayerImplExt::WebMediaPlayerImplExt(
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
    scoped_refptr<ThreadSafeBrowserInterfaceBrokerProxy> remote_interfaces)
    : WebMediaPlayerImpl(frame, client, encrypted_client, delegate, std::move(renderer_factory_selector),
      url_index, std::move(compositor), std::move(media_log), player_id, std::move(defer_load_cb), audio_renderer_sink,
      media_task_runner, worker_task_runner, compositor_task_runner, video_frame_compositor_task_runner,
      initial_cdm, request_routing_token_cb, media_observer, enable_instant_source_buffer_gc,
      embedded_media_experience_enabled, std::move(metrics_provider), std::move(create_bridge_callback),
      raster_context_provider, use_surface_layer, is_background_suspend_enabled, is_background_video_play_enabled,
      is_background_video_track_optimization_supported, std::move(demuxer_override),
#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
      remote_interfaces),
      dma_state_(kHaveExist) {
#else
      remote_interfaces) {
#endif  // ARKWEB_MEDIA_DMABUF
          weak_this_ = weak_factory_.GetWeakPtr();
}

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void WebMediaPlayerImplExt::DoReloadForPrimitive() {
  // If a demuxer override was specified or a Media Source pipeline will be
  // used, the pipeline can start immediately.
  auto loaded_url = demuxer_manager_->LoadedUrl();
  if (demuxer_manager_->HasDemuxerOverride() ||
      load_type_ == kLoadTypeMediaSource ||
      loaded_url.SchemeIs(media::remoting::kRemotingScheme)) {
    StartPipeline();
    return;
  }

  // Short circuit the more complex loading path for data:// URLs. Sending
  // them through the network based loading path just wastes memory and causes
  // worse performance since reads become asynchronous.
  if (loaded_url.SchemeIs(url::kDataScheme)) {
    std::string mime_type, charset, data;
    if (!net::DataURL::Parse(loaded_url, &mime_type, &charset, &data) ||
        data.empty()) {
      return MemoryDataSourceInitialized(false, 0);
    }
    size_t data_size = data.size();
    demuxer_manager_->SetDataSource(
        std::make_unique<media::MemoryDataSource>(std::move(data)));
    MemoryDataSourceInitialized(true, data_size);
    return;
  }

  auto data_source = std::make_unique<MultiBufferDataSource>(
      main_task_runner_,
      url_index_->GetByUrl(
          KURL(loaded_url), static_cast<UrlData::CorsMode>(cors_mode_),
          is_cache_disabled_ ? UrlData::kCacheDisabled : UrlData::kNormal),
      media_log_.get(), buffered_data_source_host_.get(),
      base::BindRepeating(&WebMediaPlayerImpl::NotifyDownloading, weak_this_));

  auto* mb_data_source = data_source.get();
  demuxer_manager_->SetDataSource(std::move(data_source));

  mb_data_source->OnRedirect(base::BindRepeating(
      &WebMediaPlayerImpl::OnDataSourceRedirected, weak_this_));
  mb_data_source->SetPreload(preload_);
  mb_data_source->SetIsClientAudioElement(client_->IsAudioElement());
  mb_data_source->Initialize(base::BindOnce(
      &WebMediaPlayerImpl::MultiBufferDataSourceInitialized, weak_this_));
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void WebMediaPlayerImplExt::RestartForPrimitive() {
  LOG(INFO) << "RestartForPrimitive, primitive_renderer_type_["
            << GetRendererName(primitive_renderer_type_) << "]";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "RestartForPrimitive, primitive_renderer_type_["
                     << GetRendererName(primitive_renderer_type_) << "]";
#endif  // ARKWEB_LOGGER_REPORT
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  should_create_custom_renderer_ = false;
  should_overlay_ = false;
  renderer_factory_selector_->SetBaseRendererType(primitive_renderer_type_);

  client_->RestartForPrimitive();

  SetMemoryReportingState(false);
  DoReloadForPrimitive();
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void WebMediaPlayerImplExt::OnNativeTextureCreated(
    int native_texture_id,
    media::Renderer::OnGetRectCallback callback) {
  native_texture_id_ = native_texture_id;
  if (bridge_) {
    bridge_->GetCcLayer()->layer_utils()->SetNativeEmbedId(native_texture_id);
  }
  on_get_rect_cb_ = callback;
  on_get_rect_cb_.Run(client_->GetVideoRect());
}

void WebMediaPlayerImplExt::UpdatePlaybackStatus(uint32_t status) {
  client_->UpdatePlaybackStatus(status);
}

void WebMediaPlayerImplExt::UpdateVolume(double volume) {
  client_->UpdateVolume(volume);
}

void WebMediaPlayerImplExt::UpdateMuted(bool muted) {
  client_->UpdateMuted(muted);
}

void WebMediaPlayerImplExt::UpdatePlaybackRate(double playback_rate) {
  client_->UpdatePlaybackRate(playback_rate);
}

void WebMediaPlayerImplExt::PlayWithReason(media::ActionReason reason) {
  base::AutoReset<media::ActionReason> resetter(&action_reason_, reason);
  Play();
}

void WebMediaPlayerImplExt::PauseWithReason(media::ActionReason reason) {
  base::AutoReset<media::ActionReason> resetter(&action_reason_, reason);
  Pause();
}

bool WebMediaPlayerImplExt::IsMediaPlayerShown() const {
  return !IsPageHidden();
}

bool WebMediaPlayerImplExt::IsUsingCustomRenderer() const {
  return should_create_custom_renderer_;
}

void WebMediaPlayerImplExt::SetInitialPreload(uint32_t preload) {
  initial_preload_ = preload;
}

void WebMediaPlayerImplExt::OnLayerRectChange(const gfx::Rect& rect) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (on_get_rect_cb_) {
    on_get_rect_cb_.Run(rect);
  }
  client_->OnLayerRectChange(rect);
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void WebMediaPlayerImplExt::OnLayerBoundsChange(const gfx::Rect& bounds) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  client_->OnLayerBoundsChange(bounds);
}

void WebMediaPlayerImplExt::SetVideoSurface(int32_t widget_id) {
  LOG(INFO) << "SetVideoSurface(" << widget_id
            << "), has_page_hidden_when_paused:"
            << has_page_hidden_when_paused_;
  video_surface_id_ = widget_id;
  if (surface_created_cb_) {
    surface_created_cb_.Run(widget_id);
  }

  if (paused_ && !seeking_) {
    HandleSurfaceSwitchWhenPaused();
  }
}

void WebMediaPlayerImplExt::HandleSurfaceSwitchWhenPaused() {
  if (has_page_hidden_when_paused_ && pipeline_controller_) {
    has_page_hidden_when_paused_ = false;
    pipeline_controller_->SetPreciseSeekTarget(last_frame_timestamp_);
  }

  Seek(CurrentTime());
}

void WebMediaPlayerImplExt::SaveLastFrameTimeStamp() {
  auto frame = compositor_->GetCurrentFrameOnAnyThread();
  if (frame) {
    last_frame_timestamp_ = (frame->timestamp()).InMicroseconds();
    LOG(DEBUG) << "WebMediaPlayerImplExt::SaveLastFrameTimeStamp:"
               << last_frame_timestamp_;
  }
}

bool WebMediaPlayerImplExt::SupportVideoSurface() {
  return support_video_surface_;
}

void WebMediaPlayerImplExt::OnSurfaceRequested(
    media::SurfaceCreatedCB surface_created_cb,
    bool support_video_surface,
    std::string decoder_name) {
  LOG(INFO) << "OnSurfaceRequested(" << support_video_surface <<
            "), video_surface_id_[" << video_surface_id_ << "]";
  surface_created_cb_ = std::move(surface_created_cb);
  support_video_surface_ = support_video_surface;

  if (video_surface_id_ > 0) {
    surface_created_cb_.Run(video_surface_id_);
  }
  client_->OnSupportVideoSurfaceChanged(support_video_surface_, decoder_name);
}

void WebMediaPlayerImplExt::OnVideoDecoderChanaged(
    bool support_video_surface,
    std::string decoder_name) {
  LOG(INFO) << "OnVideoDecoderChanaged(" << support_video_surface
            << ", " << decoder_name << ")";
  support_video_surface_ = support_video_surface;
  client_->OnSupportVideoSurfaceChanged(support_video_surface_, decoder_name);
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
int64_t WebMediaPlayerImplExt::GetFreezeTime() const {
  return compositor_->GetFreezeTime();
}

int64_t WebMediaPlayerImplExt::GetPlayedTime() {
  int64_t played_time = total_play_time_;
  total_play_time_ = 0;
  return played_time;
}

void WebMediaPlayerImplExt::SetPipelineStatus(media::PipelineStatusCodes status) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  pipeline_status_ = status;
}

media::PipelineStatusCodes WebMediaPlayerImplExt::GetPipelineStatus() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  return pipeline_status_;
}

void WebMediaPlayerImplExt::OnWebURLError(int reason) {
  web_url_error_reason_ = reason;
}

int WebMediaPlayerImplExt::GetWebURLErrorReason() const {
  return web_url_error_reason_;
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
bool WebMediaPlayerImplExt::IsFrameHidden() {
  return IsPageHidden();
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
void WebMediaPlayerImpl::PipEnable(bool enable) {
  LOG(INFO) << "Pip WebMediaPlayerImpl::PipEnable" << enable << "]";
  if (pipeline_controller_) {
    pipeline_controller_->PipEnable(enable);
  } else {
    LOG(ERROR) << "Pip pipeline_controller_ is null.";
  }
  if (!enable) {
    video_surface_id_ = -1;
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void WebMediaPlayerImplExt::SetDmaBufferSeekState(bool state) {
  dmabuf_seeking_enabled_ = state;
}

bool WebMediaPlayerImplExt::IsDmaBufferRecycleEnabled() {
  if (GetDemuxerType() == media::DemuxerType::kChunkDemuxer) {
    return false;
  }

  if (!dmabuf_seeking_enabled_) {
    return false;
  }

  LOG(INFO) << "DMABUF::WebMediaPlayerImplExt(), IsDmaBufferRecycleEnabled = true"
            << " delegate_id_:" << delegate_id_;
  return dmabuf_recycled.value_or(true);
}

void WebMediaPlayerImplExt::RecycleDmaBuffer() {
  base::AutoLock lock(lock_);

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
  // Called by the OnPageHidden
  if (memory_pressure_level_ < base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE) {
    return;
  }
#endif  // ARKWEB_MEDIA_MEMORY_PRESSURE

  if (IsPageHidden() || (IsHidden() && should_pause_when_frame_is_hidden_)) {
    if (ShouldPausePlaybackWhenHidden() || !base::ohos::IsPcDevice()) {
      LOG(INFO) << "DMABUF::The device is not a PC or not have media player, No need RecycleDmaBuffer";
      return;
    }
  }
  
  // Called by the memory pressure listener
  if (!paused_ || !IsPageHidden()) {
    return;
  }

  if (!pipeline_controller_) {
    LOG(ERROR) << "DMABUF::WebMediaPlayerImplExt::RecycleDmaBuffer, pipeline_controller is null";
    return;
  }
  
  if (dma_state_ == kHaveExist) {
    LOG(INFO) << "DMABUF::WebMediaPlayerImplExt, RecycleDmaBuffer()"
              << " delegate_id_:" << delegate_id_;
    pipeline_controller_->RecycleDmaBuffer();
    dma_state_ = kHaveRecycled;
  }
}

void WebMediaPlayerImplExt::ResumeDmaBuffer() {
  base::AutoLock lock(lock_);

  if (!pipeline_controller_) {
    LOG(ERROR) << "DMABUF::WebMediaPlayerImplExt::ResumeDmaBuffer, pipeline_controller is null";
    return;
  }

  if (dma_state_ == kHaveRecycled) {
    LOG(INFO) << "DMABUF::WebMediaPlayerImplExt, ResumeDmaBuffer()"
              << " delegate_id_:" << delegate_id_;
    pipeline_controller_->ResumeDmaBuffer();
    dma_state_ = kHaveExist;
    if (client_) {
      dmabuf_seeking_enabled_ = true;
      client_->OnDmaBufferSeekTo(paused_time_);
    } else {
      LOG(ERROR) << "DMABUF::WebMediaPlayerImplExt::ResumeDmaBuffer, client is null";  
    }
  }
}
#endif  // ARKWEB_MEDIA_DMABUF

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
void WebMediaPlayerImplExt::NotifyMemoryLevel(
    base::MemoryPressureListener::MemoryPressureLevel memory_pressure_level) {
  memory_pressure_level_ = memory_pressure_level;
}
#endif  // ARKWEB_MEDIA_MEMORY_PRESSURE

#if BUILDFLAG(ARKWEB_BFCACHE)
void WebMediaPlayerImplExt::MediaResumeFromBFCachePage(bool restoring_in_bfcache) {
  LOG(INFO) << "MediaResumeFromBFCachePage restoring_in_bfcache" << restoring_in_bfcache;
  if (!client_) {
    LOG(ERROR) << "OhMedia::media_player_client is nullptr";
    return;
  }
  bool is_media_resume = client_->IsMediaResumeFromBFCachePage();
  LOG(INFO) << "MediaResumeFromBFCachePage is_media_resume: " << is_media_resume;
  if (restoring_in_bfcache && !is_media_resume) {
    LOG(INFO) << "OhMedia::WebPage is restored from BFCACHE without resuming playback.";
  } else {
    client_->ResumePlayback();  // Calls UpdatePlayState() so return afterwards.
  }
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)
}
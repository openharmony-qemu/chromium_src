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

#if BUILDFLAG(ENABLE_HLS_DEMUXER)
#include "media/filters/hls_data_source_provider_impl.h"
#include "third_party/blink/renderer/platform/media/multi_buffer_data_source_factory.h"
#endif  // BUILDFLAG(ENABLE_HLS_DEMUXER)

#if BUILDFLAG(IS_ANDROID)
#include "media/base/android/media_codec_util.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "ohos_nweb/include/nweb_native_media_player.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_BFCACHE)
#include "third_party/blink/public/common/scheduler/web_scheduler_tracked_feature.h"
#include "third_party/blink/renderer/platform/scheduler/public/frame_scheduler.h"
#endif  // ARKWEB_BFCACHE

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/web_media_player_impl_utils.h"
#include "third_party/blink/renderer/platform/media/web_media_player_impl.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/web_media_player_impl_ext.h"

namespace blink {

WebMediaPlayerImplUtils::WebMediaPlayerImplUtils(WebMediaPlayerImpl* WebMediaPlayerImpl) {
  this->impl = WebMediaPlayerImpl;
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  impl->should_create_custom_renderer_ = impl->client_->IsCustomVideoPlayerEnabled();
  impl->should_overlay_ = impl->should_create_custom_renderer_ &&
                    impl->client_->ShouldCustomVideoPlayerOverlay();
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (impl->should_create_custom_renderer_) {
    auto scheduler = impl->frame_->Scheduler();
    if (scheduler) {
      scheduler->RegisterStickyFeature(
          scheduler::WebSchedulerTrackedFeature::kEnableCacheMediaTakeOver,
          {SchedulingPolicy::DisableBackForwardCache()});
    }
  }
#endif  // ARKWEB_BFCACHE
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

void WebMediaPlayerImplUtils::ExitedFullscreenExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  bool surface_changed = impl->video_surface_id_ != -1;
  impl->video_surface_id_ = -1;
  if (surface_changed && impl->surface_created_cb_) {
    impl->surface_created_cb_.Run(impl->video_surface_id_);
    if (impl->Paused() && !impl->ended_) {
      impl->Seek(impl->CurrentTime());
    }
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

bool WebMediaPlayerImplUtils::DoLoadExt(WebMediaPlayer::CorsMode cors_mode, bool is_cache_disabled) {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  if (impl->demuxer_manager_->LoadedUrl().SchemeIs(media::remoting::kRemotingScheme)) {
    LOG(INFO) << "disable custom renderer for remote scheme";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "disable custom renderer for remote scheme";
#endif  // ARKWEB_LOGGER_REPORT
    impl->should_create_custom_renderer_ = false;
    impl->should_overlay_ = false;
  }
  if (impl->should_create_custom_renderer_) {
    impl->cors_mode_ = cors_mode;
    impl->is_cache_disabled_ = is_cache_disabled;
    impl->StartPipeline();
    return true;
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
return false;
}

void WebMediaPlayerImplUtils::PlayExt() {
  impl->pipeline_controller_->SetMediaPlayerState(false);
  if (impl->action_reason_ != media::ActionReason::kNormal) {
    impl->pipeline_controller_->SetPlaybackRateWithReason(impl->playback_rate_,
      impl->action_reason_);
  } else {
    impl->pipeline_controller_->SetPlaybackRate(impl->playback_rate_);
  }
}

void WebMediaPlayerImplUtils::PlayCapabilitiesExt() {
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  impl->AsWebMediaPlayerImplExt()->start_play_time_ =
      (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds();
  impl->compositor_->SetStartTime(impl->AsWebMediaPlayerImplExt()->start_play_time_);
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
}

void WebMediaPlayerImplUtils::PauseCapabilitiesExt() {
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  impl->compositor_->SetStartTime(0);
  int64_t play_time =
      (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() -
      impl->AsWebMediaPlayerImplExt()->start_play_time_;
  if (play_time > 0 && impl->AsWebMediaPlayerImplExt()->start_play_time_ > 0) {
    impl->AsWebMediaPlayerImplExt()->total_play_time_ += play_time;
    if (impl->client_) {
      impl->client_->ScheduleVideoFreezeEvent();
    }
  }
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
}

void WebMediaPlayerImplUtils::OnMetadataExt() {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
      if (!impl->surface_layer_for_video_enabled_) {
        impl->ActivateSurfaceLayerForVideo();
      }
      impl->bridge_->GetCcLayer()->SetShouldOverlay(impl->should_overlay_);
      impl->bridge_->GetCcLayer()->layer_utils()->SetShouldInterceptTouchEvent(
          impl->should_create_custom_renderer_);
      impl->bridge_->GetCcLayer()->layer_utils()->SetNativeEmbedId(impl->native_texture_id_);
      impl->bridge_->GetCcLayer()->layer_utils()->SetMayContainNative(
          impl->should_create_custom_renderer_);
      cc::SurfaceLayer::RectChangeCallback video_rect_callback;
      if (impl->should_create_custom_renderer_) {
        video_rect_callback =
            base::BindPostTaskToCurrentDefault(base::BindRepeating(
                &WebMediaPlayerImplExt::OnLayerRectChange, impl->WebMediaPlayerImplExtWeakThis()));
      }
      impl->bridge_->SetVideoRectChangeCallback(video_rect_callback);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
      if (impl->client_->IsVideoAssistantEnabled()) {
        impl->bridge_->SetLayerBoundsChangeCallback(
            base::BindPostTaskToCurrentDefault(base::BindRepeating(
                &WebMediaPlayerImplExt::OnLayerBoundsChange, impl->WebMediaPlayerImplExtWeakThis())));
      }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void WebMediaPlayerImplUtils::OnPageHiddenExt(bool storing_in_bfcache) {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  if (storing_in_bfcache) {
    impl->pipeline_controller_->SetMediaPlayerState(
        true,
        static_cast<int>(OHOS::NWeb::SuspendType::ENTER_BACK_FORWARD_CACHE));
  } else {
    impl->pipeline_controller_->SetMediaPlayerState(
        true, static_cast<int>(OHOS::NWeb::SuspendType::ENTER_BACKGROUND));
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

void WebMediaPlayerImplUtils::CreateRendererExtSetRendererType() {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  impl->primitive_renderer_type_ =
      impl->renderer_factory_selector_->GetCurrentRendererType();
  if (impl->should_create_custom_renderer_) {
    impl->renderer_factory_selector_->SetBaseRendererType(
        media::RendererType::kOHOSCustomMediaPlayer);
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

std::unique_ptr<media::Renderer> WebMediaPlayerImplUtils::CreateRendererExtConfigRenderer(
    media::RequestOverlayInfoCB request_overlay_info_cb) {
  if (impl->should_create_custom_renderer_) {
    auto renderer =
        impl->renderer_factory_selector_->GetCurrentFactory()->CreateCustomRenderer(
            impl->media_task_runner_, impl->worker_task_runner_,
            impl->audio_source_provider_.get(), impl->compositor_.get(),
            std::move(request_overlay_info_cb), impl->client_->TargetColorSpace(),
            impl->GetDelegateId());
    std::vector<media::Renderer::MediaSourceInfo> media_source_list;
    media::Renderer::MediaSourceInfo media_source_info;
    media_source_info.media_format = impl->client_->GetMediaFormat();
    media_source_info.media_source = impl->demuxer_manager_->LoadedUrl().spec();
    media_source_list.emplace_back(media_source_info);
    for (const auto& source_info : impl->client_->GetRemainSourceInfos()) {
      media_source_list.emplace_back(source_info);
    }
    renderer->SetMediaSourceList(media_source_list);
    std::vector<std::string> controls_list;
    for (const auto& controls_list_item : impl->client_->GetMediaControlsList()) {
      controls_list.emplace_back(controls_list_item.Utf8());
    }
    renderer->SetMediaControls(impl->client_->ShouldShowMediaControls(),
                               controls_list);
    renderer->SetPoster(impl->poster_url_);
    renderer->SetMuted(impl->client_->IsMuted());
    renderer->SetAttributes(impl->client_->GetElementAttributes());
    renderer->SetReferrer(impl->client_->GetOutgoingReferrerString());

    renderer->SetSurfaceCreatedCallback(
        base::BindPostTaskToCurrentDefault(base::BindOnce(
            &WebMediaPlayerImplExt::OnNativeTextureCreated, impl->WebMediaPlayerImplExtWeakThis())));

    renderer->SetUpdatePlaybackStatusCallback(
        base::BindPostTaskToCurrentDefault(base::BindRepeating(
            &WebMediaPlayerImplExt::UpdatePlaybackStatus, impl->WebMediaPlayerImplExtWeakThis())));
    renderer->SetUpdateVolumeCallback(base::BindPostTaskToCurrentDefault(
        base::BindRepeating(&WebMediaPlayerImplExt::UpdateVolume, impl->WebMediaPlayerImplExtWeakThis())));
    renderer->SetUpdateMutedCallback(base::BindPostTaskToCurrentDefault(
        base::BindRepeating(&WebMediaPlayerImplExt::UpdateMuted, impl->WebMediaPlayerImplExtWeakThis())));
    renderer->SetUpdatePlaybackRateCallback(
        base::BindPostTaskToCurrentDefault(base::BindRepeating(
            &WebMediaPlayerImplExt::UpdatePlaybackRate, impl->WebMediaPlayerImplExtWeakThis())));

    renderer->SetIsAudio(impl->client_->IsAudioElement());
    return renderer;
  }
  return nullptr;
}

void WebMediaPlayerImplUtils::SetSuspendStateExt() {
  media::RequestSurfaceCB request_surface_cb =
      base::BindPostTaskToCurrentDefault(
          base::BindOnce(&WebMediaPlayerImplExt::OnSurfaceRequested,
              impl->WebMediaPlayerImplExtWeakThis()));
  media::VideoDecoderChangedCB video_decoder_changed_cb =
      base::BindPostTaskToCurrentDefault(
          base::BindRepeating(&WebMediaPlayerImplExt::OnVideoDecoderChanaged,
              impl->WebMediaPlayerImplExtWeakThis()));
  impl->pipeline_controller_->Resume(
      std::move(request_surface_cb),
      std::move(video_decoder_changed_cb));
}

void WebMediaPlayerImplUtils::OnIdleTimeoutExt() {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  impl->pipeline_controller_->SetMediaPlayerState(
      true, static_cast<int>(OHOS::NWeb::SuspendType::AUTO_CLEANUP));
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

void WebMediaPlayerImplUtils::PauseExt() {
  if (impl->action_reason_ != media::ActionReason::kNormal) {
    impl->pipeline_controller_->SetPlaybackRateWithReason(0.0, impl->action_reason_);
  } else {
    impl->pipeline_controller_->SetPlaybackRate(0.0);
  }
}

void WebMediaPlayerImplUtils::DoSeekExt(base::TimeDelta time) {
#if BUILDFLAG(ARKWEB_MEDIA)
  LOG(WARNING) << "OhMedia::DoSeek(" << (void*)this << "), seconds = " << time.InSecondsF() << "s)";
#endif // BUILDFLAG(ARKWEB_MEDIA)
}

void WebMediaPlayerImplUtils::SetVolumeExt(double volume) {
#if BUILDFLAG(ARKWEB_MEDIA)
  LOG(INFO) << "OhMedia:: " << __func__ << "(" << (void*)this  << "), volume =" << volume;
#endif // BUILDFLAG(ARKWEB_MEDIA)
}

void WebMediaPlayerImplUtils::OnFrameShownExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  impl->client_->OnPageVisibilityChanged();
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void WebMediaPlayerImplUtils::OnFrameHiddenExt() {
  LOG(INFO) << "WebMediaPlayerImpl::OnFrameHidden()";

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  impl->client_->OnPageVisibilityChanged();
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void WebMediaPlayerImplUtils::SetPosterExt(const WebURL& poster) {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  impl->poster_url_ = poster.GetString().Utf8();
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

bool WebMediaPlayerImplUtils::UpdatePlayStateExt(bool can_auto_suspend) {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  if (impl->should_create_custom_renderer_) {
    return false;
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
  return can_auto_suspend;
}

void WebMediaPlayerImplUtils::SetDelegateStateExt() {
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  impl->client_->DidPlayerPaused(impl->ended_);
  impl->client_->DidPlayerGone();
#endif
}

void WebMediaPlayerImplUtils::UpdatePlayState_ComputePlayStateExt(
    bool& idle_suspended, bool& can_stay_suspended) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->video_surface_id_ > 0) {
    idle_suspended = false;
    can_stay_suspended = false;
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

bool WebMediaPlayerImplUtils::ShouldPausePlaybackWhenHiddenExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  // Expect that video will pause after switching to the background while
  // loading.
  if (impl->HasVideo() && impl->pipeline_metadata_.natural_size.IsEmpty()) {
    return true;
  }
#endif
  return false;
}

void WebMediaPlayerImplUtils::DidEndAVSessionExt() {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  LOG(INFO)<<"WebMediaPlayerImplUtils: DidEndAVSessionExt() try to end avsession!";
  if (impl->client_ && impl->ShouldPausePlaybackWhenHidden()) {
    impl->client_->DidEndAVSession(true);
  }
#endif // ARKWEB_MEDIA_AVSESSION
}

}

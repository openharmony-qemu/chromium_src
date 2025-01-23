
#include "content/renderer/media/ohos/ohos_custom_media_player_renderer_client.h"

#include <utility>

#include "base/functional/bind.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"

namespace content {

OHOSCustomMediaPlayerRendererClient::OHOSCustomMediaPlayerRendererClient(
    mojo::PendingRemote<RendererExtension> renderer_extension_remote,
    mojo::PendingReceiver<ClientExtension> client_extension_receiver,
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
    std::unique_ptr<media::MojoRenderer> mojo_renderer,
    media::ScopedNativeTextureWrapper native_texture_wrapper,
    media::VideoRendererSink* sink)
    : MojoRendererWrapper(std::move(mojo_renderer)),
      native_texture_wrapper_(std::move(native_texture_wrapper)),
      client_(nullptr),
      sink_(sink),
      media_task_runner_(std::move(media_task_runner)),
      compositor_task_runner_(std::move(compositor_task_runner)),
      delayed_bind_client_extension_receiver_(
          std::move(client_extension_receiver)),
      delayed_bind_renderer_extension_remote_(
          std::move(renderer_extension_remote)) {}

OHOSCustomMediaPlayerRendererClient::~OHOSCustomMediaPlayerRendererClient() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  native_texture_wrapper_->ClearCBOnAnyThread();
}

void OHOSCustomMediaPlayerRendererClient::Initialize(
    media::MediaResource* media_resource,
    media::RendererClient* client,
#ifdef OHOS_VIDEO_ASSISTANT
    media::RequestSurfaceCB request_surface_cb,
#endif // OHOS_VIDEO_ASSISTANT
    media::PipelineStatusCallback init_cb) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(!init_cb_);

  // Consume and bind the delayed PendingRemote and PendingReceiver now that we
  // are on |media_task_runner_|.
  renderer_extension_remote_.Bind(
      std::move(delayed_bind_renderer_extension_remote_), media_task_runner_);
  client_extension_receiver_.Bind(
      std::move(delayed_bind_client_extension_receiver_), media_task_runner_);

  media_resource_ = media_resource;
  client_ = client;
  init_cb_ = std::move(init_cb);

  // Unretained is safe here because |native_texture_wrapper| resets the
  // Closure it has before destroying itself on |compositor_task_runner_|,
  // and |this| is garanteed to live until the Closur has been reset.
  native_texture_wrapper_->Initialize(
      base::BindRepeating(&OHOSCustomMediaPlayerRendererClient::OnFrameAvailable,
                          base::Unretained(this)),
      compositor_task_runner_,
      base::BindOnce(
          &OHOSCustomMediaPlayerRendererClient::OnStreamTextureWrapperInitialized,
          weak_factory_.GetWeakPtr(), media_resource),
      base::BindOnce(&OHOSCustomMediaPlayerRendererClient::OnSurfaceCreated,
                     weak_factory_.GetWeakPtr()),
      base::BindOnce(&OHOSCustomMediaPlayerRendererClient::OnSurfaceDestroyed,
                     weak_factory_.GetWeakPtr()));
}

media::RendererType OHOSCustomMediaPlayerRendererClient::GetRendererType() {
  return media::RendererType::kOHOSCustomMediaPlayer;
}

void OHOSCustomMediaPlayerRendererClient::SetSurfaceCreatedCallback(
    media::Renderer::SurfaceCreatedCallback cb) {
  surface_created_cb_ = std::move(cb);
}

void OHOSCustomMediaPlayerRendererClient::SetUpdatePlaybackStatusCallback(
    media::Renderer::UpdatePlaybackStatusCallback cb) {
  update_playback_status_cb_ = std::move(cb);
}

void OHOSCustomMediaPlayerRendererClient::SetUpdateVolumeCallback(
    media::Renderer::UpdateVolumeCallback cb) {
  update_volume_cb_ = std::move(cb);
}

void OHOSCustomMediaPlayerRendererClient::SetUpdateMutedCallback(
    media::Renderer::UpdateMutedCallback cb) {
  update_muted_cb_ = std::move(cb);
}

void OHOSCustomMediaPlayerRendererClient::SetUpdatePlaybackRateCallback(
    media::Renderer::UpdatePlaybackRateCallback cb) {
  update_playback_rate_cb_ = std::move(cb);
}

void OHOSCustomMediaPlayerRendererClient::OnStreamTextureWrapperInitialized(
    media::MediaResource* media_resource,
    bool success) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  if (!success) {
    std::move(init_cb_).Run(
        media::PipelineStatus::Codes::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  MojoRendererWrapper::Initialize(
      media_resource, client_,
#ifdef OHOS_VIDEO_ASSISTANT
      media::RequestSurfaceCB(),
#endif // OHOS_VIDEO_ASSISTANT
      base::BindOnce(&OHOSCustomMediaPlayerRendererClient::OnRemoteRendererInitialized,
                     weak_factory_.GetWeakPtr()));
}

void OHOSCustomMediaPlayerRendererClient::OnRemoteRendererInitialized(
    media::PipelineStatus status) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(!init_cb_.is_null());

  if (status != media::PIPELINE_OK) {
    status = media::PIPELINE_ERROR_INITIALIZATION_FAILED_CUSTOM_PLAYER;
  }

  if (status == media::PIPELINE_OK) {
    media::PipelineStatistics stats;
    stats.video_pipeline_info = {true, false,
                                 media::VideoDecoderType::kMediaCodec};
    stats.audio_pipeline_info = {true, false,
                                 media::AudioDecoderType::kMediaCodec};
    client_->OnStatisticsUpdate(stats);
  }
  std::move(init_cb_).Run(status);
}

void OHOSCustomMediaPlayerRendererClient::OnFrameAvailable() {
  DCHECK(compositor_task_runner_->BelongsToCurrentThread());

  auto frame = native_texture_wrapper_->GetCurrentFrame();
  auto unique_frame = media::VideoFrame::WrapVideoFrame(
    frame, frame->format(), frame->visible_rect(), frame->natural_size());
  sink_->PaintSingleFrame(std::move(unique_frame));
}

void OHOSCustomMediaPlayerRendererClient::OnVideoSizeChange(const gfx::Size& size) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  client_->OnVideoNaturalSizeChange(size);
}

void OHOSCustomMediaPlayerRendererClient::OnDurationChange(base::TimeDelta duration) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  media_resource_->ForwardDurationChangeToDemuxerHost(duration);
}

void OHOSCustomMediaPlayerRendererClient::UpdatePlaybackStatus(uint32_t status) {
  if (update_playback_status_cb_) {
    update_playback_status_cb_.Run(status);
  }
}
void OHOSCustomMediaPlayerRendererClient::UpdateVolume(double volume) {
  if (update_volume_cb_) {
    update_volume_cb_.Run(volume);
  }
}
void OHOSCustomMediaPlayerRendererClient::UpdateMuted(bool muted) {
  if (update_muted_cb_) {
    update_muted_cb_.Run(muted);
  }
}
void OHOSCustomMediaPlayerRendererClient::UpdatePlaybackRate(double playback_rate) {
  if (update_playback_rate_cb_) {
    update_playback_rate_cb_.Run(playback_rate);
  }
}
void OHOSCustomMediaPlayerRendererClient::UpdateBufferedEndTime(double buffered_time) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  media_resource_->ForwardBufferedEndTimeChangeToDemuxerHost(
      base::Seconds(buffered_time));
}

void OHOSCustomMediaPlayerRendererClient::OnGetVideoRect(const gfx::Rect& rect) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  native_texture_wrapper_->UpdateTextureSize(rect.size());

  if (has_sent_surface_id_to_remote_) {
    return;
  }
  has_sent_surface_id_to_remote_ = true;

  SetSurfaceId(surface_id_, rect);
}

void OHOSCustomMediaPlayerRendererClient::OnSurfaceCreated(int surface_id) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  surface_id_ = surface_id;
  if (surface_created_cb_) {
    std::move(surface_created_cb_).Run(surface_id,
        base::BindPostTask(media_task_runner_,
            base::BindRepeating(
                &OHOSCustomMediaPlayerRendererClient::OnGetVideoRect,
                weak_factory_.GetWeakPtr())));
  } else {
    OnGetVideoRect(gfx::Rect());
  }
}

void OHOSCustomMediaPlayerRendererClient::OnSurfaceDestroyed() {
}

} // namespace

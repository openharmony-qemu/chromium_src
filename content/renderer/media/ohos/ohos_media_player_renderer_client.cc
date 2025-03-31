// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/ohos/ohos_media_player_renderer_client.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <utility>

#include "base/functional/bind.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include <cerrno>
#include "base/logging.h"
#include "graphic_adapter.h"
#include "media/base/video_frame.h"

namespace content {

OHOSMediaPlayerRendererClient::OHOSMediaPlayerRendererClient(
    mojo::PendingRemote<RendererExtention> renderer_extension_remote,
    mojo::PendingReceiver<ClientExtention> client_extension_receiver,
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
      delayed_bind_renderer_extention_remote_(
          std::move(renderer_extension_remote)) {}

OHOSMediaPlayerRendererClient::~OHOSMediaPlayerRendererClient() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  native_texture_wrapper_->ClearCBOnAnyThread();
}

void OHOSMediaPlayerRendererClient::Initialize(
    media::MediaResource* media_resource,
    media::RendererClient* client,
#ifdef OHOS_VIDEO_ASSISTANT
    media::RequestSurfaceCB request_surface_cb,
    media::VideoDecoderChangedCB decoder_changed_cb,
#endif // OHOS_VIDEO_ASSISTANT
    media::PipelineStatusCallback init_cb) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(!init_cb_);

  // Consume and bind the delayed PendingRemote and PendingReceiver now that we
  // are on |media_task_runner_|.
  renderer_extension_remote_.Bind(
      std::move(delayed_bind_renderer_extention_remote_), media_task_runner_);
  client_extension_receiver_.Bind(
      std::move(delayed_bind_client_extension_receiver_), media_task_runner_);

  media_resource_ = media_resource;
  client_ = client;
  init_cb_ = std::move(init_cb);

#ifdef OHOS_VIDEO_ASSISTANT
  request_surface_cb_ = std::move(request_surface_cb);
  decoder_changed_cb_ = std::move(decoder_changed_cb);
#endif // OHOS_VIDEO_ASSISTANT

  native_texture_wrapper_->Initialize(
      base::BindRepeating(&OHOSMediaPlayerRendererClient::OnFrameAvailable,
                          base::Unretained(this)),
      compositor_task_runner_,
      base::BindOnce(
          &OHOSMediaPlayerRendererClient::OnStreamTextureWrapperInitialized,
          weak_factory_.GetWeakPtr(), media_resource),
      base::BindOnce(&OHOSMediaPlayerRendererClient::OnSurfaceCreated,
                     weak_factory_.GetWeakPtr()),
      base::BindOnce(&OHOSMediaPlayerRendererClient::OnSurfaceDestroyed,
                     weak_factory_.GetWeakPtr()));
}

media::RendererType OHOSMediaPlayerRendererClient::GetRendererType() {
  return media::RendererType::kOHOSMediaPlayer;
}

void OHOSMediaPlayerRendererClient::SetNativeWindowCreatedCallback(
    media::Renderer::NativeWindowCreatedCallback cb) {
  LOG(INFO) << "OHOSMediaPlayerRendererClient::SetNativeWindowCreatedCallback";
  native_window_created_cb_ = std::move(cb);
}

void OHOSMediaPlayerRendererClient::OnFinishPaintCallback() {
}

void OHOSMediaPlayerRendererClient::OnStreamTextureWrapperInitialized(
    media::MediaResource* media_resource, bool success) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  if (!success) {
    std::move(init_cb_).Run(
        media::PipelineStatus::Codes::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  MojoRendererWrapper::Initialize(
      media_resource, client_,
#ifdef OHOS_VIDEO_ASSISTANT
      std::move(request_surface_cb_),
      std::move(decoder_changed_cb_),
#endif // OHOS_VIDEO_ASSISTANT
      base::BindOnce(&OHOSMediaPlayerRendererClient::OnRemoteRendererInitialized,
                     weak_factory_.GetWeakPtr()));
}

void OHOSMediaPlayerRendererClient::OnRemoteRendererInitialized(
    media::PipelineStatus status) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(!init_cb_.is_null());

  if (status == media::PIPELINE_OK) {
    // Signal that we're using MediaPlayer so that we can properly differentiate
    // within our metrics.
    media::PipelineStatistics stats;
    stats.video_pipeline_info = {true, false,
                                 media::VideoDecoderType::kMediaCodec};
    stats.audio_pipeline_info = {true, false,
                                 media::AudioDecoderType::kMediaCodec};
    client_->OnStatisticsUpdate(stats);
  }
  std::move(init_cb_).Run(status);
}

void OHOSMediaPlayerRendererClient::OnSurfaceCreated(int native_window_id) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  LOG(INFO) << "OnSurfaceCreated, native_window_id:" << native_window_id;
  native_window_id_ = native_window_id;
  SetNativeWindowSurface(native_window_id_);

  if (native_window_created_cb_) {
    std::move(native_window_created_cb_).Run(
        base::BindPostTask(media_task_runner_,
            base::BindRepeating(
                &OHOSMediaPlayerRendererClient::OnGetVideoRect,
                weak_factory_.GetWeakPtr())));
  } else {
    OnGetVideoRect(gfx::Rect());
  }
}

void OHOSMediaPlayerRendererClient::OnSurfaceDestroyed() {
  LOG(INFO) << "OnSurfaceDestroyed";
}

void OHOSMediaPlayerRendererClient::OnFrameAvailable() {
  DCHECK(compositor_task_runner_->BelongsToCurrentThread());
  TRACE_EVENT2("base", __FILE__, "func", __func__, "line", __LINE__);

  auto frame = native_texture_wrapper_->GetCurrentFrame();
  auto unique_frame = media::VideoFrame::WrapVideoFrame(
    frame, frame->format(), frame->visible_rect(), frame->natural_size());
  sink_->PaintSingleFrame(std::move(unique_frame));

}

void OHOSMediaPlayerRendererClient::OnGetVideoRect(const gfx::Rect& rect) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  LOG(INFO) << "native_texture_wrapper_->UpdateTextureSize:" << rect.ToString();
  native_texture_wrapper_->UpdateTextureSize(rect.size());

  OnVideoSizeChange(rect.size());
}

void OHOSMediaPlayerRendererClient::OnVideoSizeChange(const gfx::Size& size) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  LOG(INFO) << "OHOSMediaPlayerRendererClient, OnVideoSizeChange size:" << size.ToString();
  native_texture_wrapper_->UpdateTextureSize(size);
  client_->OnVideoNaturalSizeChange(size);
}

void OHOSMediaPlayerRendererClient::OnDurationChange(base::TimeDelta duration) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  media_resource_->ForwardDurationChangeToDemuxerHost(duration);
}

void OHOSMediaPlayerRendererClient::OnFrameUpdate(
    media::mojom::OhosSurfaceBufferHandlePtr ohos_surface_buffer_handle) {
}

void OHOSMediaPlayerRendererClient::PaintNV12VideoFrame(
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    uint8_t* mapped,
    const uint32_t& buffer_size,
    const int& fd,
    const int& fd_browser) {
}

}  // namespace content

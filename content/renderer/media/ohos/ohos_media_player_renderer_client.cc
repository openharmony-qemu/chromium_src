// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/ohos/ohos_media_player_renderer_client.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <utility>

#include <cerrno>
#include "base/logging.h"
#include "graphic_adapter.h"
#include "media/base/video_frame.h"

namespace content {

OHOSMediaPlayerRendererClient::OHOSMediaPlayerRendererClient(
    mojo::PendingRemote<RendererExtention> renderer_extension_remote,
    mojo::PendingReceiver<ClientExtention> client_extension_receiver,
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    std::unique_ptr<media::MojoRenderer> mojo_renderer,
    media::VideoRendererSink* sink)
    : MojoRendererWrapper(std::move(mojo_renderer)),
      client_(nullptr),
      sink_(sink),
      media_task_runner_(std::move(media_task_runner)),
      delayed_bind_client_extension_receiver_(
          std::move(client_extension_receiver)),
      delayed_bind_renderer_extention_remote_(
          std::move(renderer_extension_remote)) {}

OHOSMediaPlayerRendererClient::~OHOSMediaPlayerRendererClient() {
  while (!cached_buffers_.empty()) {
    CachedBuffer& temp_buffer = cached_buffers_.front();
    munmap(temp_buffer.mapped_, temp_buffer.buffer_size_);
    close(temp_buffer.fd_);
    cached_buffers_.pop_front();
  }
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
  MojoRendererWrapper::Initialize(
      media_resource, client_,
#ifdef OHOS_VIDEO_ASSISTANT
      std::move(request_surface_cb),
      std::move(decoder_changed_cb),
#endif // OHOS_VIDEO_ASSISTANT
      base::BindOnce(
          &OHOSMediaPlayerRendererClient::OnRemoteRendererInitialized,
          weak_factory_.GetWeakPtr()));
  sink_->SetFinishPaintCallback(
      base::BindRepeating(&OHOSMediaPlayerRendererClient::OnFinishPaintCallback,
                          weak_factory_.GetWeakPtr()));
}

media::RendererType OHOSMediaPlayerRendererClient::GetRendererType() {
  return media::RendererType::kOHOSMediaPlayer;
}

void OHOSMediaPlayerRendererClient::OnFinishPaintCallback() {
  if (!media_task_runner_->RunsTasksInCurrentSequence()) {
    media_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OHOSMediaPlayerRendererClient::OnFinishPaintCallback,
                       weak_factory_.GetWeakPtr()));
    return;
  }
  if (cached_buffers_.size() > 1) {
    CachedBuffer& temp_buffer = cached_buffers_.front();
    munmap(temp_buffer.mapped_, temp_buffer.buffer_size_);
    close(temp_buffer.fd_);
    renderer_extension_remote_->FinishPaint(temp_buffer.fd_browser_);
    cached_buffers_.pop_front();
  }
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

void OHOSMediaPlayerRendererClient::OnVideoSizeChange(const gfx::Size& size) {
  client_->OnVideoNaturalSizeChange(size);
}

void OHOSMediaPlayerRendererClient::OnDurationChange(base::TimeDelta duration) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  media_resource_->ForwardDurationChangeToDemuxerHost(duration);
}

void OHOSMediaPlayerRendererClient::OnFrameUpdate(
    media::mojom::OhosSurfaceBufferHandlePtr ohos_surface_buffer_handle) {
  if (!media_task_runner_->RunsTasksInCurrentSequence()) {
    media_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSMediaPlayerRendererClient::OnFrameUpdate,
                                  base::Unretained(this),
                                  std::move(ohos_surface_buffer_handle)));
    return;
  }
  auto fd = ohos_surface_buffer_handle->buffer_fd.TakeFD().release();
  uint32_t buffer_size = ohos_surface_buffer_handle->buffer_size;
  int fd_browser = ohos_surface_buffer_handle->fd_browser;
  int32_t coded_width = ohos_surface_buffer_handle->coded_width;
  int32_t coded_height = ohos_surface_buffer_handle->coded_height;
  int32_t visible_width = ohos_surface_buffer_handle->visible_width;
  int32_t visible_height = ohos_surface_buffer_handle->visible_height;
  int32_t src_format = ohos_surface_buffer_handle->format;
  errno = 0;
  uint8_t* mapped =
      (uint8_t*)mmap(NULL, buffer_size, PROT_READ, MAP_SHARED, fd, 0);
  int temp_errno = errno;
  if (temp_errno != 0) {
    LOG(ERROR) << "Render client map failed err:" << temp_errno
               << " coded_width:" << coded_width
               << " coded_height:" << coded_height
               << " visible_width:" << visible_width
               << " visible_height:" << visible_height << " fd:" << fd
               << " mapped:" << (void*)mapped << " format:" << src_format
               << " size:" << buffer_size;
  }
  gfx::Size coded_size = gfx::Size(coded_width, coded_height);
  gfx::Rect visible_rect = gfx::Rect(0, 0, visible_width, visible_height);
  gfx::Size natural_size = gfx::Size(visible_width, visible_height);
  const base::TimeDelta kZero;
  if (src_format == OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_YCBCR_420_SP) {
    PaintNV12VideoFrame(coded_size, visible_rect, natural_size, mapped,
                        buffer_size, fd, fd_browser);
  } else {
    scoped_refptr<media::VideoFrame> dst_frame =
        media::VideoFrame::WrapExternalData(
            media::VideoPixelFormat::PIXEL_FORMAT_ABGR, coded_size,
            visible_rect, natural_size, mapped, buffer_size, kZero);
    sink_->PaintSingleFrame(std::move(dst_frame));
    cached_buffers_.push_back(
        CachedBuffer{fd_browser, fd, mapped, buffer_size});
  }
}

void OHOSMediaPlayerRendererClient::PaintNV12VideoFrame(
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    uint8_t* mapped,
    const uint32_t& buffer_size,
    const int& fd,
    const int& fd_browser) {
  const base::TimeDelta kZero;
  scoped_refptr<media::VideoFrame> src_frame =
      media::VideoFrame::WrapExternalData(
          media::VideoPixelFormat::PIXEL_FORMAT_NV12, coded_size, visible_rect,
          natural_size, mapped, buffer_size, kZero);
  if (src_frame == nullptr) {
    return;
  }
  scoped_refptr<media::VideoFrame> dst_frame = media::VideoFrame::CreateFrame(
      media::VideoPixelFormat::PIXEL_FORMAT_I420, coded_size, visible_rect,
      natural_size, src_frame->timestamp());
  auto convert_status =
      media::ConvertAndScaleFrame(*src_frame, *dst_frame, resize_buf_);
  if (!convert_status.is_ok()) {
    LOG(ERROR) << "Convert failed msg:" << convert_status.message();
    munmap(mapped, buffer_size);
    close(fd);
    renderer_extension_remote_->FinishPaint(fd_browser);
    return;
  }
  sink_->PaintSingleFrame(std::move(dst_frame));
  cached_buffers_.push_back(CachedBuffer{fd_browser, fd, mapped, buffer_size});
}

}  // namespace content

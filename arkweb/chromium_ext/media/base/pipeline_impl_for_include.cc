// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_PIPELINE_IMPL_H_
#error "must be in include form MEDIA_BASE_PIPELINE_IMPL_H_"
#endif

namespace media {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void PipelineImpl::RendererWrapper::SetMediaPlayerState(
    bool is_suspend, int suspend_type) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  if (shared_state_.renderer) {
    shared_state_.renderer->SetMediaPlayerState(is_suspend, suspend_type);
  }
}

void PipelineImpl::SetMediaPlayerState(bool is_suspend,
                                       int suspend_type) {
  DCHECK(thread_checker_.CalledOnValidThread());

  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::SetMediaPlayerState,
                                renderer_wrapper_->AsWeakPtr(),
                                is_suspend, suspend_type));
}

void PipelineImpl::RendererWrapper::SetPlaybackRateWithReason(
    double playback_rate, ActionReason reason) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  playback_rate_ = playback_rate;
  if (state_ == State::kPlaying)
    shared_state_.renderer->SetPlaybackRateWithReason(playback_rate_, reason);

  if (state_ != State::kCreated && state_ != State::kStopping && state_ != State::kStopped) {
    DCHECK(demuxer_);
    demuxer_->SetPlaybackRate(playback_rate);
  }
}

void PipelineImpl::SetPlaybackRateWithReason(
    double playback_rate, ActionReason reason) {
  DVLOG(2) << __func__ << "(" << playback_rate << ")";
  DCHECK(thread_checker_.CalledOnValidThread());

  // Not checking IsRunning() so we can set the playback rate before Start().

  if (playback_rate < 0.0) {
    DVLOG(1) << __func__ << ": Invalid playback rate " << playback_rate;
    return;
  }

  playback_rate_ = playback_rate;
  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::SetPlaybackRateWithReason,
                                renderer_wrapper_->AsWeakPtr(),
                                playback_rate_, reason));
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_PIP)
void PipelineImpl::RendererWrapper::PipEnable(bool enable) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DVLOG(2) << __func__  << "PIC PipelineImpl::RendererWrapper:: enable:" << enable;

  if (shared_state_.renderer) {
    shared_state_.renderer->PipEnable(enable);
  }
}

void PipelineImpl::PipEnable(bool enable) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DVLOG(2) << __func__  << "PIC PipelineImpl::PipEnable enable:" << enable;
  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::PipEnable,
                                renderer_wrapper_->AsWeakPtr(),
                                enable));
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void PipelineImpl::RendererWrapper::SetPreciseSeekTarget(
    int64_t target_timestamp) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  target_timestamp_ = target_timestamp;
  is_set_precise_seek_ = true;
}

void PipelineImpl::SetPreciseSeekTarget(int64_t target_timestamp) {
  DCHECK(thread_checker_.CalledOnValidThread());
  media_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&RendererWrapper::SetPreciseSeekTarget,
                     renderer_wrapper_->AsWeakPtr(), target_timestamp));
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
// LCOV_EXCL_START
void PipelineImpl::RendererWrapper::RecycleDmaBuffer() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  if (shared_state_.renderer) {
    LOG(INFO) << "DMABUF::PipelineImpl::RendererWrapper, RecycleDmaBuffer";
    shared_state_.renderer->RecycleDmaBuffer();
  } else {
    LOG(WARNING) << "DMABUF::PipelineImpl::RecycleDmaBuffer, no video renderer";
  }
}

void PipelineImpl::RecycleDmaBuffer() {
  DCHECK(thread_checker_.CalledOnValidThread());
  LOG(INFO) << "DMABUF::PipelineImpl, RecycleDmaBuffer";

  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::RecycleDmaBuffer,
                                renderer_wrapper_->AsWeakPtr()));
}

void PipelineImpl::RendererWrapper::ResumeDmaBuffer() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  if (shared_state_.renderer) {
    LOG(INFO) << "DMABUF::PipelineImpl::RendererWrapper, ResumeDmaBuffer";
    shared_state_.renderer->ResumeDmaBuffer();
  } else {
    LOG(WARNING) << "DMABUF::PipelineImpl::ResumeDmaBuffer, no video renderer";
  }
}

void PipelineImpl::ResumeDmaBuffer() {
  DCHECK(thread_checker_.CalledOnValidThread());
  LOG(INFO) << "DMABUF::PipelineImpl, ResumeDmaBuffer";

  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::ResumeDmaBuffer,
                                renderer_wrapper_->AsWeakPtr()));
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_MEDIA_DMABUF
}
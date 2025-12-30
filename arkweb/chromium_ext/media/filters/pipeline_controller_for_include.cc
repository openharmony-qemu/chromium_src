
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace media {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void PipelineController::SetMediaPlayerState(bool is_suspend,
                                             int suspend_type) {
  DCHECK(thread_checker_.CalledOnValidThread());

  pipeline_->SetMediaPlayerState(is_suspend, suspend_type);
}

void PipelineController::SetPlaybackRateWithReason(
    double playback_rate, ActionReason reason) {
  pipeline_->SetPlaybackRateWithReason(playback_rate, reason);
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_PIP)
void PipelineController::PipEnable(bool enable) {
  DCHECK(thread_checker_.CalledOnValidThread());
  pipeline_->PipEnable(enable);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void PipelineController::SetPreciseSeekTarget(int64_t target_timestamp) {
  DCHECK(thread_checker_.CalledOnValidThread());
  pipeline_->SetPreciseSeekTarget(target_timestamp);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
// LCOV_EXCL_START
void PipelineController::RecycleDmaBuffer() {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (pipeline_) {
    pipeline_->RecycleDmaBuffer();
  }
}

void PipelineController::ResumeDmaBuffer() {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (pipeline_) {
    pipeline_->ResumeDmaBuffer();
  }
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_MEDIA_DMABUF
}  // namespace media
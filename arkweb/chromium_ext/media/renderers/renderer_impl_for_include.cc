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

#if BUILDFLAG(ARKWEB_PIP)
void RendererImpl::PipEnable(bool enable) {
  DCHECK(task_runner_->RunsTasksInCurrentSequence());

  if (video_renderer_)
    video_renderer_->PipEnable(enable);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void RendererImpl::RecycleDmaBuffer() {
  DCHECK(task_runner_->RunsTasksInCurrentSequence());

  if (video_renderer_) {
    LOG(INFO) << "DMABUF::RendererImpl, RecycleDmaBuffer";
    video_renderer_->RecycleDmaBuffer();
  }
}

void RendererImpl::ResumeDmaBuffer() {
  DCHECK(task_runner_->RunsTasksInCurrentSequence());

  if (video_renderer_) {
    LOG(INFO) << "DMABUF::RendererImpl, ResumeDmaBuffer";
    video_renderer_->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF
}
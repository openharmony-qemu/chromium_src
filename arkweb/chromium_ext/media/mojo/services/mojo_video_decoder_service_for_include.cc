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
void MojoVideoDecoderService::PipEnable(bool enable) {
  if (decoder_) {
    decoder_->PipEnable(enable);
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MojoVideoDecoderService::SetPreciseSeekTarget(int64_t target_timestamp) {
  if (decoder_) {
    decoder_->SetPreciseSeekTarget(target_timestamp);
  }
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void MojoVideoDecoderService::RecycleDmaBuffer() {
  if (decoder_) {
    LOG(INFO) << "DMABUF::MojoVideoDecoderService, RecycleDmaBuffer";
    decoder_->RecycleDmaBuffer();
  }
}

void MojoVideoDecoderService::ResumeDmaBuffer() {
  if (decoder_) {
    LOG(INFO) << "DMABUF::MojoVideoDecoderService, ResumeDmaBuffer";
    decoder_->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF
}
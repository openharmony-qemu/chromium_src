// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

namespace media {

#if BUILDFLAG(ARKWEB_PIP)
void VideoRendererImpl::PipEnable(bool enable) {
  if (video_decoder_stream_) {
    video_decoder_stream_->PipEnable(enable);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void VideoRendererImpl::RecycleDmaBuffer() {
  if (video_decoder_stream_) {
    LOG(INFO) << "DMABUF::VideoRendererImpl, RecycleDmaBuffer";
    video_decoder_stream_->RecycleDmaBuffer();
  }
}

void VideoRendererImpl::ResumeDmaBuffer() {
  if (video_decoder_stream_) {
    LOG(INFO) << "DMABUF::VideoRendererImpl, ResumeDmaBuffer";
    video_decoder_stream_->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF
}
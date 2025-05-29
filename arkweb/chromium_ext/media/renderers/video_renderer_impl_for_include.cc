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

}
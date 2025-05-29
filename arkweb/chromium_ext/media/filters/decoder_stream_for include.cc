// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_DECODER_STREAM_H_
#error "must be in include form MEDIA_FILTERS_DECODER_STREAM_H_"
#endif

namespace media {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
namespace {
template<typename T>
bool SupportVideoSurface(const T& decoder_type) {
  return false;
}
template<>
bool SupportVideoSurface(const VideoDecoderType& decoder_type) {
  return decoder_type == VideoDecoderType::kMediaCodec ||
         decoder_type == VideoDecoderType::kOHOS;
}
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::SetVideoSurface(int surface_id) {
  traits_->SetVideoSurface(decoder_.get(), surface_id);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::PipEnable(bool enable) {
  traits_->PipEnable(decoder_.get(), enable);
}
#endif
}
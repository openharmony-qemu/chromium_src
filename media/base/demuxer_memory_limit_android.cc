// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/demuxer_memory_limit.h"

#if !BUILDFLAG(IS_OHOS)
#include "base/android/build_info.h"
#endif
#include "base/system/sys_info.h"

namespace media {

namespace {

size_t SelectLimit(size_t default_limit,
                   size_t low_limit,
                   size_t very_low_limit) {
  if (!base::SysInfo::IsLowEndDeviceOrPartialLowEndModeEnabled()) {
    return default_limit;
  }
#if !BUILDFLAG(IS_OHOS)
  constexpr int kPhysicalMemoryLow = 512;
  // Use very low limit on 512MiB Android Go devices only.
  if (base::android::BuildInfo::GetInstance()->sdk_int() >=
          base::android::SDK_VERSION_OREO &&
      base::SysInfo::AmountOfPhysicalMemoryMB() <= kPhysicalMemoryLow) {
    return very_low_limit;
  }
#endif
  return low_limit;
}

}  // namespace

size_t GetDemuxerStreamAudioMemoryLimit(
    const AudioDecoderConfig* /*audio_config*/) {
  static const size_t limit =
      SelectLimit(internal::kDemuxerStreamAudioMemoryLimitDefault,
                  internal::kDemuxerStreamAudioMemoryLimitLow,
                  internal::kDemuxerStreamAudioMemoryLimitVeryLow);
  return limit;
}

size_t GetDemuxerStreamVideoMemoryLimit(
    Demuxer::DemuxerTypes /*demuxer_type*/,
    const VideoDecoderConfig* /*video_config*/) {
  static const size_t limit =
      SelectLimit(internal::kDemuxerStreamVideoMemoryLimitDefault,
                  internal::kDemuxerStreamVideoMemoryLimitLow,
                  internal::kDemuxerStreamVideoMemoryLimitVeryLow);
  return limit;
}

size_t GetDemuxerMemoryLimit(Demuxer::DemuxerTypes demuxer_type) {
  return GetDemuxerStreamAudioMemoryLimit(nullptr) +
         GetDemuxerStreamVideoMemoryLimit(demuxer_type, nullptr);
}

}  // namespace media

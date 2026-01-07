// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_

#include <string>

#include <multimedia/player_framework/native_avcapability.h>

#include "base/compiler_specific.h"
#include "media/base/audio_codecs.h"
#include "media/base/sample_format.h"
#include "media/base/supported_video_decoder_config.h"
#include "media/base/video_codecs.h"
#include "media/video/video_encode_accelerator.h"

namespace media {

class OhosMediaCodecUtil {
 public:
  static OH_AVCapability* GetCodecCapability(const std::string& mime,
                                             bool is_encoder);
  /**
   * get AVC/H264 supported profiles and range for current chip by NDK
   */
  static std::optional<SupportedVideoDecoderConfig> GetAVCSupportedConfig();
  /**
   * get HEVC/H265 supported profiles and range for current chip by NDK
   */
  static std::optional<SupportedVideoDecoderConfig> GetHEVCSupportedConfig();
  /**
   * get VVC/H266 supported profiles and range for current chip by NDK
   */
  static std::optional<SupportedVideoDecoderConfig> GetVVCSupportedConfig();

  static std::vector<VideoEncodeAccelerator::SupportedProfile> GetSupportedAVCEncodeProfiles();

  static VideoCodecProfile OhosToCodecAVCProfileType(OH_AVCProfile avc_profile);

  static VideoCodecProfile OhosToCodecHEVCProfileType(OH_HEVCProfile hevc_profile);

  static VideoCodecProfile OhosToCodecVVCProfileType(OH_VVCProfile vvc_profile);

  static bool GetCodecSupportedRange(OH_AVCapability* capability, OH_AVRange* width_range, OH_AVRange* height_range);

  static std::string CodecToOHOSMimeType(AudioCodec codec);
  static std::string CodecToOHOSMimeType(AudioCodec codec,
                                         SampleFormat sample_format);
  static std::string CodecToOHOSMimeType(VideoCodec codec);
  static bool IsPassthroughAudioFormat(AudioCodec codec);
  static bool CanDecode(VideoCodec codec, bool is_secure);
  static bool CanDecode(AudioCodec codec);
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_

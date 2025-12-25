// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_codec_util.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "base/logging.h"

namespace media {

OH_AVCapability* OhosMediaCodecUtil::GetCodecCapability(const std::string& mime,
                                                        bool is_codec) {
  std::vector<char> mimeCStr(mime.size() + 1);
  // since mimeCStr.size is defined by mime.size, so strcpy should not have
  // problem
  strcpy(mimeCStr.data(), mime.c_str());
  return OH_AVCodec_GetCapability(mimeCStr.data(), is_codec);
}

/**
 * get AVC/H264 supported profiles and range for current chip by NDK
 */
std::optional<SupportedVideoDecoderConfig> OhosMediaCodecUtil::GetAVCSupportedConfig() {
  std::optional<SupportedVideoDecoderConfig> avc_supported_video_decoder_config = std::nullopt;

  // check if avc capability is supported
  OH_AVCapability* avc_capability = OhosMediaCodecUtil::GetCodecCapability(OH_AVCODEC_MIMETYPE_VIDEO_AVC, false);
  if (avc_capability == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] AVC/H264 acceleration not supportted";
      return avc_supported_video_decoder_config;
  }

  // get supported range
  OH_AVRange width_range = {-1, -1};
  OH_AVRange height_range = {-1, -1};
  bool range_ret = OhosMediaCodecUtil::GetCodecSupportedRange(avc_capability, &width_range, &height_range);
  if (!range_ret) {
      LOG(ERROR) << __func__ << " [VideoDecoder] AVC/H264 failed to get supported range";
      return avc_supported_video_decoder_config;
  }

  // get supported profile and convert to chromium video decoder profile
  const int32_t* avc_profiles = nullptr;
  uint32_t avc_profile_num = 0;
  int32_t ret = OH_AVCapability_GetSupportedProfiles(avc_capability, &avc_profiles, &avc_profile_num);
  if (avc_profile_num == 0 || ret != AV_ERR_OK || avc_profiles == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] AVC/H264 acceleration not supportted";
      return avc_supported_video_decoder_config;
  }

  OH_AVCProfile oh_max_avc_profile = AVC_PROFILE_BASELINE;
  for (uint32_t i = 0; i < avc_profile_num; i++) {
      OH_AVCProfile tmp = static_cast<OH_AVCProfile>(avc_profiles[i]);
      // AVC_PROFILE_HIGH is the best AVC quality but it's int value is not the largest in native_avcodec_base enums
      // OH_AVCProfile
      if (tmp == AVC_PROFILE_HIGH) {
          oh_max_avc_profile = tmp;
          break;
      }
      if (tmp > oh_max_avc_profile) {
          oh_max_avc_profile = tmp;
      }
  }
  VideoCodecProfile avc_max_profile = OhosMediaCodecUtil::OhosToCodecAVCProfileType(oh_max_avc_profile);

  avc_supported_video_decoder_config = SupportedVideoDecoderConfig(H264PROFILE_MIN,
      avc_max_profile,
      gfx::Size(width_range.minVal, height_range.minVal),
      gfx::Size(width_range.maxVal, height_range.maxVal),
      true, // avcodec support h264 and hevc decryption
      false);
  return avc_supported_video_decoder_config;
}

/**
 * get HEVC/H265 supported profiles and range for current chip by NDK
 */
std::optional<SupportedVideoDecoderConfig> OhosMediaCodecUtil::GetHEVCSupportedConfig() {
  std::optional<SupportedVideoDecoderConfig> hevc_supported_video_decoder_config = std::nullopt;

  // check if hevc capability is supported
  OH_AVCapability* hevc_capability = OhosMediaCodecUtil::GetCodecCapability(OH_AVCODEC_MIMETYPE_VIDEO_HEVC, false);
  if (hevc_capability == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] HEVC/H265 acceleration not supportted";
      return hevc_supported_video_decoder_config;
  }

  // get supported range
  OH_AVRange width_range = {-1, -1};
  OH_AVRange height_range = {-1, -1};
  bool range_ret = OhosMediaCodecUtil::GetCodecSupportedRange(hevc_capability, &width_range, &height_range);
  if (!range_ret) {
      LOG(ERROR) << __func__ << " [VideoDecoder] HEVC/H265 failed to get supported range";
      return hevc_supported_video_decoder_config;
  }

  // get supported profile and convert to chromium video decoder profile
  const int32_t* hevc_profiles = nullptr;
  uint32_t hevc_profile_num = 0;
  int32_t ret = OH_AVCapability_GetSupportedProfiles(hevc_capability, &hevc_profiles, &hevc_profile_num);
  if (hevc_profile_num == 0 || ret != AV_ERR_OK || hevc_profiles == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] HEVC/H265 acceleration not supportted";
      return hevc_supported_video_decoder_config;
  }

  OH_HEVCProfile oh_max_hevc_profile = HEVC_PROFILE_MAIN;
  for (uint32_t i = 0; i < hevc_profile_num; i++) {
      OH_HEVCProfile tmp = static_cast<OH_HEVCProfile>(hevc_profiles[i]);
      if (tmp > oh_max_hevc_profile) {
          oh_max_hevc_profile = tmp;
      }
  }
  VideoCodecProfile hevc_max_profile = OhosMediaCodecUtil::OhosToCodecHEVCProfileType(oh_max_hevc_profile);

  // compose SupportedVideoDecoderConfig
  hevc_supported_video_decoder_config = SupportedVideoDecoderConfig(HEVCPROFILE_MIN,
      hevc_max_profile,
      gfx::Size(width_range.minVal, height_range.minVal),
      gfx::Size(width_range.maxVal, height_range.maxVal),
      true, // avcodec support h264 and hevc decryption
      false);
  return hevc_supported_video_decoder_config;
}

/**
 * get VVC/H266 supported profiles and range for current chip by NDK
 */
std::optional<SupportedVideoDecoderConfig> OhosMediaCodecUtil::GetVVCSupportedConfig() {
  std::optional<SupportedVideoDecoderConfig> vvc_supported_video_decoder_config = std::nullopt;

  // check if vvc/h266 capability is supported
  OH_AVCapability* vvc_capability = OhosMediaCodecUtil::GetCodecCapability(OH_AVCODEC_MIMETYPE_VIDEO_VVC, false);
  if (vvc_capability == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] VVC/H266 acceleration not supportted";
      return vvc_supported_video_decoder_config;
  }

  // get supported range
  OH_AVRange width_range = {-1, -1};
  OH_AVRange height_range = {-1, -1};
  bool range_ret = OhosMediaCodecUtil::GetCodecSupportedRange(vvc_capability, &width_range, &height_range);
  if (!range_ret) {
      LOG(ERROR) << __func__ << " [VideoDecoder] VVC/H266 failed to get supported range";
      return vvc_supported_video_decoder_config;
  }

  // get supported profile and convert to chromium video decoder profile
  const int32_t* vvc_profiles = nullptr;
  uint32_t vvc_profile_num = 0;
  int32_t ret = OH_AVCapability_GetSupportedProfiles(vvc_capability, &vvc_profiles, &vvc_profile_num);
  if (vvc_profile_num == 0 || ret != AV_ERR_OK || vvc_profiles == nullptr) {
      LOG(WARNING) << __func__ << " [VideoDecoder] VVC/H266 acceleration not supportted";
      return vvc_supported_video_decoder_config;
  }

  OH_VVCProfile oh_max_vvc_profile = VVC_PROFILE_MAIN_10;
  for (uint32_t i = 0; i < vvc_profile_num; i++) {
      OH_VVCProfile tmp = static_cast<OH_VVCProfile>(vvc_profiles[i]);
      if (tmp > oh_max_vvc_profile) {
          oh_max_vvc_profile = tmp;
      }
  }
  VideoCodecProfile vvc_max_profile = OhosMediaCodecUtil::OhosToCodecVVCProfileType(oh_max_vvc_profile);

  // compose SupportedVideoDecoderConfig
  vvc_supported_video_decoder_config = SupportedVideoDecoderConfig(VVCPROFILE_MIN,
      vvc_max_profile,
      gfx::Size(width_range.minVal, height_range.minVal),
      gfx::Size(width_range.maxVal, height_range.maxVal),
      false, // avcodec not support vvc decryption
      false);
  return vvc_supported_video_decoder_config;
}

VideoCodecProfile OhosMediaCodecUtil::OhosToCodecAVCProfileType(OH_AVCProfile avc_profile) {
  switch (avc_profile) {
      case AVC_PROFILE_BASELINE:
          return H264PROFILE_BASELINE;
      case AVC_PROFILE_HIGH:
          return H264PROFILE_HIGH;
      case AVC_PROFILE_MAIN:
          return H264PROFILE_MAIN;
      default:
          LOG(WARNING) << __func__ << " [VideoDecoder] AVC/H264 use default H264PROFILE_BASELINE  for unknown profile: "
                       << static_cast<int>(avc_profile);
          return H264PROFILE_BASELINE;
  }
}

VideoCodecProfile OhosMediaCodecUtil::OhosToCodecHEVCProfileType(OH_HEVCProfile hevc_profile) {
  switch (hevc_profile) {
      case HEVC_PROFILE_MAIN:
          return HEVCPROFILE_MAIN;
      case HEVC_PROFILE_MAIN_10:
          return HEVCPROFILE_MAIN10;
      case HEVC_PROFILE_MAIN_STILL:
          return HEVCPROFILE_MAX;
      case HEVC_PROFILE_MAIN_10_HDR10:
          return HEVCPROFILE_MAIN10;
      case HEVC_PROFILE_MAIN_10_HDR10_PLUS:
          return HEVCPROFILE_MAIN10;
      default:
          LOG(WARNING) << __func__
                       << " [VideoDecoder] HEVC/H265 use default profile: HEVC_PROFILE_MAIN for unknown profile: "
                       << static_cast<int>(hevc_profile);
          return HEVCPROFILE_MAIN;
  }
}

VideoCodecProfile OhosMediaCodecUtil::OhosToCodecVVCProfileType(OH_VVCProfile vvc_profile) {
  switch (vvc_profile) {
      case VVC_PROFILE_MAIN_10:
          return VVCPROFILE_MAIN10;
      case VVC_PROFILE_MAIN_12:
          return VVCPROFILE_MAIN12;
      case VVC_PROFILE_MAIN_12_INTRA:
          return VVCPROFILE_MAIN12_INTRA;
      case VVC_PROFILE_MAIN_10_444:
          return VVCPROFILE_MAIN10_444;
      case VVC_PROFILE_MAIN_12_444:
          return VVCPROFILE_MAIN12_444;
      case VVC_PROFILE_MAIN_16_444:
          return VVCPROFILE_MAIN16_444;
      case VVC_PROFILE_MAIN_12_444_INTRA:
          return VVCPROFILE_MAIN12_444_INTRA;
      case VVC_PROFILE_MAIN_16_444_INTRA:
          return VVCPROFILE_MAIN16_444_INTRA;
      case VVC_PROFILE_MULTI_MAIN_10_444:
          return VVCPROFILE_MULTILAYER_MAIN10_444;
      case VVC_PROFILE_MAIN_10_STILL:
          return VVCPROFILE_MAIN10_STILL_PICTURE;
      case VVC_PROFILE_MAIN_12_STILL:
          return VVCPROFILE_MAIN12_STILL_PICTURE;
      case VVC_PROFILE_MAIN_10_444_STILL:
          return VVCPROFILE_MAIN10_444_STILL_PICTURE;
      case VVC_PROFILE_MAIN_12_444_STILL:
          return VVCPROFILE_MAIN12_444_STILL_PICTURE;
      case VVC_PROFILE_MAIN_16_444_STILL:
          return VVCPROFILE_MAIN16_444_STILL_PICTURE;
      default:
          LOG(WARNING) << __func__
                       << " [VideoDecoder] VVC/H266 use default profile: VVCPROFILE_MIN for unknown profile: "
                       << static_cast<int>(vvc_profile);
          return VVCPROFILE_MAIN10;
  }
}

bool OhosMediaCodecUtil::GetCodecSupportedRange(
    OH_AVCapability *capability, OH_AVRange *width_range, OH_AVRange *height_range) {
  int32_t width_ret = OH_AVCapability_GetVideoWidthRange(capability, width_range);
  int32_t height_ret = OH_AVCapability_GetVideoHeightRange(capability, height_range);
  if (width_ret != AV_ERR_OK || height_ret != AV_ERR_OK || width_range->minVal == -1 || height_range->minVal == -1 ||
      width_range->maxVal == -1 || height_range->maxVal == -1) {
      return false;
  }
  return true;
}

namespace {
const char K_MP3_MIME_TYPE[] = "audio/mpeg";
const char K_AAC_MIME_TYPE[] = "audio/mp4a-latm";
const char K_OPUS_MIME_TYPE[] = "audio/opus";
const char K_VORBIS_MIME_TYPE[] = "audio/vorbis";
const char K_FLAC_MIME_TYPE[] = "audio/flac";
const char K_AC3_MIME_TYPE[] = "audio/ac3";
const char K_EAC3_MIME_TYPE[] = "audio/eac3";
const char K_BITSTREAM_AUDIO_MIME_TYPE[] = "audio/raw";
const char K_AVC_MIME_TYPE[] = "video/avc";
const char K_DOLBY_VISION_MIME_TYPE[] = "video/dolby-vision";
const char K_HEVC_MIME_TYPE[] = "video/hevc";
const char K_VP8_MIME_TYPE[] = "video/x-vnd.on2.vp8";
const char K_VP9_MIME_TYPE[] = "video/x-vnd.on2.vp9";
const char K_AV1_MIME_TYPE[] = "video/av01";
const char K_DTS_MIME_TYPE[] = "audio/vnd.dts";
const char K_DTSE_MIME_TYPE[] = "audio/vnd.dts;profile=lbr";
const char K_DTSX_P2_MIME_TYPE[] = "audio/vnd.dts.uhd;profile=p2";
}  // namespace

// static
std::string OhosMediaCodecUtil::CodecToOHOSMimeType(AudioCodec codec) {
  return CodecToOHOSMimeType(codec, kUnknownSampleFormat);
}

// static
std::string OhosMediaCodecUtil::CodecToOHOSMimeType(
    AudioCodec codec,
    SampleFormat sample_format) {
  // Passthrough is possible for some bitstream formats.
  const bool isPassthrough = sample_format == kSampleFormatDts ||
                             sample_format == kSampleFormatDtsxP2 ||
                             sample_format == kSampleFormatAc3 ||
                             sample_format == kSampleFormatEac3 ||
                             sample_format == kSampleFormatMpegHAudio;

  if (IsPassthroughAudioFormat(codec) || isPassthrough) {
    return K_BITSTREAM_AUDIO_MIME_TYPE;
  }

  switch (codec) {
    case AudioCodec::kMP3:
      return K_MP3_MIME_TYPE;
    case AudioCodec::kVorbis:
      return K_VORBIS_MIME_TYPE;
    case AudioCodec::kFLAC:
      return K_FLAC_MIME_TYPE;
    case AudioCodec::kOpus:
      return K_OPUS_MIME_TYPE;
    case AudioCodec::kAAC:
      return K_AAC_MIME_TYPE;
    case AudioCodec::kAC3:
      return K_AC3_MIME_TYPE;
    case AudioCodec::kEAC3:
      return K_EAC3_MIME_TYPE;
    case AudioCodec::kDTS:
      return K_DTS_MIME_TYPE;
    case AudioCodec::kDTSE:
      return K_DTSE_MIME_TYPE;
    case AudioCodec::kDTSXP2:
      return K_DTSX_P2_MIME_TYPE;
    default:
      LOG(ERROR) << __func__ << " [AudioDecoder] not supported audio mime type: "
                 << static_cast<int>(codec);
      return std::string();
  }
}

// static
std::string OhosMediaCodecUtil::CodecToOHOSMimeType(VideoCodec codec) {
  switch (codec) {
    case VideoCodec::kH264:
      return K_AVC_MIME_TYPE;
    case VideoCodec::kHEVC:
      return K_HEVC_MIME_TYPE;
    case VideoCodec::kVP8:
      return K_VP8_MIME_TYPE;
    case VideoCodec::kVP9:
      return K_VP9_MIME_TYPE;
    case VideoCodec::kDolbyVision:
      return K_DOLBY_VISION_MIME_TYPE;
    case VideoCodec::kAV1:
      return K_AV1_MIME_TYPE;
    default:
      LOG(ERROR) << __func__ << " [VideoDecoder] not supported video mime type: "
                 << static_cast<int>(codec);
      return std::string();
  }
}

// static
bool OhosMediaCodecUtil::IsPassthroughAudioFormat(AudioCodec codec) {
  switch (codec) {
    case AudioCodec::kAC3:
    case AudioCodec::kEAC3:
    case AudioCodec::kDTS:
    case AudioCodec::kDTSXP2:
    case AudioCodec::kMpegHAudio:
      return true;
    default:
      LOG(ERROR) << __func__ << " [AudioDecoder] not supported audio format: "
                 << static_cast<int>(codec);
      return false;
  }
}

static bool CanDecodeInternal(const std::string& mime, bool is_secure) {
  if (mime.empty()) {
    return false;
  }

  OH_AVCapability* capability =
      OH_AVCodec_GetCapability(mime.c_str(), is_secure);
  if (!capability) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] can not decode: " << mime
                 << ", is_secure: " << is_secure;
    return false;
  }
  LOG(INFO) << __FUNCTION__ << " [WiseplayDRM] can decode: " << mime
            << ", is_secure: " << is_secure;
  return true;
}

// static
bool OhosMediaCodecUtil::CanDecode(VideoCodec codec, bool is_secure) {
  std::string mime = CodecToOHOSMimeType(codec);
  return CanDecodeInternal(mime, is_secure);
}

// static
bool OhosMediaCodecUtil::CanDecode(AudioCodec codec) {
  std::string mime = CodecToOHOSMimeType(codec);
  return CanDecodeInternal(mime, false);
}

}  // namespace media

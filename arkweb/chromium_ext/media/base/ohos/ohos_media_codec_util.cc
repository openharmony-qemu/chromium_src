// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_codec_util.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "base/logging.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace media {
// static
CapabilityData OHOSMediaCodecUtil::GetCodecCapability(const std::string& mime,
                                                      bool isCodec) {
  std::shared_ptr<CapabilityDataAdapter> data =
      OhosAdapterHelper::GetInstance()
          .GetMediaCodecListAdapter()
          .GetCodecCapability(mime, isCodec);

  CapabilityData result;

  if (!data) {
    return result;
  }

  result.maxWidth = data->GetMaxWidth();
  result.maxHeight = data->GetMaxHeight();
  result.maxframeRate = data->GetMaxframeRate();
  return result;
}

namespace {
const char kMp3MimeType[] = "audio/mpeg";
const char kAacMimeType[] = "audio/mp4a-latm";
const char kOpusMimeType[] = "audio/opus";
const char kVorbisMimeType[] = "audio/vorbis";
const char kFLACMimeType[] = "audio/flac";
const char kAc3MimeType[] = "audio/ac3";
const char kEac3MimeType[] = "audio/eac3";
const char kBitstreamAudioMimeType[] = "audio/raw";
const char kAvcMimeType[] = "video/avc";
const char kDolbyVisionMimeType[] = "video/dolby-vision";
const char kHevcMimeType[] = "video/hevc";
const char kVp8MimeType[] = "video/x-vnd.on2.vp8";
const char kVp9MimeType[] = "video/x-vnd.on2.vp9";
const char kAv1MimeType[] = "video/av01";
const char kDtsMimeType[] = "audio/vnd.dts";
const char kDtseMimeType[] = "audio/vnd.dts;profile=lbr";
const char kDtsxP2MimeType[] = "audio/vnd.dts.uhd;profile=p2";
}  // namespace

// static
std::string OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec codec) {
  return CodecToOHOSMimeType(codec, kUnknownSampleFormat);
}

// static
std::string OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec codec,
    SampleFormat sample_format) {
  // Passthrough is possible for some bitstream formats.
  const bool is_passthrough = sample_format == kSampleFormatDts ||
                              sample_format == kSampleFormatDtsxP2 ||
                              sample_format == kSampleFormatAc3 ||
                              sample_format == kSampleFormatEac3 ||
                              sample_format == kSampleFormatMpegHAudio;

  if (IsPassthroughAudioFormat(codec) || is_passthrough) {
    return kBitstreamAudioMimeType;
  }

  switch (codec) {
    case AudioCodec::kMP3:
      return kMp3MimeType;
    case AudioCodec::kVorbis:
      return kVorbisMimeType;
    case AudioCodec::kFLAC:
      return kFLACMimeType;
    case AudioCodec::kOpus:
      return kOpusMimeType;
    case AudioCodec::kAAC:
      return kAacMimeType;
    case AudioCodec::kAC3:
      return kAc3MimeType;
    case AudioCodec::kEAC3:
      return kEac3MimeType;
    case AudioCodec::kDTS:
      return kDtsMimeType;
    case AudioCodec::kDTSE:
      return kDtseMimeType;
    case AudioCodec::kDTSXP2:
      return kDtsxP2MimeType;
    default:
      return std::string();
  }
}

// static
std::string OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec codec) {
  switch (codec) {
    case VideoCodec::kH264:
      return kAvcMimeType;
    case VideoCodec::kHEVC:
      return kHevcMimeType;
    case VideoCodec::kVP8:
      return kVp8MimeType;
    case VideoCodec::kVP9:
      return kVp9MimeType;
    case VideoCodec::kDolbyVision:
      return kDolbyVisionMimeType;
    case VideoCodec::kAV1:
      return kAv1MimeType;
    default:
      return std::string();
  }
}

// static
bool OHOSMediaCodecUtil::IsPassthroughAudioFormat(AudioCodec codec) {
  switch (codec) {
    case AudioCodec::kAC3:
    case AudioCodec::kEAC3:
    case AudioCodec::kDTS:
    case AudioCodec::kDTSXP2:
    case AudioCodec::kMpegHAudio:
      return true;
    default:
      return false;
  }
}

static bool CanDecodeInternal(const std::string& mime, bool is_secure) {
  if (mime.empty()) {
    return false;
  }

  std::shared_ptr<CapabilityDataAdapter> data =
      OhosAdapterHelper::GetInstance()
          .GetMediaCodecListAdapter()
          .GetCodecCapability(mime, false);
  if (!data) {
    return false;
  }
  return true;
}

// static
bool OHOSMediaCodecUtil::CanDecode(VideoCodec codec, bool is_secure) {
  std::string mime = CodecToOHOSMimeType(codec);
  return CanDecodeInternal(mime, is_secure);
}

// static
bool OHOSMediaCodecUtil::CanDecode(AudioCodec codec) {
  std::string mime = CodecToOHOSMimeType(codec);
  return CanDecodeInternal(mime, false);
}
}  // namespace media

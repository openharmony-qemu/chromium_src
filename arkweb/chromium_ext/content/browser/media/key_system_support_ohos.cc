// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/key_system_support_ohos.h"

#include <utility>
#include <vector>

#include "base/feature_list.h"
#include "media/base/audio_codecs.h"
#include "media/base/content_decryption_module.h"
#include "media/base/eme_constants.h"
#include "media/base/encryption_scheme.h"
#include "media/base/media_switches.h"
#include "media/base/media_util.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/base/video_codecs.h"

using media::OHOSMediaCodecUtil;
using media::OHOSMediaDrmBridge;

namespace content {
namespace {

const media::AudioCodec kWebMAudioCodecsToQuery[] = {
    media::AudioCodec::kOpus,
};

const media::AudioCodec kMP4AudioCodecsToQuery[] = {
    media::AudioCodec::kFLAC,
#if BUILDFLAG(USE_PROPRIETARY_CODECS)
    media::AudioCodec::kAAC,
#if BUILDFLAG(ENABLE_PLATFORM_AC3_EAC3_AUDIO)
    media::AudioCodec::kAC3,  media::AudioCodec::kEAC3,
#endif
#if BUILDFLAG(ENABLE_PLATFORM_DTS_AUDIO)
    media::AudioCodec::kDTS,  media::AudioCodec::kDTSXP2,
#endif
#endif  // BUILDFLAG(USE_PROPRIETARY_CODECS)
};

const media::VideoCodec kWebMVideoCodecsToQuery[] = {
    media::VideoCodec::kVP8,
    media::VideoCodec::kVP9,
};

const media::VideoCodec kMP4VideoCodecsToQuery[] = {
    media::VideoCodec::kVP9,
#if BUILDFLAG(USE_PROPRIETARY_CODECS)
    media::VideoCodec::kH264,
#if BUILDFLAG(ENABLE_PLATFORM_HEVC)
    media::VideoCodec::kHEVC,
#endif
#if BUILDFLAG(ENABLE_PLATFORM_DOLBY_VISION)
    media::VideoCodec::kDolbyVision,
#endif
#endif  // BUILDFLAG(USE_PROPRIETARY_CODECS)
};
}  // namespace

void GetOHOSCdmCapability(const std::string& key_system,
                          CdmInfo::Robustness robustness,
                          media::CdmCapabilityCB cdm_capability_cb) {
  const bool is_secure = robustness == CdmInfo::Robustness::kHardwareSecure;
  if (!OHOSMediaDrmBridge::IsKeySystemSupported(key_system)) {
    LOG(INFO) << "[DRM] Key system " << key_system << " not supported.";
    std::move(cdm_capability_cb).Run(std::nullopt);
    return;
  }
  const std::vector<media::VideoCodecProfile> kAllProfiles = {};
  media::CdmCapability capability;
  if (OHOSMediaDrmBridge::IsKeySystemSupportedWithType(key_system,
                                                       "video/webm")) {
    for (const auto& codec : kWebMAudioCodecsToQuery) {
      if (OHOSMediaCodecUtil::CanDecode(codec)) {
        capability.audio_codecs.insert(codec);
      }
    }
    for (const auto& codec : kWebMVideoCodecsToQuery) {
      if (OHOSMediaCodecUtil::CanDecode(codec, is_secure)) {
        capability.video_codecs.emplace(codec, kAllProfiles);
      }
    }
  }
  if (OHOSMediaDrmBridge::IsKeySystemSupportedWithType(key_system,
                                                       "video/mp4")) {
    for (const auto& codec : kMP4AudioCodecsToQuery) {
      if ((!capability.audio_codecs.contains(codec)) &&
          OHOSMediaCodecUtil::CanDecode(codec)) {
        capability.audio_codecs.insert(codec);
      }
    }
    for (const auto& codec : kMP4VideoCodecsToQuery) {
      if ((!capability.video_codecs.contains(codec)) &&
          OHOSMediaCodecUtil::CanDecode(codec, is_secure)) {
        capability.video_codecs.emplace(codec, kAllProfiles);
      }
    }
  }
  if (is_secure && capability.video_codecs.empty()) {
    std::move(cdm_capability_cb).Run(std::nullopt);
    return;
  }
  capability.encryption_schemes.insert(media::EncryptionScheme::kCenc);
  capability.encryption_schemes.insert(media::EncryptionScheme::kCbcs);
  capability.session_types.insert(media::CdmSessionType::kTemporary);
  if (OHOSMediaDrmBridge::IsPersistentLicenseTypeSupported(key_system)) {
    capability.session_types.insert(media::CdmSessionType::kPersistentLicense);
  }
  std::move(cdm_capability_cb).Run(capability);
}
}  // namespace content

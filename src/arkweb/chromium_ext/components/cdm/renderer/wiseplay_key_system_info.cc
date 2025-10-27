// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/cdm/renderer/wiseplay_key_system_info.h"

#include "base/command_line.h"
#include "base/feature_list.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "media/base/media_switches.h"
#include "media/cdm/wiseplay_cdm_common.h"
#include "media/media_buildflags.h"
#include "base/logging.h"

#if !BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
#error This file should only be built when Wiseplay is enabled.
#endif

using media::CdmSessionType;
using media::EmeConfig;
using media::EmeConfigRuleState;
using media::EmeFeatureSupport;
using media::EmeInitDataType;
using media::EmeMediaType;
using media::EncryptionScheme;
using media::SupportedCodecs;
using Robustness = cdm::WiseplayKeySystemInfo::Robustness;

namespace cdm {
namespace {
Robustness ConvertRobustness(const std::string& robustness) {
  if (robustness.empty()) {
    return Robustness::EMPTY;
  }
  Robustness wiseplay_robustness = Robustness::INVALID;
  const static std::map<std::string, Robustness> to_robustness_map = {
    {"SW_SECURE_CRYPTO", Robustness::SW_SECURE_CRYPTO},
    {"SW_SECURE_DECODE", Robustness::SW_SECURE_DECODE},
    {"HW_SECURE_CRYPTO", Robustness::HW_SECURE_CRYPTO},
    {"HW_SECURE_DECODE", Robustness::HW_SECURE_DECODE},
    {"HW_SECURE_ALL", Robustness::HW_SECURE_ALL}
  };
  auto iter = to_robustness_map.find(robustness);
  if (iter != to_robustness_map.end()) {
    wiseplay_robustness = iter->second;
  }
  return wiseplay_robustness;
}
}  // namespace

WiseplayKeySystemInfo::WiseplayKeySystemInfo(
    SupportedCodecs codecs,
    base::flat_set<EncryptionScheme> encryption_schemes,
    base::flat_set<CdmSessionType> session_types,
    SupportedCodecs hw_secure_codecs,
    base::flat_set<EncryptionScheme> hw_secure_encryption_schemes,
    base::flat_set<CdmSessionType> hw_secure_session_types,
    Robustness max_audio_robustness,
    Robustness max_video_robustness,
    EmeFeatureSupport persistent_state_support,
    EmeFeatureSupport distinctive_identifier_support)
    : codecs_(codecs),
      encryption_schemes_(std::move(encryption_schemes)),
      session_types_(std::move(session_types)),
      hw_secure_codecs_(hw_secure_codecs),
      hw_secure_encryption_schemes_(std::move(hw_secure_encryption_schemes)),
      hw_secure_session_types_(std::move(hw_secure_session_types)),
      max_audio_robustness_(max_audio_robustness),
      max_video_robustness_(max_video_robustness),
      persistent_state_support_(persistent_state_support),
      distinctive_identifier_support_(distinctive_identifier_support) {}

WiseplayKeySystemInfo::~WiseplayKeySystemInfo() = default;

std::string WiseplayKeySystemInfo::GetBaseKeySystemName() const {
  return media::kWiseplayKeySystem;
}

bool WiseplayKeySystemInfo::IsSupportedKeySystem(
    const std::string& key_system) const {
  LOG(INFO) << "[DRM]" << __func__;
  return key_system == media::kWiseplayKeySystem;
}

bool WiseplayKeySystemInfo::ShouldUseBaseKeySystemName() const {
  // Internally Wiseplay CDM only supports kWiseplayKeySystem.
  return true;
}

bool WiseplayKeySystemInfo::IsSupportedInitDataType(
    EmeInitDataType init_data_type) const {
  // Here we assume that support for a container implies support for the
  // associated initialization data type. KeySystems handles validating
  // |init_data_type| x |container| pairings.
  if (init_data_type == EmeInitDataType::WEBM)
    return (codecs_ & media::EME_CODEC_WEBM_ALL) != 0;
  if (init_data_type == EmeInitDataType::CENC)
    return (codecs_ & media::EME_CODEC_MP4_ALL) != 0;

  return false;
}

EmeConfig::Rule WiseplayKeySystemInfo::GetEncryptionSchemeConfigRule(
    EncryptionScheme encryption_scheme) const {
  bool is_supported = encryption_schemes_.contains(encryption_scheme);
  bool is_hw_secure_supported =
      hw_secure_encryption_schemes_.contains(encryption_scheme);
  if (is_supported && is_hw_secure_supported) {
    return EmeConfig::SupportedRule();
  } else if (is_supported && !is_hw_secure_supported) {
    return EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kNotAllowed};
  } else if (!is_supported && is_hw_secure_supported) {
    return EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired};
  } else {
    return media::EmeConfig::UnsupportedRule();
  }
}

SupportedCodecs WiseplayKeySystemInfo::GetSupportedCodecs() const {
  return codecs_;
}

SupportedCodecs WiseplayKeySystemInfo::GetSupportedHwSecureCodecs() const {
  return hw_secure_codecs_;
}

EmeConfig::Rule WiseplayKeySystemInfo::GetRobustnessConfigRule(
    const std::string& key_system,
    EmeMediaType media_type,
    const std::string& requested_robustness,
    const bool* hw_secure_requirement) const {
  LOG(INFO) << "[DRM]" << __func__ << ", " << requested_robustness;
  Robustness robustness = ConvertRobustness(requested_robustness);
  if (robustness == Robustness::INVALID) {
    LOG(INFO) << "[DRM]" << __func__ << ", robustness is invalid.";
    return EmeConfig::UnsupportedRule();
  }
  Robustness max_robustness = Robustness::INVALID;
  switch (media_type) {
    case EmeMediaType::AUDIO:
      max_robustness = max_audio_robustness_;
      break;
    case EmeMediaType::VIDEO:
      max_robustness = max_video_robustness_;
      break;
  }

  // We can compare robustness levels whenever they are not HW_SECURE_CRYPTO
  // and SW_SECURE_DECODE in some order. If they are exactly those two then the
  // robustness requirement is not supported.
  if ((max_robustness == Robustness::HW_SECURE_CRYPTO &&
       robustness == Robustness::SW_SECURE_DECODE) ||
      (max_robustness == Robustness::SW_SECURE_DECODE &&
       robustness == Robustness::HW_SECURE_CRYPTO) ||
      robustness > max_robustness) {
    LOG(INFO) << "[DRM]" << __func__ << ", UnsupportedRule.";
    return media::EmeConfig::UnsupportedRule();
  }

  [[maybe_unused]] bool hw_secure_codecs_required =
      hw_secure_requirement && *hw_secure_requirement;

  if (robustness >= Robustness::HW_SECURE_CRYPTO) {
    LOG(INFO) << "[DRM]" << __func__ << " require hw secure codecs.";
    return EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired};
  }
  LOG(INFO) << "[DRM]" << __func__ << ", SupportedRule End.";
  return media::EmeConfig::SupportedRule();
}

EmeConfig::Rule WiseplayKeySystemInfo::GetPersistentLicenseSessionSupport()
    const {
  bool is_supported =
      session_types_.contains(CdmSessionType::kPersistentLicense);

  bool is_hw_secure_supported =
      hw_secure_session_types_.contains(CdmSessionType::kPersistentLicense);
  LOG(INFO) << "[DRM]" << __func__ << ", is_supported: " << is_supported
            << ", is_hw_secure_supported: " << is_hw_secure_supported;
  if (is_supported && is_hw_secure_supported) {
    return EmeConfig::SupportedRule();
  } else if (is_supported && !is_hw_secure_supported) {
    return EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kNotAllowed};
  } else if (!is_supported && is_hw_secure_supported) {
    return EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired};
  } else {
    return media::EmeConfig::UnsupportedRule();
  }
}

EmeFeatureSupport WiseplayKeySystemInfo::GetPersistentStateSupport() const {
  return persistent_state_support_;
}

EmeFeatureSupport WiseplayKeySystemInfo::GetDistinctiveIdentifierSupport()
    const {
  return distinctive_identifier_support_;
}
}  // namespace cdm

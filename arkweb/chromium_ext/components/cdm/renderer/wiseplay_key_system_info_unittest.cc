// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/cdm/renderer/wiseplay_key_system_info.h"

#include "base/test/scoped_feature_list.h"
#include "media/base/key_system_info.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/chromium_ext/media/cdm/wiseplay_cdm_common.h"

namespace cdm {
using media::CdmSessionType;
using media::EmeConfig;
using media::EmeConfigRuleState;
using media::EmeFeatureSupport;
using media::EmeInitDataType;
using media::EmeMediaType;
using media::EncryptionScheme;
using media::SupportedCodecs;

constexpr SupportedCodecs kTestCodecs = media::EME_CODEC_WEBM_ALL | media::EME_CODEC_MP4_ALL;
constexpr SupportedCodecs kTestHwSecureCodecs = media::EME_CODEC_MP4_ALL;
constexpr SupportedCodecs kEmptyCodecs = media::EME_CODEC_NONE;

const base::flat_set<EncryptionScheme> kTestEncryptionSchemes = {
    EncryptionScheme::kCenc, EncryptionScheme::kCbcs};
const base::flat_set<EncryptionScheme> kTestHwSecureEncryptionSchemes = {
    EncryptionScheme::kCenc};
const base::flat_set<EncryptionScheme> kEmptyEncryptionSchemes = {};

const base::flat_set<CdmSessionType> kTestSessionTypes = {
    CdmSessionType::kTemporary, CdmSessionType::kPersistentLicense};
const base::flat_set<CdmSessionType> kTestHwSecureSessionTypes = {
    CdmSessionType::kTemporary};
const base::flat_set<CdmSessionType> kEmptySessionTypes = {};

constexpr WiseplayKeySystemInfo::Robustness kTestMaxAudioRobustness =
    WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO;
constexpr WiseplayKeySystemInfo::Robustness kTestMaxVideoRobustness =
    WiseplayKeySystemInfo::Robustness::HW_SECURE_ALL;
constexpr WiseplayKeySystemInfo::Robustness kMinRobustness =
    WiseplayKeySystemInfo::Robustness::SW_SECURE_CRYPTO;

class WiseplayKeySystemInfoTest : public testing::Test {
 protected:
  WiseplayKeySystemInfoTest()
      : key_system_info_(
            kTestCodecs,
            kTestEncryptionSchemes,
            kTestSessionTypes,
            kTestHwSecureCodecs,
            kTestHwSecureEncryptionSchemes,
            kTestHwSecureSessionTypes,
            kTestMaxAudioRobustness,
            kTestMaxVideoRobustness,
            EmeFeatureSupport::REQUESTABLE,
            EmeFeatureSupport::NOT_SUPPORTED) {}

  void SetUpEmptyConfigKeySystem() {
    empty_key_system_info_ = std::make_unique<WiseplayKeySystemInfo>(
        kEmptyCodecs,
        kEmptyEncryptionSchemes,
        kEmptySessionTypes,
        kEmptyCodecs,
        kEmptyEncryptionSchemes,
        kEmptySessionTypes,
        kMinRobustness,
        kMinRobustness,
        EmeFeatureSupport::NOT_SUPPORTED,
        EmeFeatureSupport::NOT_SUPPORTED);
  }

  WiseplayKeySystemInfo key_system_info_;
  std::unique_ptr<WiseplayKeySystemInfo> empty_key_system_info_;
};

TEST_F(WiseplayKeySystemInfoTest, GetBaseKeySystemName) {
  EXPECT_EQ(key_system_info_.GetBaseKeySystemName(), media::kWiseplayKeySystem);
}

TEST_F(WiseplayKeySystemInfoTest, IsSupportedKeySystem) {
  EXPECT_TRUE(key_system_info_.IsSupportedKeySystem(media::kWiseplayKeySystem));
  EXPECT_FALSE(key_system_info_.IsSupportedKeySystem("com.unknown.keysystem"));
  EXPECT_FALSE(key_system_info_.IsSupportedKeySystem(""));
}

TEST_F(WiseplayKeySystemInfoTest, ShouldUseBaseKeySystemName) {
  EXPECT_TRUE(key_system_info_.ShouldUseBaseKeySystemName());
}

TEST_F(WiseplayKeySystemInfoTest, IsSupportedInitDataType) {
  // Test with codecs that support both WEBM and MP4
  EXPECT_TRUE(key_system_info_.IsSupportedInitDataType(EmeInitDataType::WEBM));
  EXPECT_TRUE(key_system_info_.IsSupportedInitDataType(EmeInitDataType::CENC));
  EXPECT_FALSE(key_system_info_.IsSupportedInitDataType(EmeInitDataType::KEYIDS));

  // Test with empty codecs
  SetUpEmptyConfigKeySystem();
  EXPECT_FALSE(empty_key_system_info_->IsSupportedInitDataType(EmeInitDataType::WEBM));
  EXPECT_FALSE(empty_key_system_info_->IsSupportedInitDataType(EmeInitDataType::CENC));
}

TEST_F(WiseplayKeySystemInfoTest, GetEncryptionSchemeConfigRule) {
  // Test all combinations from the implementation
  // 1. Both supported
  EXPECT_EQ(key_system_info_.GetEncryptionSchemeConfigRule(EncryptionScheme::kCenc),
            EmeConfig::SupportedRule());

  // 2. Only non-hw-secure supported
  EXPECT_EQ(key_system_info_.GetEncryptionSchemeConfigRule(EncryptionScheme::kCbcs),
            EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kNotAllowed});

  // 3. Only hw-secure supported (not possible with current test config)
  // Need to create a special config for this case
  WiseplayKeySystemInfo hw_only_key_system(
      kTestCodecs,
      kEmptyEncryptionSchemes,  // No regular support
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,  // Only HW support
      kTestHwSecureSessionTypes,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(hw_only_key_system.GetEncryptionSchemeConfigRule(EncryptionScheme::kCenc),
            EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired});

  // 4. Neither supported
  EXPECT_EQ(key_system_info_.GetEncryptionSchemeConfigRule(EncryptionScheme::kUnencrypted),
            EmeConfig::UnsupportedRule());
}

TEST_F(WiseplayKeySystemInfoTest, GetSupportedCodecs) {
  EXPECT_EQ(key_system_info_.GetSupportedCodecs(), kTestCodecs);
  SetUpEmptyConfigKeySystem();
  EXPECT_EQ(empty_key_system_info_->GetSupportedCodecs(), kEmptyCodecs);
}

TEST_F(WiseplayKeySystemInfoTest, GetSupportedHwSecureCodecs) {
  EXPECT_EQ(key_system_info_.GetSupportedHwSecureCodecs(), kTestHwSecureCodecs);
  SetUpEmptyConfigKeySystem();
  EXPECT_EQ(empty_key_system_info_->GetSupportedHwSecureCodecs(), kEmptyCodecs);
}

TEST_F(WiseplayKeySystemInfoTest, GetRobustnessConfigRule) {
  EXPECT_EQ(key_system_info_.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::AUDIO, "", nullptr),
            EmeConfig::SupportedRule());  // EMPTY is valid
  EXPECT_EQ(key_system_info_.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::VIDEO, "INVALID", nullptr),
            EmeConfig::UnsupportedRule());
  const std::vector<std::pair<std::string, WiseplayKeySystemInfo::Robustness>> robustness_cases = {
      {"INVALID", WiseplayKeySystemInfo::Robustness::INVALID},
      {"SW_SECURE_CRYPTO", WiseplayKeySystemInfo::Robustness::SW_SECURE_CRYPTO},
      {"SW_SECURE_DECODE", WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE},
      {"HW_SECURE_CRYPTO", WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO},
      {"HW_SECURE_DECODE", WiseplayKeySystemInfo::Robustness::HW_SECURE_DECODE},
      {"HW_SECURE_ALL", WiseplayKeySystemInfo::Robustness::HW_SECURE_ALL}};
  for (const auto& [robustness_str, expected_robustness] : robustness_cases) {
    SCOPED_TRACE(robustness_str);
    auto result = key_system_info_.GetRobustnessConfigRule(
        media::kWiseplayKeySystem, EmeMediaType::AUDIO, robustness_str, nullptr);
    if ((kTestMaxAudioRobustness == WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO &&
        expected_robustness == WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE) ||
        (kTestMaxAudioRobustness == WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE &&
        expected_robustness == WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO) ||
        expected_robustness > kTestMaxAudioRobustness ||
        expected_robustness == WiseplayKeySystemInfo::Robustness::INVALID) {
      EXPECT_EQ(result, EmeConfig::UnsupportedRule());
    } else if (expected_robustness >= WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO) {
      EXPECT_EQ(result, EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired});
    } else {
      EXPECT_EQ(result, EmeConfig::SupportedRule());
    }
    result = key_system_info_.GetRobustnessConfigRule(
        media::kWiseplayKeySystem, EmeMediaType::VIDEO, robustness_str, nullptr);
    if ((kTestMaxVideoRobustness == WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO &&
        expected_robustness == WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE) ||
        (kTestMaxVideoRobustness == WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE &&
        expected_robustness == WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO) ||
        expected_robustness > kTestMaxVideoRobustness ||
        expected_robustness == WiseplayKeySystemInfo::Robustness::INVALID) {
      EXPECT_EQ(result, EmeConfig::UnsupportedRule());
    } else if (expected_robustness >= WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO) {
      EXPECT_EQ(result, EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired});
    } else {
      EXPECT_EQ(result, EmeConfig::SupportedRule());
    }
  }
}

TEST_F(WiseplayKeySystemInfoTest, GetRobustnessConfigRule02) {
  // Test HW_SECURE_CRYPTO vs SW_SECURE_DECODE incompatible case
  // Create a config where max robustness is HW_SECURE_CRYPTO
  WiseplayKeySystemInfo crypto_max_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO,  // max audio
      WiseplayKeySystemInfo::Robustness::HW_SECURE_CRYPTO,  // max video
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(crypto_max_key_system.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::AUDIO, "SW_SECURE_DECODE", nullptr),
            EmeConfig::UnsupportedRule());

  // Test with hw_secure_requirement = true
  const bool hw_secure_required = true;
  EXPECT_EQ(key_system_info_.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::VIDEO, "SW_SECURE_CRYPTO", &hw_secure_required),
            media::EmeConfig::SupportedRule());
}

TEST_F(WiseplayKeySystemInfoTest,
       GetRobustnessConfigRuleWithHwSecureRequirement) {
  const bool hw_secure_not_required = false;
  EXPECT_EQ(key_system_info_.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::VIDEO,
                "SW_SECURE_CRYPTO", &hw_secure_not_required),
            media::EmeConfig::SupportedRule());
  const bool hw_secure_required = true;
  WiseplayKeySystemInfo decode_key_system(
      kTestCodecs, kTestEncryptionSchemes, kTestSessionTypes,
      kTestHwSecureCodecs, kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE,
      WiseplayKeySystemInfo::Robustness::SW_SECURE_DECODE,
      EmeFeatureSupport::REQUESTABLE, EmeFeatureSupport::NOT_SUPPORTED);
  EXPECT_EQ(decode_key_system.GetRobustnessConfigRule(
                media::kWiseplayKeySystem, EmeMediaType::AUDIO,
                "HW_SECURE_CRYPTO", &hw_secure_required),
            media::EmeConfig::UnsupportedRule());
}

TEST_F(WiseplayKeySystemInfoTest, GetPersistentLicenseSessionSupport) {
  // Test all combinations from the implementation
  // 1. Both supported
  WiseplayKeySystemInfo both_supported_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,  // includes persistent license
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,  // includes persistent license
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  // Need to add persistent license to hw secure session types
  base::flat_set<CdmSessionType> hw_with_persistent = kTestHwSecureSessionTypes;
  hw_with_persistent.insert(CdmSessionType::kPersistentLicense);

  WiseplayKeySystemInfo both_supported_key_system_with_persistent(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      hw_with_persistent,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(both_supported_key_system_with_persistent.GetPersistentLicenseSessionSupport(),
            EmeConfig::SupportedRule());

  // 2. Only non-hw-secure supported (current test config)
  EXPECT_EQ(key_system_info_.GetPersistentLicenseSessionSupport(),
            EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kNotAllowed});
}

TEST_F(WiseplayKeySystemInfoTest, GetPersistentLicenseSessionSupport02) {
  base::flat_set<CdmSessionType> hw_with_persistent = kTestHwSecureSessionTypes;
  hw_with_persistent.insert(CdmSessionType::kPersistentLicense);
  // 3. Only hw-secure supported
  WiseplayKeySystemInfo hw_only_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kEmptySessionTypes,  // no regular support
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,  // but hw supports temporary (not persistent)
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  // First test without persistent license in hw secure
  EXPECT_EQ(hw_only_key_system.GetPersistentLicenseSessionSupport(),
            EmeConfig::UnsupportedRule());

  // Now add persistent license to hw secure only
  WiseplayKeySystemInfo hw_only_key_system_with_persistent(
      kTestCodecs,
      kTestEncryptionSchemes,
      kEmptySessionTypes,  // no regular support
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      hw_with_persistent,  // hw supports persistent
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(hw_only_key_system_with_persistent.GetPersistentLicenseSessionSupport(),
            EmeConfig{.hw_secure_codecs = EmeConfigRuleState::kRequired});

  // 4. Neither supported
  SetUpEmptyConfigKeySystem();
  EXPECT_EQ(empty_key_system_info_->GetPersistentLicenseSessionSupport(),
            EmeConfig::UnsupportedRule());
}

TEST_F(WiseplayKeySystemInfoTest, GetPersistentStateSupport) {
  EXPECT_EQ(key_system_info_.GetPersistentStateSupport(),
            EmeFeatureSupport::REQUESTABLE);

  // Test other support levels
  WiseplayKeySystemInfo not_supported_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::NOT_SUPPORTED,  // persistent state not supported
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(not_supported_key_system.GetPersistentStateSupport(),
            EmeFeatureSupport::NOT_SUPPORTED);

  WiseplayKeySystemInfo always_supported_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::ALWAYS_ENABLED,
      EmeFeatureSupport::NOT_SUPPORTED);

  EXPECT_EQ(always_supported_key_system.GetPersistentStateSupport(),
            EmeFeatureSupport::ALWAYS_ENABLED);
}

TEST_F(WiseplayKeySystemInfoTest, GetDistinctiveIdentifierSupport) {
  EXPECT_EQ(key_system_info_.GetDistinctiveIdentifierSupport(),
            EmeFeatureSupport::NOT_SUPPORTED);

  // Test other support levels
  WiseplayKeySystemInfo requestable_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::REQUESTABLE);  // distinctive identifier requestable

  EXPECT_EQ(requestable_key_system.GetDistinctiveIdentifierSupport(),
            EmeFeatureSupport::REQUESTABLE);

  WiseplayKeySystemInfo always_supported_key_system(
      kTestCodecs,
      kTestEncryptionSchemes,
      kTestSessionTypes,
      kTestHwSecureCodecs,
      kTestHwSecureEncryptionSchemes,
      kTestHwSecureSessionTypes,
      kTestMaxAudioRobustness,
      kTestMaxVideoRobustness,
      EmeFeatureSupport::REQUESTABLE,
      EmeFeatureSupport::ALWAYS_ENABLED);  // distinctive identifier always supported

  EXPECT_EQ(always_supported_key_system.GetDistinctiveIdentifierSupport(),
            EmeFeatureSupport::ALWAYS_ENABLED);
}

}  // namespace cdm
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/cdm/renderer/key_system_support_update.h"
#include "content/public/renderer/key_system_support.h"
#include "content/public/renderer/render_accessibility.h"
#include "media/base/key_system_capability.h"
#include "media/base/key_system_info.h"
#include "media/base/video_codecs.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cdm {

class KeySystemSupportUpdateTest : public testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}

  media::KeySystemCapability MakeWiseplaySwCap(bool with_temp_session) {
    media::KeySystemCapability sw_cap;
    sw_cap.sw_secure_capability = media::CdmCapability();
    if (with_temp_session) {
      sw_cap.sw_secure_capability->session_types.insert(
          media::CdmSessionType::kTemporary);
    }
#if BUILDFLAG(USE_PROPRIETARY_CODECS)
    sw_cap.sw_secure_capability->audio_codecs.insert(media::AudioCodec::kAAC);
    media::VideoCodecInfo vinfo;
    vinfo.supports_clear_lead = true;
    sw_cap.sw_secure_capability->video_codecs[media::VideoCodec::kH264] = vinfo;
#endif
    sw_cap.sw_secure_capability->encryption_schemes = {
        media::EncryptionScheme::kCenc};
    return sw_cap;
  }

  media::KeySystemCapability MakeWiseplayHwCap(bool with_temp_session) {
    media::KeySystemCapability hw_cap;
    hw_cap.hw_secure_capability = media::CdmCapability();
    if (with_temp_session) {
      hw_cap.hw_secure_capability->session_types.insert(
          media::CdmSessionType::kTemporary);
    }
#if BUILDFLAG(USE_PROPRIETARY_CODECS)
    hw_cap.hw_secure_capability->audio_codecs.insert(media::AudioCodec::kAAC);
    media::VideoCodecInfo vinfo;
    vinfo.supports_clear_lead = true;
    hw_cap.hw_secure_capability->video_codecs[media::VideoCodec::kH264] = vinfo;
#endif
    hw_cap.hw_secure_capability->encryption_schemes = {
        media::EncryptionScheme::kCenc};
    return hw_cap;
  }
};

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithNullData) {
  media::KeySystemInfos infos{};
  auto cap = MakeWiseplaySwCap(true);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(cap, false, &infos));
  EXPECT_EQ(infos.size(), 0);
}

TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithSWSecureCapability1) {
  media::KeySystemInfos infos{};
  auto sw_cap = MakeWiseplaySwCap(false);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(sw_cap, true, &infos));
  EXPECT_EQ(infos.size(), 0);
}

TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithSWSecureCapability2) {
  media::KeySystemInfos infos{};
  auto sw_cap = MakeWiseplaySwCap(true);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(sw_cap, true, &infos));
  ASSERT_EQ(infos.size(), 1);
  EXPECT_NE(infos[0].get(), nullptr);
  std::unique_ptr<media::KeySystemInfo> info = std::move(infos[0]);
  EXPECT_NE(info->GetSupportedCodecs(), media::EME_CODEC_NONE);
}

TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithHWSecureCapability1) {
  media::KeySystemInfos infos{};
  auto hw_cap = MakeWiseplayHwCap(false);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(hw_cap, true, &infos));
  EXPECT_EQ(infos.size(), 0);
}

TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithHWSecureCapability2) {
  media::KeySystemInfos infos{};
  auto hw_cap = MakeWiseplayHwCap(true);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(hw_cap, true, &infos));
  ASSERT_EQ(infos.size(), 1);
  EXPECT_NE(infos[0].get(), nullptr);
  std::unique_ptr<media::KeySystemInfo> info = std::move(infos[0]);
  EXPECT_EQ(info->GetSupportedCodecs(), media::EME_CODEC_NONE);
}

TEST_F(KeySystemSupportUpdateTest, AddWiseplayWithNullKeySystemInfos) {
  auto cap = MakeWiseplaySwCap(true);
  ASSERT_NO_FATAL_FAILURE(TestAddWiseplay(cap, true, nullptr));
}
#endif  // BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)

}  // namespace cdm
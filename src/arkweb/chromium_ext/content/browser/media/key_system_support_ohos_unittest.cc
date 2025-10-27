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

#include "base/run_loop.h"
#include "base/test/bind.h"
#include "base/test/task_environment.h"
#include "content/browser/media/key_system_support_ohos.h"
#include "media/base/cdm_capability.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

namespace content {

class KeySystemSupportOhosTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
  base::test::TaskEnvironment task_environment_;
};

TEST_F(KeySystemSupportOhosTest, KeySystemNotSupported) {
  const std::string kUnsupportedKeySystem = "com.unsupported.keysystem";
  base::RunLoop loop;
  std::optional<::media::CdmCapability> result;

  GetOHOSCdmCapability(
      kUnsupportedKeySystem, CdmInfo::Robustness::kSoftwareSecure,
      base::BindLambdaForTesting(
          [&](std::optional<::media::CdmCapability> capability) {
            result = std::move(capability);
            loop.Quit();
          }));
  loop.Run();

  EXPECT_FALSE(result.has_value());
}

TEST_F(KeySystemSupportOhosTest, KeySystemSupported1) {
  const std::string kUnsupportedKeySystem = "com.wiseplay.drm";
  base::RunLoop loop;
  std::optional<::media::CdmCapability> result;

  GetOHOSCdmCapability(
      kUnsupportedKeySystem, CdmInfo::Robustness::kHardwareSecure,
      base::BindLambdaForTesting(
          [&](std::optional<::media::CdmCapability> capability) {
            result = std::move(capability);
            loop.Quit();
          }));
  loop.Run();

  EXPECT_TRUE(result.has_value());
}

}  // namespace content
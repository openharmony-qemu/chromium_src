/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "media/gpu/ohos/shared_image_video_provider.h"
#include <memory>
#include "absl/meta/type_traits.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace media;

class SharedImageVideoProviderTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(SharedImageVideoProviderTest, ImageSpec001) {
  const gfx::Size coded_size1(1920, 1080);
  uint64_t our_generation_id1 = 1;
  SharedImageVideoProvider::ImageSpec ImageSpec1(coded_size1,
                                                 our_generation_id1);
  ImageSpec1.color_space = gfx::ColorSpace(gfx::ColorSpace::PrimaryID::BT709,
                                           gfx::ColorSpace::TransferID::BT709);
  SharedImageVideoProvider::ImageSpec ImageSpec2(coded_size1,
                                                 our_generation_id1);
  ImageSpec2.color_space = gfx::ColorSpace(gfx::ColorSpace::PrimaryID::BT709,
                                           gfx::ColorSpace::TransferID::BT709);
  int num = 0;
  if (ImageSpec1 == ImageSpec2) {
    num++;
  }
  EXPECT_EQ(num, 1);
}

TEST_F(SharedImageVideoProviderTest, ImageSpec002) {
  const gfx::Size coded_size1(1920, 1080);
  uint64_t our_generation_id1 = 1;
  SharedImageVideoProvider::ImageSpec ImageSpec1(coded_size1,
                                                 our_generation_id1);
  ImageSpec1.color_space = gfx::ColorSpace(gfx::ColorSpace::PrimaryID::BT709,
                                           gfx::ColorSpace::TransferID::BT709);
  const gfx::Size coded_size2(640, 480);
  uint64_t our_generation_id2 = 2;
  SharedImageVideoProvider::ImageSpec ImageSpec2(coded_size2,
                                                 our_generation_id2);
  ImageSpec2.color_space = gfx::ColorSpace(gfx::ColorSpace::PrimaryID::BT470M,
                                           gfx::ColorSpace::TransferID::LINEAR);
  int num = 0;
  if (ImageSpec1 == ImageSpec2) {
    num++;
  }
  EXPECT_EQ(num, 0);
}

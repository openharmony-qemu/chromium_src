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

#include "shared_image_video_provider.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {
class SharedImageVideoProviderTest : public testing::Test {
 public:
   void SetUp() override {}
   void TearDown() override {}
};

TEST_F(SharedImageVideoProviderTest, Testoperator01) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1920, 1080);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 1;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  ASSERT_TRUE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator02) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1600, 900);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 1;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator03) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1920, 1080);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 0;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator04) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1920, 1080);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 1;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateDisplayP3D65();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator05) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1920, 1080);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 0;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateDisplayP3D65();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator06) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1600, 900);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 0;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator07) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1600, 900);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 1;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateDisplayP3D65();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

TEST_F(SharedImageVideoProviderTest, Testoperator08) {
  SharedImageVideoProvider::ImageSpec share_spec_;
  SharedImageVideoProvider::ImageSpec image_spec_;
  share_spec_.coded_size = gfx::Size(1920, 1080);
  image_spec_.coded_size = gfx::Size(1600, 900);
  share_spec_.generation_id = 1;
  image_spec_.generation_id = 0;
  share_spec_.color_space = gfx::ColorSpace::CreateSRGB();
  image_spec_.color_space = gfx::ColorSpace::CreateDisplayP3D65();
  ASSERT_FALSE(share_spec_ == image_spec_);
}

}
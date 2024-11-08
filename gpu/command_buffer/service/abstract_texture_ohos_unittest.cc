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
 
#define private public
#include "command_buffer/service/abstract_texture_ohos.h"
#include "command_buffer/service/texture_manager.h"
#undef private

#include <memory>
#include <utility>

#include "base/memory/scoped_refptr.h"
#include "ui/gfx/geometry/size.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace gpu;

class AbstractTextureOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    service_id = 789;
    abstract_texture_ohos = AbstractTextureOHOS::CreateForTesting(service_id);
	abstract_texture_shared = AbstractTextureOHOS::CreateForTesting(service_id);
  }

  void TearDown() override {}

  GLuint service_id;

 protected:
  std::unique_ptr<AbstractTextureOHOS> abstract_texture_ohos;
  std::shared_ptr<AbstractTextureOHOS> abstract_texture_shared;
};

TEST_F(AbstractTextureOHOSTest, AbstractTextureOHOS_Destructor001) {
  abstract_texture_shared->have_context_ = false;
  abstract_texture_shared->texture_ = nullptr;
  abstract_texture_shared->texture_passthrough_ = nullptr;
  abstract_texture_shared.reset();
  EXPECT_EQ(abstract_texture_shared.use_count(), 0);
}

TEST_F(AbstractTextureOHOSTest, AbstractTextureOHOS_Destructor002) {
  abstract_texture_shared->have_context_ = true;
  abstract_texture_shared->texture_ = nullptr;
  abstract_texture_shared->texture_passthrough_ = nullptr;
  abstract_texture_shared.reset();
  EXPECT_EQ(abstract_texture_shared.use_count(), 0);
}

TEST_F(AbstractTextureOHOSTest, AbstractTextureOHOS_Destructor003) {
  abstract_texture_shared->have_context_ = true;
  abstract_texture_shared->texture_ = nullptr;
  GLuint service_id = 0;
  gfx::Size size(1, 1);
  auto texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES, GL_RGBA, size.width(), size.height(),
      1, 0, GL_RGBA, GL_UNSIGNED_BYTE);
  abstract_texture_shared->texture_passthrough_ = std::move(texture);
  abstract_texture_shared->texture_passthrough_->have_context_ = false;
  abstract_texture_shared.reset();
  EXPECT_EQ(abstract_texture_shared.use_count(), 0);
}

TEST_F(AbstractTextureOHOSTest, NotifyOnContextLostFalse) {
  abstract_texture_ohos->texture_passthrough_ = nullptr;
  abstract_texture_ohos->NotifyOnContextLost();
  EXPECT_FALSE(abstract_texture_ohos->have_context_);
}

TEST_F(AbstractTextureOHOSTest, NotifyOnContextLostTrue) {
  abstract_texture_ohos->texture_passthrough_ =
      base::MakeRefCounted<gles2::TexturePassthrough>(2, 2);
  abstract_texture_ohos->NotifyOnContextLost();
  EXPECT_FALSE(abstract_texture_ohos->have_context_);
}

TEST_F(AbstractTextureOHOSTest, BindToServiceId_Test001) {
  abstract_texture_ohos->texture_passthrough_ = nullptr;
  abstract_texture_ohos->BindToServiceId(service_id);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
}

TEST_F(AbstractTextureOHOSTest, GetTextureBase_Test001) {
  EXPECT_FALSE(abstract_texture_ohos->texture_);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
  abstract_texture_ohos->texture_for_testing_ = nullptr;

  EXPECT_FALSE(abstract_texture_ohos->GetTextureBase());
}

TEST_F(AbstractTextureOHOSTest, GetTextureBase_Test002) {
  EXPECT_FALSE(abstract_texture_ohos->texture_);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
  EXPECT_TRUE(abstract_texture_ohos->texture_for_testing_);

  EXPECT_TRUE(abstract_texture_ohos->GetTextureBase());
}

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

#include "media/base/mock_filters.h"
#include "media/mojo/services/mojo_cdm_service_context.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "media/mojo/services/mojo_renderer_service.h"
#undef private

namespace media {

using testing::StrictMock;
using ::testing::Test;

class MojoRendererServiceIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    auto mock_renderer = std::make_unique<StrictMock<MockRenderer>>();
    mock_renderer_ = mock_renderer.get();
    mojo_renderer_service_ = std::make_unique<MojoRendererService>(
        &mojo_cdm_service_context_, std::move(mock_renderer));
  }

  void TearDown() override {
    mojo_renderer_service_.reset();
    mock_renderer_ = nullptr;
  }

  std::unique_ptr<MojoRendererService> mojo_renderer_service_;
  MojoCdmServiceContext mojo_cdm_service_context_;
  raw_ptr<StrictMock<MockRenderer>> mock_renderer_;
};

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(MojoRendererServiceIncludeTest, SetMediaSourceList1) {
  std::vector<media::mojom::MediaSourceInfoPtr> vec;
  ASSERT_NO_FATAL_FAILURE(
      mojo_renderer_service_->SetMediaSourceList(std::move(vec)));

  vec.push_back(media::mojom::MediaSourceInfo::New("src1", "fmt1"));
  vec.push_back(media::mojom::MediaSourceInfo::New("src2", "fmt2"));
  ASSERT_NO_FATAL_FAILURE(
      mojo_renderer_service_->SetMediaSourceList(std::move(vec)));
  EXPECT_NE(mojo_renderer_service_->renderer_, nullptr);
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MojoRendererServiceIncludeTest, RecycleDmaBuffer1) {
  mojo_renderer_service_->renderer_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_service_->RecycleDmaBuffer());
}

TEST_F(MojoRendererServiceIncludeTest, RecycleDmaBuffer2) {
  EXPECT_CALL(*mock_renderer_, RecycleDmaBuffer())
      .Times(1)
      .WillOnce(testing::Return());
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_service_->RecycleDmaBuffer());
}

TEST_F(MojoRendererServiceIncludeTest, ResumeDmaBuffer1) {
  mojo_renderer_service_->renderer_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_service_->ResumeDmaBuffer());
}

TEST_F(MojoRendererServiceIncludeTest, ResumeDmaBuffer2) {
  EXPECT_CALL(*mock_renderer_, ResumeDmaBuffer())
      .Times(1)
      .WillOnce(testing::Return());
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_service_->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF

}  // namespace media
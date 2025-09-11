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

#include "media/mojo/services/mojo_cdm_service_context.h"
#include "media/filters/fake_video_decoder.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "media/mojo/services/mojo_video_decoder_service.h"
#undef private

namespace media {
class MojoDecoderServiceIncludeTest : public ::testing::Test {
 protected:
   void SetUp() override {}
   void TearDown() override {}
};

class FakeMojoMediaClient : public media::MojoMediaClient {
 public:
 private:
  media::SupportedVideoDecoderConfigs supported_video_decoder_configs_;
};

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MojoDecoderServiceIncludeTest, TestPipEnable) {
  FakeMojoMediaClient mojo_media_client_;
  MojoCdmServiceContext cdm_service_context_;
  auto mojo_void_service_ = std::make_unique<media::MojoVideoDecoderService>(
            &mojo_media_client_, &cdm_service_context_,
            mojo::PendingRemote<media::stable::mojom::StableVideoDecoder>());

  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->PipEnable(true));
  mojo_void_service_->decoder_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->PipEnable(true));
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MojoDecoderServiceIncludeTest, TestRecycleDmaBuffer) {
  FakeMojoMediaClient mojo_media_client_;
  MojoCdmServiceContext cdm_service_context_;
  auto mojo_void_service_ = std::make_unique<media::MojoVideoDecoderService>(
            &mojo_media_client_, &cdm_service_context_,
            mojo::PendingRemote<media::stable::mojom::StableVideoDecoder>());

  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->RecycleDmaBuffer());
  mojo_void_service_->decoder_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->RecycleDmaBuffer());
}

TEST_F(MojoDecoderServiceIncludeTest, TestResumeDmaBuffer) {
  FakeMojoMediaClient mojo_media_client_;
  MojoCdmServiceContext cdm_service_context_;
  auto mojo_void_service_ = std::make_unique<media::MojoVideoDecoderService>(
            &mojo_media_client_, &cdm_service_context_,
            mojo::PendingRemote<media::stable::mojom::StableVideoDecoder>());
  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->ResumeDmaBuffer());
  mojo_void_service_->decoder_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(mojo_void_service_->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF
}
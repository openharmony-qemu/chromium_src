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

#include "base/test/task_environment.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "media/mojo/clients/mojo_video_decoder.h"
#undef private

namespace media {

class MojoVideoDecoderIncludeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mojo::PendingRemote<mojom::VideoDecoder> pending;
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    decoder_ = std::make_unique<MojoVideoDecoder>(task_runner_,
        nullptr, nullptr, std::move(pending), RequestOverlayInfoCB(), gfx::ColorSpace());
  }
  void TearDown() override {
    decoder_.reset();
  }
  
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<MojoVideoDecoder> decoder_;
};

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MojoVideoDecoderIncludeTest, PipEnable) {
  decoder_->has_connection_error_ = true;
  EXPECT_NO_FATAL_FAILURE(decoder_->PipEnable(true));
  decoder_->has_connection_error_ = false;
  mojo::PendingRemote<media::mojom::VideoDecoder> pending;
  auto receiver = pending.InitWithNewPipeAndPassReceiver();
  EXPECT_TRUE(receiver.is_valid());
  decoder_->remote_decoder_.Bind(std::move(pending));
  decoder_->remote_decoder_.set_disconnect_handler(base::DoNothing());
  EXPECT_NO_FATAL_FAILURE(decoder_->PipEnable(false));
  decoder_->remote_decoder_.reset();
  EXPECT_FALSE(decoder_->remote_decoder_.is_bound());
}
#endif  // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MojoVideoDecoderIncludeTest, RecycleDmaBuffer) {
  decoder_->has_connection_error_ = true;
  EXPECT_NO_FATAL_FAILURE(decoder_->RecycleDmaBuffer());
  decoder_->has_connection_error_ = false;
  EXPECT_NO_FATAL_FAILURE(decoder_->RecycleDmaBuffer());
  mojo::PendingRemote<media::mojom::VideoDecoder> pending;
  auto receiver = pending.InitWithNewPipeAndPassReceiver();
  EXPECT_TRUE(receiver.is_valid());
  decoder_->remote_decoder_.Bind(std::move(pending));
  decoder_->remote_decoder_.set_disconnect_handler(base::DoNothing());
  EXPECT_NO_FATAL_FAILURE(decoder_->RecycleDmaBuffer());
  decoder_->remote_decoder_.reset();
  EXPECT_FALSE(decoder_->remote_decoder_.is_bound());
}

TEST_F(MojoVideoDecoderIncludeTest, ResumeDmaBuffer) {
  decoder_->has_connection_error_ = true;
  EXPECT_NO_FATAL_FAILURE(decoder_->ResumeDmaBuffer());
  decoder_->has_connection_error_ = false;
  EXPECT_NO_FATAL_FAILURE(decoder_->ResumeDmaBuffer());
  mojo::PendingRemote<media::mojom::VideoDecoder> pending;
  auto receiver = pending.InitWithNewPipeAndPassReceiver();
  EXPECT_TRUE(receiver.is_valid());
  decoder_->remote_decoder_.Bind(std::move(pending));
  decoder_->remote_decoder_.set_disconnect_handler(base::DoNothing());
  EXPECT_NO_FATAL_FAILURE(decoder_->ResumeDmaBuffer());
  decoder_->remote_decoder_.reset();
  EXPECT_FALSE(decoder_->remote_decoder_.is_bound());
}
#endif  // ARKWEB_MEDIA_DMABUF

}  // namespace media
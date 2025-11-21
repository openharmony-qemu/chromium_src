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

#include "arkweb/chromium_ext/gpu/ipc/service/stream_texture_ohos.h"
#include "gtest/gtest.h"
#include <cstdint>
#include "gpu/ipc/service/gpu_channel.h"
#include "base/run_loop.h"
#include "base/test/test_simple_task_runner.h"
#include "build/build_config.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread.h"

namespace gpu {

class StreamChannelTest : public GpuChannelTestCommon {
 public:
  StreamChannelTest() : GpuChannelTestCommon(true /* use_stub_bindings */) {}
  ~StreamChannelTest() override = default;

  void SetUp() {
    channel_ = CreateChannel(kDefaultClientId, true /* is_gpu_host */);
    ASSERT_TRUE(channel_);
  }

  void TearDown() {
    channel_ = nullptr;
  }

  static constexpr int32_t kDefaultClientId = 1;
  GpuChannel* channel_ = nullptr;
};

class StreamTextureOHOSTest : public StreamChannelTest {
protected:
  void SetUp() {
    StreamChannelTest::SetUp();
    context_state_ = channel_->gpu_channel_manager()->GetSharedContextState(&result_);
    ASSERT_EQ(result_, ContextResult::kSuccess);
  }

  void TearDown() {
    StreamChannelTest::TearDown();
  }

  ContextResult result_;
  scoped_refptr<SharedContextState> context_state_;
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
};

TEST_F(StreamTextureOHOSTest, Create) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                1,
                                                gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
}

TEST_F(StreamTextureOHOSTest, RunCallback) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                1,
                                                gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    base::Thread thread("MyThread");
    thread.Start(); 
    scoped_refptr<base::SingleThreadTaskRunner> task_runner = thread.task_runner();
    // just for UT TEST
    base::WeakPtrFactory<StreamTexture> weak_factory(stream_texture.get());
    base::WeakPtr<StreamTexture> weak_ptr = weak_factory.GetWeakPtr();
    StreamTexture::RunCallback(task_runner, weak_ptr);
    thread.Stop();
}

TEST_F(StreamTextureOHOSTest, ReleaseChannel) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                1,
                                                gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    stream_texture->ReleaseChannel();
}

TEST_F(StreamTextureOHOSTest, OnFrameAvailable) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                1,
                                                gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    stream_texture->OnFrameAvailable();
}

TEST_F(StreamTextureOHOSTest, UpdatesSize) {
    const gfx::Size kTestSize(100, 200);

    auto stream_texture = StreamTexture::Create(channel_,
                                                    1,
                                                    gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                    std::move(receiver));
    EXPECT_TRUE(stream_texture->rotated_visible_size_.IsEmpty());
    stream_texture->UpdateRotatedVisibleSize(kTestSize);
    EXPECT_EQ(stream_texture->rotated_visible_size_, kTestSize);
}

TEST_F(StreamTextureOHOSTest, FailsIfChannelNull) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                    1,
                                                    gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                    std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    stream_texture->ReleaseChannel();
    stream_texture->UpdateRotatedVisibleSize(gfx::Size(100, 200));
}

TEST_F(StreamTextureOHOSTest, ReturnsCachedValue) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                    1,
                                                    gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                    std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    // Set initial cached value
    stream_texture->native_embed_id_ = 123;
    
    EXPECT_EQ(stream_texture->NativeEmbedID(), 123);
}

TEST_F(StreamTextureOHOSTest, GeneratesNewIdWhenUnset) {
    auto stream_texture = StreamTexture::Create(channel_,
                                                    1,
                                                    gl::ohos::TextureOwnerMode::kNativeImageTexture,
                                                    std::move(receiver));
    EXPECT_NE(stream_texture, nullptr);
    // Set initial cached value
    stream_texture->native_embed_id_ = -1;
    
    EXPECT_NE(stream_texture->NativeEmbedID(), -1);
}


} // namespace gpu
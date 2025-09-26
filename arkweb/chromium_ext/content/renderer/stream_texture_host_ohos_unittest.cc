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

#include "content/renderer/stream_texture_host_ohos.h"
#include "base/test/task_environment.h"
#include "gpu/command_buffer/common/sync_token.h"
#include "gpu/ipc/client/gpu_channel_host.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace content {

using ::testing::_;

class MockStreamTextureListener : public StreamTextureHost::Listener {
 public:
  MOCK_METHOD0(OnFrameAvailable, void());
  MOCK_METHOD4(OnFrameWithInfoAvailable, void(const gpu::Mailbox&, const gfx::Size&, const gfx::Rect&,
                                              const absl::optional<gpu::VulkanYCbCrInfo>&));
  MOCK_METHOD0(OnDestroySurface, void());
};

class TestGpuChannelHost : public gpu::GpuChannelHost {
 public:
  TestGpuChannelHost()
      : gpu::GpuChannelHost(
            0 /* channel_id */,
            gpu::GPUInfo(),
            gpu::GpuFeatureInfo(),
            gpu::SharedImageCapabilities(),
            mojo::ScopedMessagePipeHandle(
                mojo::MessagePipeHandle(mojo::kInvalidHandleValue))) {}

 protected:
  ~TestGpuChannelHost() override {}
};
class StreamTextureHostTest : public testing::Test {
 protected:
  void SetUp() override {
  }
  void TearDown() override {
  }
  void TestOnFrameAvailable (StreamTextureHost* host) {
    host->OnFrameAvailable();
  }
  void TestOnFrameWithInfoAvailable(StreamTextureHost* host, const gpu::Mailbox& mailbox, const gfx::Size& coded_size,
      const gfx::Rect& visible_rect, absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info) {
    host->OnFrameWithInfoAvailable(mailbox, coded_size, visible_rect, ycbcr_info);
  }
  void TestOnDestroySurface(StreamTextureHost* host) {
    host->OnDestroySurface();
  }
  base::test::SingleThreadTaskEnvironment task_environment_;
  scoped_refptr<TestGpuChannelHost> channel_;
  MockStreamTextureListener mock_listener_;
};

TEST_F(StreamTextureHostTest, BindToCurrentThread_True) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  EXPECT_TRUE(host->BindToCurrentThread(&mock_listener_));
}

TEST_F(StreamTextureHostTest, BindToCurrentThread_False) {
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, mojo::NullAssociatedRemote());
  EXPECT_FALSE(host->BindToCurrentThread(&mock_listener_));
}

TEST_F(StreamTextureHostTest, OnFrameWithInfoAvailable_WithListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  EXPECT_TRUE(host->BindToCurrentThread(&mock_listener_));
  EXPECT_CALL(mock_listener_, OnFrameWithInfoAvailable(_, _, _, _)).Times(1);
  TestOnFrameWithInfoAvailable(host.get(), gpu::Mailbox(), gfx::Size(), gfx::Rect(), absl::nullopt);
}

TEST_F(StreamTextureHostTest, OnFrameWithInfoAvailable_WithOutListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  TestOnFrameWithInfoAvailable(host.get(), gpu::Mailbox(), gfx::Size(), gfx::Rect(), absl::nullopt);
}

TEST_F(StreamTextureHostTest, OnFrameAvailable_WithListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  EXPECT_TRUE(host->BindToCurrentThread(&mock_listener_));
  EXPECT_CALL(mock_listener_, OnFrameAvailable()).Times(1);
  TestOnFrameAvailable(host.get());
}

TEST_F(StreamTextureHostTest, OnFrameAvailable_WithOutListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  TestOnFrameAvailable(host.get());
}

TEST_F(StreamTextureHostTest, OnDestroySurface_WithListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  EXPECT_TRUE(host->BindToCurrentThread(&mock_listener_));
  EXPECT_CALL(mock_listener_, OnDestroySurface()).Times(1);
  TestOnDestroySurface(host.get());
}

TEST_F(StreamTextureHostTest, OnDestroySurface_WithOutListener) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  TestOnDestroySurface(host.get());
}

TEST_F(StreamTextureHostTest, UpdateRotatedVisibleSize) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  EXPECT_TRUE(host->BindToCurrentThread(&mock_listener_));
  host->UpdateRotatedVisibleSize(gfx::Size(100, 100));
}

TEST_F(StreamTextureHostTest, UpdateRotatedVisibleSize_TextureNotBound) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  host->UpdateRotatedVisibleSize(gfx::Size(100, 100));
}

TEST_F(StreamTextureHostTest, GenUnverifiedSyncToken_WithChannel) {
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture;
  std::ignore = texture.InitWithNewEndpointAndPassReceiver();
  texture.EnableUnassociatedUsage();
  std::unique_ptr<StreamTextureHost> host = std::make_unique<StreamTextureHost>(
      channel_.get(), 1, std::move(texture));
  auto token = host->GenUnverifiedSyncToken();
  EXPECT_FALSE(token.HasData());
}

}  // namespace content

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on stream_texture_wrapper_impl.cc originally written by
// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "content/renderer/media/ohos/native_texture_factory.h"
#undef private
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/test/task_environment.h"
#include "gpu/ipc/client/client_shared_image_interface.h"
#include "gpu/ipc/client/gpu_channel_host.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/common/mock_gpu_channel.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "mojo/public/cpp/bindings/sync_call_restrictions.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

class MockSingleThreadTaskRunner : public base::SingleThreadTaskRunner {
 public:
  MOCK_METHOD(bool, BelongsToCurrentThread, (), (const, override));
  MOCK_METHOD(bool,
              DeleteOrReleaseSoonInternal,
              (const base::Location& from_here,
               void (*deleter)(const void*),
               const void* object),
              (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeTicks delayed_run_time,
               base::subtle::DelayPolicy delay_policy),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
};

namespace content {

class TestGpuChannelHost : public gpu::GpuChannelHost {
 public:
  explicit TestGpuChannelHost(gpu::mojom::GpuChannel& gpu_channel)
      : GpuChannelHost(0 /* channel_id */,
                       gpu::GPUInfo(),
                       gpu::GpuFeatureInfo(),
                       gpu::SharedImageCapabilities(),
                       mojo::ScopedMessagePipeHandle(
                           mojo::MessagePipeHandle(mojo::kInvalidHandleValue))),
        gpu_channel_(gpu_channel) {}

  gpu::mojom::GpuChannel& GetGpuChannel() override { return *gpu_channel_; }

 protected:
  ~TestGpuChannelHost() override = default;

  const raw_ref<gpu::mojom::GpuChannel> gpu_channel_;
};

class NativeTextureProxyTest : public testing::Test {
 public:
  NativeTextureProxyTest() {}
  ~NativeTextureProxyTest() override {}

 protected:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    channel_ = base::MakeRefCounted<TestGpuChannelHost>(mock_gpu_channel_);
    textureFactory_ = NativeTextureFactory::Create(channel_);
    EXPECT_CALL(mock_gpu_channel_,
                CreateNativeTextureReturnBool(::testing::_, ::testing::_,
                                        ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(
            [](int32_t a, int32_t b,
               mojo::PendingAssociatedReceiver<gpu::mojom::StreamTexture>,
               int32_t* out_value) {
              *out_value = 1;
              return true;
            }));

    proxy_ = textureFactory_->CreateProxy(
        gl::ohos::TextureOwnerMode::kNativeImageTexture);
    EXPECT_NE(proxy_, nullptr);
  }
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  ScopedNativeTextureProxy proxy_;
  scoped_refptr<NativeTextureFactory> textureFactory_;
  scoped_refptr<gpu::GpuChannelHost> channel_;
  gpu::MockGpuChannel mock_gpu_channel_;
};

TEST_F(NativeTextureProxyTest, BindToTaskRunner001) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(false));
  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void()> destroy_texture_cb = base::DoNothing();
  NativeTextureProxy::CreateVideoFrameCB callback = base::BindRepeating(
      [](const gpu::Mailbox& mailbox, const gfx::Size& coded_size,
         const gfx::Rect& visible_rect,
         const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) {});
  proxy_->native_embed_id_ = -1;
  proxy_->destroy_texture_cb_.Reset();
  EXPECT_NO_FATAL_FAILURE(proxy_->BindToTaskRunner(
      received_frame_cb, callback, std::move(destroy_texture_cb), task_runner));
  EXPECT_TRUE(proxy_->destroy_texture_cb_.is_null());
}

TEST_F(NativeTextureProxyTest, BindToTaskRunner002) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(false));
  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void()> destroy_texture_cb = base::DoNothing();
  NativeTextureProxy::CreateVideoFrameCB callback = base::BindRepeating(
      [](const gpu::Mailbox& mailbox, const gfx::Size& coded_size,
         const gfx::Rect& visible_rect,
         const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) {});
  proxy_->destroy_texture_cb_.Reset();
  EXPECT_NO_FATAL_FAILURE(proxy_->BindToTaskRunner(
      received_frame_cb, callback, std::move(destroy_texture_cb), task_runner));
  EXPECT_FALSE(proxy_->destroy_texture_cb_.is_null());
}

TEST_F(NativeTextureProxyTest, BindToTaskRunner003) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void()> destroy_texture_cb = base::DoNothing();
  NativeTextureProxy::CreateVideoFrameCB callback = base::BindRepeating(
      [](const gpu::Mailbox& mailbox, const gfx::Size& coded_size,
         const gfx::Rect& visible_rect,
         const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) {});
  proxy_->host_->pending_texture_.reset();
  EXPECT_NO_FATAL_FAILURE(proxy_->BindToTaskRunner(
      received_frame_cb, callback, std::move(destroy_texture_cb), task_runner));
}

TEST_F(NativeTextureProxyTest, OnFrameAvailable001) {
  static int res = 0;
  base::RepeatingClosure received_frame_cb =
      base::BindRepeating([]() { res = 1; });
  proxy_->received_frame_cb_ = received_frame_cb;
  proxy_->received_frame_cb_.Reset();
  proxy_->OnFrameAvailable();
  EXPECT_NE(res, 1);
}

TEST_F(NativeTextureProxyTest, OnFrameAvailable002) {
  static int res = 0;
  base::RepeatingClosure received_frame_cb =
      base::BindRepeating([]() { res = 1; });
  proxy_->received_frame_cb_ = std::move(received_frame_cb);
  proxy_->OnFrameAvailable();
  EXPECT_EQ(res, 1);
}

TEST_F(NativeTextureProxyTest, OnFrameWithInfoAvailable001) {
  gpu::Mailbox mailbox;
  gfx::Size coded_size(1920, 1080);
  gfx::Rect visible_rect(0, 0, 1920, 1080);
  absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;
  static int res = 0;
  NativeTextureProxy::CreateVideoFrameCB callback = base::BindRepeating(
      [](const gpu::Mailbox& mailbox, const gfx::Size& coded_size,
         const gfx::Rect& visible_rect,
         const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) { res = 1; });
  proxy_->create_video_frame_cb_ = callback;

  EXPECT_NO_FATAL_FAILURE(proxy_->OnFrameWithInfoAvailable(
      mailbox, coded_size, visible_rect, ycbcr_info));
  EXPECT_EQ(res, 1);
}

TEST_F(NativeTextureProxyTest, OnFrameWithInfoAvailable002) {
  gpu::Mailbox mailbox;
  gfx::Size coded_size(1920, 1080);
  gfx::Rect visible_rect(0, 0, 1920, 1080);
  absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;
  static int res = 0;
  base::RepeatingClosure received_frame_cb =
      base::BindRepeating([]() { res = 1; });
  proxy_->received_frame_cb_ = std::move(received_frame_cb);

  EXPECT_NO_FATAL_FAILURE(proxy_->OnFrameWithInfoAvailable(
      mailbox, coded_size, visible_rect, ycbcr_info));
  EXPECT_EQ(res, 1);
}

TEST_F(NativeTextureProxyTest, OnFrameWithInfoAvailable003) {
  gpu::Mailbox mailbox;
  gfx::Size coded_size(1920, 1080);
  gfx::Rect visible_rect(0, 0, 1920, 1080);
  absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;
  static int res = 0;
  base::RepeatingClosure received_frame_cb =
      base::BindRepeating([]() { res = 1; });
  proxy_->received_frame_cb_ = std::move(received_frame_cb);
  proxy_->received_frame_cb_.Reset();
  EXPECT_NO_FATAL_FAILURE(proxy_->OnFrameWithInfoAvailable(
      mailbox, coded_size, visible_rect, ycbcr_info));
  EXPECT_EQ(res, 0);
}

TEST_F(NativeTextureProxyTest, UpdateRotatedVisibleSize001) {
  gfx::Size coded_size(1920, 1080);
  proxy_->task_runner_ = nullptr;
  EXPECT_NO_FATAL_FAILURE(proxy_->UpdateRotatedVisibleSize(coded_size));
}

TEST_F(NativeTextureProxyTest, UpdateRotatedVisibleSize002) {
  gfx::Size coded_size(1920, 1080);
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(false));
  proxy_->task_runner_ = std::move(task_runner);
  EXPECT_NO_FATAL_FAILURE(proxy_->UpdateRotatedVisibleSize(coded_size));
}

TEST_F(NativeTextureProxyTest, UpdateRotatedVisibleSize003) {
  gfx::Size coded_size(1920, 1080);
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(true));
  proxy_->task_runner_ = std::move(task_runner);
  EXPECT_NO_FATAL_FAILURE(proxy_->UpdateRotatedVisibleSize(coded_size));
}

TEST_F(NativeTextureProxyTest, OnDestroySurface001) {
  static int res = 0;
  base::OnceCallback<void()> destroy_texture_cb =
      base::BindOnce([]() { res = 1; });
  proxy_->destroy_texture_cb_ = std::move(destroy_texture_cb);
  EXPECT_NO_FATAL_FAILURE(proxy_->OnDestroySurface());
  EXPECT_EQ(res, 1);
}

TEST_F(NativeTextureProxyTest, OnDestroySurface002) {
  static int res = 0;
  base::OnceCallback<void()> destroy_texture_cb =
      base::BindOnce([]() { res = 1; });
  proxy_->destroy_texture_cb_ = std::move(destroy_texture_cb);
  proxy_->destroy_texture_cb_.Reset();
  EXPECT_NO_FATAL_FAILURE(proxy_->OnDestroySurface());
  EXPECT_EQ(res, 0);
}

class NativeTextureFactoryTest : public testing::Test {
 public:
  NativeTextureFactoryTest() {}
  ~NativeTextureFactoryTest() override {}

 protected:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    channel_ = base::MakeRefCounted<TestGpuChannelHost>(mock_gpu_channel_);
    textureFactory_ = NativeTextureFactory::Create(channel_);
  }
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  scoped_refptr<NativeTextureFactory> textureFactory_;
  scoped_refptr<gpu::GpuChannelHost> channel_;
  gpu::MockGpuChannel mock_gpu_channel_;
};
TEST_F(NativeTextureFactoryTest, CreateProxy001) {
  ASSERT_NE(textureFactory_, nullptr);
  auto proxy = textureFactory_->CreateProxy(gl::ohos::TextureOwnerMode::kNone);
  EXPECT_EQ(proxy, nullptr);

  EXPECT_CALL(mock_gpu_channel_,
              CreateNativeTextureReturnBool(::testing::_, ::testing::_, ::testing::_,
                                      ::testing::_))
      .WillOnce(::testing::Invoke(
          [](int32_t a, int32_t b,
             mojo::PendingAssociatedReceiver<gpu::mojom::StreamTexture>,
             int32_t* out_value) {
            *out_value = 1;
            return true;
          }));

  proxy = textureFactory_->CreateProxy(
      gl::ohos::TextureOwnerMode::kNativeImageTexture);
  EXPECT_NE(proxy, nullptr);
}

TEST_F(NativeTextureFactoryTest, SharedImageInterface001) {
  ASSERT_NE(textureFactory_, nullptr);
  EXPECT_EQ(textureFactory_->shared_image_interface_, nullptr);
  auto interface = textureFactory_->SharedImageInterface();
  EXPECT_NE(interface, nullptr);
  auto interface1 = textureFactory_->SharedImageInterface();
  EXPECT_EQ(interface1, interface);
}
}  // namespace content

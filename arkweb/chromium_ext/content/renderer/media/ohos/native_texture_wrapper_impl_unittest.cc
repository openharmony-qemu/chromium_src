// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on stream_texture_wrapper_impl.cc originally written by
// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "content/renderer/media/ohos/native_texture_wrapper_impl.h"
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

class MockClientSharedImageInterface : public gpu::ClientSharedImageInterface {
 public:
  MockClientSharedImageInterface(gpu::SharedImageInterfaceProxy* proxy,
                                 scoped_refptr<gpu::GpuChannelHost> channel)
      : gpu::ClientSharedImageInterface(proxy, channel) {};
  // gpu::SharedImageInterface implementation.
  MOCK_METHOD2(
      CreateSharedImage,
      scoped_refptr<gpu::ClientSharedImage>(const gpu::SharedImageInfo& si_info,
                                            gpu::SurfaceHandle surface_handle));
  MOCK_METHOD2(CreateSharedImage,
               scoped_refptr<gpu::ClientSharedImage>(
                   const gpu::SharedImageInfo& si_info,
                   base::span<const uint8_t> pixel_data));
  MOCK_METHOD4(CreateSharedImage,
               scoped_refptr<gpu::ClientSharedImage>(
                   const gpu::SharedImageInfo& si_info,
                   gpu::SurfaceHandle surface_handle,
                   gfx::BufferUsage buffer_usage,
                   gfx::GpuMemoryBufferHandle buffer_handle));
  MOCK_METHOD2(CreateSharedImage,
               scoped_refptr<gpu::ClientSharedImage>(
                   const gpu::SharedImageInfo& si_info,
                   gfx::GpuMemoryBufferHandle buffer_handle));
  MOCK_METHOD1(CreateSharedImage,
               SharedImageMapping(const gpu::SharedImageInfo& si_info));
  MOCK_METHOD2(UpdateSharedImage,
               void(const gpu::SyncToken& sync_token,
                    const gpu::Mailbox& mailbox));
  MOCK_METHOD2(DestroySharedImage,
               void(const gpu::SyncToken& sync_token,
                    const gpu::Mailbox& mailbox));
  MOCK_METHOD2(DestroySharedImage,
               void(const gpu::SyncToken& sync_token,
                    scoped_refptr<gpu::ClientSharedImage> client_shared_image));
  MOCK_METHOD1(ImportSharedImage,
               scoped_refptr<gpu::ClientSharedImage>(
                   const gpu::ExportedSharedImage& exported_shared_image));
  MOCK_METHOD6(CreateSwapChain,
               SwapChainSharedImages(viz::SharedImageFormat format,
                                     const gfx::Size& size,
                                     const gfx::ColorSpace& color_space,
                                     GrSurfaceOrigin surface_origin,
                                     SkAlphaType alpha_type,
                                     gpu::SharedImageUsageSet usage));
  MOCK_METHOD2(PresentSwapChain,
               void(const gpu::SyncToken& sync_token,
                    const gpu::Mailbox& mailbox));
  MOCK_METHOD0(GenUnverifiedSyncToken, gpu::SyncToken());
  MOCK_METHOD0(GenVerifiedSyncToken, gpu::SyncToken());
  MOCK_METHOD1(VerifySyncToken, void(gpu::SyncToken& sync_token));
  MOCK_METHOD1(WaitSyncToken, void(const gpu::SyncToken& sync_token));
  MOCK_METHOD0(Flush, void());
  MOCK_METHOD1(UsageForMailbox,
               gpu::SharedImageUsageSet(const gpu::Mailbox& mailbox));

  // Informs that existing |mailbox| with the specified metadata can be passed
  // to DestroySharedImage().
  MOCK_METHOD7(
      NotifyMailboxAdded1,
      scoped_refptr<gpu::ClientSharedImage>(const gpu::Mailbox& mailbox,
                                            viz::SharedImageFormat format,
                                            const gfx::Size& size,
                                            const gfx::ColorSpace& color_space,
                                            GrSurfaceOrigin surface_origin,
                                            SkAlphaType alpha_type,
                                            gpu::SharedImageUsageSet usage));
  MOCK_METHOD1(GetNativePixmap,
               scoped_refptr<gfx::NativePixmap>(const gpu::Mailbox& mailbox));
  MOCK_METHOD8(
      NotifyMailboxAddedWithTarget,
      scoped_refptr<gpu::ClientSharedImage>(const gpu::Mailbox& mailbox,
                                            viz::SharedImageFormat format,
                                            const gfx::Size& size,
                                            const gfx::ColorSpace& color_space,
                                            GrSurfaceOrigin surface_origin,
                                            SkAlphaType alpha_type,
                                            gpu::SharedImageUsageSet usage,
                                            uint32_t texture_target));
  virtual scoped_refptr<gpu::ClientSharedImage> NotifyMailboxAdded(
      const gpu::Mailbox& mailbox,
      viz::SharedImageFormat format,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      gpu::SharedImageUsageSet usage) override {
    return NotifyMailboxAdded1(mailbox, format, size, color_space,
                               surface_origin, alpha_type, usage);
  }

  virtual scoped_refptr<gpu::ClientSharedImage> NotifyMailboxAdded(
      const gpu::Mailbox& mailbox,
      viz::SharedImageFormat format,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      gpu::SharedImageUsageSet usage,
      uint32_t texture_target) override {
    return NotifyMailboxAddedWithTarget(mailbox, format, size, color_space,
                                        surface_origin, alpha_type, usage,
                                        texture_target);
  }
  MOCK_METHOD0(GetCapabilities, const gpu::SharedImageCapabilities&());

 protected:
  ~MockClientSharedImageInterface() override = default;
};

namespace content {
class TestGpuChannelHost : public gpu::GpuChannelHost {
 public:
  explicit TestGpuChannelHost(gpu::mojom::GpuChannel& gpu_channel)
      : GpuChannelHost(0,
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

class NativeTextureWrapperImplTest : public testing::Test {
 public:
  NativeTextureWrapperImplTest() {}
  ~NativeTextureWrapperImplTest() override {}
  struct NativeTextureWrapperImplDeleter {
    void operator()(NativeTextureWrapperImpl* ptr) {
      if (ptr) {
        ptr->Destroy();
      }
    }
  };

 protected:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    channel_ = base::MakeRefCounted<TestGpuChannelHost>(mock_gpu_channel_);
    factory_ = NativeTextureFactory::Create(channel_);
    ON_CALL(mock_gpu_channel_,
            CreateNativeTextureReturnBool(::testing::_, ::testing::_, ::testing::_,
                                    ::testing::_))
        .WillByDefault(::testing::Invoke(
            [](int32_t a, int32_t b,
               mojo::PendingAssociatedReceiver<gpu::mojom::StreamTexture>,
               int32_t* out_value) {
              *out_value = 1;
              return true;
            }));

    impl_ = std::unique_ptr<NativeTextureWrapperImpl,
                            NativeTextureWrapperImplDeleter>(
        new NativeTextureWrapperImpl(
            true, gl::ohos::TextureOwnerMode::kNone, factory_,
            base::SingleThreadTaskRunner::GetCurrentDefault()),
        NativeTextureWrapperImplDeleter{});
    impl_->compositor_task_runner_ =
        base::MakeRefCounted<MockSingleThreadTaskRunner>();
    ASSERT_NE(impl_, nullptr);
  }

  void InitializeOnMainThread() {
    scoped_refptr<base::SingleThreadTaskRunner> task_runner =
        base::MakeRefCounted<MockSingleThreadTaskRunner>();
    auto mock_task_runner =
        static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
    ON_CALL(*mock_task_runner, BelongsToCurrentThread())
        .WillByDefault(testing::Return(true));
    impl_->main_task_runner_ = task_runner;

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
    base::RepeatingClosure received_frame_cb = base::DoNothing();
    bool init_result = false;
    base::OnceCallback<void(bool)> init_cb = base::BindOnce(
        [](bool* result, bool success) { *result = success; }, &init_result);
    base::OnceCallback<void(int)> create_texture_cb = base::DoNothing();
    base::RepeatingCallback<void()> destroy_texture_cb = base::DoNothing();
    impl_->InitializeOnMainThread(received_frame_cb, std::move(init_cb),
                                  std::move(create_texture_cb),
                                  destroy_texture_cb);
  }

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  gpu::MockGpuChannel mock_gpu_channel_;
  scoped_refptr<NativeTextureFactory> factory_;
  std::unique_ptr<NativeTextureWrapperImpl, NativeTextureWrapperImplDeleter>
      impl_;
  scoped_refptr<TestGpuChannelHost> channel_;
};

TEST_F(NativeTextureWrapperImplTest, NativeTextureWrapperImpRelease) {
  EXPECT_NO_FATAL_FAILURE(impl_->native_texture_proxy_.reset());
}

TEST_F(NativeTextureWrapperImplTest, CreateVideoFrame001) {
  gpu::SharedImageInterfaceProxy proxy = gpu::SharedImageInterfaceProxy(
      channel_.get(), 0, gpu::SharedImageCapabilities());
  scoped_refptr<gpu::ClientSharedImageInterface> shared_image =
      base::MakeRefCounted<MockClientSharedImageInterface>(&proxy, channel_);
  auto mock_image =
      static_cast<MockClientSharedImageInterface*>(shared_image.get());
  factory_->shared_image_interface_ = std::move(shared_image);
  auto test_shared_image = gpu::ClientSharedImage::CreateForTesting();
  EXPECT_CALL(*mock_image, NotifyMailboxAddedWithTarget(
                               testing::_, testing::_, testing::_, testing::_,
                               testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(
          testing::Return(test_shared_image));

  gpu::Mailbox mailbox;
  gfx::Size coded_size{100, 100};
  gfx::Rect visible_rect{0, 0, 100, 100};
  absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;

  impl_->current_frame_.reset();
  impl_->enable_texture_copy_ = false;
  impl_->CreateVideoFrame(mailbox, coded_size, visible_rect, ycbcr_info);
  EXPECT_NE(impl_->current_frame_, nullptr);
}

TEST_F(NativeTextureWrapperImplTest, CreateVideoFrame002) {
  gpu::SharedImageInterfaceProxy proxy = gpu::SharedImageInterfaceProxy(
      channel_.get(), 0, gpu::SharedImageCapabilities());
  scoped_refptr<gpu::ClientSharedImageInterface> shared_image =
      base::MakeRefCounted<MockClientSharedImageInterface>(&proxy, channel_);
  auto mock_image =
      static_cast<MockClientSharedImageInterface*>(shared_image.get());
  factory_->shared_image_interface_ = std::move(shared_image);
  auto test_shared_image = gpu::ClientSharedImage::CreateForTesting();
  EXPECT_CALL(*mock_image, NotifyMailboxAddedWithTarget(
                               testing::_, testing::_, testing::_, testing::_,
                               testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(
          testing::Return(test_shared_image));
  gpu::Mailbox mailbox;
  gfx::Size coded_size{100, 100};
  gfx::Rect visible_rect{0, 0, 100, 100};
  absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;
  impl_->current_frame_.reset();
  impl_->enable_texture_copy_ = true;
  impl_->CreateVideoFrame(mailbox, coded_size, visible_rect, ycbcr_info);
  EXPECT_NE(impl_->current_frame_, nullptr);
}

TEST_F(NativeTextureWrapperImplTest, ClearCBOnAnyThread001) {
  impl_->ClearCBOnAnyThread();
  InitializeOnMainThread();
  impl_->ClearCBOnAnyThread();
  EXPECT_TRUE(impl_->native_texture_proxy_->received_frame_cb_.is_null());
}

TEST_F(NativeTextureWrapperImplTest, UpdateTextureSize001) {
  gfx::Size new_size{640, 480};
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(true));
  impl_->main_task_runner_ = task_runner;
  impl_->UpdateTextureSize(new_size);
  EXPECT_NE(impl_->rotated_visible_size_, new_size);
}

TEST_F(NativeTextureWrapperImplTest, UpdateTextureSize002) {
  gfx::Size new_size{640, 480};
  impl_->native_texture_proxy_.reset();
  impl_->UpdateTextureSize(new_size);
  EXPECT_NE(impl_->rotated_visible_size_, new_size);
}

TEST_F(NativeTextureWrapperImplTest, UpdateTextureSize003) {
  InitializeOnMainThread();
  impl_->rotated_visible_size_.set_width(640);
  impl_->rotated_visible_size_.set_height(480);
  gfx::Size new_size{640, 480};
  impl_->UpdateTextureSize(new_size);
  EXPECT_EQ(impl_->rotated_visible_size_, new_size);
}

TEST_F(NativeTextureWrapperImplTest, UpdateTextureSize004) {
  InitializeOnMainThread();
  impl_->rotated_visible_size_.set_width(640);
  impl_->rotated_visible_size_.set_height(480);
  gfx::Size new_size{640, 280};
  impl_->UpdateTextureSize(new_size);
  EXPECT_EQ(impl_->rotated_visible_size_, new_size);
}

TEST_F(NativeTextureWrapperImplTest, InitializeOnMainThread001) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(true));
  impl_->main_task_runner_ = task_runner;
  bool init_result = false;
  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void(bool)> init_cb = base::BindOnce(
      [](bool* result, bool success) { *result = success; }, &init_result);
  base::OnceCallback<void(int)> create_texture_cb = base::DoNothing();
  base::RepeatingCallback<void()> destroy_texture_cb = base::DoNothing();
  impl_->factory_ = nullptr;
  impl_->InitializeOnMainThread(received_frame_cb, std::move(init_cb),
                                std::move(create_texture_cb),
                                destroy_texture_cb);

  EXPECT_FALSE(init_result);
}

TEST_F(NativeTextureWrapperImplTest, InitializeOnMainThread002) {
  bool init_result = false;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(true));
  impl_->main_task_runner_ = task_runner;
  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void(bool)> init_cb = base::BindOnce(
      [](bool* result, bool success) { *result = success; }, &init_result);
  base::OnceCallback<void(int)> create_texture_cb = base::DoNothing();
  base::RepeatingCallback<void()> destroy_texture_cb = base::DoNothing();
  ON_CALL(mock_gpu_channel_,
          CreateNativeTextureReturnBool(::testing::_, ::testing::_, ::testing::_,
                                  ::testing::_))
      .WillByDefault(::testing::Invoke(
          [](int32_t a, int32_t b,
             mojo::PendingAssociatedReceiver<gpu::mojom::StreamTexture>,
             int32_t* out_value) {
            *out_value = -1;
            return true;
          }));
  impl_->InitializeOnMainThread(received_frame_cb, std::move(init_cb),
                                std::move(create_texture_cb),
                                destroy_texture_cb);

  EXPECT_FALSE(init_result);
}

TEST_F(NativeTextureWrapperImplTest, InitializeOnMainThread003) {
  bool init_result = false;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  ON_CALL(*mock_task_runner, BelongsToCurrentThread())
      .WillByDefault(testing::Return(true));
  impl_->main_task_runner_ = task_runner;

  base::RepeatingClosure received_frame_cb = base::DoNothing();
  base::OnceCallback<void(bool)> init_cb = base::BindOnce(
      [](bool* result, bool success) { *result = success; }, &init_result);
  base::OnceCallback<void(int)> create_texture_cb = base::DoNothing();
  base::RepeatingCallback<void()> destroy_texture_cb = base::DoNothing();
  impl_->InitializeOnMainThread(received_frame_cb, std::move(init_cb),
                                std::move(create_texture_cb),
                                destroy_texture_cb);

  EXPECT_TRUE(init_result);
}
}  // namespace content

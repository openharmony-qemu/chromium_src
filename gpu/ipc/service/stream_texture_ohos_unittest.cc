/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <memory>
#define private public
#include "gpu/ipc/service/stream_texture_ohos.h"
#undef private
#include "command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/gpu_timing.h"
#include "ui/gl/scoped_make_current.h"

namespace gpu {
namespace {
std::unique_ptr<ui::ScopedMakeCurrent> MakeCurrent(
    SharedContextState* context_state) {
  std::unique_ptr<ui::ScopedMakeCurrent> scoped_make_current;
  bool needs_make_current =
      !context_state->IsCurrent(nullptr, /*needs_gl=*/true);
  if (needs_make_current) {
    scoped_make_current = std::make_unique<ui::ScopedMakeCurrent>(
        context_state->context(), context_state->surface());
  }
  return scoped_make_current;
}
}  // namespace

class MockSingleThreadTaskRunner : public base::SingleThreadTaskRunner {
 public:
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

class StreamTextureOhosTest : public GpuChannelTestCommon {
 public:
  StreamTextureOhosTest()
      : GpuChannelTestCommon(true /* use_stub_bindings */) {}
  ~StreamTextureOhosTest() override = default;
};

class MockGLSurface : public gl::GLSurface {
 public:
  MockGLSurface() = default;
  ~MockGLSurface() override = default;
  MOCK_METHOD(bool, Initialize, (gl::GLSurfaceFormat format), ());
  MOCK_METHOD(void, Destroy, (), ());
  MOCK_METHOD(bool,
              Resize,
              (const gfx::Size&, float, const gfx::ColorSpace&, bool));
  MOCK_METHOD(bool, Recreate, (), ());
  MOCK_METHOD(bool, DeferDraws, (), ());
  MOCK_METHOD(bool, IsOffscreen, (), ());
  using PresentationCallback =
      base::OnceCallback<void(const gfx::PresentationFeedback& feedback)>;
  MOCK_METHOD(gfx::SwapResult,
              SwapBuffers,
              (PresentationCallback callback, gfx::FrameData data));
  MOCK_METHOD(gfx::Size, GetSize, (), ());
  MOCK_METHOD(void*, GetHandle, (), ());
  MOCK_METHOD(bool, SupportsSwapBuffersWithBounds, (), ());
#if BUILDFLAG(IS_OHOS)
  MOCK_METHOD(gfx::SwapResult,
              SwapBuffersWithDamage,
              (const std::vector<int>& rects,
               PresentationCallback callback,
               gfx::FrameData data),
              ());
#endif
  MOCK_METHOD(bool, SupportsPostSubBuffer, (), ());
  MOCK_METHOD(bool, SupportsAsyncSwap, (), ());
  MOCK_METHOD(unsigned int, GetBackingFramebufferObject, (), ());
  using SwapCompletionCallback =
      base::OnceCallback<void(gfx::SwapCompletionResult)>;
  MOCK_METHOD(void,
              SwapBuffersAsync,
              (SwapCompletionCallback completion_callback,
               PresentationCallback presentation_callback,
               gfx::FrameData data),
              ());
  MOCK_METHOD(gfx::SwapResult,
              SwapBuffersWithBounds,
              (const std::vector<gfx::Rect>& rects,
               PresentationCallback callback,
               gfx::FrameData data),
              ());
  MOCK_METHOD(gfx::SwapResult,
              PostSubBuffer,
              (int x,
               int y,
               int width,
               int height,
               PresentationCallback callback,
               gfx::FrameData data),
              ());
  MOCK_METHOD(void,
              PostSubBufferAsync,
              (int x,
               int y,
               int width,
               int height,
               SwapCompletionCallback completion_callback,
               PresentationCallback presentation_callback,
               gfx::FrameData data),
              ());
  MOCK_METHOD(bool, OnMakeCurrent, (gl::GLContext * context), ());
  MOCK_METHOD(bool, SetBackbufferAllocation, (bool allocated), ());
  MOCK_METHOD(void, SetFrontbufferAllocation, (bool allocated), ());
  MOCK_METHOD(void*, GetShareHandle, (), ());
  MOCK_METHOD(gl::GLDisplay*, GetGLDisplay, (), ());
  MOCK_METHOD(void*, GetConfig, (), ());
  MOCK_METHOD(gl::GLSurfaceFormat, GetFormat, (), ());
  MOCK_METHOD(gfx::VSyncProvider*, GetVSyncProvider, (), ());
  MOCK_METHOD(void, SetVSyncEnabled, (bool enabled), ());

  MOCK_METHOD(bool, SetEnableDCLayers, (bool enable), ());
  MOCK_METHOD(bool, IsSurfaceless, (), (const));
  MOCK_METHOD(gfx::SurfaceOrigin, GetOrigin, (), (const));
  MOCK_METHOD(bool, BuffersFlipped, (), (const));
  MOCK_METHOD(bool, SupportsDCLayers, (), (const));
  MOCK_METHOD(bool, SupportsProtectedVideo, (), (const));
  MOCK_METHOD(bool, SupportsOverridePlatformSize, (), (const));
  MOCK_METHOD(bool, SetDrawRectangle, (const gfx::Rect& rect), ());
  MOCK_METHOD(gfx::Vector2d, GetDrawOffset, (), (const));
  MOCK_METHOD(bool, SupportsSwapTimestamps, (), (const));
  MOCK_METHOD(void, SetEnableSwapTimestamps, (), ());
  MOCK_METHOD(bool, SupportsPlaneGpuFences, (), (const));
  MOCK_METHOD(int, GetBufferCount, (), (const));
  MOCK_METHOD(gl::EGLTimestampClient*, GetEGLTimestampClient, (), ());
  MOCK_METHOD(bool, SupportsGpuVSync, (), (const));
  MOCK_METHOD(void, SetGpuVSyncEnabled, (bool enabled), ());
  MOCK_METHOD(void, SetFrameRate, (float frame_rate), ());
  MOCK_METHOD(gl::GLSurface*, GetCurrent, (), ());
  MOCK_METHOD(void, SetCurrent, (), ());
  MOCK_METHOD(bool, IsCurrent, (), ());
  MOCK_METHOD(bool,
              ExtensionsContain,
              (const char* extensions, const char* name),
              ());
  MOCK_METHOD(bool, SupportsDelegatedInk, (), ());
  MOCK_METHOD(void,
              SetDelegatedInkTrailStartPoint,
              (std::unique_ptr<gfx::DelegatedInkMetadata> metadata));
  MOCK_METHOD(void,
              InitDelegatedInkPointRendererReceiver,
              (mojo::PendingReceiver<gfx::mojom::DelegatedInkPointRenderer>
                   pending_receiver));
};

class MockGLContext : public gl::GLContext {
 public:
  explicit MockGLContext(gl::GLShareGroup* share_group)
      : GLContext(share_group) {}
  ~MockGLContext() override = default;
  MOCK_METHOD(bool,
              Initialize,
              (gl::GLSurface * compatible_surface,
               const gl::GLContextAttribs& attribs),
              ());
  MOCK_METHOD(bool, MakeCurrent, (gl::GLSurface * surface), ());
  MOCK_METHOD(void, ReleaseCurrent, (gl::GLSurface * surface), ());
  MOCK_METHOD(bool, IsCurrent, (gl::GLSurface * surface), ());
  MOCK_METHOD(void*, GetHandle, (), ());
  MOCK_METHOD(scoped_refptr<gl::GPUTimingClient>,
              CreateGPUTimingClient,
              (),
              ());
  MOCK_METHOD(void,
              SetGLWorkarounds,
              (const gl::GLWorkarounds& workarounds),
              ());
  MOCK_METHOD(void,
              SetDisabledGLExtensions,
              (const std::string& disabled_gl_extensions),
              ());
  MOCK_METHOD(gl::GLStateRestorer*, GetGLStateRestorer, (), ());
  MOCK_METHOD(void,
              SetGLStateRestorer,
              (gl::GLStateRestorer * state_restorer),
              ());
  MOCK_METHOD(const gfx::ExtensionSet&, GetExtensions, (), ());
  MOCK_METHOD(void, SetSafeToForceGpuSwitch, (), ());
  MOCK_METHOD(bool, ForceGpuSwitchIfNeeded, (), ());
  MOCK_METHOD(void, SetUnbindFboOnMakeCurrent, (), ());
  MOCK_METHOD(void, SetVisibility, (bool visibility), ());
  MOCK_METHOD(bool, HasExtension, (const char* name), ());
  MOCK_METHOD(const gl::GLVersionInfo*, GetVersionInfo, (), ());
  MOCK_METHOD(gl::GLShareGroup*, share_group, (), ());
  MOCK_METHOD(unsigned int, CheckStickyGraphicsResetStatus, (), ());
  MOCK_METHOD(bool,
              MakeVirtuallyCurrent,
              (GLContext * virtual_context, gl::GLSurface* surface),
              ());
  MOCK_METHOD(void,
              OnReleaseVirtuallyCurrent,
              (GLContext * virtual_context),
              ());
  MOCK_METHOD(std::string, GetGLVersion, (), ());
  MOCK_METHOD(std::string, GetGLRenderer, (), ());
  MOCK_METHOD(gl::CurrentGL*, GetCurrentGL, (), ());
  MOCK_METHOD(void, ReinitializeDynamicBindings, (), ());
  MOCK_METHOD(void, ForceReleaseVirtuallyCurrent, (), ());
  MOCK_METHOD(void, DirtyVirtualContextState, (), ());
#if defined(USE_EGL)
  MOCK_METHOD(gl::GLDisplayEGL*, GetGLDisplayEGL, (), ());
#endif  // USE_EGL

  MOCK_METHOD(gl::GLApi*, CreateGLApi, (gl::DriverGL * driver), ());
  MOCK_METHOD(void, BindGLApi, (), ());
  MOCK_METHOD(void, SetCurrent, (gl::GLSurface * surface), ());
  MOCK_METHOD(void, InitializeDynamicBindings, (), ());
  MOCK_METHOD(bool, MakeCurrentImpl, (gl::GLSurface * surface), ());
  MOCK_METHOD(unsigned int, CheckStickyGraphicsResetStatusImpl, (), ());
  MOCK_METHOD(void, ResetExtensions, (), ());
  MOCK_METHOD(std::unique_ptr<gl::GLVersionInfo>,
              GenerateGLVersionInfo,
              (),
              ());
};

TEST_F(StreamTextureOhosTest, Create) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  EXPECT_NE(StreamTexture, nullptr);
}

TEST_F(StreamTextureOhosTest, MakeCurrent_001) {
  using ContextLostCallback = base::OnceCallback<void(bool)>;
  scoped_refptr<gl::GLShareGroup> share_group(new gl::GLShareGroup());
  scoped_refptr<gpu::MockGLSurface> surface(new gpu::MockGLSurface());
  std::unique_ptr<gpu::MockGLContext> context =
      std::make_unique<gpu::MockGLContext>(share_group.get());
  EXPECT_CALL(*context, IsCurrent(testing::_))
      .WillRepeatedly(testing::Return(true));
  ContextLostCallback context_lost_callback = base::BindOnce([](bool) {});
  gpu::SharedContextState* shared_context_state(new gpu::SharedContextState(
      share_group, surface, context.get(), false,
      std::move(context_lost_callback), GrContextType::kGL));
  gpu::MakeCurrent(shared_context_state);
  EXPECT_FALSE(!shared_context_state->IsCurrent(nullptr, true));
}

TEST_F(StreamTextureOhosTest, MakeCurrent_002) {
  using ContextLostCallback = base::OnceCallback<void(bool)>;
  scoped_refptr<gl::GLShareGroup> share_group(new gl::GLShareGroup());
  scoped_refptr<gpu::MockGLSurface> surface(new gpu::MockGLSurface());
  std::unique_ptr<gpu::MockGLContext> context =
      std::make_unique<gpu::MockGLContext>(share_group.get());
  EXPECT_CALL(*context, IsCurrent(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*context, ReleaseCurrent(testing::_));
  EXPECT_CALL(*context, MakeCurrentImpl(testing::_))
      .WillRepeatedly(testing::Return(true));
  ContextLostCallback context_lost_callback = base::BindOnce([](bool) {});
  gpu::SharedContextState* shared_context_state(new gpu::SharedContextState(
      share_group, surface, context.get(), false,
      std::move(context_lost_callback), GrContextType::kGL));
  gpu::MakeCurrent(shared_context_state);
  EXPECT_TRUE(!shared_context_state->IsCurrent(nullptr, true));
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_001) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  StreamTexture->OnFrameAvailable();
  EXPECT_FALSE(StreamTexture->client_);
  EXPECT_TRUE(StreamTexture->native_texture_owner_);
  EXPECT_TRUE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_002) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  mojo::PendingAssociatedRemote<mojom::StreamTextureClient> pending_bar;
  mojo::PendingAssociatedReceiver<mojom::StreamTextureClient> bar_receiver =
      pending_bar.InitWithNewEndpointAndPassReceiver();
  StreamTexture->StartListening(std::move(pending_bar));
  StreamTexture->OnFrameAvailable();
  EXPECT_TRUE(StreamTexture->client_);
  EXPECT_TRUE(StreamTexture->native_texture_owner_);
  EXPECT_TRUE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_003) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  StreamTexture->OnFrameAvailable();
  EXPECT_FALSE(StreamTexture->client_);
  EXPECT_TRUE(StreamTexture->native_texture_owner_);
  EXPECT_TRUE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_004) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  StreamTexture->native_texture_owner_ = nullptr;
  StreamTexture->OnFrameAvailable();
  EXPECT_FALSE(StreamTexture->client_);
  EXPECT_FALSE(StreamTexture->native_texture_owner_);
  EXPECT_TRUE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_005) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  StreamTexture->OnFrameAvailable();
  EXPECT_FALSE(StreamTexture->client_);
  EXPECT_TRUE(StreamTexture->native_texture_owner_);
  EXPECT_TRUE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_006) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  StreamTexture->channel_ = nullptr;
  StreamTexture->OnFrameAvailable();
  EXPECT_FALSE(StreamTexture->client_);
  EXPECT_TRUE(StreamTexture->native_texture_owner_);
  EXPECT_FALSE(StreamTexture->channel_);
}

TEST_F(StreamTextureOhosTest, OnFrameAvailable_007) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> StreamTexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  mojo::PendingAssociatedRemote<mojom::StreamTextureClient> pending_bar;
  mojo::PendingAssociatedReceiver<mojom::StreamTextureClient> bar_receiver =
      pending_bar.InitWithNewEndpointAndPassReceiver();
  StreamTexture->StartListening(std::move(pending_bar));
  StreamTexture->OnFrameAvailable();
  ASSERT_TRUE(StreamTexture->rotated_visible_size_.IsEmpty());
}

TEST_F(StreamTextureOhosTest, RunCallback_001) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  EXPECT_CALL(*mock_task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_task_runner, PostDelayedTask)
      .WillOnce(testing::Return(false));
  StreamTexture::RunCallback(task_runner,
                             streamtexture->weak_factory_.GetWeakPtr());
}

TEST_F(StreamTextureOhosTest, RunCallback_002) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  EXPECT_CALL(*mock_task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(true));
  auto weak = streamtexture->weak_factory_.GetWeakPtr();
  weak->has_pending_frame_ = false;
  StreamTexture::RunCallback(task_runner, weak);
  EXPECT_TRUE(weak->has_pending_frame_);
}

TEST_F(StreamTextureOhosTest, RunCallback_003) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_task_runner =
      static_cast<MockSingleThreadTaskRunner*>(task_runner.get());
  EXPECT_CALL(*mock_task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(true));
  auto weak = streamtexture->weak_factory_.GetWeakPtr();
  weak->has_pending_frame_ = false;
  StreamTexture::RunCallback(task_runner, nullptr);
  EXPECT_FALSE(weak->has_pending_frame_);
}

TEST_F(StreamTextureOhosTest, UpdateRotatedVisibleSize_001) {
  gfx::Size rotated_visible_size(1, 1);
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  streamtexture->rotated_visible_size_ = rotated_visible_size;
  streamtexture->UpdateRotatedVisibleSize(rotated_visible_size);
  EXPECT_FALSE(streamtexture->rotated_visible_size_.IsEmpty());
}

TEST_F(StreamTextureOhosTest, UpdateRotatedVisibleSize_002) {
  gfx::Size rotated_visible_size;
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  streamtexture->UpdateRotatedVisibleSize(rotated_visible_size);
  EXPECT_TRUE(streamtexture->rotated_visible_size_.IsEmpty());
}

TEST_F(StreamTextureOhosTest, UpdateRotatedVisibleSize_003) {
  gfx::Size rotated_visible_size(1, 1);
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  streamtexture->rotated_visible_size_ = rotated_visible_size;
  streamtexture->has_pending_frame_ = true;
  streamtexture->UpdateRotatedVisibleSize(rotated_visible_size);
  EXPECT_TRUE(streamtexture->has_pending_frame_);
}

TEST_F(StreamTextureOhosTest, UpdateRotatedVisibleSize_004) {
  gfx::Size rotated_visible_size;
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  streamtexture->rotated_visible_size_ = rotated_visible_size;
  streamtexture->has_pending_frame_ = true;
  streamtexture->UpdateRotatedVisibleSize(rotated_visible_size);
  EXPECT_TRUE(streamtexture->has_pending_frame_);
}

TEST_F(StreamTextureOhosTest, NativeEmbedID_001) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  EXPECT_EQ(streamtexture->native_embed_id_, -1);
  int result = streamtexture->NativeEmbedID();
  EXPECT_EQ(result, 1);
}

TEST_F(StreamTextureOhosTest, NativeEmbedID_002) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  scoped_refptr<StreamTexture> streamtexture =
      StreamTexture::Create(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  streamtexture->native_embed_id_ = 0;
  int result = streamtexture->NativeEmbedID();
  EXPECT_EQ(result, 0);
}
}  // namespace gpu

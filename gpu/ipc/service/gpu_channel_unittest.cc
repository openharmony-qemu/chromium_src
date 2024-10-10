// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstddef>
#include "command_buffer/service/ohos/native_image_texture_owner.h"
#define private public
#include "gpu/ipc/service/gpu_channel.cc"
#include "gpu/ipc/service/gpu_channel.h"
#undef private

#include <stdint.h>

#include "base/run_loop.h"
#include "base/test/bind.h"
#if defined(OHOS_UNITTESTS)
#define protected public
#endif  // OHOS_UNITTESTS
#include "base/test/test_simple_task_runner.h"
#if defined(OHOS_UNITTESTS)
#undef protected
#endif  // OHOS_UNITTESTS
#include "build/build_config.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"

namespace gpu {

class GpuChannelTest : public GpuChannelTestCommon {
 public:
  GpuChannelTest() : GpuChannelTestCommon(true /* use_stub_bindings */) {}
  ~GpuChannelTest() override = default;
};

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_IOS)
const SurfaceHandle kFakeSurfaceHandle = reinterpret_cast<SurfaceHandle>(1);
#else
const SurfaceHandle kFakeSurfaceHandle = 1;
#endif

TEST_F(GpuChannelTest, CreateViewCommandBufferAllowed) {
  // TODO(crbug/1406585): Currently it's not possible to create onscreen
  // GLSurface with Null binding with angle.
  if (channel_manager()->use_passthrough_cmd_decoder()) {
    GTEST_SKIP();
  }

  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  ASSERT_TRUE(channel);

  SurfaceHandle surface_handle = kFakeSurfaceHandle;
  DCHECK_NE(surface_handle, kNullSurfaceHandle);

  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = surface_handle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kSuccess);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  ASSERT_TRUE(stub);
}

TEST_F(GpuChannelTest, CreateViewCommandBufferDisallowed) {
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  ASSERT_TRUE(channel);

  SurfaceHandle surface_handle = kFakeSurfaceHandle;
  DCHECK_NE(surface_handle, kNullSurfaceHandle);

  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = surface_handle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kFatalFailure);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  EXPECT_FALSE(stub);
}

TEST_F(GpuChannelTest, CreateOffscreenCommandBuffer) {
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, true);
  ASSERT_TRUE(channel);

  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = kNullSurfaceHandle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kSuccess);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  EXPECT_TRUE(stub);
}

TEST_F(GpuChannelTest, IncompatibleStreamIds) {
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, true);
  ASSERT_TRUE(channel);

  // Create first context.
  int32_t kRouteId1 =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  int32_t kStreamId1 = 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = kNullSurfaceHandle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = kStreamId1;
  init_params->stream_priority = SchedulingPriority::kNormal;
  auto init_params2 = init_params.Clone();

  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId1,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kSuccess);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId1);
  EXPECT_TRUE(stub);

  // Create second context in same share group but different stream.
  int32_t kRouteId2 = kRouteId1 + 1;
  int32_t kStreamId2 = 2;

  init_params2->share_group_id = kRouteId1;
  init_params2->stream_id = kStreamId2;
  init_params2->stream_priority = SchedulingPriority::kNormal;
  CreateCommandBuffer(*channel, std::move(init_params2), kRouteId2,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kFatalFailure);

  stub = channel->LookupCommandBuffer(kRouteId2);
  EXPECT_FALSE(stub);
}

TEST_F(GpuChannelTest, CreateFailsIfSharedContextIsLost) {
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Create first context, we will share this one.
  int32_t kSharedRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  {
    SCOPED_TRACE("kSharedRouteId");
    auto init_params = mojom::CreateCommandBufferParams::New();
    init_params->surface_handle = kNullSurfaceHandle;
    init_params->share_group_id = MSG_ROUTING_NONE;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kSharedRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities);
    EXPECT_EQ(result, gpu::ContextResult::kSuccess);
  }
  EXPECT_TRUE(channel->LookupCommandBuffer(kSharedRouteId));

  // This context shares with the first one, this should be possible.
  int32_t kFriendlyRouteId = kSharedRouteId + 1;
  {
    SCOPED_TRACE("kFriendlyRouteId");
    auto init_params = mojom::CreateCommandBufferParams::New();
    init_params->surface_handle = kNullSurfaceHandle;
    init_params->share_group_id = kSharedRouteId;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kFriendlyRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities);
    EXPECT_EQ(result, gpu::ContextResult::kSuccess);
  }
  EXPECT_TRUE(channel->LookupCommandBuffer(kFriendlyRouteId));

  // The shared context is lost.
  channel->LookupCommandBuffer(kSharedRouteId)->MarkContextLost();

  // Meanwhile another context is being made pointing to the shared one. This
  // should fail.
  int32_t kAnotherRouteId = kFriendlyRouteId + 1;
  {
    SCOPED_TRACE("kAnotherRouteId");
    auto init_params = mojom::CreateCommandBufferParams::New();
    init_params->surface_handle = kNullSurfaceHandle;
    init_params->share_group_id = kSharedRouteId;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kAnotherRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities);
    EXPECT_EQ(result, gpu::ContextResult::kTransientFailure);
  }
  EXPECT_FALSE(channel->LookupCommandBuffer(kAnotherRouteId));

  // The lost context is still around though (to verify the failure happened due
  // to the shared context being lost, not due to it being deleted).
  EXPECT_TRUE(channel->LookupCommandBuffer(kSharedRouteId));

  // Destroy the command buffers we initialized before destoying GL.
  channel->DestroyCommandBuffer(kFriendlyRouteId);
  channel->DestroyCommandBuffer(kSharedRouteId);
}

class GpuChannelExitForContextLostTest : public GpuChannelTestCommon {
 public:
  GpuChannelExitForContextLostTest()
      : GpuChannelTestCommon({EXIT_ON_CONTEXT_LOST} /* enabled_workarounds */,
                             true /* use_stub_bindings */) {}
};

TEST_F(GpuChannelExitForContextLostTest,
       CreateFailsDuringLostContextShutdown_1) {
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Put channel manager into shutdown state.
  channel_manager()->OnContextLost(-1 /* context_lost_count */,
                                   false /* synthetic_loss */);

  // Calling OnContextLost() above may destroy the gpu channel via post task.
  // Ensure that post task has happened.
  base::RunLoop().RunUntilIdle();

  // If the channel is destroyed, then skip the test.
  if (!channel_manager()->LookupChannel(kClientId)) {
    return;
  }

  // Try to create a context.
  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = kNullSurfaceHandle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kTransientFailure);
  EXPECT_FALSE(channel->LookupCommandBuffer(kRouteId));
}

TEST_F(GpuChannelExitForContextLostTest,
       CreateFailsDuringLostContextShutdown_2) {
  // Put channel manager into shutdown state. Do this before creating a channel,
  // as doing this may destroy any active channels.
  channel_manager()->OnContextLost(-1 /* context_lost_count */,
                                   false /* synthetic_loss */);

  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Try to create a context.
  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
  init_params->surface_handle = kNullSurfaceHandle;
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kTransientFailure);
  EXPECT_FALSE(channel->LookupCommandBuffer(kRouteId));
}

TEST_F(GpuChannelTest, ExecuteDeferredRequest1) {
  mojom::DeferredRequestParamsPtr param;
  auto params = mojom::DeferredRequestParamsPtr();
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  param.ptr_ = nullptr;
  channel->ExecuteDeferredRequest(std::move(param));
  EXPECT_FALSE(params.get());
}

TEST_F(GpuChannelTest, ExecuteDeferredRequest2) {
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  params->tag_ = mojom::DeferredRequestParams::Tag::kDestroyNativeTexture;
  channel->ExecuteDeferredRequest(std::move(params));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("Trying to destroy a non-existent native texture"),
            std::string::npos);
}

TEST_F(GpuChannelTest, CreateNativeTexture1) {
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  int result = channel->CreateNativeTexture(1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  EXPECT_NE(result, -1);
}

TEST_F(GpuChannelTest, CreateNativeTexture2) {
  testing::internal::CaptureStderr();
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  channel->CreateNativeTexture(1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  channel->CreateNativeTexture(1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[NativeEmbed] Trying to create a StreamTexture "
                            "with an existing native_id."),
            std::string::npos);
}

TEST_F(GpuChannelTest, DestroyNativeTexture1) {
  testing::internal::CaptureStderr();
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  int32_t native_id = 10;
  channel->DestroyNativeTexture(native_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find(
          "[NativeEmbed] Trying to destroy a non-existent native texture."),
      std::string::npos);
}

TEST_F(GpuChannelTest, DestroyNativeTexture2) {
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  int32_t native_id = 1;
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  channel->CreateNativeTexture(native_id, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  channel->DestroyNativeTexture(native_id);
  auto found = channel->native_textures_.find(native_id);
  EXPECT_TRUE(found == channel->native_textures_.end());
}

TEST_F(GpuChannelTest, ExecuteDeferredRequest3) {
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  uint32_t num = 5;
  params->tag_ = (mojom::DeferredRequestParams::Tag)num;
  channel->ExecuteDeferredRequest(std::move(params));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("Trying to destroy a non-existent native texture"),
            std::string::npos);
}

TEST_F(GpuChannelTest, TryCreateNativeTexture1) {
  base::WeakPtr<GpuChannel> channel;
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  int32_t result = TryCreateNativeTexture(channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  EXPECT_EQ(result, -1);
}

TEST_F(GpuChannelTest, TryCreateNativeTexture2) {
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  base::WeakPtr<GpuChannel> channell = channel->AsWeakPtr();
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  int32_t result = TryCreateNativeTexture(channell, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
  EXPECT_NE(result, -1);
}

#if defined(OHOS_UNITTESTS)
TEST_F(GpuChannelTest, FlushDeferredRequests1) {
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  const GpuPreferences gpu_preferences;
  Scheduler* scheduler = new Scheduler(new SyncPointManager(), gpu_preferences);
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     main_task_runner);
  std::vector<mojom::DeferredRequestPtr> requests;
  std::vector<SyncToken> sync_token_fences;
  params->tag_ = mojom::DeferredRequestParams::Tag::kDestroyNativeTexture;
  requests.push_back(mojom::DeferredRequest::New(std::move(params),
                                                 std::move(sync_token_fences)));
  gpu_filter.FlushDeferredRequests(std::move(requests));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("Trying to destroy a non-existent nativetexture"),
            std::string::npos);
  delete scheduler;
}

TEST_F(GpuChannelTest, FlushDeferredRequests2) {
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  const GpuPreferences gpu_preferences;
  Scheduler* scheduler = new Scheduler(new SyncPointManager(), gpu_preferences);
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     main_task_runner);
  std::vector<mojom::DeferredRequestPtr> requests;
  std::vector<SyncToken> sync_token_fences;
  uint32_t tag = 5;
  params->tag_ = (mojom::DeferredRequestParams::Tag)tag;
  requests.push_back(mojom::DeferredRequest::New(std::move(params),
                                                 std::move(sync_token_fences)));
  gpu_filter.FlushDeferredRequests(std::move(requests));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(requests.empty());
  delete scheduler;
}

TEST_F(GpuChannelTest, CreateNativeTextureF2) {
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  const GpuPreferences gpu_preferences;
  Scheduler* scheduler = new Scheduler(new SyncPointManager(), gpu_preferences);
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     main_task_runner);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  gpu_filter.CreateNativeTexture(
      kClientId, 1, std::move(receiver),
      base::BindLambdaForTesting([](int32_t value) {}));
  EXPECT_TRUE(gpu_filter.main_task_runner_);
}
#endif  // OHOS_UNITTESTS

}  // namespace gpu

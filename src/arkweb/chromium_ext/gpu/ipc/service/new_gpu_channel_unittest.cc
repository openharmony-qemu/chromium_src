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

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#include <cstddef>
#include "../../command_buffer/service/ohos/native_image_texture_owner.h"
#define private public
#include "gpu/ipc/service/gpu_channel.cc"
#include "gpu/ipc/service/gpu_channel.h"
#undef private
#else
#include "gpu/ipc/service/gpu_channel.h"
#endif // ARKWEB_UNITTESTS

#include "cstdint"

#include "base/run_loop.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#define protected public
#endif  // ARKWEB_UNITTESTS
#include "base/test/bind.h"
#include "base/test/test_simple_task_runner.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#undef protected
#endif  // ARKWEB_UNITTESTS
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

class FenceSyncReleaseDelegateTest : public FenceSyncReleaseDelegate {
public:
    FenceSyncReleaseDelegateTest() : FenceSyncReleaseDelegate(nullptr) {}
};

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_IOS)
const SurfaceHandle kFakeSurfaceHandle = reinterpret_cast<SurfaceHandle>(1);
#else
const SurfaceHandle kFakeSurfaceHandle = 1;
#endif

TEST_F(GpuChannelTest, CreateViewCommandBufferAllowed)
{
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
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kSuccess);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  ASSERT_TRUE(stub);
}

TEST_F(GpuChannelTest, CreateViewCommandBufferDisallowed)
{
  int32_t kClientId = 1;
  bool is_gpu_host = false;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  ASSERT_TRUE(channel);

  SurfaceHandle surface_handle = kFakeSurfaceHandle;
  DCHECK_NE(surface_handle, kNullSurfaceHandle);

  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kFatalFailure);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  EXPECT_FALSE(stub);
}

TEST_F(GpuChannelTest, CreateOffscreenCommandBuffer)
{
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, true);
  ASSERT_TRUE(channel);

  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kSuccess);

  CommandBufferStub* stub = channel->LookupCommandBuffer(kRouteId);
  EXPECT_TRUE(stub);
}

TEST_F(GpuChannelTest, IncompatibleStreamIds)
{
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, true);
  ASSERT_TRUE(channel);

  // Create first context.
  int32_t kRouteId1 =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  int32_t kStreamId1 = 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = kStreamId1;
  init_params->stream_priority = SchedulingPriority::kNormal;
  auto init_params2 = init_params.Clone();

  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId1,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
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
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kFatalFailure);

  stub = channel->LookupCommandBuffer(kRouteId2);
  EXPECT_FALSE(stub);
}

TEST_F(GpuChannelTest, CreateFailsIfSharedContextIsLost)
{
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Create first context, we will share this one.
  int32_t kSharedRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  {
    SCOPED_TRACE("kSharedRouteId");
    auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
    init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
    init_params->share_group_id = MSG_ROUTING_NONE;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    gpu::GLCapabilities gl_capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kSharedRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities,
                        &gl_capabilities);
    EXPECT_EQ(result, gpu::ContextResult::kSuccess);
  }
  EXPECT_TRUE(channel->LookupCommandBuffer(kSharedRouteId));

  // This context shares with the first one, this should be possible.
  int32_t kFriendlyRouteId = kSharedRouteId + 1;
  {
    SCOPED_TRACE("kFriendlyRouteId");
    auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
    init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
    init_params->share_group_id = kSharedRouteId;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    gpu::GLCapabilities gl_capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kSharedRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities,
                        &gl_capabilities);
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
#if BUILDFLAG(IS_ANDROID)
    init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
    init_params->share_group_id = kSharedRouteId;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->active_url = GURL();
    gpu::ContextResult result = gpu::ContextResult::kSuccess;
    gpu::Capabilities capabilities;
    gpu::GLCapabilities gl_capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kSharedRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities,
                        &gl_capabilities);
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
    CreateFailsDuringLostContextShutdown_1)
{
  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Put channel manager into shutdown state.
  channel_manager()->OnContextLost(-1 /* context_lost_count */,
                                   false /* synthetic_loss */,
                                   error::ContextLostReason::kUnknown);

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
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kTransientFailure);
  EXPECT_FALSE(channel->LookupCommandBuffer(kRouteId));
}

TEST_F(GpuChannelExitForContextLostTest,
    CreateFailsDuringLostContextShutdown_2)
{
  // Put channel manager into shutdown state. Do this before creating a channel,
  // as doing this may destroy any active channels.
  channel_manager()->OnContextLost(-1 /* context_lost_count */,
                                   false /* synthetic_loss */,
                                   error::ContextLostReason::kUnknown);

  int32_t kClientId = 1;
  GpuChannel* channel = CreateChannel(kClientId, false);
  ASSERT_TRUE(channel);

  // Try to create a context.
  int32_t kRouteId =
      static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;
  auto init_params = mojom::CreateCommandBufferParams::New();
#if BUILDFLAG(IS_ANDROID)
  init_params->surface_handle = surface_handle;
#endif // IS_ANDROID
  init_params->share_group_id = MSG_ROUTING_NONE;
  init_params->stream_id = 0;
  init_params->stream_priority = SchedulingPriority::kNormal;
  init_params->attribs = ContextCreationAttribs();
  init_params->active_url = GURL();
  gpu::ContextResult result = gpu::ContextResult::kSuccess;
  gpu::Capabilities capabilities;
  gpu::GLCapabilities gl_capabilities;
  CreateCommandBuffer(*channel, std::move(init_params), kRouteId,
                      GetSharedMemoryRegion(), &result, &capabilities,
                      &gl_capabilities);
  EXPECT_EQ(result, gpu::ContextResult::kTransientFailure);
  EXPECT_FALSE(channel->LookupCommandBuffer(kRouteId));
}

#if BUILDFLAG(ARKWEB_UNITTESTS)
TEST_F(GpuChannelTest, ExecuteDeferredRequest1)
{
  mojom::DeferredRequestParamsPtr param;
  auto params = mojom::DeferredRequestParamsPtr();
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  param.ptr_ = nullptr;
  FenceSyncReleaseDelegate* release_delegate = new FenceSyncReleaseDelegateTest();
  channel->ExecuteDeferredRequest(std::move(param), release_delegate);
  EXPECT_FALSE(params.get());
}

TEST_F(GpuChannelTest, ExecuteDeferredRequest2)
{
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  params->tag_ = mojom::DeferredRequestParams::Tag::kDestroyNativeTexture;
  FenceSyncReleaseDelegate* release_delegate = new FenceSyncReleaseDelegateTest();
  channel->ExecuteDeferredRequest(std::move(params), release_delegate);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("Trying to destroy a non-existent native texture"),
            std::string::npos);
}


TEST_F(GpuChannelTest, ExecuteDeferredRequest3)
{
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
  FenceSyncReleaseDelegate* release_delegate = new FenceSyncReleaseDelegateTest();
  channel->ExecuteDeferredRequest(std::move(params), release_delegate);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("Trying to destroy a non-existent native texture"),
            std::string::npos);
}


TEST_F(GpuChannelTest, FlushDeferredRequests1)
{
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  Scheduler* scheduler = new Scheduler(new SyncPointManager());
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  const gfx::GpuExtraInfo info;
  gpu::GpuMemoryBufferFactory* gpu_memory_buffer_factory = nullptr;
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     info, gpu_memory_buffer_factory,
                                     main_task_runner);
  std::vector<mojom::DeferredRequestPtr> requests;
  std::vector<SyncToken> sync_token_fences;
  params->tag_ = mojom::DeferredRequestParams::Tag::kDestroyNativeTexture;
  uint64_t release_count = 1;
  requests.push_back(mojom::DeferredRequest::New(std::move(params),
                                                 std::move(sync_token_fences),
                                                 release_count));
  gpu_filter.FlushDeferredRequests(std::move(requests), 1);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("Trying to destroy a non-existent nativetexture"),
            std::string::npos);
  delete scheduler;
}

TEST_F(GpuChannelTest, FlushDeferredRequests2)
{
  testing::internal::CaptureStderr();
  auto command_buffer_request = mojom::DeferredCommandBufferRequest::New();
  auto params = mojom::DeferredRequestParams::NewCommandBufferRequest(
      std::move(command_buffer_request));
  params->set_destroy_native_texture(1);
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  Scheduler* scheduler = new Scheduler(new SyncPointManager());
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  const gfx::GpuExtraInfo info;
  gpu::GpuMemoryBufferFactory* gpu_memory_buffer_factory = nullptr;
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     info, gpu_memory_buffer_factory,
                                     main_task_runner);
  std::vector<mojom::DeferredRequestPtr> requests;
  std::vector<SyncToken> sync_token_fences;
  uint32_t tag = 5;
  params->tag_ = (mojom::DeferredRequestParams::Tag)tag;
  uint64_t release_count = 1;
  requests.push_back(mojom::DeferredRequest::New(std::move(params),
                                                 std::move(sync_token_fences),
                                                 release_count));
  gpu_filter.FlushDeferredRequests(std::move(requests), 1);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(requests.empty());
  delete scheduler;
}

TEST_F(GpuChannelTest, CreateNativeTextureF2)
{
  int32_t kClientId = 1;
  bool is_gpu_host = true;
  GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
  const base::UnguessableToken channel_token = base::UnguessableToken::Create();
  Scheduler* scheduler = new Scheduler(new SyncPointManager());
  ImageDecodeAcceleratorWorker* image_decode_accelerator_worker = nullptr;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner(
      new base::TestSimpleTaskRunner());
  const gfx::GpuExtraInfo info;
  gpu::GpuMemoryBufferFactory* gpu_memory_buffer_factory = nullptr;
  GpuChannelMessageFilter gpu_filter(channel, channel_token, scheduler,
                                     image_decode_accelerator_worker,
                                     info, gpu_memory_buffer_factory,
                                     main_task_runner);
  mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
  gpu_filter.CreateNativeTexture(
      kClientId, 1, std::move(receiver),
      base::BindLambdaForTesting([](int32_t value) {}));
  EXPECT_TRUE(gpu_filter.main_task_runner_);
}
#endif // ARKWEB_UNITTESTS
}  // namespace gpu

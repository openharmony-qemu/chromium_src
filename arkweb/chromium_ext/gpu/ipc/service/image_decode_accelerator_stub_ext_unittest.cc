/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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


#include "arkweb/chromium_ext/gpu/ipc/service/image_decode_accelerator_stub_ext.h"

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/functional/bind.h"
#include "base/memory/scoped_refptr.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "gpu/command_buffer/common/buffer.h"
#include "gpu/command_buffer/common/context_result.h"
#include "gpu/command_buffer/common/discardable_handle.h"
#include "gpu/command_buffer/service/mocks.h"
#include "gpu/command_buffer/service/service_transfer_cache.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_factory.h"
#include "gpu/command_buffer/service/shared_image/test_image_backing.h"
#include "gpu/command_buffer/service/sync_point_manager.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/command_buffer_stub.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "gpu/ipc/service/image_decode_accelerator_worker.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSize.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/color_space.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "arkweb/chromium_ext/gpu/command_buffer/service/service_transfer_cache_ext.h"
#include "arkweb/chromium_ext/gpu/ipc/service/shared_image_stub_ext.h"

using testing::InSequence;
using testing::Mock;
using testing::NiceMock;
using testing::Return;
using testing::StrictMock;

namespace gpu {

namespace {

// The size of a decoded buffer to report for a successful decode.
constexpr size_t kDecodedBufferByteSize = 123u;

std::unique_ptr<MemoryTracker> CreateMockMemoryTracker() {
  return std::make_unique<NiceMock<gles2::MockMemoryTracker>>();
}

scoped_refptr<Buffer> MakeBufferForTesting() {
  return MakeMemoryBuffer(sizeof(base::subtle::Atomic32));
}

class TestSharedImageBackingFactory : public SharedImageBackingFactory {
 public:
  TestSharedImageBackingFactory() : SharedImageBackingFactory(kUsageAll) {}

  // SharedImageBackingFactory implementation.
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      viz::SharedImageFormat format,
      SurfaceHandle surface_handle,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      SharedImageUsageSet usage,
      std::string debug_label,
      bool is_thread_safe) override {
    NOTREACHED();
  }
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      viz::SharedImageFormat format,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      SharedImageUsageSet usage,
      std::string debug_label,
      bool is_thread_safe,
      base::span<const uint8_t> pixel_data) override {
    NOTREACHED();
  }
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      viz::SharedImageFormat format,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      SharedImageUsageSet usage,
      std::string debug_label,
      gfx::GpuMemoryBufferHandle handle) override {
    auto test_image_backing = std::make_unique<TestImageBacking>(
        mailbox, format, size, color_space, surface_origin, alpha_type, usage,
        0);

    // If the backing is not cleared, SkiaImageRepresentation errors out
    // when trying to create the scoped read access.
    test_image_backing->SetCleared();

    return std::move(test_image_backing);
  }
  bool IsSupported(SharedImageUsageSet usage,
                   viz::SharedImageFormat format,
                   const gfx::Size& size,
                   bool thread_safe,
                   gfx::GpuMemoryBufferType gmb_type,
                   GrContextType gr_context_type,
                   base::span<const uint8_t> pixel_data) override {
    return true;
  }
  SharedImageBackingType GetBackingType() override {
    return SharedImageBackingType::kTest;
  }
};

// Mock worker for testing.
class MockImageDecodeAcceleratorWorker : public ImageDecodeAcceleratorWorker {
 public:
  MockImageDecodeAcceleratorWorker() = default;

  MockImageDecodeAcceleratorWorker(const MockImageDecodeAcceleratorWorker&) =
      delete;
  MockImageDecodeAcceleratorWorker& operator=(
      const MockImageDecodeAcceleratorWorker&) = delete;

  void Decode(std::vector<uint8_t> encoded_data,
              const gfx::Size& output_size,
              CompletedDecodeCB decode_cb) override {
    NOTREACHED();
  }

  MOCK_METHOD(void ReleaseDecodedPixelMap, (), (override));
  MOCK_METHOD0(GetSupportedProfiles,
               std::vector<ImageDecodeAcceleratorSupportedProfile>());
};

}  // namespace

const int kChannelId = 1;

const int32_t kCommandBufferRouteId =
    static_cast<int32_t>(GpuChannelReservedRoutes::kMaxValue) + 1;

// Test fixture for ImageDecodeAcceleratorStubExt.
class ImageDecodeAcceleratorStubExtTest : public GpuChannelTestCommon {
 public:
  ImageDecodeAcceleratorStubExtTest()
      : GpuChannelTestCommon(false /* use_stub_bindings */) {}

  ImageDecodeAcceleratorStubExtTest(const ImageDecodeAcceleratorStubExtTest&) =
      delete;
  ImageDecodeAcceleratorStubExtTest& operator=(
      const ImageDecodeAcceleratorStubExtTest&) = delete;

  ~ImageDecodeAcceleratorStubExtTest() override = default;

  void SetUp() override {
    GpuChannelTestCommon::SetUp();

    // Register Skia's memory dump provider.
    base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
        skia::SkiaMemoryDumpProvider::GetInstance(), "Skia", nullptr);

    // Initialize the GrContext.
    ContextResult context_result;
    scoped_refptr<SharedContextState> shared_context_state =
        channel_manager()->GetSharedContextState(&context_result);
    ASSERT_EQ(ContextResult::kSuccess, context_result);
    ASSERT_TRUE(shared_context_state);
    shared_context_state->InitializeSkia(GpuPreferences(),
                                         GpuDriverBugWorkarounds());

    GpuChannel* channel = CreateChannel(kChannelId, false /* is_gpu_host */);
    ASSERT_TRUE(channel);
    channel->shared_image_stub()
        ->factory()
        ->RegisterSharedImageBackingFactoryForTesting(&test_factory_);

    // Create a raster command buffer.
    CommandBufferStub::SetMemoryTrackerFactoryForTesting(
        base::BindRepeating(&CreateMockMemoryTracker));
    auto init_params = mojom::CreateCommandBufferParams::New();
    init_params->share_group_id = MSG_ROUTING_NONE;
    init_params->stream_id = 0;
    init_params->stream_priority = SchedulingPriority::kNormal;
    init_params->attribs = ContextCreationAttribs();
    init_params->attribs.enable_gles2_interface = false;
    init_params->attribs.enable_raster_interface = true;
    init_params->attribs.bind_generates_resource = false;
    init_params->active_url = GURL();
    ContextResult result = ContextResult::kTransientFailure;
    Capabilities capabilities;
    GLCapabilities gl_capabilities;
    CreateCommandBuffer(*channel, std::move(init_params), kCommandBufferRouteId,
                        GetSharedMemoryRegion(), &result, &capabilities,
                        &gl_capabilities);
    ASSERT_EQ(ContextResult::kSuccess, result);
    CommandBufferStub* command_buffer =
        channel->LookupCommandBuffer(kCommandBufferRouteId);
    ASSERT_TRUE(command_buffer);

    // Make sure there are no pending tasks before starting the test.
    base::RunLoop().RunUntilIdle();
    ASSERT_TRUE(task_environment().MainThreadIsIdle());
  }

  void TearDown() override {
    channel_manager()->DestroyAllChannels();
  }

  ServiceTransferCache* GetServiceTransferCache() {
    ContextResult context_result;
    scoped_refptr<SharedContextState> shared_context_state =
        channel_manager()->GetSharedContextState(&context_result);
    if (context_result != ContextResult::kSuccess || !shared_context_state) {
      return nullptr;
    }
    return shared_context_state->transfer_cache();
  }

  int GetRasterDecoderId() {
    GpuChannel* channel = channel_manager()->LookupChannel(kChannelId);
    DCHECK(channel);
    CommandBufferStub* command_buffer =
        channel->LookupCommandBuffer(kCommandBufferRouteId);
    if (!command_buffer || !command_buffer->decoder_context())
      return -1;
    return command_buffer->decoder_context()->GetRasterDecoderId();
  }

  scoped_refptr<SharedContextState> GetSharedContextState() {
    ContextResult context_result;
    scoped_refptr<SharedContextState> shared_context_state =
        channel_manager()->GetSharedContextState(&context_result);
    if (context_result != ContextResult::kSuccess || !shared_context_state) {
      return nullptr;
    }
    return shared_context_state;
  }

  GpuChannel* GetGpuChannel() {
    return channel_manager()->LookupChannel(kChannelId);
  }

  void RunTasksUntilIdle() { task_environment().RunUntilIdle(); }

 protected:
  StrictMock<MockImageDecodeAcceleratorWorker> mock_worker_;

 private:
  TestSharedImageBackingFactory test_factory_;
};

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)

// Test construction and destruction of ImageDecodeAcceleratorStubExt.
TEST_F(ImageDecodeAcceleratorStubExtTest, ConstructDestruct) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  ASSERT_NE(stub_ext, nullptr);
}

// Test ProcessCompletedDecodeExt with successful decode.
TEST_F(ImageDecodeAcceleratorStubExtTest, ProcessCompletedDecodeExtSuccess) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a valid decode result.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  // Register discardable handle buffer.
  CommandBufferStub* command_buffer =
      channel->LookupCommandBuffer(kCommandBufferRouteId);
  ASSERT_TRUE(command_buffer);
  command_buffer->RegisterTransferBufferForTest(1, MakeBufferForTesting());

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should succeed (return false means no error).
  EXPECT_FALSE(result);
  EXPECT_TRUE(notify_gl_state_changed.has_value());
}

// Test ProcessCompletedDecodeExt with missing command buffer.
TEST_F(ImageDecodeAcceleratorStubExtTest,
       ProcessCompletedDecodeExtMissingCommandBuffer) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a valid decode result.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params with invalid route ID.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId + 999;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should fail (return true means error).
  EXPECT_TRUE(result);
}

// Test ProcessCompletedDecodeExt with invalid discardable handle parameters.
TEST_F(ImageDecodeAcceleratorStubExtTest,
       ProcessCompletedDecodeExtInvalidDiscardableHandle) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a valid decode result.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params with invalid discardable handle (buffer not registered).
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 999;  // Invalid ID.
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should fail (return true means error).
  EXPECT_TRUE(result);
}

// Test ReleasePixmapData with finish event.
TEST_F(ImageDecodeAcceleratorStubExtTest, ReleasePixmapDataWithEvent) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  base::WaitableEvent finish_event(
      base::WaitableEvent::ResetPolicy::AUTOMATIC,
      base::WaitableEvent::InitialState::NOT_SIGNALED);

  EXPECT_CALL(mock_worker_, ReleaseDecodedPixelMap()).Times(1);

  stub_ext->ReleasePixmapData(&finish_event);

  // The event should be signaled after ReleasePixmapData returns.
  EXPECT_TRUE(finish_event.IsSignaled());
}

// Test ReleasePixmapData without finish event.
TEST_F(ImageDecodeAcceleratorStubExtTest, ReleasePixmapDataWithoutEvent) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  EXPECT_CALL(mock_worker_, ReleaseDecodedPixelMap()).Times(1);

  // Should not crash when finish_event is nullptr.
  stub_ext->ReleasePixmapData(nullptr);
}

// Test ProcessCompletedDecodeExt with multiple planes.
TEST_F(ImageDecodeAcceleratorStubExtTest, ProcessCompletedDecodeExtMultiplePlanes) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a decode result with multiple planes.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  // Register discardable handle buffer.
  CommandBufferStub* command_buffer =
      channel->LookupCommandBuffer(kCommandBufferRouteId);
  ASSERT_TRUE(command_buffer);
  command_buffer->RegisterTransferBufferForTest(1, MakeBufferForTesting());

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should succeed.
  EXPECT_FALSE(result);
  EXPECT_TRUE(notify_gl_state_changed.has_value());
}

// Test ProcessCompletedDecodeExt with zero size output.
TEST_F(ImageDecodeAcceleratorStubExtTest, ProcessCompletedDecodeExtZeroSize) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a decode result with zero size.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(0, 0);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(0, 0);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  // Register discardable handle buffer.
  CommandBufferStub* command_buffer =
      channel->LookupCommandBuffer(kCommandBufferRouteId);
  ASSERT_TRUE(command_buffer);
  command_buffer->RegisterTransferBufferForTest(1, MakeBufferForTesting());

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should still succeed (zero size is handled).
  EXPECT_FALSE(result);
}

// Test ProcessCompletedDecodeExt creates transfer cache entry.
TEST_F(ImageDecodeAcceleratorStubExtTest,
       ProcessCompletedDecodeExtCreatesTransferCacheEntry) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a valid decode result.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  // Register discardable handle buffer.
  CommandBufferStub* command_buffer =
      channel->LookupCommandBuffer(kCommandBufferRouteId);
  ASSERT_TRUE(command_buffer);
  command_buffer->RegisterTransferBufferForTest(1, MakeBufferForTesting());

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should succeed.
  EXPECT_FALSE(result);

  // Check that the transfer cache has the entry.
  ServiceTransferCache* transfer_cache = GetServiceTransferCache();
  ASSERT_TRUE(transfer_cache);
  EXPECT_GT(transfer_cache->entries_count_for_testing(), 0u);
}

// Test ProcessCompletedDecodeExt sets GL state changed notification.
TEST_F(ImageDecodeAcceleratorStubExtTest,
       ProcessCompletedDecodeExtSetsGLStateChanged) {
  GpuChannel* channel = GetGpuChannel();
  ASSERT_TRUE(channel);

  auto stub_ext = std::make_unique<ImageDecodeAcceleratorStubExt>(
      &mock_worker_, channel,
      static_cast<int32_t>(GpuChannelReservedRoutes::kImageDecodeAccelerator));

  // Create a valid decode result.
  auto completed_decode = std::make_unique<ImageDecodeAcceleratorWorker::DecodeResult>();
  completed_decode->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  completed_decode->handle.native_pixmap_handle.planes.emplace_back(
      0 /* stride */, 0 /* offset */, 0 /* size */, base::ScopedFD());
  completed_decode->visible_size = gfx::Size(100, 100);
  completed_decode->buffer_format = gfx::BufferFormat::RGBA_8888;
  completed_decode->buffer_byte_size = kDecodedBufferByteSize;

  // Create decode params.
  mojom::ScheduleImageDecodeParams params;
  params.output_size = gfx::Size(100, 100);
  params.raster_decoder_route_id = kCommandBufferRouteId;
  params.transfer_cache_entry_id = 1u;
  params.discardable_handle_shm_id = 1;
  params.discardable_handle_shm_offset = 0;
  params.discardable_handle_release_count = 1u;

  // Register discardable handle buffer.
  CommandBufferStub* command_buffer =
      channel->LookupCommandBuffer(kCommandBufferRouteId);
  ASSERT_TRUE(command_buffer);
  command_buffer->RegisterTransferBufferForTest(1, MakeBufferForTesting());

  std::vector<sk_sp<SkImage>> plane_sk_images;
  std::optional<base::ScopedClosureRunner> notify_gl_state_changed;

  auto shared_context_state = GetSharedContextState();
  ASSERT_TRUE(shared_context_state);
  ASSERT_FALSE(shared_context_state->need_context_state_reset());

  // Call ProcessCompletedDecodeExt.
  bool result = stub_ext->ProcessCompletedDecodeExt(
      params, shared_context_state, plane_sk_images, notify_gl_state_changed,
      completed_decode);

  // Should succeed and notify_gl_state_changed should be set.
  EXPECT_FALSE(result);
  EXPECT_TRUE(notify_gl_state_changed.has_value());

  // Run the notification to set the state.
  notify_gl_state_changed->RunAndReset();
  EXPECT_TRUE(shared_context_state->need_context_state_reset());
}

#endif  // BUILDFLAG(ARKWEB_HEIF_SUPPORT)

}  // namespace gpu

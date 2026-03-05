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

#include "services/viz/privileged/mojom/gl/gpu_service.mojom.h"
#include "components/viz/test/gpu_host_impl_test_api.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "mojo/public/cpp/bindings/sync_call_restrictions.h"
#define private public
#include "components/viz/host/gpu_host_impl.h"
#undef private

namespace viz{
namespace {
class DelegateMock : public GpuHostImpl::Delegate {
 public:
   DelegateMock() = default;
   ~DelegateMock() = default;
   MOCK_METHOD(gpu::GPUInfo, GetGPUInfo, (), (const, override));
   MOCK_METHOD(gpu::GpuFeatureInfo, GetGpuFeatureInfo, (), (const, override));
   MOCK_METHOD5(DidInitialize, void(const gpu::GPUInfo& gpu_info, const gpu::GpuFeatureInfo& gpu_feature_info,
     const std::optional<gpu::GPUInfo>& gpu_info_for_hardware_gpu,
     const std::optional<gpu::GpuFeatureInfo>& gpu_feature_info_for_hardware_gpu,
     const gfx::GpuExtraInfo& gpu_extra_info));
   MOCK_METHOD0(DidFailInitialize, void());
   MOCK_METHOD0(DidCreateContextSuccessfully, void());
   MOCK_METHOD0(MaybeShutdownGpuProcess, void());
   MOCK_METHOD1(DidUpdateGPUInfo, void(const gpu::GPUInfo& gpu_info));
#if BUILDFLAG(IS_WIN)
   MOCK_METHOD1(DidUpdateOverlayInfo, void(const gpu::OverlayInfo& overlay_info));
   MOCK_METHOD1(DidUpdateDXGIInfo, void(gfx::mojom::DXGIInfoPtr dxgi_info));
#endif
   MOCK_METHOD2(BlockDomainsFrom3DAPIs, void(const std::set<GURL>& urls, gpu::DomainGuilt guilt));
   MOCK_METHOD2(GetIsolationKey, std::string(int32_t client_id, const blink::WebGPUExecutionContextToken& token));
   MOCK_METHOD0(DisableGpuCompositing, void());
   MOCK_METHOD(bool, GpuAccessAllowed, (), (const, override));
   MOCK_METHOD0(GetGpuDiskCacheFactory, gpu::GpuDiskCacheFactory*());
   MOCK_METHOD3(RecordLogMessage, void(int32_t severity, const std::string& header, const std::string& message));
   MOCK_METHOD1(BindDiscardableMemoryReceiver, void(
     mojo::PendingReceiver<discardable_memory::mojom::DiscardableSharedMemoryManager> receiver));
   MOCK_METHOD2(BindInterface, void(const std::string& interface_name, mojo::ScopedMessagePipeHandle interface_pipe));
#if BUILDFLAG(IS_OZONE)
   MOCK_METHOD1(TerminateGpuProcess, void(const std::string& message));
#endif
};

class VizMainMock : public viz::mojom::VizMain {
 public:
   using FrameSinkManagerParamsPtr = mojo::StructPtr<mojom::FrameSinkManagerParams>;
   VizMainMock() = default;
   ~VizMainMock() = default;
   MOCK_METHOD1(CreateFrameSinkManager, void(FrameSinkManagerParamsPtr params));
   MOCK_METHOD5(CreateGpuService, void(
     ::mojo::PendingReceiver<::viz::mojom::GpuService> gpu_service,
     ::mojo::PendingRemote<::viz::mojom::GpuHost> gpu_host,
     ::mojo::PendingRemote<::discardable_memory::mojom::DiscardableSharedMemoryManager> discardable_memory_manager,
     ::base::UnsafeSharedMemoryRegion use_shader_cache_shm_count,
     mojom::GpuServiceCreationParamsPtr params));
   MOCK_METHOD(void, SetRenderParams, (
     ::gfx::FontRenderParams::SubpixelRendering subpixel_rendering, float text_contrast, float text_gamma));
};

class GpuServiceMock : public mojom::GpuService {
 public:
   GpuServiceMock() = default;
   ~GpuServiceMock() = default;

   MOCK_METHOD7(EstablishGpuChannel, bool(int32_t client_id,
     uint64_t client_tracing_id,
     bool is_gpu_host,
     ::mojo::ScopedMessagePipeHandle* out_channel_handle,
     ::gpu::GPUInfo* out_gpu_info,
     ::gpu::GpuFeatureInfo* out_gpu_feature_info,
     ::gpu::SharedImageCapabilities* out_shared_image_capabilities));
   MOCK_METHOD4(EstablishGpuChannel, void(int32_t client_id,
     uint64_t client_tracing_id,
     bool is_gpu_host,
     EstablishGpuChannelCallback callback));
   MOCK_METHOD2(SetChannelClientPid, void(int32_t client_id, ::base::ProcessId client_pid));
   MOCK_METHOD2(GetSurfaceId, bool(int32_t native_embed_id, std::string* out_surfaceId));
   MOCK_METHOD2(GetSurfaceId, void(int32_t native_embed_id, GetSurfaceIdCallback callback));
   MOCK_METHOD1(Discard, void(uint32_t native_window_id));
   MOCK_METHOD2(SetTransformHint, void(uint32_t rotation, uint32_t window_id));
   MOCK_METHOD1(DestroyNativeWindow, void(uint32_t native_window_id));
   MOCK_METHOD2(SetChannelDiskCacheHandle, void(int32_t client_id, const ::gpu::GpuDiskCacheHandle& cache_handle));
   MOCK_METHOD1(OnDiskCacheHandleDestoyed, void(const ::gpu::GpuDiskCacheHandle& cache_handle));
   MOCK_METHOD1(CloseChannel, void(int32_t client_id));
   MOCK_METHOD2(BindClientGmbInterface, void(::mojo::PendingReceiver<::gpu::mojom::ClientGmbInterface> receiver,
     int32_t client_id));
   MOCK_METHOD1(CreateVideoEncodeAcceleratorProvider,
     void(::mojo::PendingReceiver<::media::mojom::VideoEncodeAcceleratorProvider> vea_provider));
   MOCK_METHOD2(BindWebNNContextProvider,
     void(::mojo::PendingReceiver<::webnn::mojom::WebNNContextProvider> receiver, int32_t client_id));
   MOCK_METHOD7(CreateGpuMemoryBuffer, bool(
     ::gfx::GpuMemoryBufferId id,
     const ::gfx::Size& size,
     ::gfx::BufferFormat format,
     ::gfx::BufferUsage usage,
     int32_t client_id,
     ::gpu::SurfaceHandle surface_handle,
     ::gfx::GpuMemoryBufferHandle* out_buffer_handle));
   MOCK_METHOD(void, CreateGpuMemoryBuffer, (::gfx::GpuMemoryBufferId id,
     const ::gfx::Size& size,
     ::gfx::BufferFormat format,
     ::gfx::BufferUsage usage,
     int32_t client_id,
     ::gpu::SurfaceHandle surface_handle,
     CreateGpuMemoryBufferCallback callback));
   MOCK_METHOD2(DestroyGpuMemoryBuffer, void(::gfx::GpuMemoryBufferId id, int32_t client_id));
   MOCK_METHOD3(CopyGpuMemoryBuffer, void(::gfx::GpuMemoryBufferHandle buffer_handle,
     ::base::UnsafeSharedMemoryRegion shared_memory,
     CopyGpuMemoryBufferCallback callback));
   MOCK_METHOD1(GetVideoMemoryUsageStats, void(GetVideoMemoryUsageStatsCallback callback));
   MOCK_METHOD1(StartPeakMemoryMonitor, void(uint32_t sequence_num));
   MOCK_METHOD2(GetPeakMemoryUsage, void(uint32_t sequence_num,
     GetPeakMemoryUsageCallback callback));
   MOCK_METHOD3(LoadedBlob, void(const ::gpu::GpuDiskCacheHandle& cache_handle,
     const std::string& key, const std::string& data));
   MOCK_METHOD0(WakeUpGpu, void());
   MOCK_METHOD1(GpuSwitched, void(::gl::GpuPreference active_gpu_heuristic));
   MOCK_METHOD0(DisplayAdded, void());
   MOCK_METHOD0(DisplayRemoved, void());
   MOCK_METHOD0(DisplayMetricsChanged, void());
   MOCK_METHOD0(DestroyAllChannels, void());
   MOCK_METHOD0(OnBackgroundCleanup, void());
   MOCK_METHOD0(OnBackgrounded, void());
   MOCK_METHOD0(OnForegrounded, void());
   MOCK_METHOD1(OnMemoryPressure, void(::base::MemoryPressureListener::MemoryPressureLevel level));
   MOCK_METHOD2(GetDawnInfo, void(bool collect_metrics, GetDawnInfoCallback callback));
   MOCK_METHOD0(Crash, void());
   MOCK_METHOD0(Hang, void());
   MOCK_METHOD0(ThrowJavaException, void());
   MOCK_METHOD2(SetVisible, void(int32_t nweb_id, bool visible));
   MOCK_METHOD1(StartMonitor, void(int32_t nweb_id));
   MOCK_METHOD0(StopMonitor, void());
   MOCK_METHOD1(SetHasTouchPoint, void(bool has_touch_point));
   MOCK_METHOD1(ReportSlidingFrameRate, void(int32_t frame_rate));
   MOCK_METHOD1(SetLTPOStrategy, void(int32_t strategy));
   MOCK_METHOD1(DumpGpuInfo, void(DumpGpuInfoCallback callback));
   MOCK_METHOD1(SetIsFling, void(bool is_fling_enabled));
   MOCK_METHOD1(SetIsScroll, void(bool is_scroll_enabled));
#if BUILDFLAG(USE_CLANG_COVERAGE)
#if BUILDFLAG(CLANG_PROFILING_INSIDE_SANDBOX)
   MOCK_METHOD1(WriteClangProfilingProfile, void(WriteClangProfilingProfileCallback callback));
#endif
#endif
};
}

class GpuHostImplTest : public testing::Test {
 public:
   GpuHostImplTest() = default;
   ~GpuHostImplTest() = default;

 protected:
   struct TestEnvironment {
      DelegateMock delegate;
      mojo::PendingRemote<viz::mojom::VizMain> viz_main_pending_remote;
      std::unique_ptr<VizMainMock> viz_main_mock;
      std::unique_ptr<mojo::Receiver<viz::mojom::VizMain>> viz_main_receiver_impl;
      GpuHostImpl::InitParams params;
      std::unique_ptr<GpuHostImpl> gpu_host;
      std::unique_ptr<mojo::Remote<viz::mojom::GpuService>> gpu_service_remote;
      std::unique_ptr<GpuServiceMock> gpu_service_mock;
      std::unique_ptr<mojo::Receiver<viz::mojom::GpuService>> gpu_service_receiver_impl;
      std::unique_ptr<viz::GpuHostImplTestApi> gpu_host_test_api;

      TestEnvironment() {
        auto viz_main_receiver = viz_main_pending_remote.InitWithNewPipeAndPassReceiver();
        viz_main_mock = std::make_unique<VizMainMock>();
        viz_main_receiver_impl = std::make_unique<mojo::Receiver<viz::mojom::VizMain>>(
            viz_main_mock.get(), std::move(viz_main_receiver));

        gpu_host = std::make_unique<GpuHostImpl>(&delegate, std::move(viz_main_pending_remote), std::move(params));

        gpu_service_remote = std::make_unique<mojo::Remote<viz::mojom::GpuService>>();
        auto gpu_service_receiver = gpu_service_remote->BindNewPipeAndPassReceiver();
        gpu_service_mock = std::make_unique<GpuServiceMock>();
        gpu_service_receiver_impl = std::make_unique<mojo::Receiver<viz::mojom::GpuService>>(
            gpu_service_mock.get(), std::move(gpu_service_receiver));

        gpu_host_test_api = std::make_unique<viz::GpuHostImplTestApi>(gpu_host.get());
        gpu_host_test_api->SetGpuService(std::move(*gpu_service_remote));
      }
   };

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void TestGetSurfaceId() {
    auto env = std::make_unique<TestEnvironment>();
    std::string surface_id = env->gpu_host->GetSurfaceId(123);
    EXPECT_EQ(surface_id, "");
  }

  void TestSetTransformHint() {
    auto env = std::make_unique<TestEnvironment>();
    ASSERT_NO_FATAL_FAILURE(env->gpu_host->SetTransformHint(123, 1));
  }

  void TestDestroyNativeWindow() {
    auto env = std::make_unique<TestEnvironment>();
    ASSERT_NO_FATAL_FAILURE(env->gpu_host->DestroyNativeWindow(123));
  }
#endif

  void TestDiscard() {
    auto env = std::make_unique<TestEnvironment>();
    ASSERT_NO_FATAL_FAILURE(env->gpu_host->Discard(123));
  }
};

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(GpuHostImplTest, GetSurfaceId) {
  TestGetSurfaceId();
}

TEST_F(GpuHostImplTest, SetTransformHint) {
  TestSetTransformHint();
}

TEST_F(GpuHostImplTest, DestroyNativeWindow) {
  TestDestroyNativeWindow();
}
#endif

TEST_F(GpuHostImplTest, Discard) {
  TestDiscard();
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    mojom::BlanklessSendInfoPtr CreateInfoPtr() {
      auto infoPtr = mojom::BlanklessSendInfo::New();
      infoPtr->blankless_key = 123456789;
      infoPtr->nweb_id = 1;
      infoPtr->lcp_time = 1000;
      infoPtr->system_time = 1234567890;
      infoPtr->pref_hash = -1;
      return infoPtr;
    }

    mojom::BlanklessBitmapMetadataPtr CreateMetaData(){
      auto metadata = mojom::BlanklessBitmapMetadata::New();
      metadata->width = 100;
      metadata->height = 100;
      metadata->color_type = static_cast<int32_t>(SkColorType::kRGBA_8888_SkColorType);
      metadata->alpha_type = static_cast<int32_t>(SkAlphaType::kPremul_SkAlphaType);
      metadata->size = 100 * 100 * 4;
      return metadata;
    }

    std::vector<gfx::Rect> CreateQuadList() {
      std::vector<gfx::Rect> quad_list;
      quad_list.emplace_back(0, 0, 100, 100);
      quad_list.emplace_back(100, 100, 200, 200);
      return quad_list;
    }

    void TestSendBlanklessSnapshotInfo() {
        DelegateMock delegate;
        mojo::PendingRemote<viz::mojom::VizMain> viz_main_pending_remote;
        auto viz_main_receiver = viz_main_pending_remote.InitWithNewPipeAndPassReceiver();
        VizMainMock viz_main_mock;
        mojo::Receiver<viz::mojom::VizMain> viz_main_receiver_impl(&viz_main_mock, std::move(viz_main_receiver));
        GpuHostImpl::InitParams params;
        GpuHostImpl gpu_host(&delegate, std::move(viz_main_pending_remote), std::move(params));

        auto infoPtr = CreateInfoPtr();
        auto metadata = CreateMetaData();
        size_t buffer_size = metadata->size;
        mojo::ScopedSharedBufferHandle buffer = mojo::SharedBufferHandle::Create(buffer_size);
        ASSERT_NO_FATAL_FAILURE(
          gpu_host.SendBlanklessSnapshotInfo(
            nullptr, std::move(buffer), std::move(metadata)));
        ASSERT_NO_FATAL_FAILURE(
          gpu_host.SendBlanklessSnapshotInfo(
            std::move(infoPtr), std::move(buffer), std::move(metadata)));
    }

    void TestDumpBlanklessSnapshot01() {
      DelegateMock delegate;
      mojo::PendingRemote<viz::mojom::VizMain> viz_main_pending_remote;
      auto viz_main_receiver = viz_main_pending_remote.InitWithNewPipeAndPassReceiver();
      VizMainMock viz_main_mock;
      mojo::Receiver<viz::mojom::VizMain> viz_main_receiver_impl(&viz_main_mock, std::move(viz_main_receiver));
      GpuHostImpl::InitParams params;
      GpuHostImpl gpu_host(&delegate, std::move(viz_main_pending_remote), std::move(params));

      auto infoPtr = CreateInfoPtr();
      auto metadata = CreateMetaData();
      mojo::ScopedSharedBufferHandle buffer = mojo::SharedBufferHandle::Create(metadata->size);
      EXPECT_EQ(buffer.is_valid(), true);

      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        nullptr, std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), mojo::ScopedSharedBufferHandle(), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      buffer = mojo::SharedBufferHandle::Create(111);
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), nullptr));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->width = -1;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->height = -1;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));
    }

    void TestDumpBlanklessSnapshot02() {
      DelegateMock delegate;
      mojo::PendingRemote<viz::mojom::VizMain> viz_main_pending_remote;
      auto viz_main_receiver = viz_main_pending_remote.InitWithNewPipeAndPassReceiver();
      VizMainMock viz_main_mock;
      mojo::Receiver<viz::mojom::VizMain> viz_main_receiver_impl(&viz_main_mock, std::move(viz_main_receiver));
      GpuHostImpl::InitParams params;
      GpuHostImpl gpu_host(&delegate, std::move(viz_main_pending_remote), std::move(params));

      auto infoPtr = CreateInfoPtr();
      auto metadata = CreateMetaData();
      mojo::ScopedSharedBufferHandle buffer = mojo::SharedBufferHandle::Create(111);
      metadata->size = 0;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
          std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->color_type = -1;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->color_type = 111111;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->alpha_type = -1;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(metadata->size);
      metadata->alpha_type = 100000;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
        std::move(infoPtr), std::move(buffer), std::move(metadata)));

      infoPtr = CreateInfoPtr();
      metadata = CreateMetaData();
      buffer = mojo::SharedBufferHandle::Create(4);
      metadata->width = 1;
      metadata->height = 1;
      ASSERT_NO_FATAL_FAILURE(gpu_host.DumpBlanklessSnapshot(
      std::move(infoPtr), std::move(buffer), std::move(metadata)));
    }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(GpuHostImplTest, SendBlanklessSnapshotInfo) {
  TestSendBlanklessSnapshotInfo();
}

TEST_F(GpuHostImplTest, DumpBlanklessSnapshot) {
  TestDumpBlanklessSnapshot01();
  TestDumpBlanklessSnapshot02();
}
#endif
} // namespace viz

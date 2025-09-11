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

#include "base/run_loop.h"
#include "base/barrier_closure.h"
#include "base/test/bind.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "components/viz/common/frame_sinks/arkweb_copy_output_result_utils.h"
#include "components/viz/service/gl/gpu_service_impl.h"
#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#undef private

namespace viz {
class GpuServiceImplUtilsTest : public testing::Test {
public:
  GpuServiceImplUtilsTest()
      : io_thread_("TestIOThread"),
        wait_(base::WaitableEvent::ResetPolicy::MANUAL,
              base::WaitableEvent::InitialState::NOT_SIGNALED) {}
  ~GpuServiceImplUtilsTest() = default;

  void SetUp() {
    ASSERT_TRUE(io_thread_.Start());
    gpu::GPUInfo gpu_info;
    gpu_info.in_process_gpu = false;
    GpuServiceImpl::InitParams init_params;
    init_params.io_runner = io_thread_.task_runner();
    init_params.exit_callback = base::DoNothing();
    gpu_service_ = std::make_unique<GpuServiceImpl>(
        gpu::GpuPreferences(), gpu_info, gpu::GpuFeatureInfo(), gpu::GPUInfo(),
        gpu::GpuFeatureInfo(), gfx::GpuExtraInfo(), std::move(init_params));
  }

  void TearDown() {
    DestroyService();
    base::RunLoop runloop;
    runloop.RunUntilIdle();
    io_thread_.Stop();
  }

  GpuServiceImpl* gpu_service() { return gpu_service_.get(); }

  void DestroyService() { gpu_service_ = nullptr; }

private:
  base::Thread io_thread_;
  std::unique_ptr<GpuServiceImpl> gpu_service_;
  base::WaitableEvent wait_;
};

TEST_F(GpuServiceImplUtilsTest, SetWakeUpGpuClosure) {
  auto count = 0;
  base::RepeatingClosure closure = base::BarrierClosure(
      0, base::BindLambdaForTesting([&count] { ++count; }));
  gpu_service()->SetWakeUpGpuClosure(std::move(closure));
  EXPECT_EQ(count, 1);
}

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(GpuServiceImplUtilsTest, GetSurfaceId) {
    mojo::Remote<mojom::GpuService> gpu_service_remote;
    gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
    const int32_t kTestNativeEmbedId = 1;
    const std::string kTestSurfaceId = "test_surface_id_12345";
    gfx::AcceleratedWidget widget = 123;
    std::string non_const_id = kTestSurfaceId;
    gpu::GpuSurfaceIdTracker::SurfaceRecord record(widget, non_const_id);
    gpu::GpuSurfaceIdTracker::Get()->AddSurfaceForNativeWidget(std::move(record));
    base::RunLoop run_loop;
    std::string received_surface_id;
    gpu_service()->GetSurfaceId(kTestNativeEmbedId, base::BindOnce(
      [](base::RunLoop* loop, std::string* result, const std::string& id) {
        *result = id;
        loop->Quit();
      }, &run_loop, &received_surface_id));
    run_loop.Run();
    EXPECT_EQ(received_surface_id, kTestSurfaceId);
}

TEST_F(GpuServiceImplUtilsTest, DestroyNativeWindow) {
    mojo::Remote<mojom::GpuService> gpu_service_remote;
    gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
    ASSERT_NO_FATAL_FAILURE(gpu_service()->DestroyNativeWindow(1223));
}
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
TEST_F(GpuServiceImplUtilsTest, DumpGpuInfo){
    mojo::Remote<mojom::GpuService> gpu_service_remote;
    gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
    ASSERT_NO_FATAL_FAILURE(gpu_service()->DumpGpuInfo(base::BindOnce([](float){})));
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResult) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(nullptr));
  gpu_service_remote.FlushForTesting();

  auto result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 0, 0), false);
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  gpu_service_remote.FlushForTesting();

  result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 0, 0), false);
  result->copy_output_result_utils()->info_.blankless_key = 11;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  gpu_service_remote.FlushForTesting();

  SkBitmap bitmap;
  bitmap.allocN32Pixels(100, 100);
  bitmap.eraseColor(SK_ColorRED);
  std::unique_ptr<viz::CopyOutputResult> result2 = std::make_unique<viz::CopyOutputSkBitmapResult>(
        gfx::Rect(0, 0, 100, 100),
        std::move(bitmap)
    );
  result2->copy_output_result_utils()->info_.blankless_key = 11;

  mojo::PendingRemote<mojom::GpuHost> gpu_host_proxy;
  std::ignore = gpu_host_proxy.InitWithNewPipeAndPassReceiver();
  gpu_service()->InitializeWithHost(
      std::move(gpu_host_proxy), gpu::GpuProcessShmCount(),
      gl::init::CreateOffscreenGLSurface(gl::GetDefaultDisplay(), gfx::Size()),
      mojom::GpuServiceCreationParams::New());
  gpu_service_remote.FlushForTesting();
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result2)));
}
#endif
} //namespace viz
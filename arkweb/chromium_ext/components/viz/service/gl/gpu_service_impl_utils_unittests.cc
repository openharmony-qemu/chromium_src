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
        gpu::GpuPreferences(), gpu_info, gpu::GpuFeatureInfo(),
        std::optional<gpu::GPUInfo>(), std::optional<gpu::GpuFeatureInfo>(),
        gfx::GpuExtraInfo(), std::move(init_params));
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
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->SetWakeUpGpuClosure(std::move(closure));
  EXPECT_EQ(count, 1);
}

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(GpuServiceImplUtilsTest, GetSurfaceId) {
    mojo::Remote<mojom::GpuService> gpu_service_remote;
    ASSERT_NE(gpu_service(), nullptr);
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
    ASSERT_NE(gpu_service(), nullptr);
    gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
    ASSERT_NO_FATAL_FAILURE(gpu_service()->DestroyNativeWindow(1223));
}
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
TEST_F(GpuServiceImplUtilsTest, DumpGpuInfo){
    mojo::Remote<mojom::GpuService> gpu_service_remote;
    ASSERT_NE(gpu_service(), nullptr);
    gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
    ASSERT_NO_FATAL_FAILURE(gpu_service()->DumpGpuInfo(base::BindOnce([](float){})));
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResult) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(nullptr));
  gpu_service_remote.FlushForTesting();

  auto result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 0, 0), false);
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  gpu_service_remote.FlushForTesting();

  auto result1 = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 0, 0), false);
  result1->copy_output_result_utils()->info_.blankless_key = 11;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result1)));
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

TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResultWithEmptyRect) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  auto result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(), false);
  result->copy_output_result_utils()->info_.blankless_key = 22;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  gpu_service_remote.FlushForTesting();
}

TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResultWithLargeRect) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  auto result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 1920, 1080), false);
  result->copy_output_result_utils()->info_.blankless_key = 33;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  gpu_service_remote.FlushForTesting();
}

TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResultWithDifferentFormats) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  auto result_rgba = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 50, 50), false);
  result_rgba->copy_output_result_utils()->info_.blankless_key = 44;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result_rgba)));

  auto result_i420 = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::I420_PLANES,
    CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 50, 50), false);
  result_i420->copy_output_result_utils()->info_.blankless_key = 55;
  ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result_i420)));

  gpu_service_remote.FlushForTesting();
}

TEST_F(GpuServiceImplUtilsTest, OnFrameSnapshotCopyOutputResultMultipleCalls) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());

  for (int i = 0; i < 5; ++i) {
    auto result = std::make_unique<CopyOutputResult>(CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10), false);
    result->copy_output_result_utils()->info_.blankless_key = static_cast<uint64_t>(i + 100);
    ASSERT_NO_FATAL_FAILURE(gpu_service()->OnFrameSnapshotCopyOutputResult(std::move(result)));
  }
  gpu_service_remote.FlushForTesting();
}
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(GpuServiceImplUtilsTest, SetTransformHintWithZeroRotation) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(0, 1));
}

TEST_F(GpuServiceImplUtilsTest, SetTransformHintWithDifferentRotations) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(0, 1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(90, 2));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(180, 3));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(270, 4));
}

TEST_F(GpuServiceImplUtilsTest, SetTransformHintWithZeroWindowId) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(90, 0));
}

TEST_F(GpuServiceImplUtilsTest, SetTransformHintWithMaxValues) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(UINT32_MAX, UINT32_MAX));
}

TEST_F(GpuServiceImplUtilsTest, SetTransformHintMultipleCalls) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  for (uint32_t i = 0; i < 5; ++i) {
    ASSERT_NO_FATAL_FAILURE(gpu_service()->SetTransformHint(i * 90, i + 1));
  }
}

TEST_F(GpuServiceImplUtilsTest, DiscardWithZero) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(0));
}

TEST_F(GpuServiceImplUtilsTest, DiscardWithDifferentIds) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(456));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(789));
}

TEST_F(GpuServiceImplUtilsTest, DiscardWithMaxId) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(UINT32_MAX));
}

TEST_F(GpuServiceImplUtilsTest, DiscardMultipleCalls) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  for (uint32_t i = 0; i < 10; ++i) {
    ASSERT_NO_FATAL_FAILURE(gpu_service()->Discard(i * 100));
  }
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
TEST_F(GpuServiceImplUtilsTest, SetVisibleWithVisibleTrue) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(1, true));
}

TEST_F(GpuServiceImplUtilsTest, SetVisibleWithVisibleFalse) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(1, false));
}

TEST_F(GpuServiceImplUtilsTest, SetVisibleWithDifferentNwebIds) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(0, true));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(100, false));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(INT32_MAX, true));
}

TEST_F(GpuServiceImplUtilsTest, SetVisibleMultipleCalls) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(1, true));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(1, false));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(2, true));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetVisible(2, false));
}

TEST_F(GpuServiceImplUtilsTest, SetHasTouchPointTrue) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(true));
}

TEST_F(GpuServiceImplUtilsTest, SetHasTouchPointFalse) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(false));
}

TEST_F(GpuServiceImplUtilsTest, SetHasTouchPointMultipleCalls) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(true));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(false));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(true));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetHasTouchPoint(false));
}

TEST_F(GpuServiceImplUtilsTest, ReportSlidingFrameRateWithZero) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(0));
}

TEST_F(GpuServiceImplUtilsTest, ReportSlidingFrameRateWithCommonValues) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(30));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(60));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(90));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(120));
}

TEST_F(GpuServiceImplUtilsTest, ReportSlidingFrameRateWithMaxValue) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->ReportSlidingFrameRate(INT32_MAX));
}

TEST_F(GpuServiceImplUtilsTest, SetLTPOStrategyWithZero) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(0));
}

TEST_F(GpuServiceImplUtilsTest, SetLTPOStrategyWithDifferentValues) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(2));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(3));
}

TEST_F(GpuServiceImplUtilsTest, SetLTPOStrategyWithNegativeValue) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(-1));
}

TEST_F(GpuServiceImplUtilsTest, SetLTPOStrategyWithMaxValue) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->SetLTPOStrategy(INT32_MAX));
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
TEST_F(GpuServiceImplUtilsTest, StartMonitorWithZero) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(0));
}

TEST_F(GpuServiceImplUtilsTest, StartMonitorWithPositiveId) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(1));
}

TEST_F(GpuServiceImplUtilsTest, StartMonitorWithDifferentNwebIds) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(100));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(INT32_MAX));
}

TEST_F(GpuServiceImplUtilsTest, StopMonitorBasic) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StopMonitor());
}

TEST_F(GpuServiceImplUtilsTest, StartAndStopMonitor) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StopMonitor());
}

TEST_F(GpuServiceImplUtilsTest, MultipleStartAndStopMonitor) {
  mojo::Remote<mojom::GpuService> gpu_service_remote;
  ASSERT_NE(gpu_service(), nullptr);
  gpu_service()->Bind(gpu_service_remote.BindNewPipeAndPassReceiver());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(1));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StopMonitor());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(2));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StopMonitor());
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StartMonitor(3));
  ASSERT_NO_FATAL_FAILURE(gpu_service()->StopMonitor());
}
#endif

} //namespace viz
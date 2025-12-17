/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "third_party/blink/renderer/platform/widget/input/software_compositor_proxy_ohos.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/shared_memory_mapping.h"
#include "cc/mojo_embedder/async_layer_tree_frame_sink.h"
#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/test/test_context_provider.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkRegion.h"

namespace blink {
namespace {
std::shared_ptr<SoftwareCompositorProxyOhos> g_softwareCompositor;
}

using DemandDrawSwAsyncCallback = base::OnceCallback<void(bool)>;

class SoftwareCompositorProxyOhosTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};
void SoftwareCompositorProxyOhosTest::SetUpTestCase(void) {}

void SoftwareCompositorProxyOhosTest::TearDownTestCase(void) {}

void SoftwareCompositorProxyOhosTest::SetUp(void) {
  g_softwareCompositor = std::make_shared<SoftwareCompositorProxyOhos>();
  ASSERT_NE(g_softwareCompositor, nullptr);
}

void SoftwareCompositorProxyOhosTest::TearDown(void) {
  g_softwareCompositor = nullptr;
}

class MockAsyncLayerTreeFrameSink
    : public cc::mojo_embedder::AsyncLayerTreeFrameSink {
 public:
  MockAsyncLayerTreeFrameSink(
      scoped_refptr<viz::RasterContextProvider> context_provider,
      scoped_refptr<cc::RasterContextProviderWrapper>
          worker_context_provider_wrapper,
      scoped_refptr<gpu::ClientSharedImageInterface> shared_image_interface,
      InitParams* params)
      : AsyncLayerTreeFrameSink(context_provider,
                                worker_context_provider_wrapper,
                                shared_image_interface,
                                params) {}
  MockAsyncLayerTreeFrameSink(const MockAsyncLayerTreeFrameSink&) = delete;
  ~MockAsyncLayerTreeFrameSink() = default;
};

class MockSoftwareCompositorRegistryOhos
    : public cc::mojo_embedder::SoftwareCompositorRegistryOhos {
 public:
  MockSoftwareCompositorRegistryOhos() = default;
  ~MockSoftwareCompositorRegistryOhos() = default;
  void RegisterSoftwareRenderer(
      cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer);
  void UnregisterSoftwareRenderer(
      cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer);
};
void MockSoftwareCompositorRegistryOhos::RegisterSoftwareRenderer(
    cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer) {}
void MockSoftwareCompositorRegistryOhos::UnregisterSoftwareRenderer(
    cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer) {}

class WritableSharedMemoryRegionMock : public base::WritableSharedMemoryRegion {
 public:
  WritableSharedMemoryRegionMock() = default;
  WritableSharedMemoryRegionMock(const WritableSharedMemoryRegionMock&) =
      delete;
  ~WritableSharedMemoryRegionMock() = default;

  MOCK_METHOD0(IsValid, bool());
  MOCK_METHOD0(Map, base::WritableSharedMemoryMapping());
};

class MockSoftwareCompositorRendererOhos
    : public cc::mojo_embedder::SoftwareCompositorRendererOhos {
public:
  MockSoftwareCompositorRendererOhos(
    cc::mojo_embedder::AsyncLayerTreeFrameSink* sink,
    cc::mojo_embedder::SoftwareCompositorRegistryOhos* registry)
      : cc::mojo_embedder::SoftwareCompositorRendererOhos(sink, registry) {}
  MockSoftwareCompositorRendererOhos(
      const MockSoftwareCompositorRendererOhos&) = delete;
  MockSoftwareCompositorRendererOhos& operator=(
      const MockSoftwareCompositorRendererOhos&) = delete;

  MOCK_METHOD(void, DrawRect, (const gfx::Rect& rect), (override));
};

TEST_F(SoftwareCompositorProxyOhosTest, BindChannel) {
  mojo::PendingReceiver<mojom::blink::SoftwareCompositorOhos>
      compositor_request;

  g_softwareCompositor->BindChannel(std::move(compositor_request));
}

TEST_F(SoftwareCompositorProxyOhosTest, SetSharedMemory) {
  base::WritableSharedMemoryRegion shm_region =
      WritableSharedMemoryRegionMock::Create(1024 * 1024);
  bool callback_result = false;

  g_softwareCompositor->SetSharedMemory(
      std::move(shm_region),
      base::BindOnce(
          [](bool* out_result, bool result) { *out_result = result; },
          &callback_result));

  base::WritableSharedMemoryRegion invalid_shm_region;
  g_softwareCompositor->SetSharedMemory(
      std::move(invalid_shm_region),
      base::BindOnce(
          [](bool* out_result, bool result) { *out_result = result; },
          &callback_result));
}

TEST_F(SoftwareCompositorProxyOhosTest, SetSharedMemory001) {
  base::WritableSharedMemoryRegion shared_region =
      WritableSharedMemoryRegionMock::Create(1024 * 1024);

  bool callback_result = false;
  base::SharedMemoryMapper* shared_mapper_ptr =
      base::SharedMemoryMapper::GetDefaultInstance();
  shared_region.Map(shared_mapper_ptr);

  g_softwareCompositor->SetSharedMemory(
      std::move(shared_region),
      base::BindOnce(
          [](bool* out_result, bool result) { *out_result = result; },
          &callback_result));
  ASSERT_TRUE(callback_result);
}

TEST_F(SoftwareCompositorProxyOhosTest, SetSoftwareRenderer) {
  g_softwareCompositor->SetSoftwareRenderer(nullptr);
}

TEST_F(SoftwareCompositorProxyOhosTest, DrawRect) {
  g_softwareCompositor->SetSoftwareRenderer(nullptr);
  gfx::Rect rect(10, 10, 100, 100);
  g_softwareCompositor->DrawRect(rect);
}

TEST_F(SoftwareCompositorProxyOhosTest, DrawRect002) {
  scoped_refptr<viz::TestContextProvider> provider =
      viz::TestContextProvider::CreateRaster();
  gpu::TestGpuMemoryBufferManager test_gpu_memory_buffer_manager;

  mojo::PendingRemote<viz::mojom::CompositorFrameSink> sink_remote;
  mojo::PendingReceiver<viz::mojom::CompositorFrameSink> sink_receiver =
      sink_remote.InitWithNewPipeAndPassReceiver();
  mojo::PendingRemote<viz::mojom::CompositorFrameSinkClient> client;

  cc::mojo_embedder::AsyncLayerTreeFrameSink::InitParams init_params;
  init_params.gpu_memory_buffer_manager = &test_gpu_memory_buffer_manager;
  init_params.pipes.compositor_frame_sink_remote = std::move(sink_remote);
  init_params.pipes.client_receiver = client.InitWithNewPipeAndPassReceiver();

  auto sink = std::make_unique<cc::mojo_embedder::AsyncLayerTreeFrameSink>(
      std::move(provider), nullptr, nullptr, &init_params);

  auto registry = std::make_shared<MockSoftwareCompositorRegistryOhos>();
  auto renderer = std::make_shared<MockSoftwareCompositorRendererOhos>(
      sink.get(), registry.get());
  g_softwareCompositor->SetSoftwareRenderer(renderer.get());
  gfx::Rect test_rect(10, 10, 100, 100);
  EXPECT_CALL(*renderer, DrawRect(test_rect)).Times(1);
  gfx::Rect rect(10, 10, 100, 100);
  g_softwareCompositor->DrawRect(rect);
}

TEST_F(SoftwareCompositorProxyOhosTest, DrawRect001) {
  gfx::Rect rect(10, 10, 100, 100);
  g_softwareCompositor->DrawRect(rect);
}

TEST_F(SoftwareCompositorProxyOhosTest, DemandDrawSwAsync_InstallPixelsFail) {
  base::WritableSharedMemoryRegion shm_region = 
      base::WritableSharedMemoryRegion::Create(100);

  bool set_result = false;
  g_softwareCompositor->SetSharedMemory(
      std::move(shm_region),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
                    &set_result));
  ASSERT_TRUE(set_result);

  auto params = mojom::blink::SoftwareCompositorDemandDrawSwParams::New();
  params->size = gfx::SizeF(100.0f, 100.0f);
  params->offset = gfx::PointF(0.0f, 0.0f);

  bool callback_result = true;
  g_softwareCompositor->DemandDrawSwAsync(
      std::move(params),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
                    &callback_result));

  EXPECT_FALSE(callback_result);
}

TEST_F(SoftwareCompositorProxyOhosTest, DemandDrawSwAsync_RendererNull) {
  base::WritableSharedMemoryRegion shm_region = 
      base::WritableSharedMemoryRegion::Create(1024 * 1024);

  bool set_result = false;
  g_softwareCompositor->SetSharedMemory(
      std::move(shm_region),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
                    &set_result));
  ASSERT_TRUE(set_result);

  auto params = mojom::blink::SoftwareCompositorDemandDrawSwParams::New();
  params->size = gfx::SizeF(100.0f, 100.0f);
  params->offset = gfx::PointF(0.0f, 0.0f);

  bool callback_result = true;
  g_softwareCompositor->DemandDrawSwAsync(
      std::move(params),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
                    &callback_result));

  EXPECT_FALSE(callback_result);
}

TEST_F(SoftwareCompositorProxyOhosTest, DemandDrawSwAsync001) {
  base::WritableSharedMemoryRegion shm_region =
      base::WritableSharedMemoryRegion::Create(1024 * 1024);

  bool set_result = false;
  g_softwareCompositor->SetSharedMemory(
      std::move(shm_region),
      base::BindOnce(
          [](bool* out_result, bool result) { *out_result = result; },
          &set_result));
  ASSERT_TRUE(set_result);

  auto params = mojom::blink::SoftwareCompositorDemandDrawSwParams::New();
  params->size = gfx::SizeF(100.0f, 100.0f);
  params->offset = gfx::PointF(0.0f, 0.0f);
  g_softwareCompositor->SetSoftwareRenderer(nullptr);
  bool callback_result = true;
  g_softwareCompositor->DemandDrawSwAsync(
      std::move(params),
      base::BindOnce(
          [](bool* out_result, bool result) { *out_result = result; },
          &callback_result));

  EXPECT_FALSE(callback_result);
}

TEST_F(SoftwareCompositorProxyOhosTest, DemandDrawSwAsync002) {
  base::WritableSharedMemoryRegion shm_region =
      base::WritableSharedMemoryRegion::Create(100);
  bool set_result = false;
  g_softwareCompositor->SetSharedMemory(
      std::move(shm_region),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
          &set_result));
  ASSERT_TRUE(set_result);
  auto params = mojom::blink::SoftwareCompositorDemandDrawSwParams::New();
  params->size = gfx::SizeF(0.0f, 0.0f);
  params->offset = gfx::PointF(0.0f, 0.0f);
  bool callback_result = true;
  g_softwareCompositor->DemandDrawSwAsync(
      std::move(params),
      base::BindOnce([](bool* out_result, bool result) { *out_result = result; },
          &callback_result));
  EXPECT_FALSE(callback_result);
}
}  // namespace blink

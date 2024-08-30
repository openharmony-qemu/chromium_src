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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"
#include "components/viz/common/quads/compositor_frame.h"

namespace cc {
namespace mojo_embedder {

namespace {
std::shared_ptr<SoftwareCompositorRendererOhos> g_softwareCompositor;
}  // namespace

class MockSoftwareCompositorRegistryOhos
    : public SoftwareCompositorRegistryOhos {
 public:
  MockSoftwareCompositorRegistryOhos() = default;
  ~MockSoftwareCompositorRegistryOhos() = default;
  void RegisterSoftwareRenderer(
      SoftwareCompositorRendererOhos* software_renderer);
  void UnregisterSoftwareRenderer(
      SoftwareCompositorRendererOhos* software_renderer);
};

class MockAsyncLayerTreeFrameSink : public AsyncLayerTreeFrameSink {
 public:
  MockAsyncLayerTreeFrameSink(
      scoped_refptr<viz::ContextProvider> context_provider,
      scoped_refptr<RasterContextProviderWrapper>
          worker_context_provider_wrapper,
      InitParams* params)
      : AsyncLayerTreeFrameSink(context_provider,
                                worker_context_provider_wrapper,
                                params) {}
  MockAsyncLayerTreeFrameSink(const MockAsyncLayerTreeFrameSink&) = delete;
  ~MockAsyncLayerTreeFrameSink() = default;
};

void MockSoftwareCompositorRegistryOhos::RegisterSoftwareRenderer(
    SoftwareCompositorRendererOhos* software_renderer) {}
void MockSoftwareCompositorRegistryOhos::UnregisterSoftwareRenderer(
    SoftwareCompositorRendererOhos* software_renderer) {}

class SoftwareCompositorRendererOhosMock
    : public SoftwareCompositorRendererOhos {
 public:
  SoftwareCompositorRendererOhosMock(AsyncLayerTreeFrameSink* sink,
                                     SoftwareCompositorRegistryOhos* registry)
      : SoftwareCompositorRendererOhos(sink, registry) {}
  SoftwareCompositorRendererOhosMock(const SoftwareCompositorRendererOhos&) =
      delete;
  ~SoftwareCompositorRendererOhosMock() = default;
};

class SoftwareCompositorRendererOhosTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};
void SoftwareCompositorRendererOhosTest::SetUpTestCase(void) {}

void SoftwareCompositorRendererOhosTest::TearDownTestCase(void) {}

void SoftwareCompositorRendererOhosTest::SetUp(void) {
  cc::mojo_embedder::AsyncLayerTreeFrameSink::InitParams params;
  auto sink_ =
      std::make_unique<MockAsyncLayerTreeFrameSink>(nullptr, nullptr, &params);

  auto registry_ = std::make_shared<MockSoftwareCompositorRegistryOhos>();
  g_softwareCompositor = std::make_shared<SoftwareCompositorRendererOhos>(
      sink_.get(), registry_.get());
  ASSERT_NE(g_softwareCompositor, nullptr);
}

void SoftwareCompositorRendererOhosTest::TearDown(void) {
  g_softwareCompositor = nullptr;
}

class LayerTreeFrameSinkClientMock : public cc::LayerTreeFrameSinkClient {
 public:
  LayerTreeFrameSinkClientMock() = default;
  ~LayerTreeFrameSinkClientMock() = default;

  void SetBeginFrameSource(viz::BeginFrameSource* source);
  absl::optional<viz::HitTestRegionList> BuildHitTestData();
  void ReclaimResources(std::vector<viz::ReturnedResource> resources);
  void SetTreeActivationCallback(base::RepeatingClosure callback);
  void DidReceiveCompositorFrameAck();
  void DidPresentCompositorFrame(uint32_t frame_token,
                                 const viz::FrameTimingDetails& details);
  void OnDraw(const gfx::Transform& transform,
              const gfx::Rect& viewport,
              bool resourceless_software_draw,
              bool skip_draw);
  void DidLoseLayerTreeFrameSink();
  void SetMemoryPolicy(const ManagedMemoryPolicy& policy);
  void SetExternalTilePriorityConstraints(const gfx::Rect& viewport_rect,
                                          const gfx::Transform& transform);
  void SetDrawRectState(bool isNeedDrawRect);
};

void LayerTreeFrameSinkClientMock::SetBeginFrameSource(
    viz::BeginFrameSource* source) {}
absl::optional<viz::HitTestRegionList>
LayerTreeFrameSinkClientMock::BuildHitTestData() {
  return absl::nullopt;
}
void LayerTreeFrameSinkClientMock::ReclaimResources(
    std::vector<viz::ReturnedResource> resources) {}
void LayerTreeFrameSinkClientMock::SetTreeActivationCallback(
    base::RepeatingClosure callback) {}
void LayerTreeFrameSinkClientMock::DidReceiveCompositorFrameAck() {}
void LayerTreeFrameSinkClientMock::DidPresentCompositorFrame(
    uint32_t frame_token,
    const viz::FrameTimingDetails& details) {}
void LayerTreeFrameSinkClientMock::OnDraw(const gfx::Transform& transform,
                                          const gfx::Rect& viewport,
                                          bool resourceless_software_draw,
                                          bool skip_draw) {}
void LayerTreeFrameSinkClientMock::DidLoseLayerTreeFrameSink() {}
void LayerTreeFrameSinkClientMock::SetMemoryPolicy(
    const ManagedMemoryPolicy& policy) {}
void LayerTreeFrameSinkClientMock::SetExternalTilePriorityConstraints(
    const gfx::Rect& viewport_rect,
    const gfx::Transform& transform) {}
void LayerTreeFrameSinkClientMock::SetDrawRectState(bool isNeedDrawRect) {}

class BeginFrameSourceMock : public viz::BeginFrameSource {
 public:
  explicit BeginFrameSourceMock(uint32_t restart_id)
      : BeginFrameSource(restart_id) {}
  BeginFrameSourceMock(const BeginFrameSourceMock&) = delete;
  ~BeginFrameSourceMock() = default;

  void DidFinishFrame(viz::BeginFrameObserver* obs);
  void AddObserver(viz::BeginFrameObserver* obs);
  void RemoveObserver(viz::BeginFrameObserver* obs);
  void OnGpuNoLongerBusy();
};

void BeginFrameSourceMock::DidFinishFrame(viz::BeginFrameObserver* obs) {}
void BeginFrameSourceMock::AddObserver(viz::BeginFrameObserver* obs) {}
void BeginFrameSourceMock::RemoveObserver(viz::BeginFrameObserver* obs) {}
void BeginFrameSourceMock::OnGpuNoLongerBusy() {}

TEST_F(SoftwareCompositorRendererOhosTest, BindToClient) {
  auto client = std::make_shared<LayerTreeFrameSinkClientMock>();
  auto source = std::make_shared<BeginFrameSourceMock>(1);
  g_softwareCompositor->BindToClient(client.get(), source.get());
  EXPECT_NE(client, nullptr);
}

TEST_F(SoftwareCompositorRendererOhosTest, DetachFromClient) {
  g_softwareCompositor->DetachFromClient();
}

TEST_F(SoftwareCompositorRendererOhosTest, DemandDrawSw) {
  SkCanvas canvas;
  gfx::SizeF size(800, 600);
  gfx::PointF offset(0, 0);
  gfx::Transform expected_transform;
  expected_transform.Translate(offset.x(), offset.y());
  auto client = std::make_shared<LayerTreeFrameSinkClientMock>();
  auto source = std::make_shared<BeginFrameSourceMock>(1);
  g_softwareCompositor->BindToClient(client.get(), source.get());

  bool result = g_softwareCompositor->DemandDrawSw(&canvas, size, offset);

  EXPECT_FALSE(result);
}

TEST_F(SoftwareCompositorRendererOhosTest, SendCompositorFrameAckToClient) {
  auto client = std::make_shared<LayerTreeFrameSinkClientMock>();
  auto source = std::make_shared<BeginFrameSourceMock>(1);
  g_softwareCompositor->BindToClient(client.get(), source.get());
  g_softwareCompositor->SendCompositorFrameAckToClient();
}

TEST_F(SoftwareCompositorRendererOhosTest, DrawRect) {
  gfx::Rect rect(10, 20, 30, 40);
  g_softwareCompositor->DrawRect(rect);
}
}  // namespace mojo_embedder
}  // namespace cc
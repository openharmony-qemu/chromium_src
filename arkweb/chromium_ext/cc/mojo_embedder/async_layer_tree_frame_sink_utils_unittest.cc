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

#include "arkweb/chromium_ext/cc/mojo_embedder/async_layer_tree_frame_sink_utils.h"
#include "base/threading/thread.h"
#include "cc/mojo_embedder/async_layer_tree_frame_sink.h"
#include "components/viz/test/test_context_provider.h"
#include "gpu/command_buffer/client/test_gpu_memory_buffer_manager.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "services/viz/public/mojom/compositing/compositor_frame_sink.mojom.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::_;
using testing::Mock;
using testing::NiceMock;

namespace cc {
namespace mojo_embedder {
namespace{

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

class MockSoftwareCompositorRendererOhos : public SoftwareCompositorRendererOhos {
 public:
  MockSoftwareCompositorRendererOhos(
      AsyncLayerTreeFrameSink* sink,
      SoftwareCompositorRegistryOhos* registry)
      : SoftwareCompositorRendererOhos(sink, registry) {}
  
  MOCK_METHOD(void, BindToClient, (LayerTreeFrameSinkClient*, viz::BeginFrameSource*), ());
  MOCK_METHOD(void, DetachFromClient, (), ());
  MOCK_METHOD(bool, InSoftwareDraw, (), (const));
  MOCK_METHOD(void, DrawAndSwapOnRenderer, (viz::CompositorFrame), ());
};

class AsyncLayerTreeFrameSinkUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    base::Thread bg_thread("BG Thread");
    bg_thread.Start();
    scoped_refptr<viz::TestContextProvider> provider =
      viz::TestContextProvider::CreateRaster();
    gpu::TestGpuMemoryBufferManager test_gpu_memory_buffer_manager;

    mojo::PendingRemote<viz::mojom::CompositorFrameSink> sink_remote;
    mojo::PendingReceiver<viz::mojom::CompositorFrameSink> sink_receiver =
        sink_remote.InitWithNewPipeAndPassReceiver();
    mojo::PendingRemote<viz::mojom::CompositorFrameSinkClient> client;

    AsyncLayerTreeFrameSink::InitParams init_params;
    init_params.compositor_task_runner = bg_thread.task_runner();
    init_params.gpu_memory_buffer_manager = &test_gpu_memory_buffer_manager;
    init_params.pipes.compositor_frame_sink_remote = std::move(sink_remote);
    init_params.pipes.client_receiver = client.InitWithNewPipeAndPassReceiver();
    auto layer_tree_frame_sink = std::make_unique<AsyncLayerTreeFrameSink>(
        std::move(provider), nullptr, /*shared_image_interface=*/nullptr,
        &init_params);
    utils_ = std::make_unique<AsyncLayerTreeFrameSinkUtils>(layer_tree_frame_sink.get());
    mock_registry_ = std::make_unique<MockSoftwareCompositorRegistryOhos>();
  }

  std::unique_ptr<mojo_embedder::AsyncLayerTreeFrameSinkUtils> utils_;
  std::unique_ptr<MockSoftwareCompositorRegistryOhos> mock_registry_;
};

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, BindToClientWithAndWithoutRenderer) {
  utils_->BindToClientArkWebSoftCom(nullptr, nullptr);
  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());
  EXPECT_CALL(*mock_renderer, BindToClient(_, _)).Times(1);
  utils_->BindToClientArkWebSoftCom(nullptr, nullptr);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, DetachFromClientWithAndWithoutRenderer) {
  utils_->DetachFromClientArkWebSoftCom();

  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());
  
  EXPECT_CALL(*mock_renderer, DetachFromClient()).Times(1);
  utils_->DetachFromClientArkWebSoftCom();
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameConditions) {
  viz::CompositorFrame frame;

  EXPECT_FALSE(utils_->SubmitCompositorFrameArkWebSoftCom(frame));

  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());

  ON_CALL(*mock_renderer, InSoftwareDraw()).WillByDefault(testing::Return(false));
  EXPECT_FALSE(utils_->SubmitCompositorFrameArkWebSoftCom(frame));

  ON_CALL(*mock_renderer, InSoftwareDraw()).WillByDefault(testing::Return(true));
  utils_->SubmitCompositorFrameArkWebSoftCom(frame);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, DfxDumpLogFirstAndSubsequentCalls) {
  viz::LocalSurfaceId id;
  utils_->SubmitCompositorFrameDfxDumpLog(id);
  utils_->SubmitCompositorFrameDfxDumpLog(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, OnBeginFrameArkWebDfxTrace) {
  int64_t id = 1;
  utils_->OnBeginFrameArkWebDfxTrace(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, OnBeginFrameArkWebSwBuTr) {
  int64_t id = 1;
  utils_->OnBeginFrameArkWebSwBuTr(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameArkWebSwapBuTr) {
  int64_t id = 1;
  utils_->SubmitCompositorFrameArkWebSwapBuTr(id, FrameSkippedReason::kRecoverLatency);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameDfxDumpTrace) {
  int64_t id = 1;
  utils_->SubmitCompositorFrameDfxDumpTrace(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, BindToClientWithAndWithoutRenderer1) {
  utils_->BindToClientArkWebSoftCom(nullptr, nullptr);
  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());
  EXPECT_CALL(*mock_renderer, BindToClient(_, _)).Times(1);
  utils_->BindToClientArkWebSoftCom(nullptr, nullptr);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, DetachFromClientWithAndWithoutRenderer1) {
  utils_->DetachFromClientArkWebSoftCom();

  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());
  
  EXPECT_CALL(*mock_renderer, DetachFromClient()).Times(1);
  utils_->DetachFromClientArkWebSoftCom();
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameConditions1) {
  viz::CompositorFrame frame;

  EXPECT_FALSE(utils_->SubmitCompositorFrameArkWebSoftCom(frame));

  auto mock_renderer = new NiceMock<MockSoftwareCompositorRendererOhos>(
      utils_->asyncLayerTreeFrameSink, mock_registry_.get());
  utils_->InitSoftComRenderArkWebSoftCom(mock_registry_.get());

  ON_CALL(*mock_renderer, InSoftwareDraw()).WillByDefault(testing::Return(false));
  EXPECT_FALSE(utils_->SubmitCompositorFrameArkWebSoftCom(frame));

  ON_CALL(*mock_renderer, InSoftwareDraw()).WillByDefault(testing::Return(true));
  utils_->SubmitCompositorFrameArkWebSoftCom(frame);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, DfxDumpLogFirstAndSubsequentCalls1) {
  viz::LocalSurfaceId id;
  utils_->SubmitCompositorFrameDfxDumpLog(id);
  utils_->SubmitCompositorFrameDfxDumpLog(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, OnBeginFrameArkWebDfxTrace1) {
  int64_t id = 1;
  utils_->OnBeginFrameArkWebDfxTrace(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, OnBeginFrameArkWebSwBuTr1) {
  int64_t id = 1;
  utils_->OnBeginFrameArkWebSwBuTr(id);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameArkWebSwapBuTr1) {
  int64_t id = 1;
  utils_->SubmitCompositorFrameArkWebSwapBuTr(id, FrameSkippedReason::kRecoverLatency);
}

TEST_F(AsyncLayerTreeFrameSinkUtilsTest, SubmitCompositorFrameDfxDumpTrace1) {
  int64_t id = 1;
  utils_->SubmitCompositorFrameDfxDumpTrace(id);
}

}  // namespace
}  // namespace mojo_embedder
}  // namespace cc
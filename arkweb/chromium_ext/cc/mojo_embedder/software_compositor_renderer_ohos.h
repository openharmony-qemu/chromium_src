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

#ifndef CC_MOJO_EMBEDDER_SOFTWARE_COMPOSITOR_RENDERER_OHOS_H_
#define CC_MOJO_EMBEDDER_SOFTWARE_COMPOSITOR_RENDERER_OHOS_H_

#include "cc/mojo_embedder/async_layer_tree_frame_sink.h"
#include "cc/mojo_embedder/mojo_embedder_export.h"
#include "cc/mojo_embedder/software_compositor_registry_ohos.h"
#include "cc/trees/layer_tree_frame_sink.h"
#include "cc/trees/layer_tree_frame_sink_client.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/common/surfaces/local_surface_id.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/geometry/transform.h"

namespace viz {
class BeginFrameSource;
class CompositorFrameSinkSupport;
class Display;
class FrameSinkManagerImpl;
class ParentLocalSurfaceIdAllocator;
}  // namespace viz

namespace cc::mojo_embedder {

class AsyncLayerTreeFrameSink;
class SoftwareDisplayClientOhos;

class CC_MOJO_EMBEDDER_EXPORT SoftwareCompositorRendererOhos {
 public:
  SoftwareCompositorRendererOhos(AsyncLayerTreeFrameSink* sink,
                                 SoftwareCompositorRegistryOhos* registry);

  SoftwareCompositorRendererOhos(const SoftwareCompositorRendererOhos&) =
      delete;
  SoftwareCompositorRendererOhos& operator=(
      const SoftwareCompositorRendererOhos&) = delete;

  ~SoftwareCompositorRendererOhos();

  void BindToClient(LayerTreeFrameSinkClient* client,
                    viz::BeginFrameSource* source);

  void DetachFromClient();

  void DrawAndSwapOnRenderer(viz::CompositorFrame frame);

  bool DemandDrawSw(SkCanvas* canvas, gfx::SizeF size, gfx::PointF offset);

  void DrawRect(const gfx::Rect& rect);

  void SendCompositorFrameAckToClient();

  bool InSoftwareDraw() { return in_software_draw_; }

 private:
  raw_ptr<AsyncLayerTreeFrameSink> sink_ = nullptr;
  raw_ptr<SoftwareCompositorRegistryOhos> registry_ = nullptr;
  raw_ptr<LayerTreeFrameSinkClient> client_ = nullptr;

  std::unique_ptr<viz::ServerSharedBitmapManager> shared_bitmap_manager_;
  std::unique_ptr<viz::FrameSinkManagerImpl> frame_sink_manager_;
  std::unique_ptr<viz::CompositorFrameSinkSupport> root_support_;
  std::unique_ptr<SoftwareDisplayClientOhos> display_client_;
  std::unique_ptr<viz::Display> display_;

  viz::ParentLocalSurfaceIdAllocator root_local_surface_id_allocator_;
  viz::LocalSurfaceId root_local_surface_id_;

  // weak reference  Owned by |display_|.
  // std::unique_ptr<viz::OutputSurface> output_surface_;
  SkCanvas* current_sw_canvas_ = nullptr;
  viz::DebugRendererSettings debug_settings_;

  // Draw info
  gfx::Size size_{0, 0};
  bool in_software_draw_ = false;
  bool software_draw_result_ = false;
  base::WeakPtrFactory<SoftwareCompositorRendererOhos> weak_ptr_factory_{this};
};
}  // namespace cc::mojo_embedder

#endif  // CC_MOJO_EMBEDDER_SOFTWARE_COMPOSITOR_RENDERER_OHOS_H_

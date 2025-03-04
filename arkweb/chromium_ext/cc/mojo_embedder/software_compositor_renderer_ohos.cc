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

#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"

#include "base/logging.h"
#include "cc/trees/layer_tree_frame_sink.h"
#include "cc/trees/layer_tree_frame_sink_client.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/common/surfaces/parent_local_surface_id_allocator.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display/display_client.h"
#include "components/viz/service/display/output_surface.h"
#include "components/viz/service/display/overlay_processor_stub.h"
#include "components/viz/service/display/software_output_device.h"
#include "components/viz/service/display_embedder/software_output_surface.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/transform.h"
#include "ui/gfx/skia_util.h"

namespace cc {
const viz::FrameSinkId kRootFrameSinkId(0, 1);

namespace mojo_embedder {

class SoftwareDisplayClientOhos : public viz::DisplayClient {
 public:
  SoftwareDisplayClientOhos() {}
  ~SoftwareDisplayClientOhos() override = default;
  void DisplayOutputSurfaceLost() override {}
  void DisplayWillDrawAndSwap(
      bool will_draw_and_swap,
      viz::AggregatedRenderPassList* render_passes) override {}
  void DisplayDidDrawAndSwap() override {}
  void DisplayDidReceiveCALayerParams(
      const gfx::CALayerParams& ca_layer_params) override {}
  void DisplayDidCompleteSwapWithSize(const gfx::Size& pixel_size) override {}
  void DisplayAddChildWindowToBrowser(
      gpu::SurfaceHandle child_window) override {}
  void SetWideColorEnabled(bool enabled) override {}
  void SetPreferredFrameInterval(base::TimeDelta interval) override {}
  base::TimeDelta GetPreferredFrameIntervalForFrameSinkId(
      const viz::FrameSinkId& id,
      viz::mojom::CompositorFrameSinkType* type) override {
    return viz::BeginFrameArgs::MinInterval();
  }
  void RestoreRenderFit(const viz::FrameSinkId& frame_sink_id) override {}
};

class SoftwareOutputDeviceOhos : public viz::SoftwareOutputDevice {
 public:
  SoftwareOutputDeviceOhos(SkCanvas** canvas) : canvas_(canvas) {}
  SoftwareOutputDeviceOhos(const SoftwareOutputDeviceOhos&) = delete;
  SoftwareOutputDeviceOhos& operator=(const SoftwareOutputDeviceOhos&) = delete;

  void Resize(const gfx::Size& pixel_size, float device_scale_factor) override {
  }

  SkCanvas* BeginPaint(const gfx::Rect& damage_rect) override {
    DCHECK(*canvas_) << "BeginPaint with no canvas set";
    return *canvas_;
  }
  void EndPaint() override {}

 private:
  SkCanvas** canvas_;
};

SoftwareCompositorRendererOhos::SoftwareCompositorRendererOhos(
    AsyncLayerTreeFrameSink* sink,
    SoftwareCompositorRegistryOhos* registry)
    : sink_(sink), registry_(registry) {}

SoftwareCompositorRendererOhos::~SoftwareCompositorRendererOhos() {}

void SoftwareCompositorRendererOhos::BindToClient(
    LayerTreeFrameSinkClient* client,
    viz::BeginFrameSource* source) {
  client_ = client;

  registry_->RegisterSoftwareRenderer(this);

  shared_bitmap_manager_ = std::make_unique<viz::ServerSharedBitmapManager>();
  frame_sink_manager_ = std::make_unique<viz::FrameSinkManagerImpl>(
      viz::FrameSinkManagerImpl::InitParams(shared_bitmap_manager_.get()));

  constexpr bool root_support_is_root = true;
  root_support_ = std::make_unique<viz::CompositorFrameSinkSupport>(
      sink_.get(), frame_sink_manager_.get(), kRootFrameSinkId,
      root_support_is_root);

  auto output_surface_ = std::make_unique<viz::SoftwareOutputSurface>(
      std::make_unique<SoftwareOutputDeviceOhos>(&current_sw_canvas_));

  viz::RendererSettings software_renderer_settings;
  auto overlay_processor = std::make_unique<viz::OverlayProcessorStub>();

  std::unique_ptr<viz::DisplayScheduler> scheduler;
  display_client_ = std::make_unique<SoftwareDisplayClientOhos>();

  display_ = std::make_unique<viz::Display>(
      shared_bitmap_manager_.get(), /*shared_image_manager=*/nullptr,
      /*sync_point_manager=*/nullptr,
      /*gpu_scheduler=*/nullptr, software_renderer_settings, &debug_settings_,
      kRootFrameSinkId, nullptr /* gpu::GpuTaskSchedulerHelper */,
      std::move(output_surface_), std::move(overlay_processor),
      std::move(scheduler) /* scheduler */, nullptr /* current_task_runner */);
  display_->Initialize(display_client_.get(),
                       frame_sink_manager_->surface_manager());
  frame_sink_manager_->RegisterFrameSinkId(kRootFrameSinkId, false);
  frame_sink_manager_->RegisterBeginFrameSource(source, kRootFrameSinkId);
  display_->SetVisible(true);
}

void SoftwareCompositorRendererOhos::DetachFromClient() {
  registry_->UnregisterSoftwareRenderer(this);
  shared_bitmap_manager_.reset();
  root_support_.reset();
  display_client_.reset();
  display_.reset();
  frame_sink_manager_ = nullptr;
}

void SoftwareCompositorRendererOhos::DrawAndSwapOnRenderer(
    viz::CompositorFrame frame) {
  display_->Resize(size_);

  root_local_surface_id_allocator_.GenerateId();
  root_local_surface_id_ =
      root_local_surface_id_allocator_.GetCurrentLocalSurfaceId();
  display_->SetLocalSurfaceId(root_local_surface_id_, 1.0f);
  root_support_->SubmitCompositorFrame(root_local_surface_id_, std::move(frame),
                                       client_->BuildHitTestData(), 0);

  LOG(INFO) << "SW render DrawAndSwap";
  base::TimeTicks now = base::TimeTicks::Now();
  bool result = display_->DrawAndSwap({now, now});
  software_draw_result_ = result;

  in_software_draw_ = false;
  SendCompositorFrameAckToClient();
}

bool SoftwareCompositorRendererOhos::DemandDrawSw(SkCanvas* canvas,
                                                  gfx::SizeF size,
                                                  gfx::PointF offset) {
  LOG(INFO) << "SW render DemandDrawSw";

  in_software_draw_ = true;
  current_sw_canvas_ = canvas;
  size_ = gfx::ToRoundedSize(size);

  gfx::Transform offset_transform;
  offset_transform.Translate(offset.x(), offset.y());

  client_->OnDraw(offset_transform, gfx::Rect(size_), in_software_draw_, false);
  return software_draw_result_;
}

void SoftwareCompositorRendererOhos::DrawRect(const gfx::Rect& rect) {
  if (client_ == nullptr) {
    LOG(ERROR) << "SW render DrawRect.";
    return;
  }
  client_->SetExternalTilePriorityConstraints(rect, gfx::Transform());
}

void SoftwareCompositorRendererOhos::SendCompositorFrameAckToClient() {
  client_->DidReceiveCompositorFrameAck();
}

}  // namespace mojo_embedder
}  // namespace cc
                  
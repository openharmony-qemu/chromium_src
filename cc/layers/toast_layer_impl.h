// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_TOAST_LAYER_IMPL_H_
#define CC_LAYERS_TOAST_LAYER_IMPL_H_

#include <memory>
#include <string>
#include <vector>

#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "cc/cc_export.h"
#include "cc/layers/layer_impl.h"
#include "cc/metrics/web_vital_metrics.h"
#include "cc/resources/memory_history.h"
#include "cc/resources/resource_pool.h"
#include "cc/trees/debug_rect_history.h"
#include "cc/trees/layer_tree_impl.h"
#include "third_party/skia/include/core/SkRefCnt.h"

class SkTypeface;
struct SkRect;

namespace viz {
class ClientResourceProvider;
}

namespace cc {
class DroppedFrameCounter;
class LayerTreeFrameSink;
class PaintCanvas;
class PaintFlags;

enum class ToastTextAlign { kLeft, kCenter, kRight };

class CC_EXPORT ToastLayerImpl : public LayerImpl {
 public:
  static std::unique_ptr<ToastLayerImpl> Create(
      LayerTreeImpl* tree_impl,
      int id) {
    return base::WrapUnique(new ToastLayerImpl(tree_impl, id));
  }
  ToastLayerImpl(const ToastLayerImpl&) = delete;
  ~ToastLayerImpl() override;

  ToastLayerImpl& operator=(const ToastLayerImpl&) = delete;

  std::unique_ptr<LayerImpl> CreateLayerImpl(
      LayerTreeImpl* tree_impl) const override;

  bool WillDraw(DrawMode draw_mode,
                viz::ClientResourceProvider* resource_provider) override;
  void AppendQuads(viz::CompositorRenderPass* render_pass,
                   AppendQuadsData* append_quads_data) override;
  void UpdateToastTexture(DrawMode draw_mode,
                          LayerTreeFrameSink* frame_sink,
                          viz::ClientResourceProvider* resource_provider,
                          bool gpu_raster,
                          const viz::CompositorRenderPassList& list);

  void ReleaseResources() override;

  gfx::Rect GetEnclosingVisibleRectInTargetSpace() const override;

  bool IsAnimatingToastContents() const;

  void SetToastTypeface(sk_sp<SkTypeface> typeface);

  void SetToastMessage(const LayerTreeExtraState& toast_message);

  // LayerImpl overrides.
  void PushPropertiesTo(LayerImpl* layer) override;

 private:
  ToastLayerImpl(LayerTreeImpl* tree_impl, int id);

  const char* LayerTypeAsString() const override;

  void AsValueInto(base::trace_event::TracedValue* dict) const override;

  void DrawToastContents(PaintCanvas* canvas);

  int bounds_width_in_dips() const {
    // bounds() is specified in layout coordinates, which is painted dsf away
    // from DIPs.
    return bounds().width() / layer_tree_impl()->painted_device_scale_factor();
  }

  ResourcePool::InUsePoolResource in_flight_resource_;
  std::unique_ptr<ResourcePool> pool_;
  // A reference to the DrawQuad that will be replaced by a quad containing the
  // toast's contents. The actual quad can't be created until UpdateToastTexture()
  // which happens during draw, so we hold this reference to it when
  // constructing the placeholder between these two steps in the draw process.
  raw_ptr<viz::DrawQuad> placeholder_quad_ = nullptr;
  // Used for software raster when it will be uploaded to a texture.
  sk_sp<SkSurface> staging_surface_;

  sk_sp<SkTypeface> typeface_;

  float internal_contents_scale_ = 1.0f;
  gfx::Size internal_content_bounds_;

  LayerTreeExtraState toast_message_;
  base::TimeTicks animationEndTime_;
  int toast_fade_step_ = 0;
};

}  // namespace cc

#endif  // CC_LAYERS_TOAST_LAYER_IMPL_H_

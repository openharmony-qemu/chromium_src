// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/toast_layer.h"

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "cc/layers/toast_layer_impl.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_refptr<ToastLayer> ToastLayer::Create() {
  return base::WrapRefCounted(new ToastLayer());
}

ToastLayer::ToastLayer()
    : typeface_(SkTypeface::MakeFromName("HarmonyOS Sans SC", SkFontStyle())) {
  if (!typeface_.Read(*this)) {
    typeface_.Write(*this) =
        SkTypeface::MakeFromName("sans-serif", SkFontStyle::Bold());
  }
  DCHECK(typeface_.Read(*this).get());
  SetIsDrawable(true);
  SkString skString;
  typeface_.Read(*this).get()->getFamilyName(&skString);
}

ToastLayer::~ToastLayer() = default;

void ToastLayer::UpdateLocationAndSize(
    const gfx::Size& device_viewport,
    float device_scale_factor) {
  float multiplier = 1.f / (device_scale_factor *
                            layer_tree_host()->painted_device_scale_factor());
  gfx::Size device_viewport_in_dips =
      gfx::ScaleToFlooredSize(device_viewport, multiplier);

  gfx::Size bounds_in_dips;

  const int kMaxToastWidth = 400;
  const int kToastLayerWidth = device_viewport_in_dips.width();
  const int kToastHeight = 36;
  // Approximate height for shadow.
  const int kToastLayerHeight = kToastHeight * 6;
  const int distance_to_bottom = 80;
  bounds_in_dips.SetSize(std::min(kToastLayerWidth, kMaxToastWidth),
      kToastLayerHeight);


  // DIPs are layout coordinates if painted dsf is 1. If it's not 1, then layout
  // coordinates are DIPs * painted dsf.
  auto bounds_in_layout_space = gfx::ScaleToCeiledSize(
      bounds_in_dips, layer_tree_host()->painted_device_scale_factor());

  SetBounds(bounds_in_layout_space);

  // set position
  float x = (device_viewport_in_dips.width() - bounds_in_dips.width()) / 2.f;
  float y = device_viewport_in_dips.height();
  y -= bounds_in_dips.height() / 2;
  y -= kToastHeight / 2;
  y -= distance_to_bottom;
  x *= layer_tree_host()->painted_device_scale_factor();
  y *= layer_tree_host()->painted_device_scale_factor();
  SetOffsetToTransformParent({x, y});
}

void ToastLayer::UpdateToastInfo(const LayerTreeExtraState& toast_message) {
  toast_message_.Write(*this) = toast_message;
  SetNeedsPushProperties();
}

std::unique_ptr<LayerImpl> ToastLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl) const {
  return ToastLayerImpl::Create(tree_impl, id());
}

void ToastLayer::PushPropertiesTo(
    LayerImpl* layer,
    const CommitState& commit_state,
    const ThreadUnsafeCommitState& unsafe_state) {
  Layer::PushPropertiesTo(layer, commit_state, unsafe_state);
  TRACE_EVENT0("cc", "ToastLayer::PushPropertiesTo");
  ToastLayerImpl* layer_impl = static_cast<ToastLayerImpl*>(layer);

  layer_impl->SetToastTypeface(typeface_.Write(*this));
  layer_impl->SetToastMessage(toast_message_.Read(*this));
}

bool ToastLayer::HasDrawableContent() const {
  return true;
}

} // namespace

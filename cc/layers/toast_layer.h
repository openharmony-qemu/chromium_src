// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_TOAST_LAYER_H_
#define CC_LAYERS_TOAST_LAYER_H_

#include "cc/cc_export.h"
#include "cc/debug/layer_tree_extra_state.h"
#include "cc/layers/layer.h"
#include "cc/metrics/web_vital_metrics.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

class ToastLayer : public Layer {
 public:
  static scoped_refptr<ToastLayer> Create();

  ToastLayer(const ToastLayer&) = delete;
  ToastLayer& operator=(const ToastLayer&) = delete;

  void UpdateLocationAndSize(const gfx::Size& device_viewport,
                             float device_scale_factor);

  void UpdateToastInfo(const LayerTreeExtraState& toast_message);

  std::unique_ptr<LayerImpl> CreateLayerImpl(
      LayerTreeImpl* tree_impl) const override;

  // Layer overrides.
  void PushPropertiesTo(LayerImpl* layer,
                        const CommitState& commit_state,
                        const ThreadUnsafeCommitState& unsafe_state) override;
 protected:
  ToastLayer();
  bool HasDrawableContent() const override;

 private:
  ~ToastLayer() override;

  ProtectedSequenceWritable<sk_sp<SkTypeface>> typeface_;
  ProtectedSequenceWritable<LayerTreeExtraState> toast_message_;
};

} // namespace
#endif // CC_LAYERS_TOAST_LAYER_H_

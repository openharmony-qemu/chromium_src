// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SURFACE_LAYER_H_
#define CC_LAYERS_SURFACE_LAYER_H_

#include <memory>

#include "cc/cc_export.h"
#include "cc/layers/deadline_policy.h"
#include "cc/layers/layer.h"
#include "components/viz/common/surfaces/surface_info.h"
#include "components/viz/common/surfaces/surface_range.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/size.h"

namespace base {
class WaitableEvent;
}

namespace cc {

// If given true, we should submit frames, as we are unoccluded on screen.
// If given false, we should not submit compositor frames.
// The second parameter is only used in tests to ensure that the
// UpdateSubmissionStateCB is called synchronously relative to the calling
// thread. That is, the calling thread will block on the given waitable event
// when calling the callback. It is the responsibility of the callback to signal
// the event once the state has been updated. If blocking is not required, then
// the second parameter will be nullptr.
using UpdateSubmissionStateCB =
    base::RepeatingCallback<void(bool is_visible, base::WaitableEvent*)>;

// A layer that renders a surface referencing the output of another compositor
// instance or client.
class CC_EXPORT SurfaceLayer : public Layer {
 public:
  static scoped_refptr<SurfaceLayer> Create();
  static scoped_refptr<SurfaceLayer> Create(UpdateSubmissionStateCB);

  SurfaceLayer(const SurfaceLayer&) = delete;
  SurfaceLayer& operator=(const SurfaceLayer&) = delete;

  void SetSurfaceId(const viz::SurfaceId& surface_id,
                    const DeadlinePolicy& deadline_policy);
  void SetOldestAcceptableFallback(const viz::SurfaceId& surface_id);

  // When stretch_content_to_fill_bounds is true, the scale of the embedded
  // surface is ignored and the content will be stretched to fill the bounds.
  void SetStretchContentToFillBounds(bool stretch_content_to_fill_bounds);
  bool stretch_content_to_fill_bounds() const {
    return stretch_content_to_fill_bounds_.Read(*this);
  }

  void SetSurfaceHitTestable(bool surface_hit_testable);

  void SetHasPointerEventsNone(bool has_pointer_events_none);

  void SetIsReflection(bool is_reflection);

  void SetOverrideChildPaintFlags(bool override_child_paint_flags);

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  using RectChangeCallback = base::RepeatingCallback<void(const gfx::Rect&)>;
  void SetVideoRectChangeCallback(RectChangeCallback callback);
  void OnLayerRectUpdate(const gfx::Rect& rect) override;
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  using LayerBoundsChangeCallback =
      base::RepeatingCallback<void(const gfx::Rect&)>;
  void SetLayerBoundsChangeCallback(LayerBoundsChangeCallback callback);
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  using RectVisibilityChangeCallback = base::RepeatingCallback<void(bool)>;
  static scoped_refptr<SurfaceLayer> Create(UpdateSubmissionStateCB update_submission_state_callback,
                                          RectChangeCallback callback,
                                          RectVisibilityChangeCallback visibilitycallback);
  using LayerRemovedVisibilityCallback = base::RepeatingCallback<void(bool)>;
  static scoped_refptr<SurfaceLayer> Create(
      UpdateSubmissionStateCB update_submission_state_callback,
      RectChangeCallback callback,
      RectVisibilityChangeCallback visibilitycallback,
      LayerRemovedVisibilityCallback layerRemovedCallback);
#endif

  // Layer overrides.
  std::unique_ptr<LayerImpl> CreateLayerImpl(
      LayerTreeImpl* tree_impl) const override;
  bool RequiresSetNeedsDisplayOnHdrHeadroomChange() const override;
  void SetLayerTreeHost(LayerTreeHost* host) override;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnLayerBoundsUpdate(const gfx::Rect& bounds) override;
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnLayerRectVisibilityChange(bool visibility) override;
  void ResetLayerRectUpdateCallback();
  void ResetLayerRectVisibilityChangeCallback();
  void CleanupVisibilityForRemovedLayer(bool visibility) override;
#endif

  const viz::SurfaceId& surface_id() const {
    return surface_range_.Read(*this).end();
  }

  const std::optional<viz::SurfaceId>& oldest_acceptable_fallback() const {
    return surface_range_.Read(*this).start();
  }

  std::optional<uint32_t> deadline_in_frames() const {
    return deadline_in_frames_.Read(*this);
  }

 protected:
  SurfaceLayer();
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  SurfaceLayer(UpdateSubmissionStateCB updateSubmissionStateCB,
                                          RectChangeCallback callback,
                                          RectVisibilityChangeCallback visibilitycallback,
                                          LayerRemovedVisibilityCallback layerRemovedCallback);
#endif
  explicit SurfaceLayer(UpdateSubmissionStateCB);
  bool HasDrawableContent() const override;

  void PushDirtyPropertiesTo(
      LayerImpl* layer,
      uint8_t dirty_flag,
      const CommitState& commit_state,
      const ThreadUnsafeCommitState& unsafe_state) override;

 private:
  ~SurfaceLayer() override;

  ProtectedSequenceWritable<UpdateSubmissionStateCB>
      update_submission_state_callback_;

  ProtectedSequenceReadable<viz::SurfaceRange> surface_range_;
  ProtectedSequenceWritable<std::optional<uint32_t>> deadline_in_frames_;

  ProtectedSequenceReadable<bool> stretch_content_to_fill_bounds_;

  // Whether or not the surface should submit hit test data when submitting
  // compositor frame. The bit represents that the surface layer may be
  // associated with an out-of-process iframe and viz hit testing needs to know
  // the hit test information of that iframe. This bit is different from a layer
  // being hit testable in the renderer, a hit testable surface layer may not
  // be surface hit testable (e.g., a surface layer created by video).
  ProtectedSequenceReadable<bool> surface_hit_testable_;

  // Whether or not the surface can accept pointer events. It is set to true if
  // the frame owner has pointer-events: none property.
  // TODO(sunxd): consider renaming it to oopif_has_pointer_events_none_ for
  // disambiguation.
  ProtectedSequenceWritable<bool> has_pointer_events_none_;

  // This surface layer is reflecting the root surface of another display.
  ProtectedSequenceReadable<bool> is_reflection_;

  // If true, then this layer should override its child layer's PaintFlags.
  // This is used for SurfaceLayers where the child layer is in the same DOM.
  ProtectedSequenceWritable<bool> override_child_paint_flags_{false};

  // Keep track when we change LayerTreeHosts as SurfaceLayerImpl needs to know
  // in order to keep the visibility callback state consistent.
  ProtectedSequenceWritable<bool> callback_layer_tree_host_changed_;

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  RectChangeCallback video_rect_change_callback_;
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  LayerBoundsChangeCallback layer_bounds_change_callback_;
#endif // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  base::RepeatingCallback<void(const gfx::Rect&)> rect_change_callback_;
  base::RepeatingCallback<void(bool)> rect_visibility_change_callback_;
  base::RepeatingCallback<void(bool)> layer_removed_visibility_callback_;
#if BUILDFLAG(ARKWEB_TEST)
  friend class SurfaceLayerForIncludeTest;
#endif
#endif // ARKWEB_SAME_LAYER
};

}  // namespace cc

#endif  // CC_LAYERS_SURFACE_LAYER_H_

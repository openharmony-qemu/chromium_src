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

#ifndef CC_LAYERS_LAYER_EXT_H_
#define CC_LAYERS_LAYER_EXT_H_

#include "ui/gfx/geometry/rect.h"

namespace cc {

class LayerExt {
 public:
  LayerExt() = default;
  ~LayerExt() = default;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void OnLayerRectUpdate(const gfx::Rect& rect) {}

  virtual void OnLayerRectVisibilityChange(bool visibility) {}

  virtual void CleanupVisibilityForRemovedLayer(bool visibility) {}

  virtual bool NativeEmbedOverlay() {
    return native_embed_overlay_;
  }

  virtual void SetNativeEmbedOverlay(bool native_embed_overlay) {
    native_embed_overlay_ = native_embed_overlay;
  }

  virtual bool NativeEmbedOverlayInfinity() {
    return native_embed_overlay_;
  }

  virtual void SetNativeEmbedOverlayInfinity(bool native_embed_overlay_infinity) {
    native_embed_overlay_infinity_ = native_embed_overlay_infinity;
  }
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  virtual void OnLayerBoundsUpdate(const gfx::Rect& bounds) {}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  virtual bool ShouldOverlay() { return should_overlay_; }
  virtual void SetShouldOverlay(bool should_overlay) {
    should_overlay_ = should_overlay;
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

 private:
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  bool should_overlay_{false};
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool native_embed_overlay_{false};
  bool native_embed_overlay_infinity_{false};
#endif
};
}  // namespace cc

#endif  // CC_LAYERS_LAYER_EXT_H_

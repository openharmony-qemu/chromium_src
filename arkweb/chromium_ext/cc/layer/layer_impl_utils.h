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

#ifndef CC_LAYERS_LAYER_IMPL_UTILS_H_
#define CC_LAYERS_LAYER_IMPL_UTILS_H_

#include <memory>
#include <string>

#include "cc/layers/layer_impl.h"
#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "ui/gfx/geometry/rect_f.h"

namespace cc {

class LayerImplUtils {
 public:
  LayerImplUtils(LayerImpl* layer_impl);

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void set_may_contain_native(bool yes) { may_contain_native_ = yes; }
  bool may_contain_native() const { return may_contain_native_; }
  void set_native_embed_id(int embedId) { native_embed_id_ = embedId; }
  int native_embed_id() const { return native_embed_id_; }
  void SetNativeRect(const gfx::RectF& rect);
  void SetInitScale(float scale);
  float GetInitScale() { return init_scale_; }
  gfx::RectF NativeRect() const;
  gfx::RectF GetNativeRect();

  void LayerImplPushPropertiesTo(LayerImpl* layer);
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void SetShouldInterceptTouchEvent(bool intercept);
  bool ShouldInterceptTouchEvent() const;
#endif
 private:
  raw_ptr<LayerImpl> layer_impl_;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool may_contain_native_;
  gfx::RectF native_rect_;
  int native_embed_id_;
  float init_scale_ = -1.0f;
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  bool should_intercept_touch_event_ = false;
#endif
};

}  // namespace cc

#endif  // CC_LAYERS_LAYER_IMPL_UTILS_H_
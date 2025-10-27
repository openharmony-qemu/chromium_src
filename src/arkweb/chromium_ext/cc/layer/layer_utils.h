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

#ifndef CC_LAYERS_LAYER_UTILS_H_
#define CC_LAYERS_LAYER_UTILS_H_

#include <memory>
#include <string>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

class LayerUtils {
 public:
  LayerUtils(Layer* layerl);

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetMayContainNative(bool value);

  bool may_contain_native() const;

  void SetNativeEmbedId(int embedId);

  int native_embed_id() const;

  void SetNativeRect(const gfx::RectF& rect);

  void PushPropertiesToImpl(LayerImpl* layer_impl);
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void SetShouldInterceptTouchEvent(bool intercept);
  bool ShouldInterceptTouchEvent();
#endif

 private:
  raw_ptr<Layer> layer_;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  ProtectedSequenceReadable<bool> native_;
  ProtectedSequenceReadable<int> native_embed_id_;
  gfx::RectF native_rect_;
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  ProtectedSequenceReadable<bool> should_intercept_touch_event_{false};
#endif
};

}  // namespace cc

#endif  // CC_LAYERS_LAYER_UTILS_H_
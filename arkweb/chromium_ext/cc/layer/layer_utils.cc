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

#include "arkweb/chromium_ext/cc/layer/layer_utils.h"

namespace cc {

LayerUtils::LayerUtils(Layer* layer)
    : layer_(layer),
#if BUILDFLAG(ARKWEB_SAME_LAYER)
      native_(false)
#endif
{}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void LayerUtils::PushPropertiesToImpl(LayerImpl* layer_impl) {
  layer_impl->layer_impl_utils()->set_may_contain_native(may_contain_native());
  layer_impl->layer_impl_utils()->set_native_embed_id(native_embed_id());
  layer_impl->layer_impl_utils()->SetNativeRect(native_rect_);
  layer_impl->layer_impl_utils()->SetShouldInterceptTouchEvent(
      ShouldInterceptTouchEvent());
}

void LayerUtils::SetMayContainNative(bool value) {
  native_.Write(*(layer_.get())) = value;
}

bool LayerUtils::may_contain_native() const {
  return native_.Read(*(layer_.get()));
}

int LayerUtils::native_embed_id() const {
  return native_embed_id_.Read(*(layer_.get()));
}

void LayerUtils::SetNativeRect(const gfx::RectF& rect) {
  native_rect_ = rect;
}

void LayerUtils::SetNativeEmbedId(int embedId) {
  if (native_embed_id() == embedId) {
    return;
  }
  native_embed_id_.Write(*(layer_.get())) = embedId;
  layer_->SetNeedsPushProperties();
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void LayerUtils::SetShouldInterceptTouchEvent(bool intercept) {
  should_intercept_touch_event_.Write(*(layer_.get())) = intercept;
}
bool LayerUtils::ShouldInterceptTouchEvent() {
  return should_intercept_touch_event_.Read(*(layer_.get()));
}
#endif

}  // namespace cc

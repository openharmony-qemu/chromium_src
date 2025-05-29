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

#ifndef CC_LAYERS_VIDEO_LAYER_IMPL_UTILS_H_
#define CC_LAYERS_VIDEO_LAYER_IMPL_UTILS_H_

#include <memory>
#include <string>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {
class VideoLayerImpl;

class VideoLayerImplUtils {
public:
  VideoLayerImpl* impl;
  VideoLayerImplUtils(VideoLayerImpl* videoLayerImpl);

  void VisibilityChange();
  void LayerRectUpdate();
  bool HasNativeLayer();

private:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  gfx::Rect bounds_quad_rect_;
  bool visibility_ = false;
#endif
};

}  // namespace cc

#endif  // CC_LAYERS_VIDEO_LAYER_IMPL_UTILS_H_
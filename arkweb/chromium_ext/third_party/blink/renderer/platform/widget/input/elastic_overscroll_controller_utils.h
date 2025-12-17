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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_ELASTIC_OVERSCROLL_CONTROLLER_UTILS_H
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_ELASTIC_OVERSCROLL_CONTROLLER_UTILS_H

#include "arkweb/build/features/features.h"
#include "cc/input/scroll_elasticity_helper.h"
#include "third_party/blink/renderer/platform/widget/input/elastic_overscroll_controller.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/vector2d_conversions.h"
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_manager.h"
#endif

namespace blink {
class ElasticOverscrollController;

class PLATFORM_EXPORT ElasticOverscrollControllerUtils {
 public:
  explicit ElasticOverscrollControllerUtils(ElasticOverscrollController*);

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  gfx::Vector2dF GetOverScrollOffset() {
    return elastic_overscroll_controller_->stretch_scroll_force_;
  }
  void SetInputHandlerProxyClient(InputHandlerProxyClient* client) {
    client_ = client;
  }
  void OnOverScrollOffsetChanged() {
    if (client_) {
      client_->OnOverScrollOffsetChanged(
          elastic_overscroll_controller_->stretch_scroll_force_.x(),
          elastic_overscroll_controller_->stretch_scroll_force_.y());
    }
  }
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetOverscrollMode(int mode) { overscroll_mode_ = mode; }
  int GetOverscrollMode() { return overscroll_mode_; }
  cc::ScrollElasticityHelper* GetScrollElasticityHelper() const {
    return elastic_overscroll_controller_->helper_;
  }
  int overscroll_mode_ = 0;
#endif
  raw_ptr<ElasticOverscrollController> elastic_overscroll_controller_;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  raw_ptr<InputHandlerProxyClient> client_;
#endif 
};
}  // namespace blink
#endif  // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_ELASTIC_OVERSCROLL_CONTROLLER_UTILS_H

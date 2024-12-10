/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CONTENT_BROWSER_OHOS_OVERSCROLL_CONTROLLER_ANDROID_H_
#define CONTENT_BROWSER_OHOS_OVERSCROLL_CONTROLLER_ANDROID_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "content/common/content_export.h"
#include "third_party/blink/public/mojom/input/input_event_result.mojom-shared.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/ohos/overscroll_refresh.h"

namespace blink {
class WebGestureEvent;
}

namespace cc::slim {
class Layer;
}

namespace ui {
struct DidOverscrollParams;
}

namespace content {

// Glue class for handling all inputs into Android-specific overscroll effects,
// both the passive overscroll glow and the active overscroll pull-to-refresh.
// Note that all input coordinates (both for events and overscroll) are in DIPs.
class CONTENT_EXPORT OverscrollControllerOHOS {
 public:
  OverscrollControllerOHOS(ui::OverscrollRefreshHandler* handler);

  OverscrollControllerOHOS(const OverscrollControllerOHOS&) = delete;
  OverscrollControllerOHOS& operator=(const OverscrollControllerOHOS&) = delete;

  ~OverscrollControllerOHOS();

  // Returns true if |event| is consumed by an overscroll effect, in which
  // case it should cease propagation.
  bool WillHandleGestureEvent(const blink::WebGestureEvent& event);

  // To be called upon receipt of a gesture event ack.
  void OnGestureEventAck(const blink::WebGestureEvent& event,
                         blink::mojom::InputEventResultState ack_result);

  // To be called upon receipt of an overscroll event.
  void OnOverscrolled(const ui::DidOverscrollParams& overscroll_params);

  // To be called whenever the content frame has been updated.
  void OnFrameMetadataUpdated(float page_scale_factor,
                              float device_scale_factor,
                              const gfx::SizeF& scrollable_viewport_size,
                              const gfx::SizeF& root_layer_size,
                              const gfx::PointF& root_scroll_offset,
                              bool root_overflow_y_hidden);

  // Toggle activity of any overscroll effects. When disabled, events will be
  // ignored until the controller is re-enabled.
  void Enable();
  void Disable();

  void DidStopRefresh();

 private:
  void SetNeedsAnimate();

  bool enabled_;

  // TODO(jdduke): Factor out a common API from the two overscroll effects.
  std::unique_ptr<ui::OverscrollRefresh> refresh_effect_;
};

}  // namespace content

#endif  // CONTENT_BROWSER_OHOS_OVERSCROLL_CONTROLLER_ANDROID_H_

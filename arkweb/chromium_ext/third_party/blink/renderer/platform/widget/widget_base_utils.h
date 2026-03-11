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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_BASE_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_BASE_UTILS_H_

#include "base/sequence_checker.h"
#include "third_party/blink/renderer/platform/widget/compositing/layer_tree_view_delegate.h"
#include "third_party/blink/renderer/platform/widget/compositing/render_frame_metadata_observer_impl.h"
#include "third_party/blink/renderer/platform/widget/input/widget_base_input_handler.h"
#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_manager.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"

namespace blink {
class WidgetBase;

class BLINK_PLATFORM_EXPORT WidgetBaseUtils {
 public:
  WidgetBaseUtils(WidgetBase* widget_base);

#if BUILDFLAG(IS_ARKWEB)
  void ReportForegroundThreadPool();
  bool GetCmdValue();
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetZoomLevel(float magnify_delta, const gfx::Point& anchor);
  void SetOverscrollMode(int mode);
  void SetRequestKeyboardReason(int32_t requestKeyboardReason);
  int32_t GetRequestKeyboardReason() const { return requestKeyboardReason_; }
  bool IsElementExist(std::string xPath);
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  gfx::Vector2dF GetOverScrollOffset();
  void OnOverScrollOffsetChanged(float offset_x, float offset_y);
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void TouchHitTest(const WebPointerEvent& event, size_t i);
  void NativeHitTestResult(bool isNative, size_t fingerId, int layerId);
  void DidNativeEmbedEvent(blink::WebInputEvent::Type type,
                           std::string embedId,
                           int32_t id,
                           float x,
                           float y);
  void MouseHitTest(const WebMouseEvent& event, int32_t button);
  void NativeMouseHitTestResult(bool isNative, int layerId, int32_t button);
  void DidNativeEmbedMouseEvent(blink::WebInputEvent::Type type, blink::WebInputEvent::Modifiers modifiers,
    std::string embedId, bool isHitNativeArea, float x, float y);
#endif
  base::WeakPtr<WidgetBaseUtils> GetWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }

 private:
  raw_ptr<WidgetBase> widget_base_;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  int32_t requestKeyboardReason_ = 0;
#endif
#if BUILDFLAG(IS_ARKWEB)
  bool cmd_value_ = false;
#endif
  SEQUENCE_CHECKER(sequence_checker_);
  base::WeakPtrFactory<WidgetBaseUtils> weak_ptr_factory_{this};
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_BASE_UTILS_H_

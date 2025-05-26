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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_EVENT_MANAGER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_EVENT_MANAGER_EXT_H_

#include "third_party/blink/renderer/core/input/mouse_event_manager.h"
#include "arkweb/build/features/features.h"

#if BUILDFLAG(IS_ARKWEB)
#include "third_party/blink/renderer/core/dom/events/native_event_listener.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/skia/include/core/SkBitmap.h"
#endif

namespace gfx {
class PointF;
}  // namespace gfx

namespace blink {

class LocalFrame;
class ScrollManager;
class MouseEventManager;
class NativeEventListener;
class LayoutObject;

#if BUILDFLAG(ARKWEB_AI)
enum class HitOverlayStatus { kNone, kCreating, kCreated };
#endif

class MouseEventManagerExt
    : public MouseEventManager {
 public:
  MouseEventManagerExt(LocalFrame&, ScrollManager&);
  MouseEventManagerExt(const MouseEventManagerExt&) = delete;
  MouseEventManagerExt& operator=(const MouseEventManagerExt&) = delete;
  virtual ~MouseEventManagerExt();
  MouseEventManagerExt* AsMouseEventManagerExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool IsDraging();
#endif

#if BUILDFLAG(ARKWEB_AI)
  void HandleCreateOverlayWhenDrag(const MouseEventWithHitTestResults& event);
  void StopCreateOverlayTimer();
  void HandleGestureCreateOverlay(const WebGestureEvent& gesture_event);
  void CreateOverlayCallback();
  bool GetOverlayInProgress();
  void ConverCoordinates(float& left, float& top, float& width, float& height, std::vector<gfx::PointF>& pointfs);
  void SetOverlayStyle(Element* overlay_div, float left, float top, float width, float height);
  void SetTextStyle(Element* text_div, float left, float top, float width, float height, float font_size);
  void RemoveElementOnMouseOut(const std::string& id, blink::Event* event);
  template <typename T>
  void HandleCreateOverlay(T const& targeted_event);
  enum class FoldStatus : uint32_t {
    UNKNOWN = 0,
    FULL = 1,
    MAIN = 2,
    SUB = 3,
    COORDINATION = 4,
  };
  bool IsValidOverlayNode(Node* node);
  HitOverlayStatus GetHitOverlayStatus(const HitTestResult& hit_test_result, bool ignore_overlay_status = false);
  HitOverlayStatus GetHitOverlayStatusFromMouseEvent(
      const MouseEventWithHitTestResults& event);
  void CloseImageOverlay();
  void GetAbsImageRect(gfx::RectF& abs_rect);
  void SetOverlayInProgress(bool flag);
  void SetOverlayCreatingStatus(bool flag);
  void OnDestroyImageAnalyzerOverlay();
  void OnFoldStatusChanged(uint32_t foldstatus);
  void CloseImageOverlayWhenMousePress(const MouseEventWithHitTestResults& event);
  void Trace(Visitor*) const override;
#endif

#if BUILDFLAG(IS_ARKWEB)
  bool overlay_in_progress_ = false;
  WebMouseEvent last_mouse_drag_;
  Image* last_analyzed_image_ = nullptr;
  base::RetainingOneShotTimer create_overlay_timer_;
  WeakCellFactory<MouseEventManagerExt> weak_factory_{this};
  gfx::Rect image_rect_;
  gfx::Rect view_rect_;
  SkBitmap bm_;
  bool overlay_creating_ = false;
  double fold_screen_status_ = 1.0;
  Member<Node> hit_image_node_;

  class OverLayerMouseLeaveEventListener : public NativeEventListener {
   public:
    OverLayerMouseLeaveEventListener(blink::Element* element);
    void Invoke(ExecutionContext*, Event* event) override;
    void Trace(Visitor* visitor) const override;

   private:
    Member<blink::Element> element_;
  };
#endif
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_EVENT_MANAGER_EXT_H_
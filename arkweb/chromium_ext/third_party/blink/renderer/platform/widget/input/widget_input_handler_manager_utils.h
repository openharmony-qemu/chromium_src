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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_WIDGET_INPUT_HANDLER_MANAGER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_WIDGET_INPUT_HANDLER_MANAGER_UTILS_H_


#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_manager.h"

namespace cc {

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
namespace mojo_embedder {
class SoftwareCompositorRegistryOhos;
}
#endif
}

namespace blink {

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
class SoftwareCompositorProxyRegistryOhos;
#endif
class WidgetInputHandlerManager;


class WidgetInputHandlerManagerUtils {
public:
  WidgetInputHandlerManagerUtils(WidgetInputHandlerManager* manager);
  ~WidgetInputHandlerManagerUtils();

  base::WeakPtr<WidgetInputHandlerManagerUtils> AsWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void AttachSoftwareCompositorOhos(
      mojo::PendingReceiver<mojom::blink::SoftwareCompositorOhos>
          compositor_request);
  void CreateProxy();
  void DestroyProxy();
  cc::mojo_embedder::SoftwareCompositorRegistryOhos*
  GetSoftwareCompositorRegistryOhos();
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetZoomLevel(float magnify_delta, const gfx::Point& anchor);
  void AsyncSetZoomLevel(float magnify_delta, const gfx::Point& anchor);
  void SetOverscrollMode(int mode);
  void ScrollBy(float delta_x, float delta_y);
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  gfx::Vector2dF GetOverScrollOffset();
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition);
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void DidNativeEmbedEvent(blink::WebInputEvent::Type type,
                           std::string embedId,
                           int32_t id,
                           float x,
                           float y);
  void SetGestureEventResult(bool result, bool stopPropagation);
  void NativeHitTestResult(bool isNative, size_t fingerId, int layerId);
  void AsyncNativeHitTestResult(bool isNative, size_t fingerId, int layerId);
  void TouchHitTest(const WebPointerEvent& event, size_t fingerId);
  void SetNativeEmbedMode(bool flag);
  void DidNativeEmbedMouseEvent(blink::WebInputEvent::Type type,
                          blink::WebInputEvent::Modifiers modifiers,
                          std::string embedId,
                          bool isHitNativeArea,
                          float x,
                          float y);
  void SetMouseEventResult(bool result, bool stopPropagation);
  void NativeMouseHitTestResult(bool isNative, int layerId);
  void AsyncNativeMouseHitTestResult(bool isNative, int layerId);
  void MouseHitTest(const WebMouseEvent& event);
#endif

private:
  raw_ptr<WidgetInputHandlerManager> manager_;
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  std::unique_ptr<SoftwareCompositorProxyRegistryOhos> software_proxy_registry_;
#endif
  base::WeakPtrFactory<WidgetInputHandlerManagerUtils> weak_ptr_factory_{this};
};

}
#endif  //THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_WIDGET_INPUT_HANDLER_MANAGER_UTILS_H_
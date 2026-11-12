// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_CLIENT_H_

namespace blink {

// All callbacks invoked from the compositor thread.
class InputHandlerProxyClient {
 public:
  // Called just before the InputHandlerProxy shuts down.
  virtual void WillShutdown() = 0;
  virtual void DidStartScrollingViewport() = 0;
  virtual void SetAllowedTouchAction(cc::TouchAction touch_action) = 0;
  virtual bool AllowsScrollResampling() = 0;
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void DidNativeEmbedEvent(blink::WebInputEvent::Type type,
                                   std::string embedId,
                                   int32_t id,
                                   float x,
                                   float y) = 0;
  virtual void TouchHitTest(const WebPointerEvent& event, size_t i) = 0;
  virtual void DidNativeEmbedMouseEvent(blink::WebInputEvent::Type type,
                                  blink::WebInputEvent::Modifiers modifiers,
                                  std::string embedId,
                                  bool isHitNativeArea,
                                  float x,
                                  float y) = 0;
  virtual void MouseHitTest(const WebMouseEvent& event, int32_t button) = 0;
#endif
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  virtual void OnOverScrollOffsetChanged(float offset_x, float offset_y) = 0;
#endif
 protected:
  virtual ~InputHandlerProxyClient() {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_CLIENT_H_

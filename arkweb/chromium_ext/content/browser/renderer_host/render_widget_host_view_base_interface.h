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

#ifndef CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_INTERFACE_H_
#define CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_INTERFACE_H_

namespace content {

// Basic implementation shared by concrete RenderWidgetHostView subclasses.
class CONTENT_EXPORT RenderWidgetHostViewBaseInterface {
 public:
#if BUILDFLAG(IS_ARKWEB)
  virtual void SendInternalBeginFrame() {}
#endif
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  virtual void SendAccessibilityEvent(int64_t accessibilityId,
                                      int32_t eventType,
                                      const std::string& argument) {}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  virtual void DynamicFrameLossEvent(const std::string& sceneId, bool isStart) {}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void DidNativeEmbedEvent(const blink::mojom::NativeEmbedTouchEventPtr& touchEvent) {}
  virtual void DidNativeEmbedMouseEvent(const blink::mojom::NativeEmbedMouseEventPtr& mouseEvent) {}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void KeyboardReDispatch(const input::NativeWebKeyboardEvent& event, bool isUsed) {}
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual std::vector<int8_t> GetWordSelection(const std::string& text,
                                               int8_t offset) {
    return { -1, -1 };
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  virtual void OnDetectedBlankScreen(const std::string& url,
                                     int32_t blankScreenReason,
                                     int32_t detectedContentfulNodesCount) {}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
  virtual void OnFirstScreenPaint(const std::string& url,
                                  int64_t navigationStartTime,
                                  int64_t firstScreenPaintTime) {}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  virtual void SendCurrentLanguage(const std::string& ans) {}
#endif
};

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_INTERFACE_H_

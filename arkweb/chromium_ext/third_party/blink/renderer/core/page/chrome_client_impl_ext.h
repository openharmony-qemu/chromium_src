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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_IMPL_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_IMPL_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/page/chrome_client_impl.h"

namespace blink {
class ChromeClientImpl;
class WebViewImpl;

class ChromeClientImplExt : public ChromeClientImpl {
 public:
  explicit ChromeClientImplExt(WebViewImpl*);

  ChromeClientImplExt* AsChromeClientImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect GetVisibleRectToWeb(LocalFrame*) override;
#endif  // ARKWEB_DRAG_DROP

#if BUILDFLAG(ARKWEB_AI)
  void CreateOverlay(
      LocalFrame* frame,
      const SkBitmap& image,
      const gfx::Point& touch_point,
      GetAbsImageRectCallback get_rect_callback,
      OnTextSelectedCallback callback,
      OnDestroyImageAnalyzerOverlayCallback destroy_callback) override;
  uint32_t GetFoldStatus(LocalFrame* frame) override;
  WTF::Vector<int8_t> GetWordSelection(LocalFrame* frame,
                                       const WTF::String& text,
                                       int8_t offset) override;
#endif  // ARKWEB_AI

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  bool SetScrollbarAvoidAreaTop(double margin) override;
  bool SetScrollbarAvoidAreaBottom(double margin) override;
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_IMPL_H_

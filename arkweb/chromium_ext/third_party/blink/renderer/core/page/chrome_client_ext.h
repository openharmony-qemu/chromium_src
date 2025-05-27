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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_EXT_H_
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_EXT_H_

#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "arkweb/build/features/features.h"

namespace blink {
class LocalFrame;

class ChromeClientExt : public ChromeClient {
public:
  ChromeClientExt(const ChromeClientExt&) = delete;
  ChromeClientExt& operator=(const ChromeClientExt&) = delete;
  ~ChromeClientExt() override = default;
  ChromeClientExt* AsChromeClientExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  virtual gfx::Rect GetVisibleRectToWeb(LocalFrame*) { return gfx::Rect(); }
#endif

#if BUILDFLAG(ARKWEB_AI)
  using GetAbsImageRectCallback = base::RepeatingCallback<void(gfx::RectF&)>;
  using OnTextSelectedCallback = base::RepeatingCallback<void(bool)>;
  using OnDestroyImageAnalyzerOverlayCallback = base::RepeatingCallback<void()>;
  virtual void CreateOverlay(
      LocalFrame* frame,
      const SkBitmap& image,
      const gfx::Point& touch_point,
      GetAbsImageRectCallback get_rect_callback,
      OnTextSelectedCallback callback,
      OnDestroyImageAnalyzerOverlayCallback destroy_callback) {}
  virtual uint32_t GetFoldStatus(LocalFrame* frame) { return 0; }
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual WTF::Vector<int8_t> GetWordSelection(LocalFrame* frame,
                                               const WTF::String& text,
                                               int8_t offset) { return {-1, -1}; }
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  virtual bool SetScrollbarAvoidAreaTop(double margin) { return false; }
  virtual bool SetScrollbarAvoidAreaBottom(double margin) { return false; }
#endif // ARKWEB_SCROLLBAR_AVOID_AREA
protected:
  ChromeClientExt() = default;

};
}



#endif // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CHROME_CLIENT_EXT_H_
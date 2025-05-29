// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_DESKTOP_BROWSER_FRAME_AURA_OHOS_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_DESKTOP_BROWSER_FRAME_AURA_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/views/frame/desktop_browser_frame_aura.h"

class BrowserDesktopWindowTreeHostOhos;

// Provides the window frame for the Chrome browser window on Desktop ohos.
class DesktopBrowserFrameAuraOhos : public DesktopBrowserFrameAura {
 public:
  DesktopBrowserFrameAuraOhos(BrowserFrame* browser_frame,
                              BrowserView* browser_view);

  DesktopBrowserFrameAuraOhos(const DesktopBrowserFrameAuraOhos&) = delete;
  DesktopBrowserFrameAuraOhos& operator=(const DesktopBrowserFrameAuraOhos&) =
      delete;

  void set_host(BrowserDesktopWindowTreeHostOhos* host) { host_ = host; }

 protected:
  ~DesktopBrowserFrameAuraOhos() override;

  views::Widget::InitParams GetWidgetParams() override;
  bool UseCustomFrame() const override;

 private:
  void OnWidgetInitialized();

  raw_ptr<BrowserDesktopWindowTreeHostOhos> host_ = nullptr;
};

#endif  // CHROME_BROWSER_UI_VIEWS_FRAME_DESKTOP_BROWSER_FRAME_AURA_OHOS_H_

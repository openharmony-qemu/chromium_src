// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_DESKTOP_WINDOW_TREE_HOST_OHOS_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_DESKTOP_WINDOW_TREE_HOST_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/views/frame/browser_desktop_window_tree_host.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_ohos.h"  // nogncheck

class BrowserFrame;
class BrowserView;

namespace views {
class DesktopNativeWidgetAura;
}  // namespace views

class BrowserDesktopWindowTreeHostOhos
    : public BrowserDesktopWindowTreeHost,
      public views::DesktopWindowTreeHostOhos {
 public:
  BrowserDesktopWindowTreeHostOhos(
      views::internal::NativeWidgetDelegate* native_widget_delegate,
      views::DesktopNativeWidgetAura* desktop_native_widget_aura,
      BrowserView* browser_view,
      BrowserFrame* browser_frame);

  ~BrowserDesktopWindowTreeHostOhos() override = default;

  BrowserDesktopWindowTreeHostOhos(const BrowserDesktopWindowTreeHostOhos&) =
      delete;
  BrowserDesktopWindowTreeHostOhos& operator=(
      const BrowserDesktopWindowTreeHostOhos&) = delete;

 private:
  DesktopWindowTreeHost* AsDesktopWindowTreeHost() override;
  int GetMinimizeButtonOffset() const override;
  bool UsesNativeSystemMenu() const override;
};

#endif  // CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_DESKTOP_WINDOW_TREE_HOST_OHOS_H_

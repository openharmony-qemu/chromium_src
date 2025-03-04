// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/logging.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/host/ohos_fake_window.h"
#include "ui/ozone/platform/ohos/host/ohos_popup.h"
#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/platform_window/platform_window_init_properties.h"

namespace ui {

// static
std::unique_ptr<OhosWindow> OhosWindow::Create(
    PlatformWindowDelegate* delegate,
    OhosWindowManager* manager,
    PlatformWindowInitProperties properties) {
  std::unique_ptr<OhosWindow> window;
  switch (properties.type) {
    case PlatformWindowType::kPopup:
    case PlatformWindowType::kMenu:
    case PlatformWindowType::kTooltip:
      // sub window created with a parent window set.
      if (auto* parent = manager->GetWindow(properties.parent_widget)) {
        window = std::make_unique<OhosPopup>(delegate, manager, parent);
      } else {
        LOG(WARNING) << "Failed to determine parent for menu/popup window.";
        window = std::make_unique<OhosFakeWindow>(delegate, manager);
      }
      break;
    case PlatformWindowType::kWindow:
      window = std::make_unique<OhosToplevelWindow>(delegate, manager);
      break;
    default:
      window = std::make_unique<OhosFakeWindow>(delegate, manager);
      break;
  }
  window->Initialize(std::move(properties));
  return window;
}

}  // namespace ui

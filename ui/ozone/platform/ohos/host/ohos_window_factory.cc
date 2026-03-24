// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/logging.h"
#include "ohos/adapter/context/context_adapter.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/host/ohos_fake_window.h"
#include "ui/ozone/platform/ohos/host/ohos_pip_window.h"
#include "ui/ozone/platform/ohos/host/ohos_popup.h"
#include "ui/ozone/platform/ohos/host/ohos_toplevel_pad_window.h"
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
  LOG(INFO) << "[ohoswindow] OhosWindow::Create enter, type is "
            << static_cast<int>(properties.type);
  std::unique_ptr<OhosWindow> window;
  switch (properties.type) {
    case PlatformWindowType::kPopup:
    case PlatformWindowType::kMenu:
    case PlatformWindowType::kTooltip:
      // sub window created with a parent window set.
      if (auto* parent = manager->GetWindow(properties.parent_widget)) {
        window = std::make_unique<OhosPopup>(delegate, manager, parent);
      } else {
        LOG(WARNING) << "[ohoswindow] "
                     << "Failed to determine parent for menu/popup window.";
        window = std::make_unique<OhosFakeWindow>(delegate, manager);
      }
      break;
    case PlatformWindowType::kWindow:
      if (properties.using_system_floating_window &&
          PipWindowAdapter::GetInstance().IsSupportNativePipWindow()) {
        window = std::make_unique<OhosPipWindow>(delegate, manager);
      } else if (ohos::adapter::ContextAdapter::GetInstance().IsPcMode()) {
        window = std::make_unique<OhosToplevelWindow>(delegate, manager);
      } else {
        window = std::make_unique<OhosToplevelPadWindow>(delegate, manager);
      }
      break;
    default:
      window = std::make_unique<OhosFakeWindow>(delegate, manager);
      break;
  }
  window->Initialize(std::move(properties));
  return window;
}

}  // namespace ui

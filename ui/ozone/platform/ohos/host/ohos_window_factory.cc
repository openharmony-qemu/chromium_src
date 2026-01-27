/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>

#include "base/logging.h"
#include "ohos/adapter/context/context_adapter.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/host/ohos_fake_window.h"
#include "ui/ozone/platform/ohos/host/ohos_pip_window.h"
#include "ui/ozone/platform/ohos/host/ohos_popup.h"
#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"
#include "ui/ozone/platform/ohos/host/ohos_toplevel_pad_window.h"
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

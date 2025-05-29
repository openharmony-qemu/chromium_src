// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/common/ohos_util.h"

#include "base/check.h"
#include "base/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using WindowAdapter = ohos::adapter::xcomponent::WindowAdapter;

namespace ui {
namespace util {

const std::string kWindowPrefix = "browser";

void* GetWindow(std::string& window_id) {
  auto window = WindowAdapter::GetInstance().GetWindow(window_id);
  DCHECK(window);
  if (!window) {
    LOG(ERROR) << "Get window is not exists, windowId:" << window_id;
    return nullptr;
  }
  return window;
}

void* GetWindowFromWidget(gfx::AcceleratedWidget widget_id) {
  DCHECK(widget_id);
  std::string window_id("browser");
  window_id += std::to_string(widget_id);
  return ui::util::GetWindow(window_id);
}

int32_t GetNextWindowWidgetId() {
  return WindowAdapter::GetInstance().NextWindowWidgetId();
}

std::string ConvertWidgetIdToWindowId(const gfx::AcceleratedWidget widget_id) {
  DCHECK(widget_id);
  return kWindowPrefix + std::to_string(widget_id);
}

gfx::AcceleratedWidget ConvertWindowIdToWidgetId(std::string& window_id) {
  if (!window_id.starts_with(kWindowPrefix) ||
      !std::all_of(window_id.begin() + kWindowPrefix.size(), window_id.end(),
                   [](char i) { return isdigit(i); })) {
    return gfx::kNullAcceleratedWidget;
  }
  std::string windowIdString = window_id.substr(kWindowPrefix.size());
  return std::stoi(windowIdString);
}

}  // namespace util
}  // namespace ui

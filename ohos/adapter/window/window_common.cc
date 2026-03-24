// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/window/window_common.h"

#include <sstream>

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::window {

namespace {
std::string GetRGB(uint32_t argb) {
  auto rgb = argb & 0xFFFFFF;
  std::stringstream tmp;
  tmp << "#" << std::hex << rgb;
  return tmp.str();
}
}

WindowInitParameter::WindowInitParameter() = default;

WindowInitParameter::WindowInitParameter(
  const WindowRect& bounds)
  : bounds(bounds) {}

WindowInitParameter::WindowInitParameter(
  WindowInitParameter&& param) = default;

WindowInitParameter& WindowInitParameter::operator=(
  WindowInitParameter&&) = default;

WindowInitParameter::~WindowInitParameter() = default;

void WindowInitParameter::SetBackground(uint32_t color_argb) {
  background_color = GetRGB(color_argb);
}

JSBIND_CLASS(WindowLimits) {
  JSBIND_PROPERTY(max_height);
  JSBIND_PROPERTY(max_width);
  JSBIND_PROPERTY(min_height);
  JSBIND_PROPERTY(min_width);
}

JSBIND_CLASS(WindowRect) {
  JSBIND_PROPERTY(left);
  JSBIND_PROPERTY(top);
  JSBIND_PROPERTY(width);
  JSBIND_PROPERTY(height);
}

JSBIND_CLASS(NewWindowParam) {
  JSBIND_PROPERTY(parent_id);
  JSBIND_PROPERTY(window_id);
  JSBIND_PROPERTY(bounds);
  JSBIND_PROPERTY(init_color_argb);
  JSBIND_PROPERTY(hide_title_bar);
  JSBIND_PROPERTY(use_dark_mode);
  JSBIND_PROPERTY(caption_button_visible);
  JSBIND_PROPERTY(ability_type);
  JSBIND_PROPERTY(app_id);
  JSBIND_PROPERTY(status);
  JSBIND_PROPERTY(window_limit);
}

JSBIND_CLASS(PointCoordinate) {
  JSBIND_PROPERTY(x);
  JSBIND_PROPERTY(y);
  JSBIND_PROPERTY(displayId);
}

}  // namespace ohos::adapter::window

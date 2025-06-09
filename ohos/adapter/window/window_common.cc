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
  JSBIND_PROPERTY(is_stateless);
  JSBIND_PROPERTY(caption_button_visible);
}

JSBIND_CLASS(PointCoordinate) {
  JSBIND_PROPERTY(x);
  JSBIND_PROPERTY(y);
  JSBIND_PROPERTY(displayId);
}

}  // namespace ohos::adapter::window

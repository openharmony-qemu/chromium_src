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

#ifndef OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_
#define OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_

#include <cstdint>
#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter::window {

struct ADAPTER_EXPORT_API WindowRect {
 public:
  int32_t left;
  int32_t top;
  int32_t width;
  int32_t height;
};

enum class AbilityType {
  kEntryAbility,
  kStatelessAbility,
  kTaskManagerAbility,
};

enum class AdapterType {
  kAppWindow,
  kSubWindow,
  kSystemFloatingWindow,
};

enum class WindowStatusType {
  UNDEFINED = 0,
  FULL_SCREEN,
  MAXIMIZE,
  MINIMIZE,
  FLOATING,
  SPLIT_SCREEN
};

struct WindowLimits {
 public:
  int32_t max_height;
  int32_t max_width;
  int32_t min_height;
  int32_t min_width;
};

struct NewWindowParam {
 public:
  std::string parent_id;
  std::string window_id;
  WindowRect bounds;
  std::string init_color_argb;
  bool hide_title_bar;
  bool use_dark_mode;
  bool caption_button_visible;
  AbilityType ability_type;
  std::string app_id;
  AdapterType adapter_type{AdapterType::kAppWindow};
  WindowStatusType status{WindowStatusType::FLOATING};
  WindowLimits window_limit;
};

struct PointCoordinate {
 public:
  float x;
  float y;
  int32_t displayId;
};

enum class WindowInitType {
  kWindow,
  kPopup,
  kMenu,
  kTooltip,
  kDrag,
  kBubble,
};

class ADAPTER_EXPORT_API WindowInitParameter {
public:
  WindowInitParameter();

  // Initializes parameter with the specified |bounds|.
  explicit WindowInitParameter(const WindowRect& bounds);

  WindowInitParameter(WindowInitParameter&& props);
  WindowInitParameter& operator=(WindowInitParameter&&);
  WindowInitParameter& operator=(const WindowInitParameter& other) = default;

  ~WindowInitParameter();

  void SetBackground(uint32_t color_argb);

public:
  // Tells desired Window type
  WindowInitType type = WindowInitType::kWindow;
  // Sets the desired initial bounds.
  WindowRect bounds;
  // Tells Window which id its parent holds.
  std::string parent_id = "";
  // Tells the back ground color of a window.
  std::string background_color;
  // Whether to hide the system title bar.
  bool hide_title_bar = true;
  // Specify whether the window is a floating window.
  bool use_floating_window = false;

  bool use_dark_mode = false;
  // The requested window id may be changed.
  std::string window_id;

  bool caption_button_visible = true;

  AbilityType ability_type = AbilityType::kEntryAbility;
  // Tells the browser load is webapp.
  std::string_view app_id = "";

  WindowStatusType status = WindowStatusType::FLOATING;

  WindowLimits window_limit;
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_

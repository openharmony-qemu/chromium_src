// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

struct WindowLimits {
  int max_width;
  int max_height;
  int min_width;
  int min_height;
};

class ADAPTER_EXPORT_API WindowInitParameter {
public:
  WindowInitParameter();

  // Initializes parameter with the specified |bounds|.
  explicit WindowInitParameter(const WindowRect& bounds);

  WindowInitParameter(WindowInitParameter&& props);
  WindowInitParameter& operator=(WindowInitParameter&&);

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
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_

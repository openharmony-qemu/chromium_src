/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_EXTENSION_ACTION_ICON_H
#define NWEB_EXTENSION_ACTION_ICON_H

#include <array>
#include <map>
#include <string>
#include <vector>

#include "ohos_nweb/src/capi/browser_service/nweb_basic_pub.h"

using ColorArray = std::array<int, COLOR_ARRAY_LENGTH>;

enum class NWebExtensionActionIconColorType {
  UNKNOWN = 0,
  RGBA_8888,
  BGRA_8888,
};

enum class NWebExtensionActionIconAlphaType {
  UNKNOWN = 0,
  OPAQUE,
  PREMUL,
  UNPREMUL,
};

struct NWebExtensionActionIconBitmap {
  static constexpr int NUM_PROPERTIES = 5;
  NWebExtensionActionIconColorType colorType;
  NWebExtensionActionIconAlphaType alphaType;
  int32_t width;
  int32_t height;
  uint8_t* bitmap;
};

struct NWebExtensionActionIcon {
  static constexpr int NUM_PROPERTIES = 1;
  std::map<double, NWebExtensionActionIconBitmap*> bitmaps;
};

const NWebExtensionActionIcon web_extension_action_icon_default = {};

struct NWebExtensionActionTabDetails {
  static constexpr int NUM_PROPERTIES = 1;
  std::optional<int32_t> tabId;
};

struct NWebExtensionActionUserSettings {
  static constexpr int NUM_PROPERTIES = 1;
  bool isOnToolbar;
};

const NWebExtensionActionUserSettings
    web_extension_action_user_setting_default = {};

struct NWebExtensionActionUserSettingsChange {
  static constexpr int NUM_PROPERTIES = 1;
  bool isOnToolbar;
};

struct NWebExtensionActionOpenPopupOptions {
  static constexpr int NUM_PROPERTIES = 1;
  std::optional<int32_t> windowId;
};

const NWebExtensionActionOpenPopupOptions
    web_extension_action_open_popup_options_default = {};

struct NWebExtensionActionSetTitleDetails {
  static constexpr int NUM_PROPERTIES = 2;
  std::string title;
  std::optional<int32_t> tabId;
};

const NWebExtensionActionSetTitleDetails
    web_extension_action_set_title_details_default = {};

struct NWebExtensionActionSetPopupDetails {
  static constexpr int NUM_PROPERTIES = 2;
  std::string popup;
  std::optional<int32_t> tabId;
};

const NWebExtensionActionSetPopupDetails
    web_extension_action_set_popup_details_default = {};

struct NWebExtensionActionSetBadgeTextDetails {
  static constexpr int NUM_PROPERTIES = 2;
  std::optional<std::string> text;
  std::optional<int32_t> tabId;
};

const NWebExtensionActionSetBadgeTextDetails
    web_extension_action_set_badge_text_details_default = {};

struct NWebExtensionActionSetBadgeBackgroundColorDetails {
  static constexpr int NUM_PROPERTIES = 2;
  ColorArray color;
  std::optional<int32_t> tabId;
};

const NWebExtensionActionSetBadgeBackgroundColorDetails
    web_extension_action_set_badge_background_color_details_default = {};

struct NWebExtensionActionSetBadgeTextColorDetails {
  static constexpr int NUM_PROPERTIES = 2;
  ColorArray color;
  std::optional<int32_t> tabId;
};

const NWebExtensionActionSetBadgeTextColorDetails
    web_extension_action_set_badge_text_color_details_default = {};

struct NWebExtensionActionDisableOptionsV2 {
  static constexpr int NUM_PROPERTIES = 3;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionEnableOptionsV2 {
  static constexpr int NUM_PROPERTIES = 3;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionOpenPopupOptionsV2 {
  static constexpr int NUM_PROPERTIES = 3;
  std::optional<int32_t> windowId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionSetTitleDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  std::string title;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionSetPopupDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  std::string popup;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionIconBitmapV2 {
  static constexpr int NUM_PROPERTIES = 5;
  NWebExtensionActionIconColorType colorType;
  NWebExtensionActionIconAlphaType alphaType;
  int32_t width;
  int32_t height;
  std::vector<uint8_t> bitmap;
};

struct NWebExtensionActionIconV2 {
  static constexpr int NUM_PROPERTIES = 1;
  std::map<double, NWebExtensionActionIconBitmapV2> bitmaps;
};

struct NWebExtensionActionSetIconDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  NWebExtensionActionIconV2 icon;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionSetBadgeTextDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  std::optional<std::string> text;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionSetBadgeTextColorDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  ColorArray color;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionActionSetBadgeBackgroundColorDetailsV2 {
  static constexpr int NUM_PROPERTIES = 4;
  ColorArray color;
  std::optional<int32_t> tabId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

#endif  // NWEB_EXTENSION_ACTION_ICON_H

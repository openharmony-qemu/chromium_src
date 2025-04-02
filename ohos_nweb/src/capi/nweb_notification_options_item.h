/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_NOTIFICATION_OPTIONS_ITEM_H
#define NWEB_NOTIFICATION_OPTIONS_ITEM_H

#include <string>
#include <optional>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ohos_nweb/src/nweb_hilog.h"

enum class NWebNotificationOptionsItemIconColorType {
  UNKNOWN = 0,
  RGBA_8888,
  BGRA_8888,
};

enum class NWebNotificationOptionsItemIconAlphaType {
  UNKNOWN = 0,
  OPAQUE,
  PREMUL,
  UNPREMUL,
};

struct NWebNotificationOptionsItemIconBitmap {
  static constexpr int NUM_PROPERTIES = 5;
  NWebNotificationOptionsItemIconColorType colorType;
  NWebNotificationOptionsItemIconAlphaType alphaType;
  int32_t width;
  int32_t height;
  uint8_t* bitmap;
};

struct NWebNotificationOptionsItemIcon {
  static constexpr int NUM_PROPERTIES = 1;
  std::map<double, NWebNotificationOptionsItemIconBitmap*> bitmaps;
};

struct NWebNotificationOptionsItem {
  std::string id;
  std::string originUrl;
  std::string title;
  std::string body;
  bool requireInteraction;
  std::vector<std::string> buttons;
  NWebNotificationOptionsItemIcon* icon;

  NWebNotificationOptionsItem()
      : id(""),
        originUrl(""),
        title(""),
        body(""),
        requireInteraction(false),
        buttons(),
        icon(nullptr) {
    WVLOG_I("NWebNotificationOptionsItem() is called");
  }

  ~NWebNotificationOptionsItem() {
    WVLOG_I("~NWebNotificationOptionsItem() is called");
  }
};

struct NWebNotificationDelegateCallback {
  void (*showNotification)(std::shared_ptr<NWebNotificationOptionsItem> optionsItem);
  void (*closeNotification)(std::string id);
};

#endif  // NWEB_NOTIFICATION_OPTIONS_ITEM_H

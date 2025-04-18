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

#include <map>
#include "base/memory/raw_ptr.h"

namespace OHOS::NWeb {

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
  raw_ptr<uint8_t> bitmap;
};

struct NWebExtensionActionIcon {
  static constexpr int NUM_PROPERTIES = 1;
  std::map<double, NWebExtensionActionIconBitmap*> bitmaps;
};

}  // namespace OHOS::NWeb

#endif // NWEB_EXTENSION_ACTION_ICON_H

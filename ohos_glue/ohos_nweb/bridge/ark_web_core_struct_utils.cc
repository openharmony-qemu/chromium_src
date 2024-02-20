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

#include "ohos_nweb/bridge/ark_web_core_struct_utils.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTime
ArkWebDateTimeClassToStruct(const OHOS::NWeb::DateTime &class_value) {
  ArkWebDateTime struct_value = {.year = class_value.year,
                                 .month = class_value.month,
                                 .day = class_value.day,
                                 .hour = class_value.hour,
                                 .minute = class_value.minute,
                                 .second = class_value.second};
  return struct_value;
}

OHOS::NWeb::DateTime
ArkWebDateTimeStructToClass(const ArkWebDateTime &struct_value) {
  OHOS::NWeb::DateTime class_value = {.year = struct_value.year,
                                      .month = struct_value.month,
                                      .day = struct_value.day,
                                      .hour = struct_value.hour,
                                      .minute = struct_value.minute,
                                      .second = struct_value.second};
  return class_value;
}

OHOS::NWeb::DragEvent
ArkWebDragEventStructToClass(const ArkWebDragEvent &struct_value) {
  OHOS::NWeb::DragEvent class_value = {.x = struct_value.x,
                                       .y = struct_value.y};
  class_value.action = static_cast<OHOS::NWeb::DragAction>(struct_value.action);
  return class_value;
}

ArkWebCursorInfo
ArkWebCursorInfoClassToStruct(const OHOS::NWeb::NWebCursorInfo &class_value) {
  ArkWebCursorInfo struct_value = {.width = class_value.width,
                                   .height = class_value.height,
                                   .x = class_value.x,
                                   .y = class_value.y,
                                   .scale = class_value.scale};
  return struct_value;
}

ArkWebImageOptions
ArkWebImageOptionsClassToStruct(const OHOS::NWeb::ImageOptions &class_value) {
  ArkWebImageOptions struct_value = {.width = class_value.width,
                                     .height = class_value.height};
  struct_value.alphaType = static_cast<int>(class_value.alphaType);
  struct_value.colorType = static_cast<int>(class_value.colorType);
  return struct_value;
}

ArkWebDateTimeChooser ArkWebDateTimeChooserClassToStruct(
    const OHOS::NWeb::DateTimeChooser &class_value) {
  ArkWebDateTimeChooser struct_value = {.step = class_value.step,
                                        .suggestionIndex =
                                            class_value.suggestionIndex,
                                        .hasSelected = class_value.hasSelected};
  struct_value.type = static_cast<int>(class_value.type);
  struct_value.minimum = ArkWebDateTimeClassToStruct(class_value.minimum);
  struct_value.maximum = ArkWebDateTimeClassToStruct(class_value.maximum);
  struct_value.dialogValue =
      ArkWebDateTimeClassToStruct(class_value.dialogValue);
  return struct_value;
}

ArkWebSelectMenuBound ArkWebSelectMenuBoundClassToStruct(
    const OHOS::NWeb::SelectMenuBound &class_value) {
  ArkWebSelectMenuBound struct_value = {.x = class_value.x,
                                        .y = class_value.y,
                                        .width = class_value.width,
                                        .height = class_value.height};
  return struct_value;
}

OHOS::NWeb::TouchHandleHotZone ArkWebTouchHandleHotZoneStructToClass(
    const ArkWebTouchHandleHotZone &struct_value) {
  OHOS::NWeb::TouchHandleHotZone class_value = {.width = struct_value.width,
                                                .height = struct_value.height};
  return class_value;
}

OHOS::NWeb::NWebScreenCaptureConfig ArkWebScreenCaptureConfigStructToClass(
    const ArkWebScreenCaptureConfig &struct_value) {
  OHOS::NWeb::NWebScreenCaptureConfig class_value = {
      .mode = struct_value.mode, .sourceId = struct_value.sourceId};
  return class_value;
}

} // namespace OHOS::ArkWeb

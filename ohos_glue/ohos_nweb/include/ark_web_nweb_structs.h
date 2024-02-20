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

#ifndef ARK_WEB_NWEB_STRUCTS_H
#define ARK_WEB_NWEB_STRUCTS_H

typedef struct {
  int32_t year;
  int32_t month;
  int32_t day;
  int32_t hour;
  int32_t minute;
  int32_t second;
} ArkWebDateTime;

const ArkWebDateTime ark_web_date_time_default = {
    0,
};

typedef struct DragEvent {
  double x;
  double y;
  int action;
} ArkWebDragEvent;

const ArkWebDragEvent ark_web_drag_event_default = {
    0,
};

typedef struct {
  int32_t width;
  int32_t height;
  int32_t x;
  int32_t y;
  float scale;
} ArkWebCursorInfo;

const ArkWebCursorInfo ark_web_cursor_info_default = {
    0,
};

typedef struct {
  int colorType;
  int alphaType;
  size_t width;
  size_t height;
} ArkWebImageOptions;

const ArkWebImageOptions ark_web_image_option_default = {
    0,
};

typedef struct {
  int type;
  ArkWebDateTime dialogValue;
  ArkWebDateTime minimum;
  ArkWebDateTime maximum;
  double step;
  size_t suggestionIndex;
  bool hasSelected;
} ArkWebDateTimeChooser;

const ArkWebDateTimeChooser ark_web_date_time_chooser_default = {
    0,
};

typedef struct {
  int x;
  int y;
  int width;
  int height;
} ArkWebSelectMenuBound;

const ArkWebSelectMenuBound ark_web_select_menu_bound_default = {
    0,
};

typedef struct {
  double width;
  double height;
} ArkWebTouchHandleHotZone;

const ArkWebTouchHandleHotZone ark_web_touch_handle_zone_default = {
    0,
};

typedef struct {
  int32_t mode;
  int32_t sourceId;
} ArkWebScreenCaptureConfig;

const ArkWebScreenCaptureConfig ark_web_screen_capture_config_default = {
    0,
};

typedef int64_t (*AccessibilityIdGenerateFunc)();

typedef void (*NativeArkWebOnValidCallback)(const char *);

typedef void (*NativeArkWebOnDestroyCallback)(const char *);

typedef char *(*NativeArkWebOnJavaScriptProxyCallback)(const char **, int32_t);

#endif // ARK_WEB_NWEB_STRUCTS_H

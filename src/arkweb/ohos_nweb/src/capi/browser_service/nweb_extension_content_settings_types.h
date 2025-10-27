/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_CONTENT_SETTINGS_TYPES_H
#define OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_CONTENT_SETTINGS_TYPES_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  char *extensionId;
  bool *incognito;
  char *primaryUrl;
  char *secondaryUrl;
  char *type;
} NWebExtensionContentSettingsGetParam;

typedef struct {
  char *extensionId;
  char *primaryPattern;
  char *scope;
  char *secondaryPattern;
  char *contentSetting;
  char *type;
} NWebExtensionContentSettingsSetParam;

typedef struct {
  char *extensionId;
  char *scope;
  char *type;
} NWebExtensionContentSettingsClearParam;

typedef struct {
  char *contentSetting;
} NWebExtensionContentSettingsDetail;

typedef struct {
  char* error;
  int requestId;
  NWebExtensionContentSettingsDetail *detailParam;
} NWebExtensionContentSettingsCallbackResult;

typedef struct {
  char* error;
  int requestId;
} NWebExtensionContentSettingsCallbackSetResult;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_CONTENT_SETTINGS_TYPES_H

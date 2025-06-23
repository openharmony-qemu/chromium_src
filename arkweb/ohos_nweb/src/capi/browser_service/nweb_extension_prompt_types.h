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

#ifndef OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROMPT_TYPES_H
#define OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROMPT_TYPES_H

#include <stdint.h>

#include "ohos_nweb/src/capi/browser_service/nweb_basic_pub.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum PromptType {
  PROMPT_PERMISSION = 0, PROMPT_INSTALLATION, PROMPT_UNINSTALLATION
};

typedef struct {
  char* detail;
  char* permission;
  char* hideDetailLabel;
  char* showDetailLabel;
} NWebExtensionPermission;

typedef struct {
  OnArkWebFreeMemoryFunc free_memory_func;
  char* title;
  char* abortButtonLabel;
  char* acceptButtonLabel;
  char* permissionsHeading;

  uint32_t permissionCount;
  NWebExtensionPermission* permissions;
} NWebExtensionPromptData;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROMPT_TYPES_H

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
 
#ifndef OHOS_NWEB_SRC_NWEB_EXTENSION_CONTEXT_MENUS_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_EXTENSION_CONTEXT_MENUS_CALLBACK_H_
 
#include <map>
#include <stddef.h>
#include <string>
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"
 
struct NWebExtensionContextMenusCallback {
  void (*OnCreated)(const char* extensionId, const NWebContextMenusItem& menuItem);
  void (*OnUpdated)(const char* extensionId, const char* id, const NWebContextMenusItem& menuItem);
  void (*OnRemoved)(const char* extensionId, const char* menuItemId);
  void (*OnRemovedAll)(const char* extensionId);
};
 
#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_CONTEXT_MENUS_CALLBACK_H_
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

#ifndef OHOS_NWEB_SRC_NWEB_STORAGE_EXTENSION_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_STORAGE_EXTENSION_CALLBACK_H_

#include <cstddef>
#include <string>

struct NWebStorageExtensionCallback {
  size_t struct_size = sizeof(NWebStorageExtensionCallback);
  void (*OnGetPassword)(const char* result, int callback_id);
  void (*OnGetSavedPasswords)(std::vector<std::string> url,
                              std::vector<std::string> username,
                              int callback_id);
};

#endif  // OHOS_NWEB_SRC_NWEB_STORAGE_EXTENSION_CALLBACK_H_

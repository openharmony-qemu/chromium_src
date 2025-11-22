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

#include "arkweb/ohos_adapter_ndk/pasteboard_adapter/include/pasteboard_client_adapter_utils.h"

#include <filemanagement/file_uri/oh_file_uri.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {

int PasteboardClientAdapterUtils::GetUriFromPath(const char* path,
                                                unsigned int length,
                                                char** result) {
  return OH_FileUri_GetUriFromPath(path, length, result);
}
}  // namespace OHOS::NWeb
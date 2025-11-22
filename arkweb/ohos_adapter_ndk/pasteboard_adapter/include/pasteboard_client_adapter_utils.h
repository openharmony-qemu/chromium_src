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

#ifndef PASTEBOARD_CLIENT_ADAPTER_UTILS_H
#define PASTEBOARD_CLIENT_ADAPTER_UTILS_H

namespace OHOS::NWeb {
class PasteboardClientAdapterUtils {
 public:
  static int GetUriFromPath(const char* path,
                            unsigned int length,
                            char** result);
};
}  // namespace OHOS::NWeb

#endif  // PASTEBOARD_CLIENT_ADAPTER_UTILS_H
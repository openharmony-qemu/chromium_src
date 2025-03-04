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

#ifndef OHOS_NWEB_SRC_NWEB_EXTENSION_API_STRUCT_INFO_H_
#define OHOS_NWEB_SRC_NWEB_EXTENSION_API_STRUCT_INFO_H_

struct NWebTabCreateInfo {
  std::optional<bool> active;
  std::optional<int32_t> index;
  std::optional<int32_t> openerTabId;
  std::optional<bool> pinned;
  std::optional<std::string> url;
  std::optional<int32_t> windowId;
};

#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_API_STRUCT_INFO_H_
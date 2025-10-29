/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_
#define OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_

#include <vector>
 
struct ContextMenusOnClickedData {
  bool checked;
  bool editable;
  int frameId;
  std::string frameUrl;
  std::string linkUrl;
  std::string mediaType;
  std::string menuItemId;
  std::string pageUrl;
  std::string parentMenuItemId;
  std::string selectionText;
  std::string srcUrl;
  bool wasChecked;
};

struct ContextMenusOnClickedDataV2 {
  bool editable;
  int menuItemId = 0;
  std::string menuItemIdStr;
  std::optional<int> frameId;
  std::optional<int> parentMenuItemId;
  std::optional<bool> checked;
  std::optional<bool> wasChecked;
  std::optional<std::string> srcUrl;
  std::optional<std::string> linkUrl;
  std::optional<std::string> pageUrl;
  std::optional<std::string> frameUrl;
  std::optional<std::string> mediaType;
  std::optional<std::string> selectionText;
  std::optional<std::string> parentMenuItemIdStr;
};
#endif  //  OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_
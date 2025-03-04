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
 
#include <vector>
 
#ifndef OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_
#define OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_
 
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
 
#endif  //  OHOS_NWEB_SRC_NWEB_CONTEXT_MENUS_ON_CLICKED_DATA_H_
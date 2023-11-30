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

#ifndef NWEB_HISTORY_LIST_IMPL_H
#define NWEB_HISTORY_LIST_IMPL_H

#include <vector>
#include "cef/include/cef_navigation_entry.h"
#include "nweb_history_list.h"

namespace OHOS::NWeb {
class NWebHistoryItemImpl : public NWebHistoryItem {
 public:
  NWebHistoryItemImpl() = delete;
  explicit NWebHistoryItemImpl(CefRefPtr<CefNavigationEntry> entry);
  ~NWebHistoryItemImpl() = default;
  std::string GetHistoryRawUrl() override;
  std::string GetHistoryTitle() override;
  std::string GetHistoryUrl() override;
  bool GetFavicon(void** data,
                  int& width,
                  int& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;

 private:
  CefRefPtr<CefNavigationEntry> entry_;
};

class NWebHistoryListImpl : public NWebHistoryList {
 public:
  NWebHistoryListImpl() = default;
  ~NWebHistoryListImpl() = default;
  int32_t GetCurrentIndex() override;
  std::shared_ptr<NWebHistoryItem> GetItem(int32_t index) override;
  int32_t GetListSize() override;
  void AddHistoryItem(CefRefPtr<CefNavigationEntry> entry);
  void SetCurrentIndex(int32_t index);

 private:
  int32_t current_index_ = -1;
  std::vector<std::shared_ptr<NWebHistoryItemImpl>> item_list_;
};
}  // namespace OHOS::NWeb
#endif

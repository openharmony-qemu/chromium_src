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
 
#ifndef NWEB_EXTENSION_HISTORY_CEF_DELEGATE_H
#define NWEB_EXTENSION_HISTORY_CEF_DELEGATE_H
 
#include <string>
#include <memory>
 
#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_history_types.h"
 
namespace OHOS::NWeb {

struct NWebExtensionVisitItem {
  bool isLocal;
  int transition;

  std::string id;
  std::string visitId;
  std::string referringVisitId;

  std::optional<int64_t> visitTime;
};

using HistoryGetVisitsCallback = base::RepeatingCallback<void(
    const std::string& error,
    const std::vector<NWebExtensionVisitItem>& results)>;
using HistorySearchCallback = base::RepeatingCallback<void(const NWebExtensionHistoryItems*)>;
using HistoryAddUrlCallback = base::RepeatingCallback<void(const char* error)>;
using HistoryDeleteUrlCallback = base::RepeatingCallback<void(const char* error)>;
using HistoryDeleteAllCallback = base::RepeatingCallback<void(const char* error)>;
using HistoryDeleteRangeCallback =
    base::RepeatingCallback<void(const std::string& error)>;

class NWebExtensionHistoryCefDelegate {
  public:
    static NWebExtensionHistoryCefDelegate* GetInstance();

    bool GetVisits(const std::string& url, HistoryGetVisitsCallback callback);

    bool Search(const NWebExtensionHistoryQueryInfo* data, HistorySearchCallback callback);

    bool AddUrl(const char* url, HistoryAddUrlCallback callback);

    bool DeleteAll(HistoryDeleteAllCallback callback);

    bool DeleteUrl(const char* url, HistoryDeleteUrlCallback callback);

    bool DeleteRange(int64_t start_time,
                     int64_t end_time,
                     HistoryDeleteRangeCallback callback);

    void GetVisitsCallback(int requestId,
                           const std::string& error,
                           const std::vector<NWebExtensionVisitItem>& results);

    void SearchCallback(
      const NWebExtensionHistoryItems* items);
    
    void AddUrlCallback(int requestId, const char* error);

    void DeleteUrlCallback(int requestId, const char* error);

    void DeleteAllCallback(int requestId, const char* error);

    void DeleteRangeCallback(int requestId, const std::string& error);

    void OnVisited(const NWebExtensionHistoryItem* item);

    void OnVisitRemoved(const NWebExtensionHistoryVisiteRemovedItem* item);
 
  private:
    NWebExtensionHistoryCefDelegate() = default;
    ~NWebExtensionHistoryCefDelegate() = default;
    NWebExtensionHistoryCefDelegate(const NWebExtensionHistoryCefDelegate&) =
        delete;
    NWebExtensionHistoryCefDelegate& operator=(
        const NWebExtensionHistoryCefDelegate&) = delete;
};
} // namespace OHOS::NWeb
#endif  // NWEB_EXTENSION_HISTORY_CEF_DELEGATE_H
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_READING_LIST_CEF_DELEGATE_H_
#define NWEB_EXTENSION_READING_LIST_CEF_DELEGATE_H_

#include "base/functional/callback.h"

namespace OHOS::NWeb {

struct NWebReadingListEntry {
  bool hasBeenRead;
  double creationTime;
  double lastUpdateTime;

  std::string url;
  std::string title;
};

struct NWebAddEntryOptions {
  bool hasBeenRead;
  std::string url;
  std::string title;
};

struct NWebQueryEntryOptions {
  std::optional<bool> hasBeenRead;
  std::optional<std::string> url;
  std::optional<std::string> title;
};

struct NWebUpdateEntryOptions {
  std::string url;
  std::optional<bool> hasBeenRead;
  std::optional<std::string> title;
};

using AddEntryResultFunc = base::RepeatingCallback<void(const std::string&)>;
using QueryEntryResultFunc =
    base::RepeatingCallback<void(const std::string&,
                                 const std::vector<NWebReadingListEntry>&)>;
using RemoveEntryResultFunc = base::RepeatingCallback<void(const std::string&)>;
using UpdateEntryResultFunc = base::RepeatingCallback<void(const std::string&)>;

class NWebExtensionReadingListCefDelegate {
 public:
  ~NWebExtensionReadingListCefDelegate() = default;

  static NWebExtensionReadingListCefDelegate& GetInstance();

  bool AddEntry(const NWebAddEntryOptions& options,
                AddEntryResultFunc callback);

  bool QueryEntry(const NWebQueryEntryOptions& options,
                  QueryEntryResultFunc callback);

  bool RemoveEntry(const std::string& url, RemoveEntryResultFunc callback);

  bool UpdateEntry(const NWebUpdateEntryOptions& options,
                   UpdateEntryResultFunc callback);

  void OnEntryAdded(const NWebReadingListEntry& entry);

  void OnEntryRemoved(const NWebReadingListEntry& entry);

  void OnEntryUpdated(const NWebReadingListEntry& entry);

  void AddEntryResult(int id, const std::string& error);

  void QueryEntryResult(int id,
                        const std::string& error,
                        const std::vector<NWebReadingListEntry>& entries);

  void RemoveEntryResult(int id, const std::string& error);

  void UpdateEntryResult(int id, const std::string& error);

 private:
  NWebExtensionReadingListCefDelegate() = default;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_READING_LIST_CEF_DELEGATE_H_

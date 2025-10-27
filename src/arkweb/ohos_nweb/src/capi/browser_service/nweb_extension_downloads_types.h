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

#ifndef OHOS_NWEB_EX_BROWSER_NWEB_EXTENSION_DOWNLOADS_ITEM_H_
#define OHOS_NWEB_EX_BROWSER_NWEB_EXTENSION_DOWNLOADS_ITEM_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum DownloadsFunctionName {
  DOWNLOADS_FUNCTION_DOWNLOAD = 0,
  DOWNLOADS_FUNCTION_SEARCH = 1,
  DOWNLOADS_FUNCTION_PAUSE = 2,
  DOWNLOADS_FUNCTION_RESUME = 3,
  DOWNLOADS_FUNCTION_CANCEL = 4,
  DOWNLOADS_FUNCTION_ERASE = 5,
  // 6 unused
  DOWNLOADS_FUNCTION_ACCEPT_DANGER = 7,
  DOWNLOADS_FUNCTION_SHOW = 8,
  DOWNLOADS_FUNCTION_DRAG = 9,
  DOWNLOADS_FUNCTION_GET_FILE_ICON = 10,
  DOWNLOADS_FUNCTION_OPEN = 11,
  DOWNLOADS_FUNCTION_REMOVE_FILE = 12,
  DOWNLOADS_FUNCTION_SHOW_DEFAULT_FOLDER = 13,
  DOWNLOADS_FUNCTION_SET_SHELF_ENABLED = 14,
  DOWNLOADS_FUNCTION_DETERMINE_FILENAME = 15,
  DOWNLOADS_FUNCTION_SET_UI_OPTIONS = 16,
  // Insert new values here, not at the beginning.
  DOWNLOADS_FUNCTION_LAST
};

enum class ExDownloadsState {
  kStateNone = 0,
  kInProgress = 1,
  kInterrupted = 2,
  kComplete = 3,
};

struct ExDownloadsQueryInfo {
  static const int NUM_PROPERTIES = 29;
  std::optional<double> bytesReceived;
  int32_t danger;
  std::optional<std::string> endTime;
  std::optional<std::string> endedAfter;
  std::optional<std::string> endedBefore;
  int32_t error;
  std::optional<bool> exists;
  std::optional<double> fileSize;
  std::optional<std::string> filename;
  std::optional<std::string> filenameRegex;
  std::optional<std::string> finalUrl;
  std::optional<std::string> finalUrlRegex;
  std::optional<int32_t> id;
  std::optional<int32_t> limit;
  std::optional<std::string> mime;
  std::optional<std::vector<std::string>> orderBy;
  std::optional<bool> paused;
  std::optional<std::vector<std::string>> query;
  std::optional<std::string> startTime;
  std::optional<std::string> startedAfter;
  std::optional<std::string> startedBefore;
  int32_t state;
  std::optional<double> totalBytes;
  std::optional<double> totalBytesGreater;
  std::optional<double> totalBytesLess;
  std::optional<std::string> url;
  std::optional<std::string> urlRegex;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct ExDownloadsItem {
  std::optional<std::string> byExtensionId;
  std::optional<std::string> byExtensionName;
  double bytesReceived;
  bool canResume;
  int32_t danger;
  std::optional<std::string> endTime;
  int32_t error;
  std::optional<std::string> estimatedEndTime;
  bool exists;
  double fileSize;
  std::string filename;
  std::string finalUrl;
  int32_t id;
  bool incognito;
  std::string mime;
  bool paused;
  std::string referrer;
  std::string startTime;
  int32_t state;
  double totalBytes;
  std::string url;
};

using ExDownloadsItemVector = std::vector<ExDownloadsItem>;

struct ExDownloadsGetDownloadIdCallbackParams {
  int32_t downloadId;
  std::optional<std::string> error;
};

struct ExDownloadsGetFileIconOptions{
  int downloadId;
  std::optional<int> size;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct ExDownloadsGetFileIconCallbackParams {
  std::string iconUrl;
  std::optional<std::string> error;
};

struct ExDownloadsSearchCallbackParams {
  ExDownloadsItemVector items;
  std::optional<std::string> error;
};

enum class ExDownloadFilenameConflictAction {
  kNone = 0,
  uniquify = 1,
  overwrite = 2,
  prompt = 3,
};

typedef struct {
  std::string filename;
  std::optional<ExDownloadFilenameConflictAction> conflictAction;
} FilenameSuggestion;

struct ExDownloadsEraseCallbackParams {
  std::vector<int32_t> erasedIds;
  std::optional<std::string> error;
};

struct ExDownloadsUiOptions {
  std::string extensionId;
  bool enabled;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif
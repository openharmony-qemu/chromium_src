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

#ifndef OHOS_NWEB_EX_BROWSER_NWEB_EXTENSION_DOWNLOADER_ITEM_H_
#define OHOS_NWEB_EX_BROWSER_NWEB_EXTENSION_DOWNLOADER_ITEM_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class ReferrerPolicy {
  CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE = 0,
  REDUCE_GRANULARITY_ON_TRANSITION_CROSS_ORIGIN = 1,
  ORIGIN_ONLY_ON_TRANSITION_CROSS_ORIGIN = 2,
  NEVER_CLEAR = 3,
  ORIGIN = 4,
  CLEAR_ON_TRANSITION_CROSS_ORIGIN = 5,
  ORIGIN_CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE = 6,
  NO_REFERRER = 7,
  MAX = NO_REFERRER,
};

enum class RedirectMode {
  kFollow = 0,  
  kError = 1,  
  kManual = 2,
  kMinValue = 0,
  kMaxValue = 2,
};

enum class NWebDownloadSource {
  UNKNOWN = 0,
  NAVIGATION = 1,
  DRAG_AND_DROP = 2, 
  FROM_RENDERER = 3,
  EXTENSION_API = 4,
  EXTENSION_INSTALLER = 5,
  CONTEXT_MENU = 9,
  RETRY = 10,
};

enum class NWebTargetDisposition {
  OVERWRITE,
  PROMPT
};

enum class NWebFilenameConflictAction{
  CONFLICT_ACTION_NONE = 0,
  CONFLICT_ACTION_UNIQUIFY,
  CONFLICT_ACTION_OVERWRITE,
  CONFLICT_ACTION_PROMPT,
};

struct DownloadUrlParameters {
  std::string method;
  std::string postBody;
  std::string headers;
  std::string referrer;
  ReferrerPolicy referrerPolicy;
  std::string referrerEncoding;
  std::string initiator;
  bool preferCache;
  std::string filePath;
  std::string suggestedName;
  int32_t offset;
  RedirectMode crossOriginRedirects;
  bool transient;
  std::string guid;
  bool hasUserGesture;
};

#endif  // OHOS_NWEB_EX_BROWSER_NWEB_EXTENSION_DOWNLOADER_ITEM_H_
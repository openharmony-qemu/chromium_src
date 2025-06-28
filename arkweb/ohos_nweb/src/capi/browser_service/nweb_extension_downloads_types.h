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

typedef struct {
  uint32_t count;
  char** strs;
} NWebDownloadsVectorCharType;

typedef struct {
  double* bytesReceived;
  int* danger;
  char* endTime;
  char* endedAfter;
  char* endedBefore;
  int* error;
  bool* exists;
  double* fileSize;
  char* filename;
  char* filenameRegex;
  char* finalUrl;
  char* finalUrlRegex;
  int* id;
  int* limit;
  char* mime;
  NWebDownloadsVectorCharType* orderBy;
  bool* paused;
  NWebDownloadsVectorCharType* query;
  char* startTime;
  char* startedAfter;
  char* startedBefore;
  int* state;
  double* totalBytes;
  double* totalBytesGreater;
  double* totalBytesLess;
  char* url;
  char* urlRegex;
} NWebDownloadsQueryInfo;

typedef struct {
  bool* enabled;
} NWebExtensionUiOptions;

#endif
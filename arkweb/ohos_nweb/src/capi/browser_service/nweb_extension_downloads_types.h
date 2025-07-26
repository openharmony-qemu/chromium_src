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

typedef struct {
  uint32_t count;
  char** strs;
} ExDownloadsVectorCharType;

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
  ExDownloadsVectorCharType* orderBy;
  bool* paused;
  ExDownloadsVectorCharType* query;
  char* startTime;
  char* startedAfter;
  char* startedBefore;
  int* state;
  double* totalBytes;
  double* totalBytesGreater;
  double* totalBytesLess;
  char* url;
  char* urlRegex;
} ExDownloadsQueryInfo;

typedef struct {
  char* extensionsId;
  bool* enabled;
} ExDownloadsUiOptions;

typedef struct {
  char* byExtensionId;
  char* byExtensionName;
  double* bytesReceived;
  bool* canResume;
  int* danger;
  char* endTime;
  int* error;
  char* estimatedEndTime;
  bool* exists;
  double* fileSize;
  char* filename;
  char* finalUrl;
  int* id;
  bool* incognito;
  char* mime;
  bool* paused;
  char* referrer;
  char* startTime;
  int* state;
  double* totalBytes;
  char* url;
} ExDownloadsItem;

typedef struct {
  char* name;
  char* value;
} ExDownloadsHeaderNameValuePair;

typedef struct {
  char* body;
  int* conflictAction;
  char* filename;
  int* headerCount;
  ExDownloadsHeaderNameValuePair* headers;
  int* method;
  bool* saveAs;
  char* url;
} ExDownloadsOptions;

typedef struct {
  int* downloadId;
  int* size;
} ExDownloadsGetFileIcon;

typedef struct {
  int* colorType;
  int* alphaType;
  int* width;
  int* height;
  size_t bitmapSize;
  uint8_t* bitmap;
} ExDownloadsIconBitmap;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif
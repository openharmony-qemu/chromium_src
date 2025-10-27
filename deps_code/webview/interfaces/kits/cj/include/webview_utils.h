/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef WEBVIEW_UTILS_H
#define WEBVIEW_UTILS_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "cj_common_ffi.h"

namespace OHOS {
namespace Webview {
struct RetWebHeader {
    char* headerKey;
    char* headerValue;
};

struct ArrWebHeader {
    RetWebHeader* head;
    int64_t size;
};

struct CHistoryItem {
    int64_t icon;
    char* historyUrl;
    char* historyRawUrl;
    char* title;
};

struct LoadDatas {
    const char* cData;
    const char* cMimeType;
    const char* cEncoding;
    const char* cBaseUrl;
    const char* cHistoryUrl;
};

struct RetWebMessage {
    char* messageStr;
    CArrUI8 messageArr;
};

struct CError {
    char* errorName;
    char* errorMsg;
};

struct RetNumber {
    int64_t numberInt;
    double numberDouble;
};

struct CArrDouble {
    double* head;
    int64_t size;
};

struct CArrBool {
    bool* head;
    int64_t size;
};

struct CArrValue {
    char** strHead;
    int64_t* intHead;
    double* doubleHead;
    bool* boolHead;
    int64_t size;
};

struct CWebStorageOrigin {
    char* origin;
    int64_t quota;
    int64_t usage;
};

struct CArrWebStorageOrigin {
    CWebStorageOrigin* cWebStorageOrigin;
    int64_t size;
};

struct CScheme {
    char* name;
    bool isSupportCORS;
    bool isSupportFetch;
    bool isStandard;
    bool isLocal;
    bool isDisplayIsolated;
    bool isSecure;
    bool isCspBypassing;
    bool isCodeCacheSupported;
};

struct Scheme {
    std::string name;
    bool isSupportCORS;
    bool isSupportFetch;
    bool isStandard;
    bool isLocal;
    bool isDisplayIsolated;
    bool isSecure;
    bool isCspBypassing;
    bool isCodeCacheSupported;
    int32_t option = 0;
};

struct CArrScheme {
    CScheme* head;
    int64_t size;
};

struct CacheOptions {
    ArrWebHeader arrHead;
};

struct COfflineResourceMap {
    CArrString urlList;
    uint8_t* resource;
    int64_t resourceCSize;
    ArrWebHeader responseHeaders;
    int32_t type;
};

struct CArrOfflineResourceMap {
    COfflineResourceMap* head;
    int64_t size;
};

struct CRequestInfo {
    char* url;
    char* method;
    char* formData;
};

struct CMediaSourceInfo {
    const char* format;
    const char* source;
    int32_t type;
};

struct CArrMediaSourceInfo {
    CMediaSourceInfo* head;
    int64_t size;
};

struct MapItem {
    char* key;
    char* value;
};

struct ArrMapItem {
    MapItem* head;
    int64_t size;
};

struct CRectEvent {
    double x;
    double y;
    double width;
    double height;
};

struct CNativeMediaPlayerSurfaceInfo {
    CRectEvent rect;
    const char* id;
};

struct CMediaInfo {
    const char* embedID;
    int32_t mediaType;
    CArrMediaSourceInfo mediaSrcList;
    CNativeMediaPlayerSurfaceInfo surfaceInfo;
    bool controlsShown;
    CArrString controlList;
    bool muted;
    const char* posterUrl;
    int32_t preload;
    ArrMapItem headers;
    ArrMapItem attributes;
};

struct CSnapshotInfo {
    char* id;
    int32_t width;
    int32_t height;
    int32_t widthType;
    int32_t heightType;
};

struct CSnapshotResult {
    char* id;
    int64_t imageId;
    bool status;
    int32_t width;
    int32_t height;
    int32_t widthType;
    int32_t heightType;
};

struct RetDataCSnapshotResult {
    int32_t code;
    CSnapshotResult data;
};

struct CScrollOffset {
    int32_t x;
    int32_t y;
};

struct CCertByteData {
    CArrUI8* head;
    int64_t size;
};

char* MallocCString(const std::string& origin);
uint8_t* MallocUInt8(const std::string& origin);
char** VectorToCArrString(const std::vector<std::string>& vec);
uint8_t* VectorToCArrUI8(const std::vector<uint8_t> vec);
std::vector<std::string> CArrStringToVector(CArrString cArrStr);
} // namespace Webview
} // namespace OHOS

#endif // WEBVIEW_UTILS_H
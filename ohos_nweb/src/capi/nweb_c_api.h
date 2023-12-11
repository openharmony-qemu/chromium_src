/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_NWEB_NWEB_C_API_H
#define OHOS_NWEB_NWEB_C_API_H

#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define NWEB_EXPORT __attribute__((visibility("default")))

enum NWebDownloadItemState {
    IN_PROGRESS = 0,
    COMPLETE,
    CANCELED,
    INTERRUPTED,
    PENDING,
    PAUSED,
    MAX_DOWNLOAD_STATE,
};


typedef struct NWebDownloadDelegateCallback WebDownloadDelegateCallback;
typedef struct NWebBeforeDownloadCallbackWrapper WebBeforeDownloadCallbackWrapper;
typedef struct NWebDownloadItemCallbackWrapper WebDownloadItemCallbackWrapper;
typedef struct NWebDownloadItem NWebDownloadItem;

typedef void (*OnDownloadBeforeStart)(NWebDownloadItem *download_item, WebBeforeDownloadCallbackWrapper *wrapper);
typedef void (*OnDownloadDidUpdate)(NWebDownloadItem *download_item, WebDownloadItemCallbackWrapper *wrapper);

// / WebDownloader functions.
NWEB_EXPORT void WebDownloadManager_PutDownloadCallback(WebDownloadDelegateCallback *callback);

NWEB_EXPORT void WebDownloader_StartDownload(int32_t nweb_id, const char* url);

NWEB_EXPORT void WebDownloader_SetDownloadBeforeStart(WebDownloadDelegateCallback *callback, OnDownloadBeforeStart fun);

NWEB_EXPORT void WebDownloader_SetDownloadDidUpdate(WebDownloadDelegateCallback *callback, OnDownloadDidUpdate fun);

NWEB_EXPORT void WebDownloader_ResumeDownloadStatic(const NWebDownloadItem *download_item);

NWEB_EXPORT void WebDownloader_CreateDownloadDelegateCallback(WebDownloadDelegateCallback **callback);

NWEB_EXPORT void WebDownload_Continue(const WebBeforeDownloadCallbackWrapper *wrapper, const char *download_path);

NWEB_EXPORT void WebDownload_Cancel(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_Pause(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_Resume(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownloadItem_CreateWebDownloadItem(NWebDownloadItem **download_item);

NWEB_EXPORT void WebDownloadItem_Destroy(NWebDownloadItem *download_item);

NWEB_EXPORT void WebDownloadItem_SetFullPath(NWebDownloadItem *download_item, const char *full_path);

NWEB_EXPORT void WebDownloadItem_SetReceivedBytes(NWebDownloadItem *download_item, int64_t received_bytes);

NWEB_EXPORT void WebDownloadItem_SetTotalBytes(NWebDownloadItem *download_item, int64_t total_bytes);

NWEB_EXPORT void WebDownloadItem_SetETag(NWebDownloadItem *download_item, const char *etag);

NWEB_EXPORT void WebDownloadItem_SetLastModified(NWebDownloadItem *download_item, const char *last_modified);

NWEB_EXPORT void WebDownloadItem_SetMimeType(NWebDownloadItem *download_item, const char *mime_type);

NWEB_EXPORT void WebDownloadItem_SetUrl(NWebDownloadItem *download_item, const char *url);

NWEB_EXPORT void WebDownloadItem_SetReceivedSlices(NWebDownloadItem *download_item, const char *received_slices);

NWEB_EXPORT void WebDownloadItem_SetGuid(NWebDownloadItem *download_item, const char *guid);

NWEB_EXPORT char *WebDownloadItem_Guid(const NWebDownloadItem *download_item);

NWEB_EXPORT long WebDownloadItem_GetDownloadItemId(const NWebDownloadItem *download_item);

NWEB_EXPORT NWebDownloadItemState WebDownloadItem_GetState(const NWebDownloadItem *download_item);

NWEB_EXPORT int WebDownloadItem_CurrentSpeed(const NWebDownloadItem *download_item);

NWEB_EXPORT int WebDownloadItem_PercentComplete(const NWebDownloadItem *download_item);

NWEB_EXPORT int64_t WebDownloadItem_TotalBytes(const NWebDownloadItem *download_item);

NWEB_EXPORT int64_t WebDownloadItem_ReceivedBytes(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_FullPath(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_Url(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_OriginalUrl(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_SuggestedFileName(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_ContentDisposition(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_ETag(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_MimeType(const NWebDownloadItem *download_item);

NWEB_EXPORT bool WebDownloadItem_IsPaused(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_Method(const NWebDownloadItem *download_item);

NWEB_EXPORT int WebDownloadItem_LastErrorCode(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_ReceivedSlices(const NWebDownloadItem *download_item);

NWEB_EXPORT char *WebDownloadItem_LastModified(const NWebDownloadItem *download_item);

NWEB_EXPORT int WebDownloadItem_NWebId(const NWebDownloadItem *download_item);

NWEB_EXPORT void DestroyBeforeDownloadCallbackWrapper(WebBeforeDownloadCallbackWrapper *wrapper);

NWEB_EXPORT void DestroyDownloadItemCallbackWrapper(WebDownloadItemCallbackWrapper *wrapper);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // OHOS_NWEB_NWEB_C_API_H

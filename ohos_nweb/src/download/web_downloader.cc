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

#include <memory>

#include "ohos_nweb/src/capi/nweb_c_api.h"
#include "ohos_nweb/src/capi/nweb_download_delegate_callback.h"
#include "ohos_nweb/src/capi/nweb_screencapture_delegate_callback.h"
#include "ohos_nweb/src/nweb_hilog.h"
#include "ohos_nweb/src/nweb_impl.h"
#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
void WebDownloader_PutDownloadCallback(int32_t nweb_id,
                                       NWebDownloadDelegateCallback* callback) {
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d ", nweb_id);
    return;
  }
  if (!callback) {
    WVLOG_E("invalid callback");
    return;
  }
  nweb->PutWebDownloadDelegateCallback(
      std::make_shared<NWebDownloadDelegateCallback>(*callback));
}

void WebDownloader_StartDownload(int32_t nweb_id, const char* url) {
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d ", nweb_id);
    return;
  }
  nweb->StartDownload(url);
}

void WebDownloader_ResumeDownload(int32_t nweb_id,
                                  const NWebDownloadItem* download_item) {
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d ", nweb_id);
    return;
  }
  if (!download_item) {
    WVLOG_E("invalid download_item");
    return;
  }
  nweb->ResumeDownload(std::make_shared<NWebDownloadItem>(*download_item));
}

void WebDownloader_ResumeDownloadStatic(const NWebDownloadItem* download_item) {
  if (!download_item) {
    WVLOG_E("invalid download_item");
    return;
  }
  OHOS::NWeb::NWebImpl::ResumeDownloadStatic(
      std::make_shared<NWebDownloadItem>(*download_item));
}

void WebDownloader_CreateDownloadDelegateCallback(
    NWebDownloadDelegateCallback** callback) {
  if (callback) {
    *callback = new NWebDownloadDelegateCallback();
  }
}

void WebDownloader_SetDownloadBeforeStart(WebDownloadDelegateCallback* callback,
                                          OnDownloadBeforeStart fun) {
  if (callback) {
    ((NWebDownloadDelegateCallback*)callback)->downloadBeforeStart = fun;
  }
}

void WebDownloader_SetDownloadDidUpdate(WebDownloadDelegateCallback* callback,
                                        OnDownloadDidUpdate fun) {
  if (callback) {
    ((NWebDownloadDelegateCallback*)callback)->downloadDidUpdate = fun;
  }
}

void WebDownload_Continue(const NWebBeforeDownloadCallbackWrapper* wrapper,
                          const char* download_path) {
  if (wrapper) {
    wrapper->ContinueDownload(download_path);
  }
}

void WebDownload_CancelBeforeDownload(const NWebBeforeDownloadCallbackWrapper* wrapper) {
#ifdef OHOS_EX_DOWNLOAD
  if (wrapper) {
    wrapper->Cancel();
  }
#endif
}

void WebDownload_PauseBeforeDownload(const NWebBeforeDownloadCallbackWrapper* wrapper) {
#ifdef OHOS_EX_DOWNLOAD
  if (wrapper) {
    wrapper->Pause();
  }
#endif
}

void WebDownload_ResumeBeforeDownload(const NWebBeforeDownloadCallbackWrapper* wrapper) {
#ifdef OHOS_EX_DOWNLOAD
  if (wrapper) {
    wrapper->Resume();
  }
#endif
}

void WebDownload_Cancel(const NWebDownloadItemCallbackWrapper* wrapper) {
  if (wrapper) {
    wrapper->Cancel();
  }
}

void WebDownload_Pause(const NWebDownloadItemCallbackWrapper* wrapper) {
  if (wrapper) {
    wrapper->Pause();
  }
}

void WebDownload_Resume(const NWebDownloadItemCallbackWrapper* wrapper) {
  if (wrapper) {
    wrapper->Resume();
  }
}

NWebDownloadItemState WebDownload_GetItemState(int32_t nweb_id, long download_item_id) {
#ifdef OHOS_EX_DOWNLOAD
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d", nweb_id);
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }
  return nweb->GetDownloadItemState(download_item_id);
#else
  return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
#endif
}

NWebDownloadItemState WebDownload_GetItemStateByGuid(const std::string& guid) {
#ifdef OHOS_EX_DOWNLOAD
  return OHOS::NWeb::NWebImpl::GetDownloadItemStateByGuid(guid);
#else
  return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
#endif
}

void WebDownloadItem_CreateWebDownloadItem(NWebDownloadItem** download_item) {
  if (download_item) {
    *download_item = new NWebDownloadItem();
    return;
  }
  WVLOG_E("WebDownloadItem_CreateWebDownloadItem failed");
}

void WebDownloadItem_Destroy(NWebDownloadItem* download_item) {
  if (download_item) {
    delete download_item;
  }
}

void WebDownloadItem_SetFullPath(NWebDownloadItem* download_item,
                                 const char* full_path) {
  if (full_path) {
    if (download_item) {
      download_item->full_path = strdup(full_path);
      return;
    }
  }
  WVLOG_E("WebDownloadItem_SetFullPath failed");
}

void WebDownloadItem_SetReceivedBytes(NWebDownloadItem* download_item,
                                      int64_t received_bytes) {
  if (download_item) {
    download_item->received_bytes = received_bytes;
    return;
  }
  WVLOG_E("WebDownloadItem_SetReceivedBytes failed");
}

void WebDownloadItem_SetTotalBytes(NWebDownloadItem* download_item,
                                   int64_t total_bytes) {
  if (download_item) {
    download_item->total_bytes = total_bytes;
    return;
  }
  WVLOG_E("WebDownloadItem_SetTotalBytes failed");
}

void WebDownloadItem_SetReceivedSlices(NWebDownloadItem* download_item,
                                       const char* received_slices) {
  if (download_item) {
    download_item->received_slices = strdup(received_slices);
    return;
  }
  WVLOG_E("WebDownloadItem_SetReceivedSlices failed");
}

void WebDownloadItem_SetETag(NWebDownloadItem* download_item,
                             const char* etag) {
  if (download_item) {
    download_item->etag = strdup(etag);
    return;
  }
  WVLOG_E("WebDownloadItem_SetETag failed");
}

void WebDownloadItem_SetLastModified(NWebDownloadItem* download_item,
                                     const char* last_modified) {
  if (download_item) {
    download_item->last_modified = strdup(last_modified);
    return;
  }
  WVLOG_E("WebDownloadItem_SetLastModified failed");
}

void WebDownloadItem_SetMimeType(NWebDownloadItem* download_item,
                                 const char* mime_type) {
  if (mime_type) {
    if (download_item) {
      download_item->mime_type = strdup(mime_type);
      return;
    }
  }
  WVLOG_E("WebDownloadItem_SetMimeType failed");
}

void WebDownloadItem_SetUrl(NWebDownloadItem* download_item, const char* url) {
  if (url) {
    if (download_item) {
      download_item->url = strdup(url);
      return;
    }
  }
  WVLOG_E("WebDownloadItem_SetUrl failed");
}

void WebDownloadItem_SetGuid(NWebDownloadItem* download_item,
                             const char* guid) {
  if (guid) {
    if (download_item) {
      download_item->guid = strdup(guid);
      return;
    }
  }
  WVLOG_E("WebDownloadItem_SetGuid failed");
}

char* WebDownloadItem_Guid(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->guid;
  }
  WVLOG_E("WebDownloadItem_Guid download_item null");
  return nullptr;
}

long WebDownloadItem_GetDownloadItemId(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->download_item_id;
  }
  WVLOG_E("WebDownloadItem_GetDownloadItemId download_item null");
  return -1;
}

NWebDownloadItemState WebDownloadItem_GetState(
    const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->state;
  }
  WVLOG_E("WebDownloadItem_GetState download_item null");
  return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
}

int WebDownloadItem_CurrentSpeed(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->current_speed;
  }
  WVLOG_E("WebDownloadItem_CurrentSpeed download_item null");
  return 0;
}

int WebDownloadItem_PercentComplete(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->percent_complete;
  }
  WVLOG_E("WebDownloadItem_PercentComplete download_item null");
  return 0;
}

int64_t WebDownloadItem_TotalBytes(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->total_bytes;
  }
  WVLOG_E("WebDownloadItem_TotalBytes download_item null");
  return 0;
}

int64_t WebDownloadItem_ReceivedBytes(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->received_bytes;
  }
  WVLOG_E("WebDownloadItem_ReceivedBytes download_item null");
  return 0;
}

char* WebDownloadItem_FullPath(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->full_path;
  }
  WVLOG_E("WebDownloadItem_FullPath download_item null");
  return nullptr;
}

char* WebDownloadItem_Url(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->url;
  }
  WVLOG_E("WebDownloadItem_Url download_item null");
  return nullptr;
}

char* WebDownloadItem_OriginalUrl(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->original_url;
  }
  WVLOG_E("WebDownloadItem_OriginalUrl download_item null");
  return nullptr;
}

char* WebDownloadItem_SuggestedFileName(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->suggested_file_name;
  }
  WVLOG_E("WebDownloadItem_SuggestedFileName download_item null");
  return nullptr;
}

char* WebDownloadItem_ContentDisposition(
    const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->content_disposition;
  }
  WVLOG_E("WebDownloadItem_ContentDisposition download_item null");
  return nullptr;
}

char* WebDownloadItem_ETag(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->etag;
  }
  WVLOG_E("WebDownloadItem_ETag download_item null");
  return nullptr;
}

char* WebDownloadItem_MimeType(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->mime_type;
  }
  WVLOG_E("WebDownloadItem_MimeType download_item null");
  return nullptr;
}

bool WebDownloadItem_IsPaused(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->is_paused != 0;
  }
  WVLOG_E("WebDownloadItem_IsPaused download_item null");
  return false;
}

char* WebDownloadItem_Method(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->method;
  }
  WVLOG_E("WebDownloadItem_Method download_item null");
  return nullptr;
}

int WebDownloadItem_LastErrorCode(const NWebDownloadItem* download_item) {
  if (download_item) {
#if defined(REPORT_SYS_EVENT)
    ReportPageDownLoadErrorInfo(download_item->download_item_id, download_item->last_error_code);
#endif
    return download_item->last_error_code;
  }
  WVLOG_E("WebDownloadItem_LastErrorCode download_item null");
  return 0;
}

char* WebDownloadItem_ReceivedSlices(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->received_slices;
  }
  WVLOG_E("WebDownloadItem_ReceivedSlices download_item null");
  return nullptr;
}

char* WebDownloadItem_LastModified(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->last_modified;
  }
  WVLOG_E("WebDownloadItem_LastModified download_item null");
  return nullptr;
}

int WebDownloadItem_NWebId(const NWebDownloadItem* download_item) {
  if (download_item) {
    return download_item->nweb_id;
  }
  WVLOG_E("WebDownloadItem_NWebId download_item null");
  return 0;
}

void DestroyBeforeDownloadCallbackWrapper(
    NWebBeforeDownloadCallbackWrapper* wrapper) {
  if (wrapper) {
    delete wrapper;
  }
  return;
}

void DestroyDownloadItemCallbackWrapper(
    NWebDownloadItemCallbackWrapper* wrapper) {
  if (wrapper) {
    delete wrapper;
  }
  return;
}

void WebScreenCapture_CreateScreenCaptureDelegateCallback(
    NWebScreenCaptureDelegateCallback **callback) {
#if defined(OHOS_EX_SCREEN_CAPTURE)
  if (callback) {
    *callback = new NWebScreenCaptureDelegateCallback();
  }
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)
}

void WebScreenCapture_SetScreenCaptureOnStateChange(
    NWebScreenCaptureDelegateCallback *callback,
    OnSrceenCaptureOnStateChange fun) {
#if defined(OHOS_EX_SCREEN_CAPTURE)
  if (callback) {
    ((NWebScreenCaptureDelegateCallback *)callback)->OnStateChange = fun;
  }
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)
}

void WebScreenCapture_PutScreenCaptureCallback(
    NWebScreenCaptureDelegateCallback *callback, int32_t nweb_id) {
#if defined(OHOS_EX_SCREEN_CAPTURE)
  OHOS::NWeb::NWebImpl *nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d ", nweb_id);
    return;
  }
  if (!callback) {
    WVLOG_E("invalid callback");
    return;
  }
  nweb->PutWebScreenCaptureDelegateCallback(
      std::make_shared<NWebScreenCaptureDelegateCallback>(*callback));
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)
}

void WebScreenCapture_StopScreenCapture(int32_t nweb_id, const char* sessionid) {
#if defined(OHOS_EX_SCREEN_CAPTURE)
  OHOS::NWeb::NWebImpl *nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  if (!nweb) {
    WVLOG_E("fail to find a nweb with %{public}d ", nweb_id);
    return;
  }
  if (!sessionid) {
    WVLOG_E("sessionid is null");
    return;
  }
  nweb->StopScreenCapture(sessionid);
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)
}

#ifdef __cplusplus
}
#endif  // __cplusplus

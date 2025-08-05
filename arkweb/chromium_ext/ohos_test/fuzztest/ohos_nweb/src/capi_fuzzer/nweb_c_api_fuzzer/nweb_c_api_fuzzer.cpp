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

#include <fuzzer/FuzzedDataProvider.h>

#include "base/logging.h"
#include "arkweb/ohos_nweb/src/capi/nweb_c_api.h"
#include "arkweb/ohos_nweb/src/capi/nweb_download_delegate_callback.h"

namespace OHOS::NWeb {

class MockCefBeforeDownloadCallback : public CefBeforeDownloadCallback {
public:
  IMPLEMENT_REFCOUNTING(MockCefBeforeDownloadCallback);

  void Continue(const CefString& download_path, bool show_dialog) override {}
#if BUILDFLAG(IS_OHOS)
  void Cancel() override {}
  void Pause() override {}
  void Resume() override {}
#endif
};

class MockCefDownloadItemCallback : public CefDownloadItemCallback {
public:
  IMPLEMENT_REFCOUNTING(MockCefDownloadItemCallback);

  void Cancel() override {}
  void Pause() override {}
  void Resume() override {}
};

static void DownloadBeforeStart(struct NWebDownloadItem* item,
            struct NWebBeforeDownloadCallbackWrapper* wrapper) {}

static void DownloadDidUpdate(struct NWebDownloadItem* item,
            struct NWebDownloadItemCallbackWrapper* wrapper) {}

static void OnStateChange(int32_t nweb_id, const char* session_id, int32_t code) {}

void WebDownloader_FuzzTest(FuzzedDataProvider* fdp) {
  WebDownloader_StartDownload(fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeRandomLengthString(64).c_str());
  NWebDownloadDelegateCallback* downloadDelegateCallbackPtr = nullptr;
  WebDownloader_CreateDownloadDelegateCallback(&downloadDelegateCallbackPtr);
  if (downloadDelegateCallbackPtr == nullptr) {
    return;
  }
  downloadDelegateCallbackPtr->nweb_id = fdp->ConsumeIntegral<int32_t>();
  WebDownloader_SetDownloadBeforeStart(downloadDelegateCallbackPtr, DownloadBeforeStart);
  WebDownloader_SetDownloadDidUpdate(downloadDelegateCallbackPtr, DownloadDidUpdate);
  if (downloadDelegateCallbackPtr != nullptr) {
    delete downloadDelegateCallbackPtr;
    downloadDelegateCallbackPtr = nullptr;
  }
  WebDownloadManager_PutDownloadCallback(nullptr);
}

void WebDownload_FuzzTest(FuzzedDataProvider* fdp) {
  NWebBeforeDownloadCallbackWrapper * wrapperPtr1 = new NWebBeforeDownloadCallbackWrapper();
  CefRefPtr<MockCefBeforeDownloadCallback> callback1 = new MockCefBeforeDownloadCallback();
  wrapperPtr1->callback_ = std::move(callback1);
  WebDownload_Continue(wrapperPtr1, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownload_CancelBeforeDownload(wrapperPtr1);
  WebDownload_PauseBeforeDownload(wrapperPtr1);
  WebDownload_ResumeBeforeDownload(wrapperPtr1);
  DestroyBeforeDownloadCallbackWrapper(wrapperPtr1);

  NWebDownloadItemCallbackWrapper* wrapperPtr2 = new NWebDownloadItemCallbackWrapper();
  CefRefPtr<MockCefDownloadItemCallback> callback2 = new MockCefDownloadItemCallback();
  wrapperPtr2->callback_ = std::move(callback2);
  WebDownload_Cancel(wrapperPtr2);
  WebDownload_Pause(wrapperPtr2);
  WebDownload_Resume(wrapperPtr2);
  WebDownload_GetItemState(fdp->ConsumeIntegral<int32_t>(), fdp->ConsumeIntegral<long>());
  WebDownload_GetItemStateByGuid(fdp->ConsumeRandomLengthString(64));
  DestroyDownloadItemCallbackWrapper(wrapperPtr2);
}

void WebDownloadItem_FuzzTest(FuzzedDataProvider* fdp) {
  NWebDownloadItem* downloadItemPtr = nullptr;
  WebDownloadItem_CreateWebDownloadItem(&downloadItemPtr);
  if (downloadItemPtr == nullptr) {
    return;
  }
  WebDownloadItem_SetReceivedBytes(downloadItemPtr, fdp->ConsumeIntegral<int64_t>());
  WebDownloadItem_SetTotalBytes(downloadItemPtr, fdp->ConsumeIntegral<long>());
  WebDownloadItem_SetGuid(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetUrl(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetFullPath(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetETag(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetLastModified(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetMimeType(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_SetReceivedSlices(downloadItemPtr, fdp->ConsumeRandomLengthString(64).c_str());
  WebDownloadItem_Guid(downloadItemPtr);
  WebDownloadItem_GetDownloadItemId(downloadItemPtr);
  WebDownloadItem_GetState(downloadItemPtr);
  WebDownloadItem_CurrentSpeed(downloadItemPtr);
  WebDownloadItem_PercentComplete(downloadItemPtr);
  WebDownloadItem_TotalBytes(downloadItemPtr);
  WebDownloadItem_ReceivedBytes(downloadItemPtr);
  WebDownloadItem_FullPath(downloadItemPtr);
  WebDownloadItem_Url(downloadItemPtr);
  WebDownloadItem_OriginalUrl(downloadItemPtr);
  WebDownloadItem_SuggestedFileName(downloadItemPtr);
  WebDownloadItem_ContentDisposition(downloadItemPtr);
  WebDownloadItem_ETag(downloadItemPtr);
  WebDownloadItem_MimeType(downloadItemPtr);
  WebDownloadItem_IsPaused(downloadItemPtr);
  WebDownloadItem_Method(downloadItemPtr);
  WebDownloadItem_LastErrorCode(downloadItemPtr);
  WebDownloadItem_ReceivedSlices(downloadItemPtr);
  WebDownloadItem_NWebId(downloadItemPtr);
  WebDownloader_ResumeDownloadStatic(nullptr);
  WebDownloadItem_Destroy(downloadItemPtr);
}

void WebScreenCapture_FuzzTest(FuzzedDataProvider* fdp) {
  NWebScreenCaptureDelegateCallback* delegateCallbackPtr = nullptr;
  WebScreenCapture_CreateScreenCaptureDelegateCallback(&delegateCallbackPtr);
  if (delegateCallbackPtr == nullptr) {
    return;
  }
  WebScreenCapture_SetScreenCaptureOnStateChange(delegateCallbackPtr, OnStateChange);
  WebScreenCapture_PutScreenCaptureCallback(nullptr, fdp->ConsumeIntegral<int32_t>());
  WebTranslate_GetWebLanguage(fdp->ConsumeIntegral<int32_t>());
  WebScreenCapture_StopScreenCapture(fdp->ConsumeIntegral<int32_t>(),
                                    fdp->ConsumeRandomLengthString(64).c_str());
  WebScreenCapture_SetScreenCapturePickerShow(fdp->ConsumeIntegral<int32_t>());
  WebScreenCapture_DisableSessionReuse(fdp->ConsumeIntegral<int32_t>());
}
} // OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);
  OHOS::NWeb::WebDownloader_FuzzTest(&fdp);
  OHOS::NWeb::WebDownload_FuzzTest(&fdp);
  OHOS::NWeb::WebDownloadItem_FuzzTest(&fdp);
  OHOS::NWeb::WebScreenCapture_FuzzTest(&fdp);
  return 0;
}
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

#ifndef NWEB_EXTENSION_DOWNLOADS_CEF_DELEGATE_H
#define NWEB_EXTENSION_DOWNLOADS_CEF_DELEGATE_H
#pragma once

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_downloads_types.h"

using DownloadEraseCallback = base::RepeatingCallback<
    void(const char* error, const uint32_t size, const int* eraseIds)>;

using DownloadsOpenCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsRemoveFileCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsPauseCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsResumeCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsCancelCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsAcceptDangerCallback = base::RepeatingCallback<void(const char* error)>;
using DownloadsSetUiOptionsCallback = base::RepeatingCallback<void(const char* error)>;

namespace OHOS::NWeb {
class NWebExtensionDownloadCefDelegate {
 public:
  static NWebExtensionDownloadCefDelegate& GetInstance();

  // chrome.downloads.erase
  bool Erase(NWebDownloadsQueryInfo* query, DownloadEraseCallback callback);
  void EraseCallback(int requestId,
                     const char* error,
                     const uint32_t size,
                     const int* eraseIds);

  // chrome.downloads.open
  bool Open(int downloadId, DownloadsOpenCallback callback);
  void OpenCallback(int requestId, const char* error);

  // chrome.downloads.removeFile
  bool RemoveFile(int downloadId, DownloadsOpenCallback callback);
  void RemoveFileCallback(int requestId, const char* error);

  // chrome.downloads.pause
  bool Pause(int downloadId, DownloadsPauseCallback callback);
  void PauseCallback(int requestId, const char* error);

  // chrome.downloads.resume
  bool Resume(int downloadId, DownloadsResumeCallback callback);
  void ResumeCallback(int requestId, const char* error);

  // chrome.downloads.cancel
  bool Cancel(int downloadId, DownloadsCancelCallback callback);
  void CancelCallback(int requestId, const char* error);

  // chrome.downloads.acceptDanger
  bool AcceptDanger(int downloadId, DownloadsAcceptDangerCallback callback);
  void AcceptDangerCallback(int requestId, const char* error);

  // chrome.downloads.setUiOptions
  bool SetUiOptions(NWebExtensionUiOptions* options , DownloadsSetUiOptionsCallback callback);
  void SetUiOptionsCallback(int requestId, const char* error);

  // chrome.downloads.show
  void Show(int downloadId);

  // chrome.downloads.showDefaultFolder
  void ShowDefaultFolder();
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_DOWNLOADS_CEF_DELEGATE_H
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

#include "nweb_extension_downloads_cef_delegate.h"

#include <atomic>
#include <map>
#include <mutex>

#include "base/logging.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/downloads/download_api_ext_router.h"
#include "content/public/browser/browser_context.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_downloads_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {
static std::map<int, DownloadEraseCallback> g_downloads_erase_callback_map_;
std::mutex g_downloads_erase_callback_map_mutex;

static std::map<int, DownloadsOpenCallback> g_downloads_open_callback_map_;
std::mutex g_downloads_open_callback_map_mutex;

static std::map<int, DownloadsRemoveFileCallback>
    g_downloads_removefile_callback_map_;
std::mutex g_downloads_removefile_callback_map_mutex;

static std::map<int, DownloadsPauseCallback> g_downloads_pause_callback_map_;
std::mutex g_downloads_pause_callback_map_mutex;

static std::map<int, DownloadsResumeCallback> g_downloads_resume_callback_map_;
std::mutex g_downloads_resume_callback_map_mutex;

static std::map<int, DownloadsCancelCallback> g_downloads_cancel_callback_map_;
std::mutex g_downloads_cancel_callback_map_mutex;

static std::map<int, DownloadsAcceptDangerCallback>
    g_downloads_accept_danger_callback_map_;
std::mutex g_downloads_accept_danger_callback_map_mutex;

static std::map<int, DownloadsSetUiOptionsCallback>
    g_downloads_set_ui_options_callback_map_;
std::mutex g_downloads_set_ui_options_callback_map_mutex;

static std::map<int, DownloadsShowCallback> g_downloads_show_callback_map_;
std::mutex g_downloads_show_callback_map_mutex;

static std::map<int, DownloadSearchCallback> g_downloads_search_callback_map_;
std::mutex g_downloads_search_callback_map_mutex;

static std::map<int, DownloadGetFileIconCallback>
    g_downloads_get_fileicon_callback_map_;
std::mutex g_downloads_get_fileicon_callback_map_mutex;

static std::map<int, DownloadsIdCallback> g_downloads_download_id_map_;
std::mutex g_downloads_download_id_map_mutex;

}  // namespace

// static
NWebExtensionDownloadCefDelegate&
NWebExtensionDownloadCefDelegate::GetInstance() {
  static NWebExtensionDownloadCefDelegate instance;
  return instance;
}

// downloads.erase
bool NWebExtensionDownloadCefDelegate::Erase(ExDownloadsQueryInfo& query,
                                             DownloadEraseCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_erase_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_erase_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Erase(
      currentRequestId, query);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_erase_callback_map_mutex);
    g_downloads_erase_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::EraseCallback(int requestId,
  std::optional<std::string> error,
  std::vector<int32_t> eraseIds) {
  DownloadEraseCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_erase_callback_map_mutex);
    auto it = g_downloads_erase_callback_map_.find(requestId);
    if (it == g_downloads_erase_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_erase_callback_map_.erase(it);
  }
  std::move(callback).Run(error, eraseIds);
}

// downloads.open
bool NWebExtensionDownloadCefDelegate::Open(const int downloadId,
                                            DownloadsOpenCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Open downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_open_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_open_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Open(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_open_callback_map_mutex);
    g_downloads_open_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::OpenCallback(int requestId,
                                                    std::optional<std::string> error) {
  DownloadsOpenCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_open_callback_map_mutex);
    auto it = g_downloads_open_callback_map_.find(requestId);
    if (it == g_downloads_open_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::OpenCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_open_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.remove
bool NWebExtensionDownloadCefDelegate::RemoveFile(
    const int downloadId,
    DownloadsOpenCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::RemoveFile downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_removefile_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_removefile_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().RemoveFile(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_removefile_callback_map_mutex);
    g_downloads_removefile_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::RemoveFileCallback(int requestId,
                                                          std::optional<std::string> error) {
  DownloadsOpenCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_removefile_callback_map_mutex);
    auto it = g_downloads_removefile_callback_map_.find(requestId);
    if (it == g_downloads_removefile_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::RemoveFileCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_removefile_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.pause
bool NWebExtensionDownloadCefDelegate::Pause(const int downloadId,
                                             DownloadsPauseCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Pause downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_pause_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_pause_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Pause(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_pause_callback_map_mutex);
    g_downloads_pause_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::PauseCallback(int requestId,
                                                     std::optional<std::string> error) {
  DownloadsPauseCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_pause_callback_map_mutex);
    auto it = g_downloads_pause_callback_map_.find(requestId);
    if (it == g_downloads_pause_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::PauseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_pause_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.resume
bool NWebExtensionDownloadCefDelegate::Resume(
    const int downloadId,
    DownloadsResumeCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Resume downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_resume_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_resume_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Resume(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_resume_callback_map_mutex);
    g_downloads_resume_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::ResumeCallback(int requestId,
                                                      std::optional<std::string> error) {
  DownloadsResumeCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_resume_callback_map_mutex);
    auto it = g_downloads_resume_callback_map_.find(requestId);
    if (it == g_downloads_resume_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::ResumeCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_resume_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.cancel
bool NWebExtensionDownloadCefDelegate::Cancel(
    const int downloadId,
    DownloadsCancelCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Cancel downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_cancel_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_cancel_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Cancel(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_cancel_callback_map_mutex);
    g_downloads_cancel_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::CancelCallback(int requestId,
                                                      std::optional<std::string> error) {
  DownloadsCancelCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_cancel_callback_map_mutex);
    auto it = g_downloads_cancel_callback_map_.find(requestId);
    if (it == g_downloads_cancel_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::CancelCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_cancel_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.acceptDanger
bool NWebExtensionDownloadCefDelegate::AcceptDanger(
    const int downloadId,
    DownloadsAcceptDangerCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::AcceptDanger downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_accept_danger_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_accept_danger_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().AcceptDanger(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(
        g_downloads_accept_danger_callback_map_mutex);
    g_downloads_accept_danger_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::AcceptDangerCallback(int requestId,
                                                            std::optional<std::string> error) {
  DownloadsAcceptDangerCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_accept_danger_callback_map_mutex);
    auto it = g_downloads_accept_danger_callback_map_.find(requestId);
    if (it == g_downloads_accept_danger_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::AcceptDangerCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_accept_danger_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.setUiOptions
bool NWebExtensionDownloadCefDelegate::SetUiOptions(
    const ExDownloadsUiOptions& options,
    DownloadsSetUiOptionsCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_set_ui_options_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_set_ui_options_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().SetUiOptions(
      currentRequestId, options);
  if (!result) {
    std::lock_guard<std::mutex> lock(
        g_downloads_set_ui_options_callback_map_mutex);
    g_downloads_set_ui_options_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::SetUiOptionsCallback(int requestId,
                                                            std::optional<std::string> error) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::SetUiOptionsCallback";
  DownloadsSetUiOptionsCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_set_ui_options_callback_map_mutex);
    auto it = g_downloads_set_ui_options_callback_map_.find(requestId);
    if (it == g_downloads_set_ui_options_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::SetUiOptionsCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_set_ui_options_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.show
bool NWebExtensionDownloadCefDelegate::Show(const int downloadId,
                                            DownloadsShowCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Show downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_show_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_show_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Show(
      currentRequestId, downloadId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_show_callback_map_mutex);
    g_downloads_show_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionDownloadCefDelegate::ShowCallback(int requestId,
                                                    std::optional<std::string> error) {
  DownloadsShowCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_show_callback_map_mutex);
    auto it = g_downloads_show_callback_map_.find(requestId);
    if (it == g_downloads_show_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::ShowCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_show_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

// downloads.showDefaultFolder
void NWebExtensionDownloadCefDelegate::ShowDefaultFolder() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return;
#else
  NWebExtensionDownloadsDispatcher::GetInstance().ShowDefaultFolder();
#endif
}

// downloads.search
bool NWebExtensionDownloadCefDelegate::Search(ExDownloadsQueryInfo& query,
                                              DownloadSearchCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_search_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_search_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().Search(
      currentRequestId, query);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_search_callback_map_mutex);
    g_downloads_search_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::SearchCallback(
    int requestId,
    std::optional<std::string> error,
    const uint32_t size,
    std::vector<ExDownloadsItem> downloadItems) {
  DownloadSearchCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_search_callback_map_mutex);
    auto it = g_downloads_search_callback_map_.find(requestId);
    if (it == g_downloads_search_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::SearchCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_search_callback_map_.erase(it);
  }
  std::move(callback).Run(error, size, downloadItems);
}

// downloads.download
bool NWebExtensionDownloadCefDelegate::GetDownloadId(
    const std::string& guid,
    DownloadsIdCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::GetDownloadId"
            << "guid: " << guid;
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_downloads_download_id_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_download_id_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().GetDownloadId(
      currentRequestId, guid);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_downloads_download_id_map_mutex);
    g_downloads_download_id_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionDownloadCefDelegate::GetDownloadIdCallback(int requestId,
                                                             std::optional<std::string> error,
                                                             int downloadId) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::GetDownloadIdCallback"
            << "downloadId: " << downloadId;
  DownloadsIdCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_download_id_map_mutex);
    auto it = g_downloads_download_id_map_.find(requestId);
    if (it == g_downloads_download_id_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::GetDownloadIdCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_download_id_map_.erase(it);
  }
  std::move(callback).Run(error, downloadId);
}

// downloads.getFileIcon
bool NWebExtensionDownloadCefDelegate::GetFileIcon(
    ExDownloadsGetFileIconOptions& iconOption,
    DownloadGetFileIconCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId = 0;
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_get_fileicon_callback_map_mutex);
    currentRequestId = ++requestId;
    g_downloads_get_fileicon_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionDownloadsDispatcher::GetInstance().GetFileIcon(
      currentRequestId, iconOption);
  if (!result) {
    std::lock_guard<std::mutex> lock(
        g_downloads_get_fileicon_callback_map_mutex);
    g_downloads_get_fileicon_callback_map_.erase(currentRequestId);
  }
  return result;

#endif
}

void NWebExtensionDownloadCefDelegate::GetFileIconCallback(
    int requestId,
    std::optional<std::string> error,
    std::string iconUrl) {
  DownloadGetFileIconCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_get_fileicon_callback_map_mutex);
    auto it = g_downloads_get_fileicon_callback_map_.find(requestId);
    if (it == g_downloads_get_fileicon_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::GetFileIconCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_get_fileicon_callback_map_.erase(it);
  }
  std::move(callback).Run(error, iconUrl);
}

// getAllDownloadItem
ExDownloadsItemVector NWebExtensionDownloadCefDelegate::GetAllDownloadItem() {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::GetAllDownloadItem";
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return nullptr;
#else
  return NWebExtensionDownloadsDispatcher::GetInstance().GetAllDownloadItem();
#endif
}

// downloads.OnCreated
void NWebExtensionDownloadCefDelegate::OnCreated(ExDownloadsItem* item) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::OnCreated id: " << item->id;
  extensions::ExtensionDownloadsEventRouterEx::GetInstance().OnDownloadCreated(
      item, GetBrowserContext());
}

// downloads.OnChanged
void NWebExtensionDownloadCefDelegate::OnChanged(ExDownloadsItem* item) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::OnChanged id: " << item->id;
  extensions::ExtensionDownloadsEventRouterEx::GetInstance().OnDownloadUpdated(
      item, GetBrowserContext());
}

// downloads.OnErased
void NWebExtensionDownloadCefDelegate::OnErased(int downloadId) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::OnErased id: " << downloadId;
  extensions::ExtensionDownloadsEventRouterEx::GetInstance().OnDownloadRemoved(
      downloadId, GetBrowserContext());
}

// downloads.OnDeterminingFilename
void NWebExtensionDownloadCefDelegate::OnDeterminingFilename(
    const ExDownloadsItem* item,
    const char* suggestedPath,
    FilenameChangedCallback callback) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::OnDeterminingFilename id: "
            << item->id << ", suggestedPath: " << suggestedPath;
  extensions::ExtensionDownloadsEventRouterEx::GetInstance()
      .OnDeterminingFilename(item, suggestedPath, std::move(callback),
                             GetBrowserContext());
}

}  // namespace OHOS::NWeb
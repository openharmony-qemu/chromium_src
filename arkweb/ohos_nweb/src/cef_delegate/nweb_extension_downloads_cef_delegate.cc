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

#include <map>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/logging.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
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

}  // namespace

// static
NWebExtensionDownloadCefDelegate&
NWebExtensionDownloadCefDelegate::GetInstance() {
  static NWebExtensionDownloadCefDelegate instance;
  return instance;
}

bool NWebExtensionDownloadCefDelegate::Erase(NWebDownloadsQueryInfo* query,
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
                                                     const char* error,
                                                     const uint32_t size,
                                                     const int* eraseIds) {
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
  std::move(callback).Run(error, size, eraseIds);
}

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
                                                    const char* error) {
  DownloadsOpenCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_open_callback_map_mutex);
    auto it = g_downloads_open_callback_map_.find(requestId);
    if (it == g_downloads_open_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_open_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

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
                                                          const char* error) {
  DownloadsOpenCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_removefile_callback_map_mutex);
    auto it = g_downloads_removefile_callback_map_.find(requestId);
    if (it == g_downloads_removefile_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_removefile_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

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
                                                     const char* error) {
  DownloadsPauseCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_pause_callback_map_mutex);
    auto it = g_downloads_pause_callback_map_.find(requestId);
    if (it == g_downloads_pause_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_pause_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

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
                                                      const char* error) {
  DownloadsResumeCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_resume_callback_map_mutex);
    auto it = g_downloads_resume_callback_map_.find(requestId);
    if (it == g_downloads_resume_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_resume_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

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
                                                      const char* error) {
  DownloadsCancelCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_downloads_cancel_callback_map_mutex);
    auto it = g_downloads_cancel_callback_map_.find(requestId);
    if (it == g_downloads_cancel_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_cancel_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

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
                                                            const char* error) {
  DownloadsAcceptDangerCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_accept_danger_callback_map_mutex);
    auto it = g_downloads_accept_danger_callback_map_.find(requestId);
    if (it == g_downloads_accept_danger_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_accept_danger_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

bool NWebExtensionDownloadCefDelegate::SetUiOptions(
    NWebExtensionUiOptions* options,
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
                                                            const char* error) {
  DownloadsSetUiOptionsCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_downloads_set_ui_options_callback_map_mutex);
    auto it = g_downloads_set_ui_options_callback_map_.find(requestId);
    if (it == g_downloads_set_ui_options_callback_map_.end()) {
      LOG(ERROR) << "NWebExtensionDownloadCefDelegate::EraseCallback"
                 << "requestId not found: " << requestId;
      return;
    }
    callback = std::move(it->second);
    g_downloads_set_ui_options_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

void NWebExtensionDownloadCefDelegate::Show(const int downloadId) {
  LOG(INFO) << "NWebExtensionDownloadCefDelegate::Show downloadId: "
            << downloadId;
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  NWebExtensionDownloadsDispatcher::GetInstance().Show(downloadId);

#endif
}

void NWebExtensionDownloadCefDelegate::ShowDefaultFolder() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  NWebExtensionDownloadsDispatcher::GetInstance().ShowDefaultFolder();
#endif
}

}  // namespace OHOS::NWeb
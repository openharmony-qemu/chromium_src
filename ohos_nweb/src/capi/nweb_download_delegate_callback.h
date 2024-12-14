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

#ifndef OHOS_NWEB_SRC_NWEB_DOWNLOAD_DELEGATE_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_DOWNLOAD_DELEGATE_CALLBACK_H_

#include <cstddef>
#include "cef/include/cef_base.h"
#include "cef/include/cef_download_handler.h"
#include "ohos_nweb/src/capi/nweb_download_item.h"

struct NWebBeforeDownloadCallbackWrapper {
  CefRefPtr<CefBeforeDownloadCallback> callback_;
  void ContinueDownload(const char* file_path) const {
    if (callback_ && file_path) {
      CefString cef_file_path;
      std::string file_path_str(file_path);
      cef_file_path.FromString(file_path_str);
      callback_->Continue(cef_file_path, false);
    }
  }

#if defined(OHOS_EX_DOWNLOAD)
  void Cancel() const {
    if (callback_) {
      callback_->Cancel();
    }
  }

  void Pause() const {
    if (callback_) {
      callback_->Pause();
    }
  }

  void Resume() const {
    if (callback_) {
      callback_->Resume();
    }
  }
#endif
};

struct NWebDownloadItemCallbackWrapper {
  CefRefPtr<CefDownloadItemCallback> callback_;

  void Pause() const {
    if (callback_) {
      callback_->Pause();
    }
  }

  void Cancel() const {
    if (callback_) {
      callback_->Cancel();
    }
  }

  void Resume() const {
    if (callback_) {
      callback_->Resume();
    }
  }
};

struct NWebDownloadDelegateCallback {
  int nweb_id;

  void (*downloadBeforeStart)(
      struct NWebDownloadItem* item,
      struct NWebBeforeDownloadCallbackWrapper* wrapper);

  void (*downloadDidUpdate)(struct NWebDownloadItem* item,
                            struct NWebDownloadItemCallbackWrapper* wrapper);
};

#endif  // OHOS_NWEB_SRC_NWEB_DOWNLOAD_DELEGATE_CALLBACK_H_

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

#ifndef OHOS_NWEB_SRC_NWEB_DOWNLOAD_ITEM_H_
#define OHOS_NWEB_SRC_NWEB_DOWNLOAD_ITEM_H_

#include "cef/include/cef_download_item.h"
#include "ohos_nweb/src/capi/nweb_c_api.h"
#include "ohos_nweb/src/nweb_hilog.h"
#include "include/cef_download_item_ext.h"

//  Defined in DownloadItem::DownloadState::INTERRUPTED
//  components/download/public/common/download_item.h
const int INTERRUPTED_STATE = 3;

struct NWebDownloadItem {
  long download_item_id;
  int nweb_id;

  long current_speed;
  int percent_complete;
  long total_bytes;
  long received_bytes;
  int last_error_code;

  char* guid = nullptr;
  char* full_path = nullptr;
  char* url = nullptr;
  char* etag = nullptr;
  char* original_url = nullptr;
  char* suggested_file_name = nullptr;
  char* content_disposition = nullptr;
  char* mime_type = nullptr;
  char* last_modified = nullptr;

  int is_paused;

  char* method = nullptr;
  char* received_slices = nullptr;
  NWebDownloadItemState state;

  NWebDownloadItem() { WVLOG_I("NWebDownloadItem() is called"); }

  ~NWebDownloadItem() {
    WVLOG_I("~NWebDownloadItem() is called");
    if (guid) {
      free(guid);
      guid = nullptr;
    }
    if (full_path) {
      free(full_path);
      full_path = nullptr;
    }
    if (url) {
      free(url);
      url = nullptr;
    }
    if (etag) {
      free(etag);
      etag = nullptr;
    }
    if (original_url) {
      free(original_url);
      original_url = nullptr;
    }
    if (suggested_file_name) {
      free(suggested_file_name);
      suggested_file_name = nullptr;
    }
    if (content_disposition) {
      free(content_disposition);
      content_disposition = nullptr;
    }
    if (mime_type) {
      free(mime_type);
      mime_type = nullptr;
    }
    if (last_modified) {
      free(last_modified);
      last_modified = nullptr;
    }
    if (method) {
      free(method);
      method = nullptr;
    }
    if (received_slices) {
      free(received_slices);
      received_slices = nullptr;
    }
  }

  NWebDownloadItem(CefRefPtr<CefDownloadItem> download_item) {
    WVLOG_I("NWebDownloadItem() is called");
    CHECK(download_item);
    nweb_id = download_item->AsArkDownloadItem()->GetNWebId();
    download_item_id = (long)download_item->GetId();
    current_speed = (long)download_item->GetCurrentSpeed();
    percent_complete = download_item->GetPercentComplete();
    total_bytes = (long)download_item->GetTotalBytes();
    received_bytes = (long)download_item->GetReceivedBytes();

    std::string guid_ =
        download_item->AsArkDownloadItem()->GetGuid().ToString();
    guid = strdup(guid_.c_str());
    std::string full_path_ = download_item->GetFullPath().ToString();
    full_path = strdup(full_path_.c_str());
    std::string url_ = download_item->GetURL().ToString();
    url = strdup(url_.c_str());
    std::string original_url_ = download_item->GetOriginalUrl().ToString();
    original_url = strdup(original_url_.c_str());
    std::string content_disposition_ =
        download_item->GetContentDisposition().ToString();
    content_disposition = strdup(content_disposition_.c_str());
    std::string mime_type_ = download_item->GetMimeType().ToString();
    mime_type = strdup(mime_type_.c_str());
    state = GetNWebState(download_item);

    std::string method_ =
        download_item->AsArkDownloadItem()->GetMethod().ToString();
    method = strdup(method_.c_str());
    last_error_code = download_item->AsArkDownloadItem()->GetLastErrorCode();
    std::string received_slices_ =
        download_item->AsArkDownloadItem()->GetReceivedSlices();
    received_slices = strdup(received_slices_.c_str());
    std::string last_modified_ =
        download_item->AsArkDownloadItem()->GetLastModifiedTime();
    last_modified = strdup(last_modified_.c_str());
    std::string etag_ = download_item->AsArkDownloadItem()->GetETag();
    etag = strdup(etag_.c_str());
  }

  static NWebDownloadItemState GetNWebState(
      CefRefPtr<CefDownloadItem> download_item) {
    CHECK(download_item);
    int cef_download_item_state_ =
        download_item->AsArkDownloadItem()->GetState();
    bool is_interrupted = (cef_download_item_state_ == INTERRUPTED_STATE);
    NWebDownloadItemState nweb_state =
        NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    if (!download_item->IsValid()) {
      nweb_state = NWebDownloadItemState::CANCELED;
    } else {
      if (download_item->IsInProgress()) {
        if (download_item->AsArkDownloadItem()->IsPaused()) {
          nweb_state = NWebDownloadItemState::PAUSED;
        } else if (download_item->AsArkDownloadItem()->IsPending()) {
          nweb_state = NWebDownloadItemState::PENDING;
        } else {
          nweb_state = NWebDownloadItemState::IN_PROGRESS;
        }
      } else if (download_item->IsCanceled()) {
        nweb_state = NWebDownloadItemState::CANCELED;
      } else if (download_item->IsComplete()) {
        nweb_state = NWebDownloadItemState::COMPLETE;
      } else if (is_interrupted) {
        nweb_state = NWebDownloadItemState::INTERRUPTED;
      } else {
        nweb_state = NWebDownloadItemState::CANCELED;
      }
    }
    return nweb_state;
  }
};

#endif  //  OHOS_NWEB_SRC_NWEB_DOWNLOAD_ITEM_H_

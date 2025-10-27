/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_INCLUDE_CEF_APP_EXT_H_
#define ARKWEB_INCLUDE_CEF_APP_EXT_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
///
/// This function should be called on the main application thread.
///
/*--cef()--*/
void CefApplyHttpDns();
///
/// This function should be called on the main application thread.
///
/*--cef()--*/
void CefSetDownloadHandler(CefRefPtr<CefDownloadHandler> download_handler);
///
/// This function should be called on the main application thread.
///
/*--cef()--*/
void CefResumeDownload(const CefString& guid,
                       const CefString& url,
                       const CefString& full_path,
                       int64_t received_bytes,
                       int64_t total_bytes,
                       const CefString& etag,
                       const CefString& mime_type,
                       const CefString& last_modified,
                       const CefString& received_slices_string);
///
/// This function should be called on the main application thread.
///
/*--cef()--*/
void CefSetFileRenameOption(const int file_rename_option);

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
///
/// This function should be called on the main application thread.
///
/*--cef()--*/
CefRefPtr<CefDownloadItem> CefGetDownloadItem(const std::string& guid);

///
/// Interface to implement to be notified of asynchronous completion via
/// CefBrowserHostBase::StoreWebArchive().
///
/*--cef(source=client)--*/
class CefReadDownloadDataCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion. |result| will either be the
  /// filename under which the file was saved, or empty if saving the file
  /// failed.
  ///
  /*--cef(optional_param=guid,optional_param=buffer)--*/
  virtual void OnReadDownloadDataDone(const CefString& guid, const CefRefPtr<CefBinaryValue>& buffer) = 0;
};

///
/// This function should be called on the main application thread.
///
/*--cef()--*/
void CefReadDownloaData(const std::string& guid,
                        const int32_t read_size,
                        CefRefPtr<CefReadDownloadDataCallback> callback);
#endif
#endif  // BUILDFLAG(IS_ARKWEB)

#endif  // CEF_INCLUDE_CEF_APP_EXT_H_

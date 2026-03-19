// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on download_target_determiner.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "nweb_download_handler_delegate.h"

#include <string.h>

#include <thread>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_cookie.h"
#include "cef/include/cef_download_handler.h"
#include "cef/include/cef_parser.h"
#include "cef/include/wrapper/cef_closure_task.h"
#include "cef/include/wrapper/cef_helpers.h"
#include "chrome/grit/generated_resources.h"
#include "content/public/common/content_constants.h"
#include "net/base/filename_util.h"
#include "net/http/http_content_disposition.h"
#include "nweb_impl.h"
#include "ohos_nweb/src/capi/nweb_download_delegate_callback.h"
#include "ui/base/l10n/l10n_util.h"
#include "url/gurl.h"

#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "components/safe_browsing/content/common/file_type_policies.h"
#endif

namespace OHOS::NWeb {

namespace {

const int kDataSchemePrefixLen = 5;

const std::string GetContentDisposition(
    CefRefPtr<CefDownloadItem> download_item, std::string default_charset) {
    std::string origin_content_disposition =
        download_item->AsArkDownloadItem()->GetOriginContentDisposition()->GetStdString();
    if (origin_content_disposition.empty()) {
      // The original content-disposition is empty.
      // The content-disposition in UTF-8 format is used.
      return download_item->GetContentDisposition().ToString();
    }

    net::HttpContentDisposition header(origin_content_disposition,
                                       default_charset);
    std::string filename = header.filename();
    if (filename.empty()) {
      // If the original content-disposition fails to be decoded,
      // the content-disposition in UTF-8 format is used for decoding.
      return download_item->GetContentDisposition().ToString();
    }

    return origin_content_disposition;
}

std::string DesensitizeStr(const std::string& str) {
    if (str.length() <= 2) {
        return "**";
    }
    return str.substr(0, 2) + "**";
}

static bool IsDataScheme(const CefString& url) {
  if (url.length() < kDataSchemePrefixLen) {
    return false;
  }
  CefString prefix(url.c_str(), kDataSchemePrefixLen, true);
  CefString dataScheme("data:");
  return prefix == dataScheme;
}

}

NWebDownloadHandlerDelegate::NWebDownloadHandlerDelegate(
    std::shared_ptr<NWebPreferenceDelegate> preference_delegate)
    : preference_delegate_(preference_delegate) {}

void NWebDownloadHandlerDelegate::RegisterWebDownloadDelegateListener(
    std::shared_ptr<NWebDownloadDelegateCallback> download_delegate_listener) {
  if (web_download_delegate_listener_ == nullptr) {
    LOG(INFO) << "RegisterWebDownloadDelegateListener";
    web_download_delegate_listener_ = download_delegate_listener;
  }
}

/* CefDownloadHandler methods begin */
NO_SANITIZE("cfi-icall")
bool NWebDownloadHandlerDelegate::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    CefRefPtr<CefBeforeDownloadCallback> callback) {
  (void)suggested_name;
  if (!download_item->IsValid()) {
    LOG(ERROR)
        << "NWebDownloadHandlerDelegate::OnBeforeDownload error, not invalid";
    return false;
  }
  if (download_item->IsCanceled()) {
    LOG(ERROR) << "NWebDownloadHandlerDelegate::OnBeforeDownload error, "
                  "download cancel";
    return false;
  }
  if (web_download_delegate_listener_ != nullptr) {
    NWebBeforeDownloadCallbackWrapper* web_before_download_callback_wrapper =
        new NWebBeforeDownloadCallbackWrapper();
    web_before_download_callback_wrapper->callback_ = std::move(callback);

    // information about download_item
    struct NWebDownloadItem* item = CreateNWebDownloadItem(download_item);
    web_download_delegate_listener_->downloadBeforeStart(
        item, web_before_download_callback_wrapper);
  } else {
    LOG(ERROR) << "web_download_delegate_listener is null";
  }
  return true;
}

NO_SANITIZE("cfi-icall")
void NWebDownloadHandlerDelegate::OnDownloadUpdated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    CefRefPtr<CefDownloadItemCallback> callback) {
  LOG(INFO) << "NWebDownloadHandlerDelegate::OnDownloadUpdated";
  NWebDownloadItemCallbackWrapper* web_download_item_callback_wrapper =
      new NWebDownloadItemCallbackWrapper();
  web_download_item_callback_wrapper->callback_ = std::move(callback);
  struct NWebDownloadItem* item = CreateNWebDownloadItem(download_item);
#if defined(REPORT_SYS_EVENT)
  if (item != nullptr) {
    LOG(DEBUG) << "NWebDownloadHandlerDelegate::OnDownloadUpdated code=" << item->last_error_code;
    if (item->state == NWebDownloadItemState::INTERRUPTED ||
        item->state == NWebDownloadItemState::CANCELED) {
      ReportPageDownLoadErrorInfo(item->download_item_id, item->last_error_code);
    } else if (item->state == NWebDownloadItemState::COMPLETE) {
      ReportPageDownLoadErrorInfo(item->download_item_id, 0);
    }
  }
#endif
  web_download_delegate_listener_->downloadDidUpdate(
      item, web_download_item_callback_wrapper);
}
/* CefDownloadHandler methods end */

NWebDownloadItem* NWebDownloadHandlerDelegate::CreateNWebDownloadItem(
    CefRefPtr<CefDownloadItem> download_item) {
  struct NWebDownloadItem* item = new NWebDownloadItem(download_item);
  // generate suggested filename
  item->suggested_file_name =
      strdup(GenerateSuggestedFilename(download_item).c_str());
  LOG(INFO) << "web_download_item params, nweb_id:" << item->nweb_id
            << ",id:" << item->download_item_id << ",guid:" << item->guid
            << ",suggested:" << DesensitizeStr(item->suggested_file_name)
            << ",current_speed:" << item->current_speed
            << ",percent_complete:" << item->percent_complete
            << ",total_bytes:" << item->total_bytes
            << ",received_bytes:" << item->received_bytes
            << ",full_path:" << DesensitizeStr(item->full_path) << ",state:" << item->state
            << ",received_slices:" << item->received_slices
            << ",last_modified:" << item->last_modified
            << ",etag:" << item->etag;
  return item;
}

std::string NWebDownloadHandlerDelegate::GenerateSuggestedFilename(
    CefRefPtr<CefDownloadItem> download_item) const {
  std::string suggested_filename =
      download_item->GetSuggestedFileName().ToString();
  std::string sniffed_mime_type = download_item->GetMimeType().ToString();

  if (suggested_filename.empty() &&
      sniffed_mime_type == "application/x-x509-user-cert") {
    suggested_filename = "user.crt";
  }

  // Generate the file name, we may replace the file extension based on mime
  // type under certain condition.
  std::string default_filename(
      l10n_util::GetStringUTF8(IDS_DEFAULT_DOWNLOAD_FILENAME));
  std::shared_ptr<NWeb> nweb =
      NWebImpl::GetNWeb(download_item->AsArkDownloadItem()->GetNWebId());
  std::string default_charset =
      preference_delegate_
          ? preference_delegate_->DefaultTextEncodingFormat()
          : ((nweb && nweb->GetPreference()) ? nweb->GetPreference()->DefaultTextEncodingFormat()
                  : "utf-8");
  std::string content_disposition = GetContentDisposition(download_item,
                                                          default_charset);
  GURL gurl = GURL();
  CefString url = download_item->GetURL();
  if (!(IsDataScheme(url) && url.length() > content::kMaxLengthOfDataURLStringPub)) {
    gurl = GURL(url.ToString());
  }

  base::FilePath generated_filename = net::GenerateFileName(
      gurl, content_disposition, default_charset,
      suggested_filename, sniffed_mime_type, default_filename);
  LOG(INFO) << "GenerateSuggestedFilename mime_type: " << sniffed_mime_type
            << ", default_charset: " << default_charset
            << ", content-disposition: " << content_disposition
            << ", generated_filename: " << DesensitizeStr(generated_filename.AsUTF8Unsafe());

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  // We don't replace the file extension if sfafe browsing consider the file
  // extension to be unsafe. Just let safe browsing scan the generated file.
  if (safe_browsing::FileTypePolicies::GetInstance()->IsCheckedBinaryFile(
          generated_filename)) {
    return generated_filename.AsUTF8Unsafe();
  }
#endif

  // If no mime type or explicitly specified a name, don't replace file
  // extension.
  if (sniffed_mime_type.empty() || !suggested_filename.empty()) {
    return generated_filename.AsUTF8Unsafe();
  }

  // Trust content disposition header filename attribute.
  net::HttpContentDisposition content_disposition_header(
      download_item->GetContentDisposition(), default_charset);
  if (!content_disposition_header.filename().empty()) {
    return generated_filename.AsUTF8Unsafe();
  }

  // When headers have X-Content-Type-Options:nosniff, or for many text file
  // types like csv, sniffed mime type will be text/plain. Prefer the extension
  // generated by the URL here.
  if (sniffed_mime_type == "text/plain" &&
      download_item->AsArkDownloadItem()->GetOriginalMimeType().ToString() !=
          "text/plain") {
    return generated_filename.AsUTF8Unsafe();
  }

  // Replaces file extension based on sniffed mime type in network layer.
  generated_filename = net::GenerateFileName(
      gurl, std::string() /* content_disposition */, default_charset,
      std::string() /* suggested_filename */, sniffed_mime_type,
      default_filename, true /* should_replace_extension */);
  return generated_filename.AsUTF8Unsafe();
}

}  // namespace OHOS::NWeb

// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <memory>

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/run_loop.h"
#include "base/task/current_thread.h"
#include "base/threading/thread_restrictions.h"
#include "cef/libcef/browser/browser_info_manager.h"
#include "cef/libcef/browser/context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/browser/trace_subscriber.h"
#include "cef/libcef/common/cef_switches.h"
#include "components/download/public/common/download_item.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
// #include "content/public/browser/notification_service.h"
// #include "content/public/browser/notification_types.h"
#include "arkweb/build/features/features.h"
#include "ui/base/ui_base_switches.h"

#if BUILDFLAG(ARKWEB_HTTP_DNS)
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "content/public/browser/network_service_instance.h"
#include "services/network/public/mojom/network_service.mojom.h"
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "cef/libcef/browser/download_item_impl.h"
#include "cef/libcef/browser/download_manager_delegate.h"
#include "cef/libcef/browser/download_manager_delegate_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_download_resume_util.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_received_slice_helper_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/cef_download_item_impl_ext.h"
#include "components/download/public/common/download_utils.h"
#endif

void CefApplyHttpDns() {
#if BUILDFLAG(ARKWEB_HTTP_DNS)
  if (!net_service::NetHelpers::HasValidDnsOverHttpConfig()) {
    LOG(WARNING) << __func__ << " User input mal mode:"
                 << net_service::NetHelpers::doh_mode;
    return;
  }

  network::mojom::NetworkService* network_service =
      content::GetNetworkService();
  if (network_service) {
    auto config = net::DnsOverHttpsServerConfig::FromString(
        net_service::NetHelpers::DnsOverHttpServerConfig());
    if (config.has_value()) {
      network_service->ConfigureStubHostResolver(
          true, net_service::NetHelpers::DnsOverHttpMode(),
          net::DnsOverHttpsConfig({{std::move(*config)}}), true);
    } else {
      LOG(INFO) << __func__ << "server config is invalid";
    }
  } else {
    LOG(INFO) << __func__
              << "will apply doh config after network service created";
  }
#endif  // BUILDFLAG(ARKWEB_HTTP_DNS)
}

void CefSetDownloadHandler(CefRefPtr<CefDownloadHandler> download_handler) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  LOG(INFO) << "set global download_handler";
  net_service::NetHelpers::SetDownloadHandler(std::move(download_handler));
#endif  //  ARKWEB_EX_DOWNLOAD
}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
CefRefPtr<CefDownloadItem> CefGetDownloadItem(const std::string& guid) {
  LOG(DEBUG) << "get download item for " << guid;
  for (auto& context : CefBrowserContext::GetAll()) {
    content::DownloadManager* manager =
        context->AsBrowserContext()->GetDownloadManager();
    if (!manager) {
      continue;
    }
    download::DownloadItem* item = manager->GetDownloadByGuid(guid);
    if (item) {
      CefRefPtr<CefDownloadItemImplExt> download_item(
          new CefDownloadItemImplExt(item));
      return download_item;
    }
  }
  return nullptr;
}
#endif

void CefResumeDownload(const CefString& guid,
                       const CefString& url,
                       const CefString& full_path,
                       int64_t received_bytes,
                       int64_t total_bytes,
                       const CefString& etag,
                       const CefString& mime_type,
                       const CefString& last_modified,
                       const CefString& received_slices_string) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() > 0) {
    //  use first browser context to resume
    CefBrowserContext* context = browser_context_all[0];
    content::BrowserContext* browser_context = context->AsBrowserContext();
    content::DownloadManager* manager = browser_context->GetDownloadManager();
    if (!manager) {
      LOG(ERROR) << "download manager not exists, resume download failed";
      return;
    }
    GURL gurl = GURL(url.ToString());
    if (gurl.is_empty() || !gurl.is_valid()) {
      LOG(ERROR) << "download url is not valid, resume download failed, url:"
                 << url.ToString();
      return;
    }
    base::FilePath file_full_path =
        base::FilePath::FromUTF8Unsafe(full_path.ToString());

    std::vector<download::DownloadItem::ReceivedSlice> received_slices =
        arkweb_received_slice_helper_ext::FromString(
            received_slices_string.ToString());
    manager->GetNextId(base::BindOnce(
        &download_resume_util::ResumeDownloadWithId, manager, guid.ToString(),
        std::move(gurl), std::move(file_full_path), received_bytes, total_bytes,
        etag.ToString(), mime_type.ToString(), last_modified.ToString(),
        received_slices));
  } else {
    LOG(ERROR) << "browser contexts is empty, resume download failed";
  }
#endif  //  BUILDFLAG(ARKWEB_EX_DOWNLOAD)
}

void CefSetFileRenameOption(const int file_rename_option) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  SetFileRenameOptions((download::FileRenameOptions)file_rename_option);
#endif  //  BUILDFLAG(ARKWEB_EX_DOWNLOAD)
}

void CefOnReadDownloadDataDoneOnUIThread(const std::string& guid,
                                         CefRefPtr<CefReadDownloadDataCallback> callback,
                                         const std::vector<uint8_t>& buffer) {
  if (buffer.size() == 0) {
    LOG(INFO) << "OnReadDownloadDataDone: buffer is nullptr.";
    callback->OnReadDownloadDataDone(guid, nullptr);
    return;
  }
  LOG(INFO) << "OnReadDownloadDataDone: buffer size: " << buffer.size();
  CefRefPtr<CefBinaryValue> cefBuffer =
          CefBinaryValue::Create(buffer.data(), buffer.size());
  callback->OnReadDownloadDataDone(guid, cefBuffer);
}

void CefOnReadDownloadDataDone(const std::string& guid,
                               CefRefPtr<CefReadDownloadDataCallback> callback,
                               const std::vector<uint8_t>& buffer) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, base::BindOnce(CefOnReadDownloadDataDoneOnUIThread, guid, callback, buffer));
    return;
  }
  CefOnReadDownloadDataDoneOnUIThread(guid, callback, buffer);
}

void CefReadDownloaData(const std::string& guid,
                        const int32_t read_size,
                        CefRefPtr<CefReadDownloadDataCallback> callback) {
  LOG(INFO) << "CefReadDownloaData start guid: " << guid
            << ",read_size: " << read_size;
  for (auto& context: CefBrowserContext::GetAll()) {
    content::DownloadManager* manager = context->AsBrowserContext()->GetDownloadManager();
    if (!manager) {
      continue;
    }
    download::DownloadItem* item = manager->GetDownloadByGuid(guid);
    if (item) {
      LOG(INFO) << "CefReadDownloaData item exist";
      item->ReadDownloadData(guid, read_size,
                           base::BindOnce(&CefOnReadDownloadDataDone, guid, callback));
      return;
    }
  }
  callback->OnReadDownloadDataDone(guid, nullptr);
}

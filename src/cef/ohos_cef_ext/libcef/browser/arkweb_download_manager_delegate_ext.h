// Copyright 2024 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_EXT_H_
#define CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_EXT_H_
#pragma once

#include <memory>
#include <set>

#include "arkweb/build/features/features.h"
#include "base/memory/weak_ptr.h"
#include "cef/libcef/browser/download_manager_delegate.h"
#include "cef/libcef/browser/download_manager_delegate_impl.h"
#include "components/download/public/common/download_item.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/download_manager_delegate.h"
#include "libcef/browser/browser_host_base.h"
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "base/compiler_specific.h"
#include "cef/include/cef_download_handler.h"
#endif  //  ARKWEB_EX_DOWNLOAD

class ArkWebCefDownloadManagerDelegateExt
    : public CefDownloadManagerDelegateImpl {
 public:
  explicit ArkWebCefDownloadManagerDelegateExt(
      content::DownloadManager* manager);

  ArkWebCefDownloadManagerDelegateExt(
      const ArkWebCefDownloadManagerDelegateExt&) = delete;
  ArkWebCefDownloadManagerDelegateExt& operator=(
      const ArkWebCefDownloadManagerDelegateExt&) = delete;

 private:
  void OnDownloadUpdated(download::DownloadItem* item) override;
  // DownloadManager::Observer methods.
  void OnDownloadCreated(content::DownloadManager* manager,
                         download::DownloadItem* item) override;
  bool DetermineDownloadTarget(
      download::DownloadItem* item,
      download::DownloadTargetCallback* callback) override;
  CefRefPtr<CefBrowserHostBase> GetOrAssociateBrowser(
      download::DownloadItem* item);
};

#endif  // CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_EXT_H_

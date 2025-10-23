// Copyright 2024 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "cef/libcef/browser/download_manager_delegate.h"

#include "cef/libcef/browser/download_manager_delegate_impl.h"

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_download_manager_delegate_ext.h"
#endif

namespace cef {

// static
std::unique_ptr<cef::DownloadManagerDelegate> DownloadManagerDelegate::Create(
    content::DownloadManager* download_manager) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  return std::make_unique<ArkWebCefDownloadManagerDelegateExt>(
      download_manager);
#else
  return std::make_unique<CefDownloadManagerDelegateImpl>(
      download_manager, /*alloy_bootstrap=*/false);
#endif
}

}  // namespace cef

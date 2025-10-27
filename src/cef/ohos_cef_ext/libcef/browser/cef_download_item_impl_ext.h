#ifndef CEF_LIBCEF_BROWSER_DOWNLOAD_ITEM_IMPL_EXT_H_
#define CEF_LIBCEF_BROWSER_DOWNLOAD_ITEM_IMPL_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"
#include "cef/libcef/browser/download_item_impl.h"
#include "cef/libcef/common/value_base.h"
#include "cef/ohos_cef_ext/include/cef_download_item_ext.h"

// CefDownloadItem implementation
class CefDownloadItemImplExt : public CefDownloadItemExt,
                               public CefDownloadItemImpl {
 public:
  explicit CefDownloadItemImplExt(download::DownloadItem* value);
  explicit CefDownloadItemImplExt(download::DownloadItem* value, int nweb_id);
  CefString GetOriginalMimeType() override;
  CefString GetGuid() override;

  // additional methods
  int GetState() override;
  bool IsPaused() override;
  CefString GetMethod() override;
  int GetLastErrorCode() override;
  bool IsPending() override;
  CefString GetLastModifiedTime() override;
  CefString GetETag() override;
  CefString GetReceivedSlices() override;
  int GetNWebId() override;
  CefRefPtr<CefDownloadItemExt> AsArkDownloadItem() override;
  CefRefPtr<CefValue> GetOriginContentDisposition() override;
};

#endif  // CEF_LIBCEF_BROWSER_DOWNLOAD_ITEM_IMPL_H_

#include "cef/ohos_cef_ext/libcef/browser/cef_download_item_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "cef/libcef/common/time_util.h"
#include "components/download/public/common/download_item.h"
#include "url/gurl.h"
#undef OHOS_EX_DOWNLOAD

#include "components/download/public/common/download_item_impl.h"

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_received_slice_helper_ext.h"
const char kNWebId[] = "nweb_id";
const char kRequestMethod[] = "request_method";
#endif

CefDownloadItemImplExt::CefDownloadItemImplExt(download::DownloadItem* value)
    : CefDownloadItemImpl(value) {}

CefDownloadItemImplExt::CefDownloadItemImplExt(download::DownloadItem* value,
                                               int nweb_id)
    : CefDownloadItemImpl(value) {
  // Indicate that this object owns the controller.
  SetOwnsController();
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  download::DownloadItemImpl* item_impl =
      static_cast<download::DownloadItemImpl*>(mutable_value());
  item_impl->SetUserData(
      kNWebId,
      std::make_unique<download::ArkWebDownloadItemImplExt::NWebIdData>(
          nweb_id));
#endif  //  ARKWEB_EX_DOWNLOAD
}

CefString CefDownloadItemImplExt::GetOriginalMimeType() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
  return const_value().GetOriginalMimeType();
}

CefString CefDownloadItemImplExt::GetGuid() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
  return const_value().GetGuid();
}

// additional methods
int CefDownloadItemImplExt::GetState() {
  CEF_VALUE_VERIFY_RETURN(false, -1);
  int state = static_cast<int>(const_value().GetState());
  return state;
}

bool CefDownloadItemImplExt::IsPaused() {
  CEF_VALUE_VERIFY_RETURN(false, false);
  bool paused = const_value().IsPaused();
  return paused;
}

CefString CefDownloadItemImplExt::GetMethod() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  const download::ArkWebDownloadItemImplExt& item_impl =
      static_cast<const download::ArkWebDownloadItemImplExt&>(const_value());
  std::string request_method = item_impl.GetRequestMethod();
  return request_method;
#else
  return CefString();
#endif  //  ARKWEB_EX_DOWNLOAD
}

int CefDownloadItemImplExt::GetLastErrorCode() {
  CEF_VALUE_VERIFY_RETURN(false, -1);
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  download::DownloadInterruptReason reason = const_value().GetLastReason();
  return static_cast<int>(reason);
#else
  return -1;
#endif  //  ARKWEB_EX_DOWNLOAD
}

bool CefDownloadItemImplExt::IsPending() {
  CEF_VALUE_VERIFY_RETURN(false, false);
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  const download::ArkWebDownloadItemImplExt& item_impl =
      static_cast<const download::ArkWebDownloadItemImplExt&>(const_value());
  bool pending = item_impl.IsBeforeInProgress();
  return pending;
#else
  return false;
#endif  //  ARKWEB_EX_DOWNLOAD
}

CefString CefDownloadItemImplExt::GetLastModifiedTime() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
  return const_value().GetLastModifiedTime();
}

CefString CefDownloadItemImplExt::GetETag() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
  return const_value().GetETag();
}

CefString CefDownloadItemImplExt::GetReceivedSlices() {
  CEF_VALUE_VERIFY_RETURN(false, CefString());
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  const download::DownloadItemImpl& item_impl =
      static_cast<const download::DownloadItemImpl&>(const_value());
  return arkweb_received_slice_helper_ext::SerializeToString(
      item_impl.GetReceivedSlices());
#else
  return CefString();
#endif  //  ARKWEB_EX_DOWNLOAD
}

int CefDownloadItemImplExt::GetNWebId() {
  CEF_VALUE_VERIFY_RETURN(false, -1);
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  const download::DownloadItemImpl& item_impl =
      static_cast<const download::DownloadItemImpl&>(const_value());
  void* data_raw_ptr = item_impl.GetUserData(kNWebId);
  if (data_raw_ptr) {
    download::ArkWebDownloadItemImplExt::NWebIdData* nweb_id_data_ptr =
        (download::ArkWebDownloadItemImplExt::NWebIdData*)data_raw_ptr;
    if (nweb_id_data_ptr) {
      return nweb_id_data_ptr->nweb_id_;
    }
  }
  return -1;
#else
  return -1;
#endif  //  ARKWEB_EX_DOWNLOAD
}

CefRefPtr<CefValue> CefDownloadItemImplExt::GetOriginContentDisposition() {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  CEF_VALUE_VERIFY_RETURN(false, nullptr);
  CefRefPtr<CefValue> data = CefValue::Create();
  data->SetStdString(const_value().GetContentDisposition());
  return data;
#else
  return CefValue::Create();
#endif  //  ARKWEB_EX_DOWNLOAD
}

CefRefPtr<CefDownloadItemExt> CefDownloadItemImplExt::AsArkDownloadItem() {
  return this;
}

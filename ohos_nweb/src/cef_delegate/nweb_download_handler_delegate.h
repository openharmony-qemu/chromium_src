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

#ifndef NWEB_DOWNLOAD_HANDLER_DELEGATE_H
#define NWEB_DOWNLOAD_HANDLER_DELEGATE_H

#include "cef/include/base/cef_lock.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_dialog_handler.h"
#include "cef/include/cef_jsdialog_handler.h"
#include "cef/include/cef_media_handler.h"
#include "cef/include/cef_permission_request.h"
#include "cef/include/cef_resource_request_handler.h"

#include "nweb_event_handler.h"
#include "nweb_handler.h"
#include "nweb_preference_delegate.h"
#include "nweb_render_handler.h"

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "nweb_download_callback.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_value.h"

namespace OHOS::NWeb {

class NWebDownloadHandlerDelegate : public CefDownloadHandler {
 public:
  void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback> downloadDelegateListener);

  /* CefDownloadHandler methods begin */
  void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefDownloadItem> download_item,
                        const CefString& suggested_name,
                        CefRefPtr<CefBeforeDownloadCallback> callback) override;

  void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefDownloadItem> download_item,
                         CefRefPtr<CefDownloadItemCallback> callback) override;
  /* CefDownloadHandler methods end */
  NWebDownloadHandlerDelegate(
      std::shared_ptr<NWebPreferenceDelegate> preference_delegate);
  ~NWebDownloadHandlerDelegate() = default;

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebDownloadHandlerDelegate);

  NWebDownloadItem* CreateNWebDownloadItem(
      CefRefPtr<CefDownloadItem> download_item);

  std::string GenerateSuggestedFilename(
      CefRefPtr<CefDownloadItem> download_item) const;

  std::shared_ptr<NWebDownloadDelegateCallback>
      web_download_delegate_listener_ = nullptr;

  std::shared_ptr<NWebPreferenceDelegate> preference_delegate_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif
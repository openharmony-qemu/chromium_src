// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_SERVICE_ARK_WEB_INTERCEPTED_REQUEST_HANDLER_WRAPPER_HELPER_H_
#define CEF_LIBCEF_BROWSER_NET_SERVICE_ARK_WEB_INTERCEPTED_REQUEST_HANDLER_WRAPPER_HELPER_H_

#include "base/functional/callback_forward.h"
#include "base/task/sequenced_task_runner.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_resource_handler.h"
#include "cef/include/cef_resource_request_handler.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "cef/ohos_cef_ext/include/arkweb_resource_request_handler_ext.h"
#include "cef/ohos_cef_ext/libcef/common/arkweb_request_impl_ext.h"
#include "ui/base/page_transition_types.h"

constexpr char POST_CACHE_KEY[] = "ArkWebPostCacheKey";

namespace net_service {

class OhosInterceptCallbackWrapper : public CefInterceptCallback {
 public:
  using ResourceCallback =
      base::OnceCallback<void(CefRefPtr<CefResourceHandler>)>;
  explicit OhosInterceptCallbackWrapper(ResourceCallback callback);

  OhosInterceptCallbackWrapper(const OhosInterceptCallbackWrapper&) = delete;
  OhosInterceptCallbackWrapper& operator=(const OhosInterceptCallbackWrapper&) =
      delete;

  ~OhosInterceptCallbackWrapper() override;

  void ContinueLoad(CefRefPtr<CefResourceHandler> resource_handler) override;

 private:
  ResourceCallback callback_;

  scoped_refptr<base::SequencedTaskRunner> work_thread_task_runner_;

  IMPLEMENT_REFCOUNTING(OhosInterceptCallbackWrapper);
};

class ArkWebInterceptedRequestHandlerWrapperHelper {
 public:
  ArkWebInterceptedRequestHandlerWrapperHelper() = default;
  ~ArkWebInterceptedRequestHandlerWrapperHelper() = default;
  bool ProceedAllowCookieLoad(CefRefPtr<CefBrowserHostBase> browser,
                              network::ResourceRequest* request,
                              bool* allow);
  void OnRequestError(CefRefPtr<CefBrowserHostBase> browser,
                      CefRefPtr<CefFrame> frame,
                      int32_t request_id,
                      const network::ResourceRequest& request,
                      int error_code,
                      bool safebrowsing_hit);
  void OnHttpError(CefRefPtr<CefBrowserHostBase> browser,
                   CefRefPtr<CefRequest> request,
                   bool is_main_frame,
                   bool has_user_gesture,
                   CefRefPtr<CefResponse> error_response);
  void GetSettingOfNetHelper(const GURL& url,
                             CefRefPtr<CefBrowserHostBase> browser,
                             struct NetHelperSetting& setting);

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::string OnRewriteUrlForNavigation(
    CefRefPtr<CefBrowserHostBase> browser,
    const std::string& original_url,
    const std::string& referrer);
#endif

 private:
  bool IsIntelligentTrackingPreventionEnabled(
      CefRefPtr<CefBrowserHostBase> browser);
  void ReportITPResult(CefRefPtr<CefBrowserHostBase> browser,
                       const network::ResourceRequest& request);
  GURL GetWebContentsLastCommittedURL(CefRefPtr<CefBrowserHostBase> browser);
};

#if BUILDFLAG(ARKWEB_ITP)
void ReportITPResultInUiTask(CefRefPtr<CefBrowserHostBase> browser,
                             CefString tracker_host,
                             CefString website_host);
#endif  // BUILDFLAG(ARKWEB_ITP)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void OnRequestErrorInUiTask(CefRefPtr<CefBrowserHostBase> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<ArkWebRequestImplExt> request,
                            bool has_user_gesture,
                            int32_t error_code,
                            cef_transition_type_t transition_type);
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

}  // namespace net_service

#endif  // CEF_LIBCEF_BROWSER_NET_SERVICE_ARK_WEB_INTERCEPTED_REQUEST_HANDLER_WRAPPER_HELPER_H_

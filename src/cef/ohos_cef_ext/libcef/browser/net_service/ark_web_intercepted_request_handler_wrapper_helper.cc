// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/net_service/ark_web_intercepted_request_handler_wrapper_helper.h"

#include "base/logging.h"

#if BUILDFLAG(ARKWEB_ITP)
#include "cef/ohos_cef_ext/libcef/browser/anti_tracking/third_party_cookie_access_policy.h"
#endif  // BUILDFLAG(ARKWEB_ITP)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_host_ext.h"
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

namespace net_service {

OhosInterceptCallbackWrapper::OhosInterceptCallbackWrapper(
    ResourceCallback callback)
    : callback_(std::move(callback)),
      work_thread_task_runner_(base::SequencedTaskRunner::GetCurrentDefault()) {
}

OhosInterceptCallbackWrapper::~OhosInterceptCallbackWrapper() {
  if (!callback_.is_null()) {
    // Make sure it executes on the correct thread.
    work_thread_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(std::move(callback_), nullptr));
  }
}

void OhosInterceptCallbackWrapper::ContinueLoad(
    CefRefPtr<CefResourceHandler> resource_handler) {
  if (!work_thread_task_runner_->RunsTasksInCurrentSequence()) {
    work_thread_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OhosInterceptCallbackWrapper::ContinueLoad,
                                  this, resource_handler));
    return;
  }
  if (!callback_.is_null()) {
    std::move(callback_).Run(resource_handler);
  }
}

bool ArkWebInterceptedRequestHandlerWrapperHelper::ProceedAllowCookieLoad(
    CefRefPtr<CefBrowserHostBase> browser,
    network::ResourceRequest* request,
    bool* allow) {
#if BUILDFLAG(ARKWEB_ITP)
  if (*allow == true) {
    bool itp_cookies_enabled = IsIntelligentTrackingPreventionEnabled(browser);
    if (itp_cookies_enabled && request) {
      bool third_party_cookie_access_policy =
          ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()
              ->AllowGetCookies(*request,
                                request->site_for_cookies.RepresentativeUrl());
      if (!third_party_cookie_access_policy) {
        ReportITPResult(browser, *request);
        *allow = false;
        return true;
      }
    }
  }
  return false;
#endif  // BUILDFLAG(ARKWEB_ITP)
}

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void ArkWebInterceptedRequestHandlerWrapperHelper::OnRequestError(
    CefRefPtr<CefBrowserHostBase> browser,
    CefRefPtr<CefFrame> frame,
    int32_t request_id,
    const network::ResourceRequest& request,
    int error_code,
    bool safebrowsing_hit) {
  LOG(DEBUG) << "OnRequestError " << error_code;
  if (browser && frame && error_code != net::ERR_ABORTED) {
    CefRefPtr<ArkWebRequestImplExt> arkweb_request = new ArkWebRequestImplExt();
    arkweb_request->SetURL(CefString(request.url.spec()));
    arkweb_request->SetMethod(CefString(request.method));
    arkweb_request->Set(request.headers);
    arkweb_request->SetDestination(request.destination);
    CEF_POST_TASK(CEF_UIT, base::BindOnce(&OnRequestErrorInUiTask, browser,
                                          frame, arkweb_request,
                                          request.has_user_gesture, error_code,
                                          static_cast<cef_transition_type_t>(
                                              request.transition_type)));
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void ArkWebInterceptedRequestHandlerWrapperHelper::OnHttpError(
    CefRefPtr<CefBrowserHostBase> browser,
    CefRefPtr<CefRequest> request,
    bool is_main_frame,
    bool has_user_gesture,
    CefRefPtr<CefResponse> error_response) {
  CEF_REQUIRE_UIT();
  if (!browser || !browser->GetHost()) {
    return;
  }
  CefRefPtr<CefClient> client = browser->GetHost()->GetClient();
  if (!client) {
    return;
  }
  CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
  if (!load_handler) {
    return;
  }
  load_handler->OnHttpError(request, is_main_frame, has_user_gesture,
                            error_response);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
void ArkWebInterceptedRequestHandlerWrapperHelper::GetSettingOfNetHelper(
    const GURL& url,
    CefRefPtr<CefBrowserHostBase> browser,
    struct NetHelperSetting& setting) {
  CEF_REQUIRE_UIT();
  if (!browser) {
    return;
  }
  browser->AsArkWebBrowserHostExtImpl()->GetSettingOfNetHelper(url, setting);
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_ITP)
bool ArkWebInterceptedRequestHandlerWrapperHelper::
    IsIntelligentTrackingPreventionEnabled(
        CefRefPtr<CefBrowserHostBase> browser) {
  if (!browser) {
    return false;
  }
  return browser->AsArkWebBrowserHostExtImpl()
      ->IsIntelligentTrackingPreventionEnabled();
}

void ArkWebInterceptedRequestHandlerWrapperHelper::ReportITPResult(
    CefRefPtr<CefBrowserHostBase> browser,
    const network::ResourceRequest& request) {
  CEF_REQUIRE_IOT();
  if (!browser || !request.request_initiator || !request.url.has_host()) {
    LOG(ERROR) << "ReportITPResult failed for param error";
    return;
  }

  LOG(DEBUG) << "ReportITPResult";
  CEF_POST_TASK(CEF_UIT,
                base::BindOnce(&ReportITPResultInUiTask, browser,
                               CefString(request.url.host()),
                               CefString(request.request_initiator->host())));
}

GURL ArkWebInterceptedRequestHandlerWrapperHelper::
    GetWebContentsLastCommittedURL(CefRefPtr<CefBrowserHostBase> browser) {
  if (!browser) {
    return GURL();
  }
  return browser->AsArkWebBrowserHostExtImpl()->GetLastCommittedURL();
}
#endif  // BUILDFLAG(ARKWEB_ITP)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::string ArkWebInterceptedRequestHandlerWrapperHelper::OnRewriteUrlForNavigation(
    CefRefPtr<CefBrowserHostBase> browser,
    const std::string& original_url,
    const std::string& referrer) {
  if (!browser || !browser->GetHost()) {
    return "";
  }

  CefRefPtr<CefClient> client = browser->GetHost()->GetClient();
  if (!client) {
    return "";
  }

  CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
  if (!load_handler) {
    return "";
  }
  return load_handler->OnRewriteUrlForNavigation(original_url, referrer);
}
#endif

#if BUILDFLAG(ARKWEB_ITP)
void ReportITPResultInUiTask(CefRefPtr<CefBrowserHostBase> browser,
                             CefString tracker_host,
                             CefString website_host) {
  if (!browser || !browser->GetHost()) {
    LOG(ERROR)
        << "ReportITPResultInUiTask for browser or browser_host is nullptr";
    return;
  }
  CefRefPtr<CefClient> client = browser->GetHost()->GetClient();
  if (!client) {
    LOG(ERROR) << "ReportITPResultInUiTask for client is nullptr";
    return;
  }
  CefRefPtr<CefLoadHandler> load_handler = client->GetLoadHandler();
  if (!load_handler) {
    LOG(ERROR) << "ReportITPResultInUiTask for load_handler is nullptr";
    return;
  }
  load_handler->AsArkWebLoadHandlerExt()->OnIntelligentTrackingPreventionResult(
      website_host, tracker_host);
}
#endif  // BUILDFLAG(ARKWEB_ITP)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void OnRequestErrorInUiTask(CefRefPtr<CefBrowserHostBase> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<ArkWebRequestImplExt> request,
                            bool has_user_gesture,
                            int32_t error_code,
                            cef_transition_type_t transition_type) {
  if (!browser || !browser->GetHost() || !browser->browser_info() || !request) {
    LOG(ERROR) << "OnRequestErrorInUiTask parame error";
    return;
  }
  CefRefPtr<CefClient> client = browser->GetHost()->GetClient();
  if (client) {
    CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
    if (!load_handler) {
      LOG(ERROR) << "OnRequestErrorInUiTask get load_handler failed";
      return;
    }
    auto navigation_lock = browser->browser_info()->CreateNavigationLock();
    LOG(DEBUG) << "OnRequestErrorInUiTask IsMainFrame: "
               << request->IsMainFrame();
    if (request->IsMainFrame()) {
      load_handler->OnLoadStart(browser, frame, request->GetURL(),
                                transition_type);
      load_handler->OnLoadStarted(frame, request->GetURL());
    }
    load_handler->OnLoadErrorWithRequest(request, request->IsMainFrame(),
                                         has_user_gesture, error_code,
                                         net::ErrorToShortString(error_code));
    // To use OuterMostMainFrame.
    if (request->IsMainFrame()) {
      load_handler->OnLoadFinished(frame, request->GetURL());
    }
  }
}
#endif

}  // namespace net_service

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

#ifndef OHOS_CEF_EXT_INCLUDE_ARKWEB_LOAD_HANDLER_EXT_H_
#define OHOS_CEF_EXT_INCLUDE_ARKWEB_LOAD_HANDLER_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_callback.h"
#include "include/cef_first_meaningful_paint_details.h"
#include "include/cef_frame.h"
#include "include/cef_largest_contentful_paint_details.h"
#include "include/cef_load_committed_details.h"
#include "include/cef_load_handler.h"
#include "include/cef_response.h"

///
/// Extended from CefLoadHandler
///
class ArkWebLoadHandlerExt : public virtual CefLoadHandler,
                             public virtual CefBaseRefCounted {
 public:
  ///
  /// OnLoadErrorWithRequest
  ///
  virtual void OnLoadErrorWithRequest(CefRefPtr<CefRequest> request,
                                      bool is_main_frame,
                                      bool has_user_gesture,
                                      int error_code,
                                      const CefString& error_text) {}

  ///
  /// OnHttpError
  ///
  virtual void OnHttpError(CefRefPtr<CefRequest> request,
                           bool is_main_frame,
                           bool has_user_gesture,
                           CefRefPtr<CefResponse> response) {}

  ///
  /// OnRefreshAccessedHistory
  ///
  virtual void OnRefreshAccessedHistory(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        const CefString& url,
                                        bool isReload,
                                        bool isMainFrame) {}

  ///
  /// Notify the body that is loading the Http response to make it visible,
  /// old pages are no longer rendered.
  ///
  virtual void OnPageVisible(CefRefPtr<CefBrowser> browser,
                             const CefString& url,
                             bool success) {}

  ///
  /// OnDataResubmission.
  ///
  virtual void OnDataResubmission(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefCallback> callback) {}

  ///
  /// Called when the first content rendering of web page.
  ///
  virtual void OnFirstContentfulPaint(int64_t navigationStartTick,
                                      int64_t firstContentfulPaintMs) {}

  ///
  /// Called when received website security risk check result.
  ///
  virtual void OnSafeBrowsingCheckResult(int threat_type) {}                                      

  ///
  /// Called when the first meaningful paint rendering of web page.
  ///
  virtual void OnFirstMeaningfulPaint(
      CefRefPtr<CefFirstMeaningfulPaintDetails> details) {}

  ///
  /// Called when the largest contentful paint rendering of web page.
  ///
  virtual void OnLargestContentfulPaint(
      CefRefPtr<CefLargestContentfulPaintDetails> details) {}

  ///
  /// Called when the navigation entry has been committed.
  ///
  virtual void OnNavigationEntryCommitted(
      CefRefPtr<CefLoadCommittedDetails> details) {}

  ///
  /// Called when tracker's cookie is prevented.
  ///
  virtual void OnIntelligentTrackingPreventionResult(
      const CefString& website_host,
      const CefString& tracker_host) {}

#if BUILDFLAG(ARKWEB_BFCACHE)
  ///
  /// Called when page load from bfcache.
  ///
  virtual void UpdateFavicon(CefRefPtr<CefBrowser> browser) {}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

  virtual CefRefPtr<ArkWebLoadHandlerExt> AsArkWebLoadHandlerExt() {
    return this;
  }

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  ///
  /// OnLoadStarted.
  ///
  virtual void OnLoadStarted(CefRefPtr<CefFrame> frame,
                             const CefString& url) {}

  ///
  /// OnLoadFinished.
  ///
  virtual void OnLoadFinished(CefRefPtr<CefFrame> frame,
                              const CefString& url) {}
#endif

#if BUILDFLAG(ARKWEB_PDF)
  ///
  /// Handles the pdf scroll at bottom event.
  ///
  /*--cef()--*/
  virtual void OnPdfScrollAtBottom(const std::string& url) {}

  ///
  /// Handles the pdf load state event.
  ///
  /*--cef()--*/
  virtual void OnPdfLoadEvent(int32_t result, const std::string& url) {}
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  virtual std::string OnRewriteUrlForNavigation(
      const std::string& original_url,
      const std::string& referrer) { return ""; }
#endif
};

#endif  // OHOS_CEF_EXT_INCLUDE_ARKWEB_LOAD_HANDLER_EXT_H_

// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ARKWEB_RENDERER_BROWSER_IMPL_H_
#define ARKWEB_RENDERER_BROWSER_IMPL_H_
#pragma once

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "cef/include/cef_browser.h"
#include "cef/include/cef_client.h"
#include "cef/libcef/renderer/browser_impl.h"
#include "cef/libcef/renderer/frame_impl.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/public/web/web_view_observer.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

class CefBrowserImplOrigin;

namespace blink {
class WebFrame;
class WebNode;
class WebView;
}  // namespace blink

// Renderer plumbing for CEF features. There is a one-to-one relationship
// between RenderView on the renderer side and RenderViewHost on the browser
// side.
//
// RenderViewObserver: Interface for observing RenderView notifications.
class ArkWebBrowserExtImpl : virtual public ArkWebBrowserExt,
                             virtual public CefBrowserImpl {
 public:
  ArkWebBrowserExtImpl(blink::WebView* web_view,
                       int browser_id,
                       bool is_popup,
                       bool is_windowless,
                       bool print_preview_enabled)
      : CefBrowserImpl(web_view,
                       browser_id,
                       is_popup,
                       is_windowless,
                       print_preview_enabled) {}

  CefRefPtr<ArkWebBrowserExt> AsArkWebBrowser() override { return this; }

#if BUILDFLAG(IS_OHOS)
  void DidCommitCompositorFrame() override;
  void DidUpdateMainFrameLayout() override;
  uint32_t GetAcceleratedWidget(bool isPopup) override;
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void ExtensionSetTabId(int32_t tab_id) override;
  int32_t ExtensionGetTabId() override;
#endif

#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override;
  void GoBackOrForward(int num_steps) override;
  void DeleteHistory() override;
  bool CanStoreWebArchive() override;
  void ReloadOriginalUrl() override;
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate()
      override;
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void EnableAdsBlock(bool enable) override {}
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override {}
#endif

  // #if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void SetSavePasswordAutomatically(bool enable) override{}
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePassword(bool enable) override {}
  bool GetSavePassword() override { return false; }
  void SaveOrUpdatePassword(bool is_update) override{}
  void PasswordSuggestionSelected(int list_index) override{}
  // #endif
  // #if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void ShowFreeCopyMenu() override{}
  bool ShouldShowFreeCopyMenu() override { return false; }
  // #endif
  int GetNWebId() override { return -1; }
  //#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {
  }
  //#endif  // ARKWEB_NO_STATE_PREFETCH
  // #if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  // #endif

  // #if BUILDFLAG(ARKWEB_EXT_SECURITY_STATE)
  int GetSecurityLevel() override{ return 0; }
  // #endif
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableSafeBrowsing(bool enable) override {}
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)
#if BUILDFLAG(ARKWEB_ITP)
  void EnableIntelligentTrackingPrevention(bool enable) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
#endif
//#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  int SetUrlTrustListWithErrMsg(
    const CefString& urlTrustList, CefString& detailErrMsg) override {
    return 0;
  }
//#endif

  // #if BUILDFLAG(ARKWEB_NWEB_EX)
  // NOTE: Keep the previous line commented, add NWebEx APIs below.
  bool ShouldShowLoadingUI() override;
  // ARKWEB_EXT_TOPCONTROLS
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  // #endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const CefString& url) override;

  int UpdateNavigationEntryUrl(int index, const CefString& url) override;

  void ClearForwardList() override;
#endif

  // #if BUILDFLAG(ARKWEB_BFCACHE)
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
//#endif

 private:
#if BUILDFLAG(IS_OHOS)
  int content_width_ = 0;
  int content_height_ = 0;
  bool needs_contents_size_update_ = true;
  int viewport_width_ = 0;
  int viewport_height_ = 0;
#endif
};

#endif  // ARKWEB_RENDERER_BROWSER_IMPL_H_

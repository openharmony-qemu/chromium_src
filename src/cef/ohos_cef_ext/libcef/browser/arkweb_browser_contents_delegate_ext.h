// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_BROWSER_CONTENTS_DELEGATE_EXT_H_
#define CEF_LIBCEF_BROWSER_BROWSER_CONTENTS_DELEGATE_EXT_H_
#pragma once

#include "base/memory/raw_ptr.h"
#include "build/build_config.h"
#include "cef/include/cef_base.h"
#include "cef/libcef/browser/browser_contents_delegate.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_FAVICON)
#include "ohos_cef_ext/libcef/browser/arkweb_icon_helper_ext.h"
#endif

class ArkWebBrowserContentsDelegateExt : public CefBrowserContentsDelegate {
 public:
  ArkWebBrowserContentsDelegateExt* AsArkWebBrowserContentsDelegateExt()
      override {
    return this;
  }
  ArkWebBrowserContentsDelegateExt(scoped_refptr<CefBrowserInfo> browser_info);

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void ViewportFitChanged(blink::mojom::ViewportFit value) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnRefreshAccessedHistoryEx(CefRefPtr<CefFrame> frame,
                                  const GURL& url,
                                  bool isReload,
                                  bool isMainFrame) override {
    OnRefreshAccessedHistory(frame, url, isReload, isMainFrame);
  }
  void OnRefreshAccessedHistory(CefRefPtr<CefFrame> frame,
                                const GURL& url,
                                bool isReload,
                                bool isMainFrame);

  void NavigationStateChanged(content::WebContents* source,
                              content::InvalidateTypes changed_flags) override;
  void OnLoadStarted(CefRefPtr<CefFrame> frame, const CefString& url);
  void OnLoadFinished(CefRefPtr<CefFrame> frame, const CefString& url);

  static bool IsPrerendering(const CefRefPtr<CefFrameHostImpl> frame);
#endif

#if BUILDFLAG(ARKWEB_WPT)
  void DidStartNavigation(content::NavigationHandle* navigation) override;
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_USERAGENT)
  void DidRedirectNavigation(content::NavigationHandle* navigation) override;
#endif  // BUILDFLAG(ARKWEB_USERAGENT)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void OnActivateContent() override;
#endif

#if BUILDFLAG(ARKWEB_FAVICON)
  void InitIconHelper();
  void DocumentOnLoadCompletedInPrimaryMainFrame() override;
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void OnOldPageNoLongerRendered(const GURL& url, bool success);
#endif
#if BUILDFLAG(ARKWEB_NAVIGATION)
  void NavigationEntryCommitted(
      const content::LoadCommittedDetails& load_details) override;
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  // Shows the repost form confirmation dialog box.
  void ShowRepostFormWarningDialog(content::WebContents* source) override;
#endif  // ARKWEB_NETWORK_BASE

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void RequestPointerLock(content::WebContents* web_contents,
                          bool user_gesture,
                          bool last_unlocked_by_target) override;
  void LostPointerLock() override;
  bool HandleUserKeyEvent(const input::NativeWebKeyboardEvent& event) override;
#endif
private:
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void UnlockPointer();
  // Returns true if the pointer is locked.
  bool IsPointerLocked() const;

  // Returns true if the pointer was locked and no notification should be
  // displayed to the user.
  bool IsPointerLockedSilently() const;
  void SetTabWithExclusiveAccess(content::WebContents* tab);
  enum PointerLockState {
    POINTERLOCK_UNLOCKED,
    // pointer has been locked.
    POINTERLOCK_LOCKED,
    // pointer has been locked silently, with no notification to user.
    POINTERLOCK_LOCKED_SILENTLY
  };

  PointerLockState pointer_lock_state_;

  // Timestamp when the user last successfully escaped from a lock request.
  base::TimeTicks last_user_escape_time_;

  raw_ptr<content::WebContents> tab_with_exclusive_access_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  bool did_synthesize_page_load_ = false;
#endif
};

#endif  // CEF_LIBCEF_BROWSER_BROWSER_CONTENTS_DELEGATE_EXT_H_

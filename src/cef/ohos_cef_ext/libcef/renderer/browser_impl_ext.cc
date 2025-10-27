// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/browser_impl_ext.h"

#include <memory>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "cef/libcef/common/app_manager.h"
#include "cef/libcef/renderer/blink_glue.h"
#include "cef/libcef/renderer/browser_impl.h"
#include "cef/libcef/renderer/render_frame_util.h"
#include "cef/libcef/renderer/render_manager.h"
#include "cef/libcef/renderer/thread_util.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/renderer/render_frame.h"
#include "content/renderer/document_state.h"
#include "content/renderer/navigation_state.h"
#include "libcef/renderer/browser_impl_ext.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_frame_content_dumper.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_security_policy.h"
#include "third_party/blink/public/web/web_view.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

const char* CONTENT_SIZE_MESSAGE = "ContentSize.Message";
// ArkWebBrowserExtImpl static methods.
// -----------------------------------------------------------------------------

bool ArkWebBrowserExtImpl::CanGoBackOrForward(int num_steps) {
  CEF_REQUIRE_RT_RETURN(false);

  return blink_glue::CanGoBackOrForward(GetWebView(), num_steps);
}

void ArkWebBrowserExtImpl::GoBackOrForward(int num_steps) {
  CEF_REQUIRE_RT_RETURN_VOID();

  blink_glue::GoBackOrForward(GetWebView(), num_steps);
}

void ArkWebBrowserExtImpl::DeleteHistory() {
  CEF_REQUIRE_RT_RETURN_VOID();
}

CefRefPtr<CefBrowserPermissionRequestDelegate>
ArkWebBrowserExtImpl::GetPermissionRequestDelegate() {
  return nullptr;
}

CefRefPtr<CefGeolocationAcess>
ArkWebBrowserExtImpl::GetGeolocationPermissions() {
  return nullptr;
}

bool ArkWebBrowserExtImpl::CanStoreWebArchive() {
  // Not reached.
  return false;
}

void ArkWebBrowserExtImpl::ReloadOriginalUrl() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StartReload(
          blink::WebFrameLoadType::kReload);
    }
  }
}

// #if BUILDFLAG(ARKWEB_NWEB_EX)
// NOTE: Keep the previous line commented, add NWebEx APIs below.
bool ArkWebBrowserExtImpl::ShouldShowLoadingUI() {
  CEF_REQUIRE_RT_RETURN(false);
  return false;
}
// #endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
int ArkWebBrowserExtImpl::InsertBackForwardEntry(int index,
                                                 const CefString& url) {
  return static_cast<int>(
      content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
}

int ArkWebBrowserExtImpl::UpdateNavigationEntryUrl(int index,
                                                   const CefString& url) {
  return static_cast<int>(
      content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
}

void ArkWebBrowserExtImpl::ClearForwardList() {}
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void ArkWebBrowserExtImpl::ExtensionSetTabId(int32_t tab_id) {}
int32_t ArkWebBrowserExtImpl::ExtensionGetTabId() {
  return -1;
}
#endif

void ArkWebBrowserExtImpl::DidUpdateMainFrameLayout() {
  needs_contents_size_update_ = true;
}

void ArkWebBrowserExtImpl::DidCommitCompositorFrame() {
  if (!needs_contents_size_update_) {
    return;
  }
  needs_contents_size_update_ = false;

  if (!GetWebView() || !GetWebView()->MainFrame()) {
    return;
  }

  blink::WebFrame* main_frame = GetWebView()->MainFrame();
  blink::WebLocalFrame* web_local_frame = main_frame->ToWebLocalFrame();

  gfx::Size contents_size = main_frame->ToWebLocalFrame()->DocumentSize();

  if (contents_size.IsEmpty()) {
    contents_size = GetWebView()->ContentsPreferredMinimumSize();
  }

  int content_width = contents_size.width();
  int content_height = contents_size.height();

  gfx::Size viewport_size = blink_glue::GetVisualViewportSize(web_local_frame);

  if (content_width != content_width_ || content_height != content_height_ ||
      viewport_size.width() != viewport_width_ ||
      viewport_size.height() != viewport_height_) {
    content_width_ = content_width;
    content_height_ = content_height;
    viewport_width_ = viewport_size.width();
    viewport_height_ = viewport_size.height();
    CefRefPtr<CefProcessMessage> message =
        CefProcessMessage::Create(CONTENT_SIZE_MESSAGE);
    message->GetArgumentList()->SetInt(0, content_width_);
    message->GetArgumentList()->SetInt(1, content_height_);
    message->GetArgumentList()->SetInt(2, viewport_size.width());
    message->GetArgumentList()->SetInt(3, viewport_size.height());
    auto web_frame = CefBrowserImpl::GetMainFrame();
    if (web_frame) {
      web_frame->SendProcessMessage(PID_BROWSER, message);
    }
    LOG(DEBUG) << "Fit content SendProcessMessage Content width: "
               << content_width << ",height: " << content_height
               << ". Viewport width:" << viewport_size.width()
               << ",height:" << viewport_size.height();
  }
}

uint32_t ArkWebBrowserExtImpl::GetAcceleratedWidget(bool isPopup) {
  return 0;
}

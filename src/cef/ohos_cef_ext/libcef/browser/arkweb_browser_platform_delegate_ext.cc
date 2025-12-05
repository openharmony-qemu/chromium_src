// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_platform_delegate_ext.h"

#include "arkweb/build/features/features.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "cef/include/views/cef_window.h"
#include "cef/include/views/cef_window_delegate.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/browser/views/browser_view_impl.h"
#include "cef/libcef/common/cef_switches.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/shell_integration.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"

namespace {

void ExecuteExternalProtocol(const GURL& url) {
  CEF_REQUIRE_BLOCKING();

  // Check that an application is associated with the scheme.
  if (shell_integration::GetApplicationNameForScheme(url).empty()) {
    return;
  }

  CEF_POST_TASK(TID_UI, base::BindOnce(&platform_util::OpenExternal, url));
}

// Default popup window delegate implementation.
class PopupWindowDelegate : public CefWindowDelegate {
 public:
  explicit PopupWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
      : browser_view_(browser_view) {}

  PopupWindowDelegate(const PopupWindowDelegate&) = delete;
  PopupWindowDelegate& operator=(const PopupWindowDelegate&) = delete;

  void OnWindowCreated(CefRefPtr<CefWindow> window) override {
    window->AddChildView(browser_view_);
    window->Show();
    browser_view_->RequestFocus();
  }

  void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
    browser_view_ = nullptr;
  }

  bool CanClose(CefRefPtr<CefWindow> window) override {
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
      return browser->GetHost()->TryCloseBrowser();
    }
    return true;
  }

  cef_runtime_style_t GetWindowRuntimeStyle() override {
    return browser_view_->GetRuntimeStyle();
  }

 private:
  CefRefPtr<CefBrowserView> browser_view_;

  IMPLEMENT_REFCOUNTING(PopupWindowDelegate);
};

}  // namespace

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
void ArkWebCefBrowserPlatformDelegateExt::SendTouchpadFlingEvent(
    const CefMouseEvent& event,
    double vx,
    double vy) {
  NOTIMPLEMENTED();
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void ArkWebCefBrowserPlatformDelegateExt::OnOnlineRenderToForeground() {
  DCHECK(false);
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebCefBrowserPlatformDelegateExt::SendTouchEventList(
    const std::vector<CefTouchEvent>& event_list) {
  DCHECK(false);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void ArkWebCefBrowserPlatformDelegateExt::WasOccluded(bool occluded) {
  DCHECK(false);
}
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void ArkWebCefBrowserPlatformDelegateExt::NotifyScreenInfoChangedV2() {
  DCHECK(false);
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_AI)
void ArkWebCefBrowserPlatformDelegateExt::OnTextSelected(bool flag) {
  NOTIMPLEMENTED();
}

void ArkWebCefBrowserPlatformDelegateExt::OnDestroyImageAnalyzerOverlay() {
  NOTIMPLEMENTED();
}

void ArkWebCefBrowserPlatformDelegateExt::OnFoldStatusChanged(
    uint32_t foldStatus) {
  NOTIMPLEMENTED();
}

float ArkWebCefBrowserPlatformDelegateExt::GetPageScaleFactor() {
  return 1;
}

std::string ArkWebCefBrowserPlatformDelegateExt::GetDataDetectorSelectText() {
  return std::string();
}

void ArkWebCefBrowserPlatformDelegateExt::OnDataDetectorSelectText() {
  NOTIMPLEMENTED();
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void ArkWebCefBrowserPlatformDelegateExt::OnSafeInsetsChange(int left,
                                                             int top,
                                                             int right,
                                                             int bottom) {
  NOTIMPLEMENTED();
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool ArkWebCefBrowserPlatformDelegateExt::WebPageSnapshot(
    const char* id,
    int width,
    int height,
    cef_web_snapshot_callback_t callback) {
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_PRINT)
void ArkWebCefBrowserPlatformDelegateExt::CreateWebPrintDocumentAdapter(
    const CefString& jobName,
    void** webPrintDocumentAdapter) {
  NOTIMPLEMENTED();
}

void ArkWebCefBrowserPlatformDelegateExt::CreateWebPrintDocumentAdapterV2(
    const CefString& jobName,
    void** adapter) {
  NOTIMPLEMENTED();
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
int ArkWebCefBrowserPlatformDelegateExt::GetTopControlsOffset() {
  return 0;
}

int ArkWebCefBrowserPlatformDelegateExt::GetShrinkViewportHeight() {
  return 0;
}
#endif

#if BUILDFLAG(ARKWEB_HTML_SELECT)
void ArkWebCefBrowserPlatformDelegateExt::ShowPopupMenu(
    mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
    const gfx::Rect& bounds,
    double item_font_size,
    int selected_item,
    std::vector<blink::mojom::MenuItemPtr> menu_items,
    bool right_aligned,
    bool allow_multiple_selection) {
  NOTIMPLEMENTED();
}
#endif  // ARKWEB_HTML_SELECT

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
void ArkWebCefBrowserPlatformDelegateExt::OnBeforeUnloadFired(bool proceed) {
  NOTIMPLEMENTED();
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void ArkWebCefBrowserPlatformDelegateExt::OnShareFile(
    const std::string& filePath,
    const std::string& utdTypeId) {
  NOTIMPLEMENTED();
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebCefBrowserPlatformDelegateExt::ScrollFocusedEditableNodeIntoView() {
  LOG(WARNING) << "ArkWebCefBrowserPlatformDelegateExt::"
                  "ScrollFocusedEditableNodeIntoView";
  DCHECK(false);
}

void ArkWebCefBrowserPlatformDelegateExt::ScaleGestureChangeV2(
    int type,
    float scale,
    float originScale,
    float centerX,
    float centerY) {
  DCHECK(false);
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ArkWebCefBrowserPlatformDelegateExt::OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report) {
  NOTIMPLEMENTED();
}

bool ArkWebCefBrowserPlatformDelegateExt::TrigAdBlockEnabledForSiteFromUi(
    const std::string& main_frame_url,
    int main_frame_tree_node_id) {
  return false;
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_DATALIST)
void ArkWebCefBrowserPlatformDelegateExt::OnShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type) {
  NOTIMPLEMENTED();
}

void ArkWebCefBrowserPlatformDelegateExt::OnHideAutofillPopup() {
  NOTIMPLEMENTED();
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
CefRefPtr<CefDragData> ArkWebCefBrowserPlatformDelegateExt::GetDropData() {
  return nullptr;
}
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
bool ArkWebCefBrowserPlatformDelegateExt::OnStartBackgroundTask(
    int32_t type,
    const std::string& message) {
  return true;
}
#endif // ARKWEB_PERFORMANCE_PERSISTENT_TASK

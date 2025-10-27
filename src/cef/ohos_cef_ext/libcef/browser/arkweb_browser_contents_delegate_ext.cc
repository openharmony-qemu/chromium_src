// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_contents_delegate_ext.h"

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "cef/libcef/browser/browser_event_util.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/libcef/browser/browser_platform_delegate.h"
#include "cef/libcef/browser/native/cursor_util.h"
#include "cef/libcef/common/frame_util.h"
#include "chrome/browser/ui/views/sad_tab_view.h"
#include "chrome/common/chrome_result_codes.h"
#include "components/input/native_web_keyboard_event.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/focused_node_details.h"
#include "content/public/browser/keyboard_event_processing_result.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_observer.h"
#include "content/public/browser/render_widget_host_view.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/blink/public/mojom/favicon/favicon_url.mojom.h"
#include "third_party/blink/public/mojom/input/focus_type.mojom.h"
#include "third_party/blink/public/mojom/page/draggable_region.mojom.h"
#include "third_party/blink/public/mojom/widget/platform_widget.mojom-test-utils.h"

#if defined(OS_WIN)
#include "sandbox/win/src/sandbox_types.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/include/arkweb_load_handler_ext.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "cef/ohos_cef_ext/libcef/browser/useragent/arkweb_useragent_utils.h"
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "content/public/browser/navigation_details.h"
#include "ohos_cef_ext/libcef/browser/load_committed_details_impl.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "base/command_line.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/frame_tree_node.h"
#include "content/public/common/content_switches.h"
#include "ohos_cef_ext/libcef/browser/arkweb_frame_host_impl_ext.h"
#endif

namespace {
  
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
// The amount of time to disallow repeated pointer lock calls after the user
// successfully escapes from one lock request.
const int64_t kEffectiveUserEscapeDuration = 1250;
#endif

}  // namespace

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
class DataResubmissionCallbackImpl : public CefCallback {
 public:
  explicit DataResubmissionCallbackImpl(content::WebContents* contents)
      : contents_(contents) {}

  ~DataResubmissionCallbackImpl() override {}

  void Continue() override {
    if (contents_) {
      contents_->GetController().ContinuePendingReload();
    }
  }

  void Cancel() override {
    if (contents_) {
      contents_->GetController().CancelPendingReload();
    }
  }

 private:
  raw_ptr<content::WebContents> contents_;

  IMPLEMENT_REFCOUNTING(DataResubmissionCallbackImpl);
};
#endif  // ARKWEB_NETWORK_BASE

ArkWebBrowserContentsDelegateExt::ArkWebBrowserContentsDelegateExt(
    scoped_refptr<CefBrowserInfo> browser_info)
    : CefBrowserContentsDelegate(browser_info) {}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
void ArkWebBrowserContentsDelegateExt::OnRefreshAccessedHistory(
    CefRefPtr<CefFrame> frame,
    const GURL& url,
    bool isReload,
    bool isMainFrame) {
  CefRefPtr<CefClient> cefClient = client();
  if (!cefClient.get()) {
    LOG(ERROR) << "cef client is null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "cef client is null";
#endif
    return;
  }

  auto handler = cefClient->GetLoadHandler();
  if (!handler.get()) {
    LOG(ERROR) << "cef client handler is null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "cef client handler is null";
#endif
    return;
  }

  handler->OnRefreshAccessedHistory(browser(), frame, url.spec(), isReload, isMainFrame);
}
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)

#if BUILDFLAG(ARKWEB_WPT)
void ArkWebBrowserContentsDelegateExt::DidStartNavigation(
    content::NavigationHandle* navigation) {
  if (icon_helper_) {
    icon_helper_->ClearFailedFaviconUrlSets(navigation);

#if BUILDFLAG(ARKWEB_FAVICON)
    if (navigation->IsInMainFrame() && !navigation->IsSameDocument()) {
      icon_helper_->SetMainFrameDocumentOnLoadCompleted(false);
    }
#endif
  }

#if BUILDFLAG(ARKWEB_USERAGENT)
  // |final_ua| may be added to the navigation of the mainframe and iframe.
  arkweb_useragent_utils::MaybeOverrideUserAgentOnStartNavigation(navigation);
#endif
}
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_USERAGENT)
void ArkWebBrowserContentsDelegateExt::DidRedirectNavigation(
    content::NavigationHandle* navigation) {
  arkweb_useragent_utils::MaybeOverrideUserAgentOnRedirectNavigation(navigation);
}
#endif  // BUILDFLAG(ARKWEB_EXT_UA)

#if BUILDFLAG(ARKWEB_FAVICON)
void ArkWebBrowserContentsDelegateExt::
    DocumentOnLoadCompletedInPrimaryMainFrame() {
  if (icon_helper_) {
    icon_helper_->SetMainFrameDocumentOnLoadCompleted(true);
  }
}
#endif  // BUILDFLAG(ARKWEB_FAVICON)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void ArkWebBrowserContentsDelegateExt::OnActivateContent() {
  LOG(INFO) << "ArkWebBrowserContentsDelegateExt::ActivateContent";
  if (auto c = client()) {
    if (auto handler = c->GetFocusHandler()) {
      handler->OnActivateContent();
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
void ArkWebBrowserContentsDelegateExt::NavigationEntryCommitted(
    const content::LoadCommittedDetails& load_details) {
  if (!browser_info_) {
    return;
  }

  if (auto c = client()) {
    if (auto handler = c->GetLoadHandler()) {
      auto navigation_lock = browser_info_->CreateNavigationLock();
      CefLoadCommittedDetails::NavigationType type =
          CefLoadCommittedDetailsImpl::ConvertToCefLoadCommittedDetailsType(
              load_details);
      CefRefPtr<CefLoadCommittedDetails> details =
          new CefLoadCommittedDetailsImpl(
              load_details.current_commit_entry_url.spec(), type,
              load_details.is_main_frame, load_details.is_same_document,
              load_details.did_replace_entry);
      handler->OnNavigationEntryCommitted(details);
    }
  }
}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void ArkWebBrowserContentsDelegateExt::ViewportFitChanged(
    blink::mojom::ViewportFit value) {
  if (auto c = client()) {
    if (auto handler = c->GetDisplayHandler()) {
      handler->OnViewportFitChange(browser(), static_cast<int>(value));
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void ArkWebBrowserContentsDelegateExt::OnOldPageNoLongerRendered(
    const GURL& url,
    bool success) {
  LOG(INFO) << "ArkWebBrowserContentsDelegateExt::OldPageNoLongerRendered";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO)
      << "ArkWebBrowserContentsDelegateExt::OldPageNoLongerRendered";
#endif

  if (!browser_info_) {
    return;
  }
  if (auto c = client()) {
    if (auto handler = c->GetLoadHandler()) {
      auto navigation_lock = browser_info_->CreateNavigationLock();
      handler->OnPageVisible(browser(), url.spec(), success);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_FAVICON)
void ArkWebBrowserContentsDelegateExt::InitIconHelper() {
  if (icon_helper_) {
    return;
  }
  icon_helper_ = new IconHelper();
  if (client()) {
    if (CefRefPtr<ArkWebDisplayHandlerExt> handler =
            client()->GetDisplayHandler()) {
      icon_helper_->SetDisplayHandler(handler);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void ArkWebBrowserContentsDelegateExt::ShowRepostFormWarningDialog(
    content::WebContents* source) {
  LOG(INFO) << "ArkWebBrowserContentsDelegateExt::ShowRepostFormWarningDialog";
  if (!source) {
    return;
  }
  CefRefPtr<DataResubmissionCallbackImpl> callbackImpl =
      new DataResubmissionCallbackImpl(source);
  if (auto c = client()) {
    if (auto handler = c->GetLoadHandler()) {
      auto navigation_lock = browser_info_->CreateNavigationLock();
      handler->OnDataResubmission(browser(), callbackImpl.get());
    }
  }
}
#endif  // ARKWEB_NETWORK_BASE

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool ArkWebBrowserContentsDelegateExt::IsPointerLocked() const {
  return pointer_lock_state_ == POINTERLOCK_LOCKED ||
         pointer_lock_state_ == POINTERLOCK_LOCKED_SILENTLY;
}

bool ArkWebBrowserContentsDelegateExt::IsPointerLockedSilently() const {
  return pointer_lock_state_ == POINTERLOCK_LOCKED_SILENTLY;
}

void ArkWebBrowserContentsDelegateExt::SetTabWithExclusiveAccess(content::WebContents* tab) {
  // Tab should never be replaced with another tab, or
  // UpdateNotificationRegistrations would need updating.
  DCHECK(tab_with_exclusive_access_ == tab ||
         tab_with_exclusive_access_ == nullptr || tab == nullptr);
  tab_with_exclusive_access_ = tab;
}

bool ArkWebBrowserContentsDelegateExt::HandleUserKeyEvent(
    const input::NativeWebKeyboardEvent& event) {
  if (event.windows_key_code != ui::VKEY_ESCAPE) {
    return false;
  }
 
  if (IsPointerLocked()) {
    if (tab_with_exclusive_access_) {
      UnlockPointer();
      SetTabWithExclusiveAccess(nullptr);
      pointer_lock_state_ = POINTERLOCK_UNLOCKED;
    }
    last_user_escape_time_ = base::TimeTicks::Now();
    return true;
  }
 
  return false;
}

void ArkWebBrowserContentsDelegateExt::RequestPointerLock(
    content::WebContents* web_contents,
    bool user_gesture,
    bool last_unlocked_by_target) {
  DCHECK(!IsPointerLocked());
  if (!last_unlocked_by_target && !is_fullscreen()) {
    if (!user_gesture) {
      web_contents->GotResponseToPointerLockRequest(
          blink::mojom::PointerLockResult::kRequiresUserGesture);
      return;
    }

    base::TimeDelta userEscapeDuration = base::TimeTicks::Now() - last_user_escape_time_;
    if (userEscapeDuration.InMilliseconds() < kEffectiveUserEscapeDuration) {
      web_contents->GotResponseToPointerLockRequest(
          blink::mojom::PointerLockResult::kUserRejected);
      return;
    }
  }
  SetTabWithExclusiveAccess(web_contents);

  // Lock pointer.
  if (web_contents->GotResponseToPointerLockRequest(
          blink::mojom::PointerLockResult::kSuccess)) {
    if (last_unlocked_by_target) {
      pointer_lock_state_ = POINTERLOCK_LOCKED_SILENTLY;
    } else {
      pointer_lock_state_ = POINTERLOCK_LOCKED;
    }
  } else {
    SetTabWithExclusiveAccess(nullptr);
    pointer_lock_state_ = POINTERLOCK_UNLOCKED;
  }
}

void ArkWebBrowserContentsDelegateExt::LostPointerLock() {
  pointer_lock_state_ = POINTERLOCK_UNLOCKED;
  SetTabWithExclusiveAccess(nullptr);
}

void ArkWebBrowserContentsDelegateExt::UnlockPointer() {
  if (!tab_with_exclusive_access_) {
    return;
  }

  content::RenderWidgetHostView* pointer_lock_view = nullptr;
  content::RenderViewHost* rvh =
      tab_with_exclusive_access_->GetPrimaryMainFrame()->GetRenderViewHost();
  if (rvh) {
    pointer_lock_view = rvh->GetWidget()->GetView();
  }

  if (pointer_lock_view) {
    pointer_lock_view->UnlockPointer();
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void ArkWebBrowserContentsDelegateExt::OnLoadStarted(CefRefPtr<CefFrame> frame,
                                                     const CefString& url) {
  if (auto c = client()) {
    if (auto handler = c->GetLoadHandler()) {
      auto navigation_lock = browser_info_->CreateNavigationLock();
      // On the handler that loading has started.
      handler->OnLoadStarted(frame, url);
    }
  }
}

void ArkWebBrowserContentsDelegateExt::OnLoadFinished(CefRefPtr<CefFrame> frame,
                                                      const CefString& url) {
  if (auto c = client()) {
    if (auto handler = c->GetLoadHandler()) {
      auto navigation_lock = browser_info_->CreateNavigationLock();
      handler->OnLoadFinished(frame, url);
    }
  }
}

bool ArkWebBrowserContentsDelegateExt::IsPrerendering(
  const CefRefPtr<CefFrameHostImpl> frame) {
  // Prerendering is only supported when NwebEx is enabled.
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    return false;
  }

  if (!frame) {
    return false;
  }

  if (!frame->IsValid()) {
    return false;
  }

  return frame->IsPrerendering();
}

void ArkWebBrowserContentsDelegateExt::NavigationStateChanged(
    content::WebContents* source,
    content::InvalidateTypes changed_flags) {
  bool is_popup_window = false;
  bool has_accessed_initial_document = false;
  if (auto browser = browser_info_->browser()) {
    is_popup_window = browser->IsPopup();
    if (browser->GetWebContents()) {
      content::WebContentsImpl* web_contents_impl =
          static_cast<content::WebContentsImpl*>(browser->GetWebContents());
      if (web_contents_impl) {
        has_accessed_initial_document =
            web_contents_impl->HasAccessedInitialDocument();
      }
    }
  }

  bool should_synthesize_page_load =
      is_popup_window && has_accessed_initial_document &&
      (changed_flags == content::InvalidateTypes::INVALIDATE_TYPE_URL) &&
      !did_synthesize_page_load_;
  if (should_synthesize_page_load) {
    auto main_frame = browser_info_->GetMainFrame();
    OnLoadStarted(main_frame.get(), main_frame->GetURL());
    OnLoadFinished(main_frame.get(), main_frame->GetURL());
    did_synthesize_page_load_ = true;
  }
}
#endif

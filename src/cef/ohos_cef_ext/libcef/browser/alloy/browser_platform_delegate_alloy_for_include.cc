/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "cef/ohos_cef_ext/libcef/browser/printing/ohos_print_manager.h"
#include "chrome/browser/printing/print_view_manager_common.h"

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#endif

#if BUILDFLAG(ARKWEB_AUTOFILL)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "chrome/browser/browser_process.h"
#include "components/autofill/content/browser/content_autofill_driver_factory.h"
#include "components/autofill/core/browser/browser_autofill_manager.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/common/content_switches.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_client.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_manager.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_provider.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
void CefBrowserPlatformDelegateAlloy::SetAccessibilityState(
    cef_state_t accessibility_state) {
  // Do nothing if state is set to default. It'll be disabled by default and
  // controlled by the commmand-line flags "force-renderer-accessibility" and
  // "disable-renderer-accessibility".
  if (accessibility_state == STATE_DEFAULT) {
    return;
  }

  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);

  if (!web_contents_impl) {
    return;
  }

  ui::AXMode accMode;
  // In windowless mode set accessibility to TreeOnly mode. Else native
  // accessibility APIs, specific to each platform, are also created.
  if (accessibility_state == STATE_ENABLED) {
    accMode = IsWindowless() ? ui::kAXModeWebContentsOnly : ui::kAXModeComplete;
    accMode = ui::kAXModeComplete;
  }
  web_contents_impl->SetAccessibilityMode(accMode);
}

ui::BrowserAccessibilityManager*
CefBrowserPlatformDelegateAlloy::GetRootBrowserAccessibilityManager() {
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);

  if (!web_contents_impl) {
    return nullptr;
  }
  return web_contents_impl->GetRootBrowserAccessibilityManager();
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefBrowserPlatformDelegateAlloy::SendTouchEventToRender(
    const CefTouchEvent& event) {
  if (!browser_) {
    return;
  }
  float ratio = browser_->GetHost()->GetVirtualPixelRatio();

  // float ratio = 1;//
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }
  auto frame = browser_->GetMainFrame();
  if (frame && frame->IsValid()) {
    frame.get()->AsCefFrameHostImpl()->SendHitEvent(
        event.x * ratio, event.y * ratio, event.radius_x * ratio,
        event.radius_y * ratio);
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_PRINT)
void CefBrowserPlatformDelegateAlloy::SetToken(void* token) {
  // this check no exit
  // REQUIRE_ALLOY_RUNTIME();
  content::RenderFrameHost* rfh_to_use =
      printing::GetFrameToPrint(web_contents_);
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->SetToken(token);
}

void CefBrowserPlatformDelegateAlloy::CreateWebPrintDocumentAdapter(
    const CefString& jobName,
    void** webPrintDocumentAdapter) {
  // this check no exit
  // REQUIRE_ALLOY_RUNTIME();
  content::RenderFrameHost* rfh_to_use =
      printing::OhosPrintManager::GetRenderFrameHostToUse(web_contents_);
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->SetRfhId(rfh_to_use->GetGlobalId());
  ohos_print_manager->CreateWebPrintDocumentAdapter(jobName,
                                                    webPrintDocumentAdapter);
}

void CefBrowserPlatformDelegateAlloy::CreateWebPrintDocumentAdapterV2(
    const CefString& jobName,
    void** adapter) {
  // this check no exit
  // REQUIRE_ALLOY_RUNTIME();
  content::RenderFrameHost* rfh_to_use =
      printing::OhosPrintManager::GetRenderFrameHostToUse(web_contents_);
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->SetRfhId(rfh_to_use->GetGlobalId());
  ohos_print_manager->CreateWebPrintDocumentAdapterV2(jobName, adapter);
}

void CefBrowserPlatformDelegateAlloy::SetPrintBackground(bool enable) {
  // REQUIRE_ALLOY_RUNTIME();
  content::RenderFrameHost* rfh_to_use =
      printing::GetFrameToPrint(web_contents_);
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->SetPrintBackground(enable);
}

bool CefBrowserPlatformDelegateAlloy::GetPrintBackground() {
  // REQUIRE_ALLOY_RUNTIME();
  content::RenderFrameHost* rfh_to_use =
      printing::GetFrameToPrint(web_contents_);
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return false;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return false;
  }

  return ohos_print_manager->GetPrintBackground();
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
void CefBrowserPlatformDelegateAlloy::WebContentsDestroyed(
    content::WebContents* web_contents) {
  DCHECK(web_contents_ && web_contents_ == web_contents);
  if (!web_contents) {
    LOG(ERROR)
        << "CefBrowserPlatformDelegateAlloy WebContentsDestroyed is null. ";
    return;
  }

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExGetZoomLevel)) {
    if (!browser_) {
      return;
    }
    CefRefPtr<AlloyBrowserHostImpl> delegate =
        browser_->AsAlloyBrowserHostImpl();
    zoom::ZoomController* zoom_controller =
        zoom::ZoomController::FromWebContents(web_contents);
    if (zoom_controller && delegate) {
      zoom_controller->RemoveObserver(delegate.get());
    }
  }
#endif
  CefBrowserPlatformDelegate::WebContentsDestroyed(web_contents);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
static void HandleZoomLevelExt(CefBrowserHostBase* browser, content::WebContents* web_contents) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExGetZoomLevel)) {
    CefRefPtr<AlloyBrowserHostImpl> delegate =
        browser->AsAlloyBrowserHostImpl();
    zoom::ZoomController* zoom_controller =
        zoom::ZoomController::FromWebContents(web_contents);
    if (zoom_controller && delegate) {
      zoom_controller->AddObserver(delegate.get());
    }
  }
}
#endif

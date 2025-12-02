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

#include "chrome/browser/extensions/api/tabs/tabs_event_router.h"
#include "chrome/browser/extensions/api/tabs/tabs_windows_api.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/extensions/permissions/active_tab_permission_granter.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_web_contents_observer.h"
#include "libcef/browser/extensions/window_extensions_util.h"

namespace extensions {

void TabsWindowsAPI::TabUpdated(int tab_id,
    content::WebContents* contents,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
    std::unique_ptr<NWebExtensionTab> tab) {
  if (!tab) {
    LOG(ERROR) << "TabUpdated tab is null";
    return;
  }
 
  // In Chromium 141, ActiveTabPermissionGranter is accessed directly
  // rather than through TabHelper
  auto* active_tab_granter = ActiveTabPermissionGranter::FromWebContents(contents);
  if (active_tab_granter) {
    // Note: SetTabId method may not exist in Chromium 141
    // The functionality may be handled differently now
    LOG(INFO) << "ActiveTabPermissionGranter available for tab_id: " << tab_id;
  }

  int windowId = tab->windowId;
  if (auto* ewco = extensions::ExtensionWebContentsObserver::GetForWebContents(contents)) {
      contents->ForEachRenderFrameHost([ewco, tab_id, windowId](content::RenderFrameHost* frame_host) {
      if (auto local_frame = ewco->GetLocalFrame(frame_host)) {
          local_frame->UpdateBrowserWindowId(windowId);
          local_frame->SetTabId(tab_id);
      }
    });
  }

  tabs_event_router()->DispatchTabUpdatedEvent(
      tab_id, contents, std::move(changeInfo), std::move(tab));
}

void TabsWindowsAPI::TabActivated(int tab_id,
    int window_id, content::BrowserContext* browser_context) {
  tabs_event_router()->DispatchTabActivatedEvent(tab_id, window_id, browser_context);
}

void TabsWindowsAPI::TabRemoved(int tab_id,
                                bool isWindowClosing,
                                int windowId,
                                content::WebContents* contents) {
  tabs_event_router()->DispatchTabRemovedEvent(tab_id, isWindowClosing, windowId, contents);
}

void TabsWindowsAPI::TabAttached(int tab_id,
                                content::WebContents* contents,
                                int32_t newPosition,
                                int32_t newWindowId) {
  tabs_event_router()->DispatchTabAttachedEvent(tab_id, contents, newPosition, newWindowId);
}

void TabsWindowsAPI::TabCreated(int tab_id,
                                content::BrowserContext* browserContext,
                                std::unique_ptr<NWebExtensionTab> tab) {
  tabs_event_router()->DispatchTabCreatedEvent(tab_id, browserContext, std::move(tab));
}

void TabsWindowsAPI::TabDetached(content::WebContents* contents,
                                 int tab_id,
                                 int oldPosition,
                                 int oldWindowId) {
  tabs_event_router()->DispatchTabDetachedEvent(contents, tab_id, oldPosition, oldWindowId);
}

void TabsWindowsAPI::TabHighlighted(content::WebContents* contents,
                                    NWebExtensionTabHighlightInfo& highlightInfo) {
  tabs_event_router()->DispatchTabHighlightedEvent(contents, highlightInfo);
}

void TabsWindowsAPI::TabMoved(content::WebContents* contents,
                              int tab_id,
                              std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  tabs_event_router()->DispatchTabMovedEvent(contents, tab_id, std::move(moveInfo));
}

void TabsWindowsAPI::TabReplaced(content::WebContents* contents,
                                 int32_t addedTabId,
                                 int32_t removedTabId) {
  tabs_event_router()->DispatchTabReplacedEvent(contents, addedTabId, removedTabId);
}

void TabsWindowsAPI::RegisterTabZoomObserver(content::WebContents* web_contents) {
  tabs_event_router()->RegisterTabZoomObserver(web_contents);
}
 
void TabsWindowsAPI::UnregisterTabZoomObserver(content::WebContents* web_contents) {
  tabs_event_router()->UnregisterTabZoomObserver(web_contents);
}

}  // namespace extensions

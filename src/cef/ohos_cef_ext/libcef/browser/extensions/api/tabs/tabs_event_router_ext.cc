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
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/render_process_host.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/common/extensions/api/tabs.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/favicon_status.h"
#include "cef/ohos_cef_ext/libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"
#include "libcef/browser/extensions/tab_extensions_util.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_cef_delegate.h"
#include "url/gurl.h"

using base::Value;
using content::WebContents;
using zoom::ZoomController;

namespace extensions {

namespace {
constexpr char kGroupIdKey[] = "groupId";
constexpr char kNewPositionKey[] = "newPosition";
constexpr char kNewWindowIdKey[] = "newWindowId";

constexpr char kPinnedKey[] = "pinned";
constexpr char kAudibleKey[] = "audible";
constexpr char kFrozenKey[] = "frozen";
constexpr char kDiscardedKey[] = "discarded";
constexpr char kAutoDiscardableKey[] = "autoDiscardable";
constexpr char kMutedInfoKey[] = "mutedInfo";
constexpr char kSelectedKey[] = "selected";
constexpr char kStatusKey[] = "status";
constexpr char kTabIdKey[] = "tabId";

constexpr char kWindowClosing[] = "isWindowClosing";

constexpr char kOldPositionKey[] = "oldPosition";
constexpr char kOldWindowIdKey[] = "oldWindowId";
constexpr char kWindowIdKey[] = "windowId";
constexpr char kTabIdsKey[] = "tabIds";
constexpr char kFromIndexKey[] = "fromIndex";
constexpr char kToIndexKey[] = "toIndex";

const char kChangePropertyNameAudible[] = "audible";
const char kChangePropertyNameAutoDiscardable[] = "autoDiscardable";
const char kChangePropertyNameDiscarded[] = "discarded";
const char kChangePropertyNameFavIconUrl[] = "favIconUrl";
const char kChangePropertyNameGroupId[] = "groupId";
const char kChangePropertyNameMutedInfo[] = "mutedInfo";
const char kChangePropertyNamePinned[] = "pinned";
const char kChangePropertyNameStatus[] = "status";
const char kChangePropertyNameTitle[] = "title";
const char kChangePropertyNameUrl[] = "url";

void FillChangeInfo(const NWebExtensionTabChangeInfo& changeInfo,
                    base::Value::Dict& out) {
  if (changeInfo.audible) {
    out.Set(kChangePropertyNameAudible, changeInfo.audible.value());
  }

  if (changeInfo.autoDiscardable) {
    out.Set(kChangePropertyNameAutoDiscardable, changeInfo.autoDiscardable.value());
  }

  if (changeInfo.discarded) {
    out.Set(kChangePropertyNameDiscarded, changeInfo.discarded.value());
  }

  if (changeInfo.favIconUrl) {
    out.Set(kChangePropertyNameFavIconUrl, changeInfo.favIconUrl.value());
  }

  if (changeInfo.groupId) {
    out.Set(kChangePropertyNameGroupId, changeInfo.groupId.value());
  }

  if (changeInfo.mutedInfo) {
    out.Set(kChangePropertyNameMutedInfo, GetMutedInfoValue(changeInfo.mutedInfo.value()));
  }

  if (changeInfo.pinned) {
    out.Set(kChangePropertyNamePinned, changeInfo.pinned.value());
  }

  if (changeInfo.status) {
    out.Set(kChangePropertyNameStatus, NWebExtensionTabStatusToString(changeInfo.status.value()));
  }

  if (changeInfo.title) {
    out.Set(kChangePropertyNameTitle, changeInfo.title.value());
  }

  if (changeInfo.url) {
    out.Set(kChangePropertyNameUrl, changeInfo.url.value());
  }
}

bool HasValidMainFrameProcess(content::WebContents* contents) {
  content::RenderFrameHost* main_frame_host = contents->GetPrimaryMainFrame();
  content::RenderProcessHost* process_host = main_frame_host->GetProcess();
  return process_host->IsReady() && process_host->IsInitializedAndNotDead();
}

api::tabs::TabStatus GetLoadingStatus(content::WebContents* contents) {
  if (contents->IsLoading())
    return api::tabs::TabStatus::kLoading;

  // Anything that isn't backed by a process is considered unloaded.
  if (!HasValidMainFrameProcess(contents))
    return api::tabs::TabStatus::kUnloaded;

  // Otherwise its considered loaded.
  return api::tabs::TabStatus::kComplete;
}

api::tabs::MutedInfo CreateMutedInfo(
    content::WebContents* contents) {
  DCHECK(contents);
  api::tabs::MutedInfo info;
  info.muted = contents->IsAudioMuted();
  return info;
}

api::tabs::Tab CreateTabObject(
    int tab_id,
    content::WebContents* contents,
    const std::string& url) {
  api::tabs::Tab tab_object;
  tab_object.id = tab_id;
  tab_object.index = -1;
  tab_object.window_id = -1;
  tab_object.status = GetLoadingStatus(contents);
  tab_object.active = false;
  tab_object.selected = false;
  tab_object.highlighted = false;
  tab_object.pinned = false;
  tab_object.group_id = -1;
  tab_object.audible = false;
  tab_object.discarded = false;
  tab_object.auto_discardable = false;
  tab_object.muted_info = CreateMutedInfo(contents);
  tab_object.incognito = contents->GetBrowserContext()->IsOffTheRecord();
  gfx::Size contents_size = contents->GetContainerBounds().size();
  tab_object.width = contents_size.width();
  tab_object.height = contents_size.height();
  tab_object.url = url.length() > 0 ? url :
                                      contents->GetLastCommittedURL().spec();

  content::NavigationEntry* pending_entry =
      contents->GetController().GetPendingEntry();
  if (pending_entry) {
    tab_object.pending_url = pending_entry->GetVirtualURL().spec();
  }
  tab_object.title = base::UTF16ToUTF8(contents->GetTitle());
  content::NavigationEntry* visible_entry =
      contents->GetController().GetVisibleEntry();
  if (visible_entry && visible_entry->GetFavicon().valid) {
    tab_object.fav_icon_url = visible_entry->GetFavicon().url.spec();
  }
  return tab_object;
}

bool WillDispatchTabUpdatedEventWithTab(
    int tab_id,
    WebContents* contents,
    NWebExtensionTabChangeInfo changeInfo,
    NWebExtensionTab tab,
    content::BrowserContext* browser_context,
    mojom::ContextType target_context,
    const Extension* extension,
    const base::Value::Dict* listener_filter,
    std::optional<base::Value::List>& event_args_out,
    mojom::EventFilteringInfoPtr& event_filtering_info_out) {
  if (tab.nwebId <= 0) {
    LOG(INFO) << "WillDispatchTabUpdatedEventWithTab tab nweb id is invalid";
    return false;
  }

  ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
      ExtensionTabUtil::GetScrubTabBehavior(extension, target_context, contents);
 
  base::Value::Dict tab_value = GetTabValue(tab, scrub_tab_behavior);
  base::Value::Dict changed_properties;
  FillChangeInfo(changeInfo, changed_properties);

  event_args_out.emplace();
  event_args_out->Append(tab_id);
  event_args_out->Append(std::move(changed_properties));
  event_args_out->Append(std::move(tab_value));
  return true;
}

bool WillDispatchTabCreatedEvent(
    NWebExtensionTab tab,
    content::BrowserContext* browser_context,
    mojom::ContextType target_context,
    const Extension* extension,
    const base::Value::Dict* listener_filter,
    std::optional<base::Value::List>& event_args_out,
    mojom::EventFilteringInfoPtr& event_filtering_info_out) {
  GURL gurl(tab.url.value());
  ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
      ExtensionTabUtil::GetScrubTabBehaviorExt(extension, target_context, gurl, tab.id.value_or(-1));
 
  base::Value::Dict tab_value = extensions::GetTabValue(tab, scrub_tab_behavior);
  event_args_out.emplace();
  event_args_out->Append(std::move(tab_value));
  return true;
}

}  // namespace

void TabsEventRouter::DispatchTabUpdatedEvent(
    int tab_id,
    content::WebContents* contents,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
    std::unique_ptr<NWebExtensionTab> tab) {
  DCHECK(contents);

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());

  if (!changeInfo || !tab) {
    LOG(ERROR) << "DispatchTabUpdatedEvent tab or changeInfo is null";
    return;
  }

  NWebExtensionTabChangeInfo change_info(*changeInfo);
  NWebExtensionTab extension_tab(*tab);

  // compatible with old version, NWEB_NOTIFY is useless.
  if (change_info.status.has_value() &&
      change_info.status.value() == NWebExtensionTabStatus::NWEB_NOTIFY) {
    return;
  }

  auto event = std::make_unique<Event>(
      events::TABS_ON_UPDATED, api::tabs::OnUpdated::kEventName,
      // The event arguments depend on the extension's permission. They are set
      // in WillDispatchTabUpdatedEvent().
      base::Value::List(), profile);
  event->user_gesture = EventRouter::USER_GESTURE_NOT_ENABLED;
  event->will_dispatch_callback =
      base::BindRepeating(&WillDispatchTabUpdatedEventWithTab, tab_id,
                          contents, std::move(change_info), std::move(extension_tab));
  EventRouter::Get(profile)->BroadcastEvent(std::move(event));
}

void TabsEventRouter::DispatchTabActivatedEvent(
    int tab_id,
    int window_id,
    content::BrowserContext* browser_context) {
  DCHECK(browser_context);
  api::tabs::OnActivated::ActiveInfo info;
  info.tab_id = tab_id;
  info.window_id = window_id;

  Profile* profile = Profile::FromBrowserContext(browser_context);
  DispatchEvent(profile, events::TABS_ON_ACTIVATED,
                api::tabs::OnActivated::kEventName,
                api::tabs::OnActivated::Create(info),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabRemovedEvent(int tab_id,
                                              bool isWindowClosing,
                                              int windowId,
                                              content::WebContents* contents) {
  DCHECK(contents);
  api::tabs::OnRemoved::RemoveInfo info;
  info.window_id = windowId;
  info.is_window_closing = isWindowClosing;

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_REMOVED,
                api::tabs::OnRemoved::kEventName,
                api::tabs::OnRemoved::Create(tab_id, info),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabAttachedEvent(int tab_id,
                                             content::WebContents* contents,
                                             int32_t newPosition,
                                             int32_t newWindowId) {
  DCHECK(contents);
  api::tabs::OnAttached::AttachInfo info;
  info.new_position = newPosition;
  info.new_window_id = newWindowId;

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_ATTACHED,
                api::tabs::OnAttached::kEventName,
                api::tabs::OnAttached::Create(tab_id, info),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabCreatedEvent(int tab_id,
                                              content::BrowserContext* browserContext,
                                              std::unique_ptr<NWebExtensionTab> tab) {
  DCHECK(browserContext);
  NWebExtensionTab extension_tab(*tab);

  Profile* profile = Profile::FromBrowserContext(browserContext);
  auto event = std::make_unique<Event>(events::TABS_ON_CREATED,
                                       api::tabs::OnCreated::kEventName,
                                       base::Value::List(), profile);
  event->user_gesture = EventRouter::USER_GESTURE_NOT_ENABLED;
  event->will_dispatch_callback =
      base::BindRepeating(&WillDispatchTabCreatedEvent, std::move(extension_tab));
  EventRouter::Get(profile)->BroadcastEvent(std::move(event));
}

void TabsEventRouter::DispatchTabDetachedEvent(content::WebContents* contents,
                                               int tab_id,
                                               int oldPosition,
                                               int oldWindowId) {
  DCHECK(contents);
  base::Value::List args;
  args.Append(tab_id);

  base::Value::Dict object_args;
  object_args.Set(kOldWindowIdKey, oldWindowId);
  object_args.Set(kOldPositionKey, oldPosition);
  args.Append(std::move(object_args));

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_DETACHED,
                api::tabs::OnDetached::kEventName,
                std::move(args),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabHighlightedEvent(
    content::WebContents* contents,
    NWebExtensionTabHighlightInfo& highlightInfo) {
  DCHECK(contents);
  base::Value::List all_tabs;
  for (int tab_id : highlightInfo.tabIds) {
    all_tabs.Append(tab_id);
  }

  base::Value::List args;
  base::Value::Dict select_info;

  select_info.Set(kWindowIdKey, highlightInfo.windowId.value());
  select_info.Set(kTabIdsKey, std::move(all_tabs));
  args.Append(std::move(select_info));

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_HIGHLIGHTED,
                api::tabs::OnHighlighted::kEventName,
                std::move(args),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabMovedEvent(
    content::WebContents* contents,
    int tab_id,
    std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  DCHECK(contents);
  base::Value::List args;
  args.Append(tab_id);

  base::Value::Dict object_args;
  object_args.Set(kWindowIdKey, moveInfo->windowId);
  object_args.Set(kFromIndexKey, moveInfo->fromIndex);
  object_args.Set(kToIndexKey, moveInfo->toIndex);
  args.Append(std::move(object_args));

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_MOVED,
                api::tabs::OnMoved::kEventName,
                std::move(args),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::DispatchTabReplacedEvent(
    content::WebContents* contents,
    int32_t addedTabId,
    int32_t removedTabId) {
  DCHECK(contents);
  base::Value::List args;
  args.Append(addedTabId);
  args.Append(removedTabId);

  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::TABS_ON_REPLACED,
                api::tabs::OnReplaced::kEventName,
                std::move(args),
                EventRouter::USER_GESTURE_UNKNOWN);
}

void TabsEventRouter::RegisterTabZoomObserver(content::WebContents* contents) {
  if (auto* zoom_controller = ZoomController::FromWebContents(contents);
      !zoom_scoped_observations_.IsObservingSource(zoom_controller)) {
    zoom_scoped_observations_.AddObservation(zoom_controller);
  }
}
 
void TabsEventRouter::UnregisterTabZoomObserver(content::WebContents* contents) {
  if (auto* zoom_controller = ZoomController::FromWebContents(contents);
      zoom_scoped_observations_.IsObservingSource(zoom_controller)) {
    zoom_scoped_observations_.RemoveObservation(zoom_controller);
  }
}

}  // namespace extensions

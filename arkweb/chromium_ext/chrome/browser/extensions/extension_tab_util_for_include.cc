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

#ifndef CHROME_BROWSER_EXTENSIONS_EXTENSION_TAB_UTIL_H_
#error "must be include form CHROME_BROWSER_EXTENSIONS_EXTENSION_TAB_UTIL_H_"
#endif

#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_cef_delegate.h"

namespace extensions {
namespace {
void UpdateWithNWebExtensionTabInfo(api::tabs::Tab& tab) {
  if (!tab.id) {
    return;
  }
  int tab_id = tab.id.value();
  if (tab_id < 0) {
    return;
  }
  auto nweb_tab = OHOS::NWeb::NWebExtensionTabCefDelegate::GetTab(tab_id);
  if (!nweb_tab) {
    return;
  }
  tab.active = nweb_tab->active;
  if (nweb_tab->audible) {
    tab.audible = nweb_tab->audible.value();
  }
  tab.auto_discardable = nweb_tab->autoDiscardable;
  tab.index = nweb_tab->index;
  tab.group_id = nweb_tab->groupId;
  tab.window_id = nweb_tab->windowId;
  if (nweb_tab->openerTabId) {
    tab.opener_tab_id = nweb_tab->openerTabId;
  }
  if (nweb_tab->lastAccessed) {
    tab.last_accessed = nweb_tab->lastAccessed.value();
  }
  tab.highlighted = nweb_tab->highlighted;
  tab.pinned = nweb_tab->pinned;
  tab.discarded = nweb_tab->discarded;
  if (nweb_tab->mutedInfo) {
    if (!tab.muted_info) {
      api::tabs::MutedInfo muted_info;
      tab.muted_info = std::move(muted_info);
    }
    tab.muted_info->muted = nweb_tab->mutedInfo->muted;
    if (nweb_tab->mutedInfo->extensionId) {
      tab.muted_info->extension_id = nweb_tab->mutedInfo->extensionId.value();
    }
    if (nweb_tab->mutedInfo->reason) {
      tab.muted_info->reason =
          api::tabs::ParseMutedInfoReason(nweb_tab->mutedInfo->reason.value());
    }
  }
  if (nweb_tab->url) {
    tab.url = nweb_tab->url.value();
  }
  if (nweb_tab->pendingUrl) {
    tab.pending_url = nweb_tab->pendingUrl.value();
  }
  if (nweb_tab->title) {
    tab.title = nweb_tab->title.value();
  }
  if (nweb_tab->favIconUrl) {
    tab.fav_icon_url = nweb_tab->favIconUrl.value();
  }
  if (nweb_tab->status) {
    tab.status = api::tabs::ParseTabStatus(nweb_tab->status.value());
  }
  tab.incognito = nweb_tab->incognito;
  if (nweb_tab->sessionId) {
    tab.session_id = nweb_tab->sessionId.value();
  }
  if (nweb_tab->width) {
    tab.width = nweb_tab->width;
  }
  if (nweb_tab->height) {
    tab.height = nweb_tab->height;
  }
}
}  // namespace
}  // namespace extensions

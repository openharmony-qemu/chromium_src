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

#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/data_transfer_policy/data_transfer_endpoint.h"
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

namespace {

#if BUILDFLAG(ARKWEB_CLIPBOARD)
constexpr cef_context_menu_edit_state_flags_t kMenuCommands[] = {
    CM_EDITFLAG_CAN_CUT, CM_EDITFLAG_CAN_COPY, CM_EDITFLAG_CAN_PASTE,
    CM_EDITFLAG_CAN_DELETE, CM_EDITFLAG_CAN_SELECT_ALL};
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

}  // namespace

#if BUILDFLAG(ARKWEB_CLIPBOARD)
bool CefMenuManager::IsCommandIdEnabled(
    int command_id,
    content::ContextMenuParams& params) const {
  bool editable = params.is_editable;
  switch (command_id) {
    case CM_EDITFLAG_CAN_CUT:
      return !!(params.edit_flags & CM_EDITFLAG_CAN_CUT);
    case CM_EDITFLAG_CAN_DELETE:
      return !!(params.edit_flags & CM_EDITFLAG_CAN_DELETE);
    case CM_EDITFLAG_CAN_COPY:
      return !!(params.edit_flags & CM_EDITFLAG_CAN_COPY);
    case CM_EDITFLAG_CAN_PASTE: {
      bool can_paste = false;
      if (!editable) {
        LOG(INFO) << "This area is not editable.";
        return can_paste;
      }
      can_paste = ui::Clipboard::GetForCurrentThread()->HasPasteData();
      return can_paste;
    }
    case CM_EDITFLAG_CAN_SELECT_ALL:
      return !!(params.edit_flags & CM_EDITFLAG_CAN_SELECT_ALL);
    default:
      return false;
  }
}

void CefMenuManager::UpdateMenuEditStateFlags(
    content::ContextMenuParams& params) {
  int menu_flags = 0;
  for (const auto& command : kMenuCommands) {
    if (IsCommandIdEnabled(command, params)) {
      menu_flags |= command;
    }
  }

  params.edit_flags = menu_flags;
}
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

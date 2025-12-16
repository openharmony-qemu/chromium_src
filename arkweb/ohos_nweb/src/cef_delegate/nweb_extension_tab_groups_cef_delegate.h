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

#ifndef NWEB_EXTENSION_TABGROUPS_CEF_DELEGATE_H
#define NWEB_EXTENSION_TABGROUPS_CEF_DELEGATE_H

#include "base/values.h"
#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_common_types.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_tab_groups_types.h"

using GetTabGroupsCallback = base::RepeatingCallback<void(const NWebExtensionTabGroup&, std::optional<std::string>&)>;
using MoveTabGroupsCallback = base::RepeatingCallback<void(const NWebExtensionTabGroup&, std::optional<std::string>&)>;
using QueryTabGroupsCallback = base::RepeatingCallback<void(const std::vector<NWebExtensionTabGroup>&, std::optional<std::string>&)>;
using UpdateTabGroupsCallback = base::RepeatingCallback<void(const NWebExtensionTabGroup&, std::optional<std::string>&)>;

namespace OHOS::NWeb {

class NWebExtensionTabGroupsCefDelegate {
 public:
  static bool GetTabGroup(GetTabGroupsCallback callback, int32_t groupId,
      const NWebExtensionFunctionContext& function_context);
  static void GetTabGroupCallback(
      int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error);

  static bool MoveTabGroup(
      MoveTabGroupsCallback callback, int32_t groupId, const NWebExtensionTabGroupsMoveProperties& move_properties);
  static void MoveTabGroupCallback(
      int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error);

  static bool QueryTabGroup(QueryTabGroupsCallback callback, const NWebExtensionTabGroupsQueryInfo& query_info);
  static void QueryTabGroupCallback(
      int32_t request_id, const std::vector<NWebExtensionTabGroup>& tabGroups, std::optional<std::string>& error);

  static bool UpdateTabGroup(UpdateTabGroupsCallback callback, int32_t groupId,
      const NWebExtensionTabGroupsUpdateProperties& update_properties);
  static void UpdateTabGroupCallback(
      int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error);

  static void TabGroupCreated(const NWebExtensionTabGroup& tabGroup);
  static void TabGroupMoved(const NWebExtensionTabGroup& tabGroup);
  static void TabGroupRemoved(const NWebExtensionTabGroup& tabGroup);
  static void TabGroupUpdated(const NWebExtensionTabGroup& tabGroup);
};

}  // namespace OHOS::NWeb

#endif // NWEB_EXTENSION_TABGROUPS_CEF_DELEGATE_H
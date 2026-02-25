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

#ifndef NWEB_EXTENSION_SESSIONS_HANDLER_DELEGATE_H_
#define NWEB_EXTENSION_SESSIONS_HANDLER_DELEGATE_H_
#pragma once

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_sessions_types.h"

namespace OHOS::NWeb {

class NWebExtensionSessionsHandlerDelegate {
 public:
  using GetDevicesCallback =
      base::RepeatingCallback<void(std::vector<NWebExtensionSessionsDevice>&,
                                   const std::optional<std::string>&)>;
  using GetRecentlyClosedCallback =
      base::RepeatingCallback<void(std::vector<NWebExtensionSession>&,
                                   const std::optional<std::string>&)>;
  using RestoreCallback =
      base::RepeatingCallback<void(std::optional<NWebExtensionSession>&,
                                   const std::optional<std::string>&)>;

  static void GetDevices(
      const NWebExtensionSessionsGetDevicesParams& params,
      GetDevicesCallback callback);

  static void GetRecentlyClosed(
      const NWebExtensionSessionsGetRecentlyClosedParams& params,
      GetRecentlyClosedCallback callback);

  static void Restore(const NWebExtensionSessionsRestoreParams& params,
                      RestoreCallback callback);

  static void OnGetDevices(
      int request_id,
      NWebExtensionSessionsGetDevicesCallbackParams& params);

  static void OnRecentlyClosedReceived(
      int request_id,
      NWebExtensionSessionsGetRecentlyClosedCallbackParams& params);

  static void OnSessionRestored(
      int request_id,
      NWebExtensionSessionsRestoreCallbackParams& params);

  static void HandleGetDevicesError(int request_id);

  static void HandleGetRecentlyClosedError(int request_id);

  static void HandleRestoreError(int request_id);

  static void NotifyOnChanged();
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_SESSIONS_HANDLER_DELEGATE_H_

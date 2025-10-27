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

#include "ohos_nweb/src/capi/browser_service/nweb_extension_sessions_types.h"

namespace extensions {

class SessionsGetRecentlyClosedFunction : public ExtensionFunction {
 protected:
  ~SessionsGetRecentlyClosedFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("sessions.getRecentlyClosed",
                             SESSIONS_GETRECENTLYCLOSED)

 private:
  static void OnRecentlyClosedReceived(
      const base::WeakPtr<SessionsGetRecentlyClosedFunction>& function,
      std::vector<NWebExtensionSession>& sessions,
      const std::optional<std::string>& error);
  bool is_calling_ = false;
  base::WeakPtrFactory<SessionsGetRecentlyClosedFunction> weak_ptr_factory_{
      this};
};

class SessionsRestoreFunction : public ExtensionFunction {
 protected:
  ~SessionsRestoreFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("sessions.restore", SESSIONS_RESTORE)

 private:
  static void OnSessionRestored(
      const base::WeakPtr<SessionsRestoreFunction>& function,
      std::optional<NWebExtensionSession>& session,
      const std::optional<std::string>& error);
  bool is_calling_ = false;
  base::WeakPtrFactory<SessionsRestoreFunction> weak_ptr_factory_{this};
};

}  // namespace extensions

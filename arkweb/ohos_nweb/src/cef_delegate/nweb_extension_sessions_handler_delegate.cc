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

#include "nweb_extension_sessions_handler_delegate.h"

#include <algorithm>
#include <map>
#include <vector>

#include "base/logging.h"
#include "chrome/browser/extensions/api/sessions/sessions_api.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browser_context.h"
#include "nweb_extension_utils.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_sessions_types.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_sessions_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {

static std::map<int,
                NWebExtensionSessionsHandlerDelegate::GetDevicesCallback>
    g_get_devices_callbacks_;

static std::map<int,
                NWebExtensionSessionsHandlerDelegate::GetRecentlyClosedCallback>
    g_get_recently_closed_callbacks_;

static std::map<int, NWebExtensionSessionsHandlerDelegate::RestoreCallback>
    g_restore_callbacks_;

const char kNoNWebExError[] = "Not supported: no browser.";
const char kInternalHandleError[] = "Internal handle error.";

}  // namespace

// static
void NWebExtensionSessionsHandlerDelegate::GetDevices(
    const NWebExtensionSessionsGetDevicesParams& params,
    GetDevicesCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  std::vector<NWebExtensionSessionsDevices> empty_devices;
  callback.Run(empty_devices, kNoNWebExError);
#else
  static int request_id = 0;
  g_get_devices_callbacks_[++request_id] = std::move(callback);
  NWebExtensionSessionsDispatcher::GetDevices(request_id, params);
#endif
}

// static
void NWebExtensionSessionsHandlerDelegate::GetRecentlyClosed(
    const NWebExtensionSessionsGetRecentlyClosedParams& params,
    GetRecentlyClosedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  std::vector<NWebExtensionSession> empty_sessions;
  callback.Run(empty_sessions, kNoNWebExError);
#else
  static int request_id = 0;
  g_get_recently_closed_callbacks_[++request_id] = std::move(callback);
  NWebExtensionSessionsDispatcher::GetRecentlyClosed(request_id, params);
#endif
}

// static
void NWebExtensionSessionsHandlerDelegate::Restore(
    const NWebExtensionSessionsRestoreParams& params,
    RestoreCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  std::optional<NWebExtensionSession> null_session;
  callback.Run(null_session, kNoNWebExError);
#else
  static int request_id = 0;
  g_restore_callbacks_[++request_id] = std::move(callback);
  NWebExtensionSessionsDispatcher::Restore(request_id, params);
#endif
}

// static
void NWebExtensionSessionsHandlerDelegate::OnGetDevices(
    int request_id,
    NWebExtensionSessionsGetDevicesCallbackParams& params) {
  if (g_get_devices_callbacks_.count(request_id)) {
    std::move(g_get_devices_callbacks_[request_id])
        .Run(params.devices, params.error);
 
    g_get_devices_callbacks_.erase(request_id);
  } else {
    LOG(ERROR) << "NWeb GetDevices callback called more than once";
  }
}

// static
void NWebExtensionSessionsHandlerDelegate::OnRecentlyClosedReceived(
    int request_id,
    NWebExtensionSessionsGetRecentlyClosedCallbackParams& params) {
  if (g_get_recently_closed_callbacks_.count(request_id)) {
    std::move(g_get_recently_closed_callbacks_[request_id])
        .Run(params.sessions, params.error);

    g_get_recently_closed_callbacks_.erase(request_id);
  } else {
    LOG(ERROR) << "NWeb GetRecentlyClosed callback called more than once";
  }
}

// static
void NWebExtensionSessionsHandlerDelegate::OnSessionRestored(
    int request_id,
    NWebExtensionSessionsRestoreCallbackParams& params) {
  if (g_restore_callbacks_.count(request_id)) {
    std::move(g_restore_callbacks_[request_id])
        .Run(params.session, params.error);

    g_restore_callbacks_.erase(request_id);
  } else {
    LOG(ERROR) << "NWeb Restore callback called more than once";
  }
}

// static
void NWebExtensionSessionsHandlerDelegate::HandleGetDevicesError(
    int request_id) {
  NWebExtensionSessionsGetDevicesCallbackParams params;
  params.error = kInternalHandleError;
  OnGetDevices(request_id, params);
}

// static
void NWebExtensionSessionsHandlerDelegate::HandleGetRecentlyClosedError(
    int request_id) {
  NWebExtensionSessionsGetRecentlyClosedCallbackParams params;
  params.error = kInternalHandleError;
  OnRecentlyClosedReceived(request_id, params);
}

// static
void NWebExtensionSessionsHandlerDelegate::HandleRestoreError(int request_id) {
  NWebExtensionSessionsRestoreCallbackParams params;
  params.error = kInternalHandleError;
  OnSessionRestored(request_id, params);
}

// static
void NWebExtensionSessionsHandlerDelegate::NotifyOnChanged() {
  content::BrowserContext* global_context = GetBrowserContext();
  if (!global_context) {
    LOG(ERROR) << "notify sessions event failed: null global_context";
    return;
  }

  extensions::BrowserContextKeyedAPIFactory<extensions::SessionsAPI>* factory =
      extensions::SessionsAPI::GetFactoryInstance();
  extensions::SessionsAPI* api =
      factory ? factory->Get(global_context) : nullptr;
  if (api && api->event_router()) {
    api->event_router()->TabRestoreServiceChanged(nullptr);
  }
}

}  // namespace OHOS::NWeb

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WEB_COOKIE_MANAGER_FFI_H
#define WEB_COOKIE_MANAGER_FFI_H

#include <cstdint>

#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "webview_utils.h"

extern "C" {
// cookie_manager
FFI_EXPORT const char* FfiOHOSCookieMgrFetchCookieSync(const char* url, bool incognitoMode, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSCookieMgrConfigCookieSync(const char* url, const char* value, bool incognitoMode);
FFI_EXPORT int32_t FfiOHOSCookieMgrCfgCookieSync(const char* url, const char* value, bool incognitoMode,
    bool includeHttpOnly);
FFI_EXPORT void FfiOHOSCookieMgrPutAcceptCookieEnabled(bool accept);
FFI_EXPORT bool FfiOHOSCookieMgrIsCookieAllowed();
FFI_EXPORT void FfiOHOSCookieMgrPutAcceptThirdPartyCookieEnabled(bool accept);
FFI_EXPORT bool FfiOHOSCookieMgrIsThirdPartyCookieAllowed();
FFI_EXPORT bool FfiOHOSCookieMgrExistCookie(bool incognitoMode);
FFI_EXPORT void FfiOHOSCookieMgrClearAllCookiesSync(bool incognitoMode);
FFI_EXPORT void FfiOHOSCookieMgrClearSessionCookieSync();
FFI_EXPORT void FfiOHOSCookieMgrSaveCookieAsync(void (*callbackRef)(void));
}

#endif // WEB_COOKIE_MANAGER_FFI_H
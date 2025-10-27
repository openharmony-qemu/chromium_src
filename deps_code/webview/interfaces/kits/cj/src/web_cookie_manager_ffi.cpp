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

#include "web_cookie_manager_ffi.h"

#include "nweb_cache_options_impl.h"
#include "nweb_init_params.h"
#include "web_cookie_manager.h"
#include "web_errors.h"
#include "webview_log.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// cookie_manager
const char* FfiOHOSCookieMgrFetchCookieSync(const char* url, bool incognitoMode, int32_t* errCode)
{
    std::string curl = url;
    std::string value = OHOS::NWeb::WebCookieManager::CjGetCookie(curl, incognitoMode, *errCode);
    const char* res = MallocCString(value);
    return res;
}

int32_t FfiOHOSCookieMgrConfigCookieSync(const char* url, const char* value, bool incognitoMode)
{
    std::string curl = url;
    std::string cvalue = value;
    return OHOS::NWeb::WebCookieManager::CjSetCookie(curl, cvalue, incognitoMode);
}

int32_t FfiOHOSCookieMgrCfgCookieSync(const char* url, const char* value, bool incognitoMode, bool includeHttpOnly)
{
    std::string curl = url;
    std::string cvalue = value;
    return OHOS::NWeb::WebCookieManager::CjSetCookie(curl, cvalue, incognitoMode, includeHttpOnly);
}

void FfiOHOSCookieMgrPutAcceptCookieEnabled(bool accept)
{
    return OHOS::NWeb::WebCookieManager::CjPutAcceptCookieEnabled(accept);
}

bool FfiOHOSCookieMgrIsCookieAllowed()
{
    return OHOS::NWeb::WebCookieManager::CjIsCookieAllowed();
}

void FfiOHOSCookieMgrPutAcceptThirdPartyCookieEnabled(bool accept)
{
    return OHOS::NWeb::WebCookieManager::CjPutAcceptThirdPartyCookieEnabled(accept);
}

bool FfiOHOSCookieMgrIsThirdPartyCookieAllowed()
{
    return OHOS::NWeb::WebCookieManager::CjIsThirdPartyCookieAllowed();
}

bool FfiOHOSCookieMgrExistCookie(bool incognitoMode)
{
    return OHOS::NWeb::WebCookieManager::CjExistCookie(incognitoMode);
}

void FfiOHOSCookieMgrClearAllCookiesSync(bool incognitoMode)
{
    return OHOS::NWeb::WebCookieManager::CjDeleteEntireCookie(incognitoMode);
}

void FfiOHOSCookieMgrClearSessionCookieSync()
{
    return OHOS::NWeb::WebCookieManager::CjDeleteSessionCookie();
}

void FfiOHOSCookieMgrSaveCookieAsync(void (*callbackRef)(void))
{
    return OHOS::NWeb::WebCookieManager::CjSaveCookie(callbackRef);
}
}
} // namespace Webview
} // namespace OHOS
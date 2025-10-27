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

#ifndef WEB_COOKIE_MANAGER_H
#define WEB_COOKIE_MANAGER_H

#include <string>
#include "nweb_value_callback.h"

namespace OHOS {
namespace NWeb {
class WebCookieManager {
public:
    WebCookieManager() {}
    ~WebCookieManager() = default;

    static std::string CjGetCookie(const std::string &url, bool incognitoMode, int32_t& errCode);
    static int32_t CjSetCookie(const std::string& url, const std::string& value, bool incognitoMode);
    static int32_t CjSetCookie(const std::string& url, const std::string& value, bool incognitoMode,
        bool includeHttpOnly);
    static void CjPutAcceptCookieEnabled(bool accept);
    static bool CjIsCookieAllowed();
    static void CjPutAcceptThirdPartyCookieEnabled(bool accept);
    static bool CjIsThirdPartyCookieAllowed();
    static bool CjExistCookie(bool incognitoMode);
    static void CjDeleteEntireCookie(bool incognitoMode);
    static void CjDeleteSessionCookie();
    static void CjSaveCookie(void (*callbackRef)(void));
};

class NWebSaveCookieCallbackImpl : public NWebBoolValueCallback {
public:
    NWebSaveCookieCallbackImpl(std::function<void(void)> callback) : callback_(callback) {}
    ~NWebSaveCookieCallbackImpl() = default;

    void OnReceiveValue(bool result) override;
private:
    std::function<void(void)> callback_;
};
}
}

#endif
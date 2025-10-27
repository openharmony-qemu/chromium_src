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

#include "web_cookie_manager.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "web_errors.h"
#include "cj_lambda.h"
#include "webview_log.h"

namespace OHOS {
namespace NWeb {
const int DEFAULT_VALUE = -1;
std::string WebCookieManager::CjGetCookie(const std::string &url, bool incognitoMode, int32_t& errCode)
{
    std::string cookieContent = "";
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    bool isValid = true;
    if (cookieManager != nullptr) {
        cookieContent = cookieManager->ReturnCookie(url, isValid, incognitoMode);
    }
    if (cookieContent == "" && !isValid) {
        errCode = NWebError::INVALID_URL;
        return "";
    }
    errCode = NWebError::NO_ERROR;
    return cookieContent;
}

int32_t WebCookieManager::CjSetCookie(const std::string& url, const std::string& value, bool incognitoMode)
{
    int isSet = DEFAULT_VALUE;
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookie(url, value, incognitoMode);
    }
    return isSet;
}

int32_t WebCookieManager::CjSetCookie(const std::string& url, const std::string& value, bool incognitoMode,
    bool includeHttpOnly)
{
    int isSet = DEFAULT_VALUE;
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookieSync(url, value, incognitoMode, includeHttpOnly);
    }
    return isSet;
}

void WebCookieManager::CjPutAcceptCookieEnabled(bool accept)
{
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptCookieEnabled(accept);
    }
}

bool WebCookieManager::CjIsCookieAllowed()
{
    bool accept = true;
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsAcceptCookieAllowed();
    }
    return accept;
}

void WebCookieManager::CjPutAcceptThirdPartyCookieEnabled(bool accept)
{
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptThirdPartyCookieEnabled(accept);
    }
}

bool WebCookieManager::CjIsThirdPartyCookieAllowed()
{
    bool accept = true;
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsThirdPartyCookieAllowed();
    }
    return accept;
}

bool WebCookieManager::CjExistCookie(bool incognitoMode)
{
    bool exist = true;
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        exist = cookieManager->ExistCookies(incognitoMode);
    }
    return exist;
}

void WebCookieManager::CjDeleteEntireCookie(bool incognitoMode)
{
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteCookieEntirely(nullptr, incognitoMode);
    }
}

void WebCookieManager::CjDeleteSessionCookie()
{
    std::shared_ptr<NWebCookieManager> cookieManager = NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteSessionCookies(nullptr);
    }
}

void WebCookieManager::CjSaveCookie(void (*callbackRef)(void))
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        return;
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallbackImpl>(CJLambda::Create(callbackRef));
        cookieManager->Store(callbackImpl);
    }
}

void NWebSaveCookieCallbackImpl::OnReceiveValue(bool result)
{
    WEBVIEWLOGD("save cookie received result, result = %{public}d", result);
    callback_();
}
}
}
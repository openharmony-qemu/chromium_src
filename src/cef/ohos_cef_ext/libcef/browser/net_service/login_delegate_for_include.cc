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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void LoginDelegate::ContinueBeforeCommit(
    CefRefPtr<CefBrowserHostBase> browser,
    const net::AuthChallengeInfo& auth_info,
    const GURL& request_url,
    const content::GlobalRequestID& request_id,
    bool is_request_for_main_frame,
    const std::optional<net::AuthCredentials>& credentials,
    bool cancelled_by_extension) {
  CEF_REQUIRE_UIT();

  // The request may have been handled while the WebRequest API was processing.
  if (!browser || callback_.is_null() || cancelled_by_extension) {
    LOG(INFO) << "LoginDelegate is cancelled by extension:"
              << cancelled_by_extension;
    Cancel();
    return;
  }

  if (credentials) {
    LOG(INFO) << "LoginDelegate with credentials";
    Continue(credentials->username(), credentials->password());
    return;
  }

  LOG(INFO) << "LoginDelegate try to get credentials";
  StartInternal(browser, auth_info, request_id, request_url);
}

void LoginDelegate::StartInternal(CefRefPtr<CefBrowserHostBase> browser,
                                  const net::AuthChallengeInfo& auth_info,
                                  const content::GlobalRequestID& request_id,
                                  const GURL& origin_url) {
  auto url_request_info = CefBrowserURLRequest::FromRequestID(request_id);

  if (browser || url_request_info) {
    // AuthCallbackImpl is bound to the current thread.
    CefRefPtr<AuthCallbackImpl> callbackImpl =
        new AuthCallbackImpl(weak_ptr_factory_.GetWeakPtr()
#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
                             ,
                             auth_info.challenger.host(), auth_info.realm
#endif
        );

    // Execute callbacks on the IO thread to maintain the "old"
    // network_delegate callback behaviour.
    CEF_POST_TASK(CEF_IOT, base::BindOnce(&RunCallbackOnIOThread, browser,
                                          url_request_info, auth_info,
                                          origin_url, callbackImpl));
  } else {
    Cancel();
  }
}

void LoginDelegate::Start(
    CefRefPtr<CefBrowserHostBase> browser,
    const net::AuthChallengeInfo& auth_info,
    const content::GlobalRequestID& request_id,
    bool is_request_for_main_frame,
    const GURL& origin_url,
    scoped_refptr<net::HttpResponseHeaders> response_headers) {
  CEF_REQUIRE_UIT();

  if (browser && is_request_for_main_frame) {
    // If the WebRequest API wants to take a shot at intercepting this, we can
    // return immediately. |continuation| will eventually be invoked if the
    // request isn't cancelled.
    auto* api = extensions::BrowserContextKeyedAPIFactory<
        extensions::WebRequestAPI>::Get(browser->GetBrowserContext());
    auto continuation = base::BindOnce(&LoginDelegate::ContinueBeforeCommit,
                                       weak_ptr_factory_.GetWeakPtr(), browser,
                                       auth_info, origin_url, request_id, true);
    if (api->MaybeProxyAuthRequest(browser->GetBrowserContext(), auth_info,
                                   std::move(response_headers), request_id,
                                   true, std::move(continuation), nullptr)) {
      return;
    }
  }

  StartInternal(browser, auth_info, request_id, origin_url);
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

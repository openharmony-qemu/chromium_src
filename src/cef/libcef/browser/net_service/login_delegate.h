// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_SERVICE_LOGIN_DELEGATE_H_
#define CEF_LIBCEF_BROWSER_NET_SERVICE_LOGIN_DELEGATE_H_

#include "base/memory/weak_ptr.h"
#include "cef/include/cef_base.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/login_delegate.h"
#include "net/base/auth.h"

namespace content {
struct GlobalRequestID;
class WebContents;
}  // namespace content

class CefBrowserHostBase;
class GURL;

namespace net_service {

class LoginDelegate : public content::LoginDelegate {
 public:
  // This object will be deleted when |callback| is executed or the request is
  // canceled. |callback| should not be executed after this object is deleted.
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  LoginDelegate(const net::AuthChallengeInfo& auth_info,
                content::WebContents* web_contents,
                const content::GlobalRequestID& request_id,
                bool is_request_for_main_frame,
                const GURL& origin_url,
                scoped_refptr<net::HttpResponseHeaders> response_headers,
                content::LoginDelegate::LoginAuthRequiredCallback callback);
#else
  LoginDelegate(const net::AuthChallengeInfo& auth_info,
                content::WebContents* web_contents,
                const content::GlobalRequestID& request_id,
                const GURL& origin_url,
                LoginAuthRequiredCallback callback);
#endif

  void Continue(const CefString& username, const CefString& password);
  void Cancel();

 private:
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void Start(CefRefPtr<CefBrowserHostBase> browser,
             const net::AuthChallengeInfo& auth_info,
             const content::GlobalRequestID& request_id,
             bool is_request_for_main_frame,
             const GURL& origin_url,
             scoped_refptr<net::HttpResponseHeaders> response_headers);
  void StartInternal(CefRefPtr<CefBrowserHostBase> browser,
                     const net::AuthChallengeInfo& auth_info,
                     const content::GlobalRequestID& request_id,
                     const GURL& origin_url);
  void ContinueBeforeCommit(
      CefRefPtr<CefBrowserHostBase> browser,
      const net::AuthChallengeInfo& auth_info,
      const GURL& request_url,
      const content::GlobalRequestID& request_id,
      bool is_request_for_main_frame,
      const std::optional<net::AuthCredentials>& credentials,
      bool cancelled_by_extension);
#else
  void Start(CefRefPtr<CefBrowserHostBase> browser,
             const net::AuthChallengeInfo& auth_info,
             const content::GlobalRequestID& request_id,
             const GURL& origin_url);

#endif
  content::LoginDelegate::LoginAuthRequiredCallback callback_;
  base::WeakPtrFactory<LoginDelegate> weak_ptr_factory_;
};

}  // namespace net_service

#endif  // CEF_LIBCEF_BROWSER_NET_SERVICE_LOGIN_DELEGATE_H_

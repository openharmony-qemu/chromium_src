#ifndef OHOS_CEF_EXT_INCLUDE_CEF_REQUEST_HANDLER_EXT_H_
#define OHOS_CEF_EXT_INCLUDE_CEF_REQUEST_HANDLER_EXT_H_

#include <vector>

#include "include/cef_auth_callback.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_callback.h"
#include "include/cef_frame.h"
#include "include/cef_request.h"
#include "include/cef_resource_request_handler.h"
#include "include/cef_ssl_info.h"
#include "include/cef_unresponsive_process_callback.h"
#include "include/cef_x509_certificate.h"
#include "ohos_cef_ext/include/arkweb_cef_ssl_callback.h"

class CefSelectClientCertificateCallbackExt
    : public virtual CefSelectClientCertificateCallback {
 public:
  virtual void Select(CefRefPtr<CefX509Certificate> cert) override {}

  virtual void Select(const CefString& private_key_file,
                      const CefString& cert_chain_file) {}

  ///
  /// Cancel the select cert request.
  ///
  virtual void Cancel() = 0;

  ///
  /// Ignore the select cert request.
  ///
  virtual void Ignore() = 0;

  CefRefPtr<CefSelectClientCertificateCallbackExt>
  AsCefSelectClientCertificateCallbackExt() override {
    return this;
  }
};

///
/// IS_OHOS extended
/// Callback interface for asynchronous continuation of app link event.
///
/*--cef(source=library)--*/
class CefOpenAppLinkCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Continue load in web.
  ///
  /*--cef(capi_name=cont)--*/
  virtual void Continue() = 0;

  ///
  /// Cancel load in web.
  ///
  /*--cef()--*/
  virtual void Cancel() = 0;
};

class CefRequestHandlerExt : public virtual CefRequestHandler {
 private:
  using CefRequestHandler::OnSelectClientCertificate;

 public:
  ///
  /// Called on the UI thread when a client certificate is being requested for
  /// authentication. Return false to use the default behavior.  If the
  /// |certificates| list is not empty the default behavior will be to display a
  /// dialog for certificate selection. If the |certificates| list is empty then
  /// the default behavior will be not to show a dialog and it will continue
  /// without using any certificate. Return true and call
  /// CefSelectClientCertificateCallback::Select either in this method or at a
  /// later time to select a certificate. Do not call Select or call it with
  /// NULL to continue without using any certificate. |isProxy| indicates
  /// whether the host is an HTTPS proxy or the origin server. |host| and |port|
  /// contains the hostname and port of the SSL server. |certificates| is the
  /// list of certificates to choose from; this list has already been pruned by
  /// Chromium so that it only contains certificates from issuers that the
  /// server trusts.
  /// IS_OHOS extended
  ///
  virtual bool OnSelectClientCertificate(
      CefRefPtr<CefBrowser> browser,
      bool isProxy,
      const CefString& host,
      int port,
      const std::vector<CefString>& key_types,
      const std::vector<CefString>& principals,
      const X509CertificateList& certificates,
      CefRefPtr<CefSelectClientCertificateCallback> callback) {
    return false;
  }

  /* ------------------- IS_OHOS extended ------------------- */
  ///
  /// Called on the browser process UI thread when the url is about to be loaded
  /// into the current Web.
  ///
  /*--cef()--*/
  virtual bool ShouldOverrideUrlLoading(CefRefPtr<CefBrowser> browser,
                                        const CefString& url,
                                        const CefString& method,
                                        bool user_gesture,
                                        bool is_redirect,
                                        bool is_outermost_main_frame,
                                        const CefString& extra_request_headers) {
    return false;
  }

  ///
  /// Called on the UI thread to handle requests for URLs with an invalid
  /// SSL certificate. Return true and call CefCallback methods either in this
  /// method or at a later time to continue or cancel the request. Return false
  /// to cancel the request immediately. If
  /// cef_settings_t.ignore_certificate_errors is set all invalid certificates
  /// will be accepted without calling this method.
  ///
  /*--cef()--*/
  virtual bool OnAllCertificateError(CefRefPtr<CefBrowser> browser,
                                     cef_errorcode_t cert_error,
                                     const CefString& request_url,
                                     const CefString& origin_url,
                                     const CefString& referrer,
                                     bool is_main_frame_request,
                                     bool is_fatal_error,
                                     CefRefPtr<CefSSLInfo> ssl_info,
                                     CefRefPtr<ArkWebCefSslCallback> callback) {
    return false;
  }

  ///
  /// Called to open app link.
  ///
  /*--cef()--*/
  virtual bool OnOpenAppLink(const CefString& url,
                             CefRefPtr<CefOpenAppLinkCallback> callback) {
    return false;
  }

  ///
  /// Called when render process not responding
  ///
  /*--cef()--*/
  virtual void OnRenderProcessNotResponding(CefRefPtr<CefBrowser> browser,
                                            const CefString& referrer,
                                            int pid,
                                            int reason) {}

  ///
  /// Called when render process responding again
  ///
  /*--cef()--*/
  virtual void OnRenderProcessResponding(CefRefPtr<CefBrowser> browser) {}

  ///
  /// Called when mouse hovering the link
  ///
  /*--cef()--*/
  virtual void OnUpdateTargetURL(CefRefPtr<CefBrowser> browser,
                                 const CefString& url) {}

  /*--cef()--*/
  virtual std::string OverrideErrorPage(CefRefPtr<CefBrowser> browser,
                                        const CefString& url,
                                        const CefString& method,
                                        bool user_gesture,
                                        bool is_redirect,
                                        bool is_outermost_main_frame,
                                        const CefString& extra_request_headers,
                                        int error_code,
                                        const CefString& error_text) {
    return "";
  }

  ///
  /// Called on the UI thread to handle requests for URLs with an invalid
  /// SSL certificate. Return true and call CefCallback methods either in this
  /// method or at a later time to continue or cancel the request. Return false
  /// to cancel the request immediately. If
  /// cef_settings_t.ignore_certificate_errors is set all invalid certificates
  /// will be accepted without calling this method.
  ///
  /*--cef()--*/
  virtual bool OnCertificateErrorExt(CefRefPtr<CefBrowser> browser,
                                     cef_errorcode_t cert_error,
                                     const CefString& request_url,
                                     CefRefPtr<CefSSLInfo> ssl_info,
                                     CefRefPtr<ArkWebCefSslCallback> callback) {
    return false;
  }

  CefRefPtr<CefRequestHandlerExt> AsCefRequestHandlerExt() override {
    return this;
  }
};

#endif

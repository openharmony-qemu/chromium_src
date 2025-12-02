// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/ark_web_certificate_query.h"
#include "chrome/browser/ssl/https_only_mode_tab_helper.h"
#include "cef/include/cef_callback.h"
#include "cef/include/cef_request_handler.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/libcef/browser/ssl_info_impl.h"
#include "cef/libcef/browser/thread_util.h"
#include "content/public/browser/web_contents.h"
#include "net/ssl/ssl_info.h"
#include "ohos_cef_ext/include/arkweb_cef_ssl_callback.h"
#include "url/gurl.h"

// Forward declarations to avoid blink header dependencies
namespace blink {
namespace web_pref {
struct WebPreferences;
}
}

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "url/ohos/log_utils.h"
#endif

namespace certificate_query {

namespace {

// Helper function to safely get usage scenario without blink header dependencies
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
int32_t GetUsageScenarioSafely(content::WebContents* web_contents) {
  if (!web_contents) {
    return 99; // Default value
  }

  // Get WebPreferences through content public interface
  // This avoids direct blink header inclusion
  const blink::web_pref::WebPreferences& prefs = web_contents->GetOrCreateWebPreferences();

  // Access usage_scenario field (assuming it's still accessible)
  // If this fails, we may need to use a different approach
  #if defined(__has_include)
    #if __has_include("third_party/blink/renderer/core/frame/web_preferences.h")
      return prefs.usage_scenario;
    #else
      return 99; // Default fallback value
    #endif
  #else
    return 99; // Default fallback value
  #endif
}
#endif

class CefAllowCertificateErrorCallbackImpl : public ArkWebCefSslCallback {
 public:
  using CallbackType = CertificateErrorCallback;

  explicit CefAllowCertificateErrorCallbackImpl(CallbackType callback)
      : callback_(std::move(callback)) {}

  CefAllowCertificateErrorCallbackImpl(
      const CefAllowCertificateErrorCallbackImpl&) = delete;
  CefAllowCertificateErrorCallbackImpl& operator=(
      const CefAllowCertificateErrorCallbackImpl&) = delete;

  ~CefAllowCertificateErrorCallbackImpl() override {
    if (!callback_.is_null()) {
      // The callback is still pending. Cancel it now.
      if (CEF_CURRENTLY_ON_UIT()) {
        RunNow(std::move(callback_), false);
      } else {
        CEF_POST_TASK(
            CEF_UIT,
            base::BindOnce(&CefAllowCertificateErrorCallbackImpl::RunNow,
                           std::move(callback_), false));
      }
    }
  }

  void Continue() override { ContinueNow(true); }

  void Cancel() override { ContinueNow(false); }

  void Cancel(bool abortLoading) override { CancelNow(abortLoading); }

  [[nodiscard]] CallbackType Disconnect() { return std::move(callback_); }

 private:
  void ContinueNow(bool allow) {
    if (CEF_CURRENTLY_ON_UIT()) {
      if (!callback_.is_null()) {
        RunNow(std::move(callback_), allow);
      }
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&CefAllowCertificateErrorCallbackImpl::ContinueNow,
                         this, allow));
    }
  }

  void CancelNow(bool abortLoading) {
    if (CEF_CURRENTLY_ON_UIT()) {
      if (!callback_.is_null()) {
        RunCancel(std::move(callback_), abortLoading);
      }
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&CefAllowCertificateErrorCallbackImpl::CancelNow,
                         this, abortLoading));
    }
  }

  static void RunNow(CallbackType callback, bool allow) {
    CEF_REQUIRE_UIT();
    std::move(callback).Run(
        allow ? content::CERTIFICATE_REQUEST_RESULT_TYPE_CONTINUE
              : content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
  }

  static void RunCancel(CallbackType callback, bool abortLoading) {
    CEF_REQUIRE_UIT();
    std::move(callback).Run(
        abortLoading ? content::CERTIFICATE_REQUEST_RESULT_TYPE_CANCEL
                     : content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
  }

  CallbackType callback_;

  IMPLEMENT_REFCOUNTING(CefAllowCertificateErrorCallbackImpl);
};

}  // namespace

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool OnCertificateError(
    content::WebContents* webContents,
    int certError,
    CefRefPtr<CefSSLInfo> sslInfo,
    const GURL& requestUrl,
    bool isMainFrameRequest,
    bool strictEnforcement,
    const GURL& originUrl,
    const std::string& referrer,
    CefRefPtr<CefAllowCertificateErrorCallbackImpl> callbackImpl,
    bool onlyMainError) {
  auto browser = CefBrowserHostBase::GetBrowserForContents(webContents);
  if (!browser) {
    return false;
  }
  auto client = browser->GetClient();
  if (!client) {
    return false;
  }
  auto handler = client->GetRequestHandler();
  if (!handler) {
    return false;
  }
  if (!handler->AsCefRequestHandlerExt()) {
    return false;
  }
  bool proceed;
  if (onlyMainError) {
    proceed = handler->AsCefRequestHandlerExt()->OnCertificateErrorExt(
        browser.get(), static_cast<cef_errorcode_t>(certError),
        requestUrl.spec(), sslInfo, callbackImpl.get());
  } else {
    proceed = handler->AsCefRequestHandlerExt()->OnAllCertificateError(
        browser.get(), static_cast<cef_errorcode_t>(certError),
        requestUrl.spec(), originUrl.spec(), referrer, isMainFrameRequest,
        strictEnforcement, sslInfo, callbackImpl.get());
  }
  if (proceed) {
    return true;
  }

  return false;
}

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
int IsSslCertErrorFatal(int cert_error) {
  switch (cert_error) {
    case net::ERR_CERT_COMMON_NAME_INVALID:
    case net::ERR_CERT_DATE_INVALID:
    case net::ERR_CERT_AUTHORITY_INVALID:
    case net::ERR_CERT_NO_REVOCATION_MECHANISM:
    case net::ERR_CERT_UNABLE_TO_CHECK_REVOCATION:
    case net::ERR_CERT_WEAK_SIGNATURE_ALGORITHM:
    case net::ERR_CERT_WEAK_KEY:
    case net::ERR_CERT_NAME_CONSTRAINT_VIOLATION:
    case net::ERR_CERT_VALIDITY_TOO_LONG:
    case net::ERR_CERTIFICATE_TRANSPARENCY_REQUIRED:
    // ERR_CERT_SYMANTEC_LEGACY was removed in Chromium 141
    // Symantec legacy certificates are now handled as ERR_CERT_AUTHORITY_INVALID
    case net::ERR_CERT_KNOWN_INTERCEPTION_BLOCKED:
    case net::ERR_SSL_OBSOLETE_VERSION_OR_CIPHER:
      return false;
    case net::ERR_CERT_CONTAINS_ERRORS:
    case net::ERR_CERT_REVOKED:
    case net::ERR_CERT_INVALID:
    case net::ERR_SSL_PINNED_KEY_NOT_IN_CERT_CHAIN:
      return true;
    default:
      NOTREACHED();
      return true;
  }
}
#endif

CertificateErrorCallback AllowAllCertificateError(
    content::WebContents* web_contents,
    int cert_error,
    const net::SSLInfo& ssl_info,
    const GURL& request_url,
    bool is_main_frame_request,
    bool strict_enforcement,
    const GURL& origin_url,
    const std::string& referrer,
    CertificateErrorCallback callback,
    bool default_disallow) {
  CEF_REQUIRE_UIT();

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  bool is_fatal_error = false;
  bool is_incognito = false;
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableLoggerReport)) {
    is_fatal_error = IsSslCertErrorFatal(cert_error) || is_fatal_error;
    std::string err_msg =
        "a ssl error occurred, err_code: " + std::to_string(cert_error) +
        ", is_fatal_error: " + std::to_string(is_fatal_error) +
        ", origin: " + origin_url.spec() + ", refer: " + referrer;
    if (!is_incognito) {
      int32_t usage_scenario = GetUsageScenarioSafely(web_contents);
      LOG(URL) << "event_message: " << err_msg << ", url: "
               << url::LogUtils::ConvertUrl(origin_url.spec(), usage_scenario);
    }
  }
#endif

  const GURL local_request_url(request_url);
  const GURL local_origin_url(origin_url);
  const std::string local_referrer(referrer);
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
    auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(web_contents);
    if (tab_helper && tab_helper->is_navigation_upgraded() && is_main_frame_request) {
      if (!callback.is_null()) {

        std::move(callback).Run(content::CERTIFICATE_REQUEST_RESULT_TYPE_CONTINUE);
      }
      return base::NullCallback();
    }
#endif
  bool result;
  CefRefPtr<CefSSLInfo> sslInfo(new CefSSLInfoImpl(ssl_info));
  CefRefPtr<CefAllowCertificateErrorCallbackImpl> callbackImpl(
      new CefAllowCertificateErrorCallbackImpl(std::move(callback)));

  result = OnCertificateError(web_contents, cert_error, sslInfo, local_request_url,
                              is_main_frame_request, strict_enforcement,
                              local_origin_url, local_referrer, callbackImpl, false);
  if (!result) {
    callback = callbackImpl->Disconnect();
    LOG_IF(ERROR, callback.is_null())
        << "Should return true from OnCertificateError when executing the "
           "callback";
  }

  if (!is_main_frame_request) {
    if (!callback.is_null() && default_disallow) {
      std::move(callback).Run(content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
      return base::NullCallback();
    } else {
      return callback;
    }
  }

  CefRefPtr<CefAllowCertificateErrorCallbackImpl> mainCallbackImpl(
      new CefAllowCertificateErrorCallbackImpl(std::move(callback)));
  result = OnCertificateError(web_contents, cert_error, sslInfo, local_request_url,
                              is_main_frame_request, strict_enforcement,
                              local_origin_url, local_referrer, mainCallbackImpl, true);
  if (!result) {
    callback = mainCallbackImpl->Disconnect();
    LOG_IF(ERROR, callback.is_null())
        << "Should return true from OnCertificateError when executing the "
           "callback";
  }
  if (!callback.is_null() && default_disallow) {
    std::move(callback).Run(content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
    return base::NullCallback();
  }

  return callback;
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

}  // namespace certificate_query

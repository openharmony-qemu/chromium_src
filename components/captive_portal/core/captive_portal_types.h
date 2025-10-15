// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CAPTIVE_PORTAL_CORE_CAPTIVE_PORTAL_TYPES_H_
#define COMPONENTS_CAPTIVE_PORTAL_CORE_CAPTIVE_PORTAL_TYPES_H_

#include <string>

#include "components/captive_portal/core/captive_portal_export.h"

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
#include "net/traffic_annotation/network_traffic_annotation.h"
#endif

namespace captive_portal {

// Possible results of an attempt to detect a captive portal.
enum CaptivePortalResult {
  // There's a confirmed connection to the Internet.
  RESULT_INTERNET_CONNECTED,
  // The URL request received a network or HTTP error, or a non-HTTP response.
  RESULT_NO_RESPONSE,
  // The URL request apparently encountered a captive portal.  It received a
  // a valid HTTP response with a 2xx other than 204, 3xx, or 511 status code.
  RESULT_BEHIND_CAPTIVE_PORTAL,
  RESULT_COUNT
};

// Captive portal type of a given WebContents.
enum class CaptivePortalWindowType {
  // This browser is not used for captive portal resolution, i.e. it's not a
  // captive portal login window.
  kNone,
  // This browser is a popup for captive portal resolution.
  kPopup,
  // This browser is a tab for captive portal resolution.
  kTab,
};

CAPTIVE_PORTAL_EXPORT extern std::string CaptivePortalResultToString(
    CaptivePortalResult result);

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("captive_portal_service", R"(
        semantics {
          sender: "Captive Portal Service"
          description:
            "Checks if the system is behind a captive portal. To do so, makes"
            "an unlogged, dataless connection to a Google server and checks"
            "the response."
          trigger:
            "It is triggered on multiple cases: It is run on certain SSL "
            "errors (ERR_CONNECTION_TIMED_OUT, ERR_SSL_PROTOCOL_ERROR, and all "
            "SSL interstitials)."
          data: "None."
          destination: GOOGLE_OWNED_SERVICE
        }
        policy {
          cookies_allowed: NO
          setting:
            "Users can enable/disable this feature by toggling 'Use a web "
            "service to resolve network errors' in Chromium settings under "
            "Privacy. This feature is enabled by default."
          chrome_policy {
            AlternateErrorPagesEnabled {
              policy_options {mode: MANDATORY}
              AlternateErrorPagesEnabled: false
            }
          }
        })");
#endif

}  // namespace captive_portal

#endif  // COMPONENTS_CAPTIVE_PORTAL_CORE_CAPTIVE_PORTAL_TYPES_H_

// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "components/component_updater/component_updater_url_constants.h"

namespace component_updater {

// The default URL for the v3 protocol service endpoint. In some cases, the
// component updater is allowed to fall back to other URL endpoints, if
// the request to the default URL source fails.
//
// The responses to the requests made to these endpoints are always signed.
//
// The value of |kDefaultUrlSource| can be overridden with
// --component-updater=url-source=someurl.
const char kUpdaterJSONDefaultUrl[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://x.x.x";
#else
    "https://update.googleapis.com/service/update2/json";
#endif

const char kUpdaterJSONFallbackUrl[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://x.x.x";
#else
    "http://update.googleapis.com/service/update2/json";
#endif

}  // namespace component_updater

// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "chrome/browser/devtools/url_constants.h"

const char kRemoteFrontendDomain[] = "chrome-devtools-frontend.appspot.com";
const char kRemoteFrontendBase[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://x.x.x";
#else
    "https://chrome-devtools-frontend.appspot.com/";
#endif
const char kRemoteFrontendPath[] = "serve_file";

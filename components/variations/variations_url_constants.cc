// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "components/variations/variations_url_constants.h"

namespace variations {

// Default server of Variations seed info.
const char kDefaultServerUrl[] =
#if BUILDFLAG(IS_OHOS)
    "https://xxx";
#else
    "https://clientservices.googleapis.com/chrome-variations/seed";
#endif

const char kDefaultInsecureServerUrl[] =
#if BUILDFLAG(IS_OHOS)
    "http://xxx";
#else
    "http://clientservices.googleapis.com/chrome-variations/seed";
#endif

}  // namespace variations

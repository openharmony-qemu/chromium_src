// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/enterprise/data_protection/data_protection_utils.h"

#include <utility>

#include "base/logging.h"
#include "components/dom_distiller/core/url_utils.h"
#include "content/public/browser/render_frame_host.h"
#include "url/gurl.h"
#include "url/origin.h"
#include "url/url_constants.h"

namespace enterprise_data_protection {

GURL GetOriginalUrl(const GURL& url) {
  if (GURL original_url =
          dom_distiller::url_utils::GetOriginalUrlFromDistillerUrl(url);
      original_url.is_valid()) {
    return original_url;
  } else {
    VLOG(1) << __func__ << " got a invalid url: " << original_url;
  }
  return url;
}

GURL GetUrlFromRenderFrameHost(content::RenderFrameHost* rfh) {
  if (!rfh) {
    return GURL();
  }
  auto* main_frame = rfh->GetMainFrame();
  auto url = main_frame->GetLastCommittedURL();
  if (!url.is_valid() || url.IsAboutBlank() || url.IsAboutSrcdoc()) {
    auto origin_url = main_frame->GetLastCommittedOrigin()
                          .GetTupleOrPrecursorTupleIfOpaque()
                          .GetURL();
    if (origin_url.is_valid()) {
      url = std::move(origin_url);
    } else if (!url.is_valid()) {
      url = GURL(url::kAboutBlankURL);
    }
  }
  return GetOriginalUrl(url);
}

}  // namespace enterprise_data_protection

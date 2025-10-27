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

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "ohos_cef_ext/libcef/browser/predictors/predictor_database.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "extensions/common/constants.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/frame_tree_node.h"
#include "content/browser/renderer_host/navigation_request.h"
#include "content/public/common/content_switches.h"
#endif

namespace throttle {
 
namespace {
 
GURL SanitizeUrl(const GURL& url) {
  GURL::Replacements rep;
  rep.ClearUsername();
  rep.ClearPassword();
  rep.ClearQuery();
  return url.ReplaceComponents(rep);
}

void ArkWebRecordVisitedUrl(content::NavigationHandle* navigation_handle) {
  const bool is_main_frame = navigation_handle->IsInMainFrame();
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  // Record the url so that it can be preconnected at the next startup.
  if (is_main_frame) {
    GURL original_url = navigation_handle->GetURL();
    GURL sanitized_url = SanitizeUrl(original_url);

    predictor::VisitedUrlInfo url_info(sanitized_url);
    predictor::PredictorDatabase::GetInstance()->RecordVisitedUrl(url_info);
  }
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
}

bool ArkWebIsExtensionNavigation(content::NavigationHandle* navigation_handle) {
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  if (navigation_handle->GetURL().SchemeIs(extensions::kExtensionScheme)) {
    return true;
  }
  if (navigation_handle->GetURL().SchemeIs(extensions::kArkwebExtensionScheme)) {
    return true;
  }
  return false;
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
CefRefPtr<CefFrame> GetFrameFromGlobalIdFirst(CefRefPtr<CefBrowserHostBase> browser,
  content::GlobalRenderFrameHostId global_id,
  content::GlobalRenderFrameHostId parent_global_id, bool is_main_frame) {
  if (!browser || !browser->browser_info()) {
    return nullptr;
  }
  CefRefPtr<CefFrame> frame = browser->GetFrameForGlobalId(global_id);
  if (!frame) {
    if (is_main_frame) {
      frame = browser->GetMainFrame();
    } else {
      frame = browser->browser_info()->CreateTempSubFrame(parent_global_id);
    }
  }
  return frame;
}
#endif
}  // namespace

}  // namespace throttle
// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_tab_helper.h"

#include <string>

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_factory.h"
#include "third_party/blink/public/mojom/loader/resource_load_info.mojom.h"

using content::BrowserThread;

namespace ohos_predictors {

namespace {

bool IsHandledNavigation(content::NavigationHandle* navigation_handle) {
  return navigation_handle->IsInMainFrame() &&
         !navigation_handle->IsSameDocument() &&
         navigation_handle->GetURL().SchemeIsHTTPOrHTTPS();
}

}  // namespace

LoadingPredictorTabHelper::LoadingPredictorTabHelper(
    content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      content::WebContentsUserData<LoadingPredictorTabHelper>(*web_contents) {
  auto* predictor = LoadingPredictorFactory::GetForBrowserContext(
      web_contents->GetBrowserContext());
  if (predictor) {
    predictor_ = predictor->GetWeakPtr();
  }
}

LoadingPredictorTabHelper::~LoadingPredictorTabHelper() = default;

void LoadingPredictorTabHelper::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!predictor_) {
    return;
  }

  if (!IsHandledNavigation(navigation_handle)) {
    return;
  }

  auto navigation_id = NavigationID(web_contents(), navigation_handle->GetURL(),
                                    navigation_handle->NavigationStart());
  if (!navigation_id.is_valid()) {
    return;
  }

  predictor_->OnNavigationStarted(navigation_id);
}

void LoadingPredictorTabHelper::DidRedirectNavigation(
    content::NavigationHandle* navigation_handle) {}

void LoadingPredictorTabHelper::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!predictor_) {
    return;
  }

  if (!IsHandledNavigation(navigation_handle)) {
    return;
  }

  auto old_navigation_id = NavigationID(
      web_contents(), navigation_handle->GetRedirectChain().front(),
      navigation_handle->NavigationStart());
  auto new_navigation_id =
      NavigationID(web_contents(), navigation_handle->GetURL(),
                   navigation_handle->NavigationStart());
  if (!old_navigation_id.is_valid() || !new_navigation_id.is_valid()) {
    return;
  }

  predictor_->OnNavigationFinished(old_navigation_id, new_navigation_id,
                                   navigation_handle->IsErrorPage());
}

void LoadingPredictorTabHelper::ResourceLoadComplete(
    content::RenderFrameHost* render_frame_host,
    const content::GlobalRequestID& request_id,
    const blink::mojom::ResourceLoadInfo& resource_load_info) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
}

void LoadingPredictorTabHelper::DidLoadResourceFromMemoryCache(
    content::RenderFrameHost* render_frame_host,
    const GURL& url,
    const std::string& mime_type,
    network::mojom::RequestDestination request_destination) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
}

void LoadingPredictorTabHelper::DocumentOnLoadCompletedInPrimaryMainFrame() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(LoadingPredictorTabHelper);

}  // namespace ohos_predictors

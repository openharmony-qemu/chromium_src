// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_TAB_HELPER_H_
#define CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_TAB_HELPER_H_

#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class NavigationHandle;
}  // namespace content

namespace ohos_predictors {

class LoadingPredictor;

// Observes various page load events from the navigation start to onload
// completed and notifies the LoadingPredictor associated with the current
// profile.
//
// All methods must be called from the UI thread.
class LoadingPredictorTabHelper
    : public content::WebContentsObserver,
      public content::WebContentsUserData<LoadingPredictorTabHelper> {
 public:
  LoadingPredictorTabHelper(const LoadingPredictorTabHelper&) = delete;
  LoadingPredictorTabHelper& operator=(const LoadingPredictorTabHelper&) =
      delete;

  ~LoadingPredictorTabHelper() override;

  // content::WebContentsObserver implementation
  void DidStartNavigation(
      content::NavigationHandle* navigation_handle) override;
  void DidRedirectNavigation(
      content::NavigationHandle* navigation_handle) override;
  void DidFinishNavigation(
      content::NavigationHandle* navigation_handle) override;
  void ResourceLoadComplete(
      content::RenderFrameHost* render_frame_host,
      const content::GlobalRequestID& request_id,
      const blink::mojom::ResourceLoadInfo& resource_load_info) override;
  void DidLoadResourceFromMemoryCache(
      content::RenderFrameHost* render_frame_host,
      const GURL& url,
      const std::string& mime_type,
      network::mojom::RequestDestination request_destination) override;
  void DocumentOnLoadCompletedInPrimaryMainFrame() override;

  void SetLoadingPredictorForTesting(
      base::WeakPtr<LoadingPredictor> predictor) {
    predictor_ = predictor;
  }

 private:
  explicit LoadingPredictorTabHelper(content::WebContents* web_contents);
  friend class content::WebContentsUserData<LoadingPredictorTabHelper>;

  // Owned by profile.
  base::WeakPtr<LoadingPredictor> predictor_;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace ohos_predictors

#endif  // CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_TAB_HELPER_H_

// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_H_
#define CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/gtest_prod_util.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "components/keyed_service/core/keyed_service.h"
#include "include/cef_resource_handler.h"
#include "libcef/browser/net_service/url_loader_factory_getter.h"
#include "ohos_cef_ext/libcef/browser/predictors/navigation_id.h"
#include "ohos_cef_ext/libcef/browser/predictors/preconnect_manager.h"
#include "ohos_cef_ext/libcef/browser/predictors/resource_prefetch_predictor.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "url/gurl.h"

namespace ohos_predictors {

struct PreRequestInfo {
  GURL url;
  std::string method;
  std::string request_body;
};

// Entry point for the Loading predictor.
// From a high-level request (GURL and motivation) and a database of historical
// data, initiates predictive actions to speed up page loads.
//
// Also listens to main frame requests/redirects/responses to initiate and
// cancel page load hints.
//
// See ResourcePrefetchPredictor for a description of the resource prefetch
// predictor.
//
// All methods must be called from the UI thread.
class LoadingPredictor : public KeyedService,
                         public PreconnectManager::Delegate {
 public:
  LoadingPredictor(const LoadingPredictorConfig& config,
                   content::BrowserContext* context);
  ~LoadingPredictor() override;

  LoadingPredictor(const LoadingPredictor&) = delete;
  LoadingPredictor& operator=(const LoadingPredictor&) = delete;

  // Hints that a page load is expected for |url|, with the hint coming from a
  // given |origin|. May trigger actions, such as prefetch and/or preconnect.
  void PrepareForPageLoad(const GURL& url,
                          HintOrigin origin,
                          bool preconnectable = false,
                          int num_sockets = 1);

  // Indicates that a page load hint is no longer active.
  void CancelPageLoadHint(const GURL& url);

  // Starts initialization, will complete asynchronously.
  void StartInitialization();

  PreconnectManager* preconnect_manager();

  void PrefetchResource(
      const std::shared_ptr<PreRequestInfo>& requestInfo,
      const std::map<std::string, std::string>& request_headers,
      const std::string& cache_key,
      const uint32_t& cache_valid_time);

  void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list);

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      const GURL& url,
      const std::string& cache_key);

  // KeyedService:
  void Shutdown() override;

  void OnNavigationStarted(const NavigationID& navigation_id);
  void OnNavigationFinished(const NavigationID& old_navigation_id,
                            const NavigationID& new_navigation_id,
                            bool is_error_page);

  base::WeakPtr<LoadingPredictor> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

  // PreconnectManager::Delegate:
  void PreconnectFinished(std::unique_ptr<PreconnectStats> stats) override;

 private:
  // Cancels an active hint, from its iterator inside |active_hints_|. If the
  // iterator is .end(), does nothing. Returns the iterator after deletion of
  // the entry.
  std::map<GURL, base::TimeTicks>::iterator CancelActiveHint(
      std::map<GURL, base::TimeTicks>::iterator hint_it);
  void CleanupAbandonedHintsAndNavigations(const NavigationID& navigation_id);

  // May start preconnect and preresolve jobs according to |requests| for |url|
  // with a given hint |origin|.
  void MaybeAddPreconnect(const GURL& url,
                          std::vector<PreconnectRequest> requests,
                          HintOrigin origin);
  // If a preconnect exists for |url|, stop it.
  void MaybeRemovePreconnect(const GURL& url);

  // May start a preconnect or a preresolve for |url|. |preconnectable|
  // indicates if preconnect is possible.
  void HandleOmniboxHint(const GURL& url,
                         bool preconnectable,
                         int num_sockets = 1);

  void OnSimpleURLLoaderComplete(network::SimpleURLLoader* url_loader,
                                 const std::string& cache_key,
                                 const size_t& request_id,
                                 const uint32_t& cache_valid_time,
                                 std::unique_ptr<std::string> responce_body);

  LoadingPredictorConfig config_;
  raw_ptr<content::BrowserContext> context_;
  std::unique_ptr<PreconnectManager> preconnect_manager_;
  std::map<GURL, base::TimeTicks> active_hints_;
  std::set<NavigationID> active_navigations_;
  bool shutdown_ = false;
  size_t total_hints_activated_ = 0;

  url::Origin last_omnibox_origin_;
  base::TimeTicks last_omnibox_preconnect_time_;
  base::TimeTicks last_omnibox_preresolve_time_;

  scoped_refptr<net_service::URLLoaderFactoryGetter> loader_factory_getter_;
  std::map<size_t, std::unique_ptr<network::SimpleURLLoader>> loader_map_;
  // Next request ID
  size_t next_request_id_ = 0;

  base::WeakPtrFactory<LoadingPredictor> weak_factory_{this};
};

}  // namespace ohos_predictors

#endif  // CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_H_

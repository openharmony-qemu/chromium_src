// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"

#include <algorithm>
#include <vector>

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "libcef/common/response_impl.h"
#include "net/base/network_anonymization_key.h"
#include "ohos_cef_ext/libcef/browser/predictors/navigation_id.h"
#include "ohos_cef_ext/libcef/browser/predictors/resource_prefetch_predictor.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace ohos_predictors {

namespace {

const base::TimeDelta kMinDelayBetweenPreresolveRequests = base::Seconds(60);
const base::TimeDelta kMinDelayBetweenPreconnectRequests = base::Seconds(10);

#define MAX_POST_NUM 6
struct ResourceInfo {
  std::string cache_key;
  scoped_refptr<net::HttpResponseHeaders> response_headers;
  std::string response_body;
  base::Time response_time;
  uint32_t cache_valid_time = 300;  // 默认有效期为300s
};
std::list<std::shared_ptr<ResourceInfo>> g_predictor_post_cache_;

class PredictorResourceHandler : public CefResourceHandler {
 public:
  explicit PredictorResourceHandler(
      const GURL& url,
      std::shared_ptr<ResourceInfo> resource_response)
      : url_(url), resource_response_(resource_response) {}
  bool Open(CefRefPtr<CefRequest> request,
            bool& handle_request,
            CefRefPtr<CefCallback> callback) override {
    // Continue immediately.
    handle_request = true;
    return true;
  }

  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64_t& response_length,
                          CefString& redirectUrl) override {
    // 填充下response
    const net::HttpResponseHeaders* headers =
        resource_response_.get()->response_headers.get();
    reinterpret_cast<CefResponseImpl*>(response.get())
        ->SetResponseHeaders(*headers);
  }

  bool Read(void* data_out,
            int bytes_to_read,
            int& bytes_read,
            CefRefPtr<CefResourceReadCallback> callback) override {
    LOG(INFO) << "intercept ReadStringData";
    bool has_data = false;
    bytes_read = 0;

    data_ = resource_response_->response_body;
    if (offset_ < data_.length()) {
      // Copy the next block of data info the buffer.
      int transfer_size =
          std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
      if (memcpy_s(data_out, bytes_to_read, data_.c_str() + offset_,
                   transfer_size) != EOK) {
        LOG(WARNING) << "loading predictor prefetch memcpy failed!";
        bytes_read = net::ERR_FAILED;
        return false;
      }
      offset_ += transfer_size;

      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }

  void Cancel() override {}

 private:
  GURL url_;
  std::shared_ptr<ResourceInfo> resource_response_;
  std::string data_;
  size_t offset_ = 0;

  IMPLEMENT_REFCOUNTING(PredictorResourceHandler);
};

// Returns true iff |prediction| is not empty.
bool AddInitialUrlToPreconnectPrediction(const GURL& initial_url,
                                         PreconnectPrediction* prediction) {
  url::Origin initial_origin = url::Origin::Create(initial_url);
  // Open minimum 2 sockets to the main frame host to speed up the loading if a
  // main page has a redirect to the same host. This is because there can be a
  // race between reading the server redirect response and sending a new request
  // while the connection is still in use.
  static const int kMinSockets = 6;

  if (!prediction->requests.empty() &&
      prediction->requests.front().origin == initial_origin) {
    prediction->requests.front().num_sockets =
        std::max(prediction->requests.front().num_sockets, kMinSockets);
  } else if (!initial_origin.opaque() &&
             (initial_origin.scheme() == url::kHttpScheme ||
              initial_origin.scheme() == url::kHttpsScheme)) {
    prediction->requests.emplace(prediction->requests.begin(), initial_origin,
                                 kMinSockets,
                                 net::NetworkAnonymizationKey::CreateSameSite(
                                     net::SchemefulSite(initial_origin)));
  }

  return !prediction->requests.empty();
}

}  // namespace

LoadingPredictor::LoadingPredictor(const LoadingPredictorConfig& config,
                                   content::BrowserContext* context)
    : config_(config), context_(context) {}

LoadingPredictor::~LoadingPredictor() {
  DCHECK(shutdown_);
}

void LoadingPredictor::PrepareForPageLoad(const GURL& url,
                                          HintOrigin origin,
                                          bool preconnectable,
                                          int num_sockets) {
  if (shutdown_) {
    return;
  }

  if (origin == HintOrigin::OMNIBOX) {
    // Omnibox hints are lightweight and need a special treatment.
    HandleOmniboxHint(url, preconnectable, num_sockets);
    return;
  }

  if (active_hints_.find(url) != active_hints_.end()) {
    return;
  }

  bool has_preconnect_prediction = false;
  PreconnectPrediction prediction;
  has_preconnect_prediction =
      AddInitialUrlToPreconnectPrediction(url, &prediction);

  if (!has_preconnect_prediction) {
    return;
  }

  ++total_hints_activated_;
  active_hints_.emplace(url, base::TimeTicks::Now());
  if (IsPreconnectAllowed(context_)) {
    MaybeAddPreconnect(url, std::move(prediction.requests), origin);
  }
}

void LoadingPredictor::CancelPageLoadHint(const GURL& url) {
  if (shutdown_) {
    return;
  }

  CancelActiveHint(active_hints_.find(url));
}

void LoadingPredictor::StartInitialization() {
  if (shutdown_) {
    shutdown_ = false;
  }
}

PreconnectManager* LoadingPredictor::preconnect_manager() {
  if (shutdown_ || !IsPreconnectFeatureEnabled()) {
    return nullptr;
  }

  if (!preconnect_manager_) {
    preconnect_manager_ =
        std::make_unique<PreconnectManager>(GetWeakPtr(), context_);
  }

  return preconnect_manager_.get();
}

void LoadingPredictor::Shutdown() {
  DCHECK(!shutdown_);
  shutdown_ = true;
}

void LoadingPredictor::OnNavigationStarted(const NavigationID& navigation_id) {
  if (shutdown_) {
    return;
  }

  CleanupAbandonedHintsAndNavigations(navigation_id);
  active_navigations_.emplace(navigation_id);
  PrepareForPageLoad(navigation_id.main_frame_url, HintOrigin::NAVIGATION);
}

void LoadingPredictor::OnNavigationFinished(
    const NavigationID& old_navigation_id,
    const NavigationID& new_navigation_id,
    bool is_error_page) {
  if (shutdown_) {
    return;
  }

  active_navigations_.erase(old_navigation_id);
  CancelPageLoadHint(old_navigation_id.main_frame_url);
}

std::map<GURL, base::TimeTicks>::iterator LoadingPredictor::CancelActiveHint(
    std::map<GURL, base::TimeTicks>::iterator hint_it) {
  if (hint_it == active_hints_.end()) {
    return hint_it;
  }

  const GURL& url = hint_it->first;
  MaybeRemovePreconnect(url);
  return active_hints_.erase(hint_it);
}

void LoadingPredictor::CleanupAbandonedHintsAndNavigations(
    const NavigationID& navigation_id) {
  base::TimeTicks time_now = base::TimeTicks::Now();
  const base::TimeDelta max_navigation_age =
      base::Seconds(config_.max_navigation_lifetime_seconds);

  // Hints.
  for (auto it = active_hints_.begin(); it != active_hints_.end();) {
    base::TimeDelta prefetch_age = time_now - it->second;
    if (prefetch_age > max_navigation_age) {
      // Will go to the last bucket in the duration reported in
      // CancelActiveHint() meaning that the duration was unlimited.
      it = CancelActiveHint(it);
    } else {
      ++it;
    }
  }

  // Navigations.
  for (auto it = active_navigations_.begin();
       it != active_navigations_.end();) {
    if ((it->tab_id == navigation_id.tab_id) ||
        (time_now - it->creation_time > max_navigation_age)) {
      CancelActiveHint(active_hints_.find(it->main_frame_url));
      it = active_navigations_.erase(it);
    } else {
      ++it;
    }
  }
}

void LoadingPredictor::MaybeAddPreconnect(
    const GURL& url,
    std::vector<PreconnectRequest> requests,
    HintOrigin origin) {
  DCHECK(!shutdown_);
  if (preconnect_manager()) {
    preconnect_manager()->Start(url, std::move(requests));
  }
}

void LoadingPredictor::MaybeRemovePreconnect(const GURL& url) {
  DCHECK(!shutdown_);
  if (!preconnect_manager_) {
    return;
  }

  preconnect_manager_->Stop(url);
}

void LoadingPredictor::HandleOmniboxHint(const GURL& url,
                                         bool preconnectable,
                                         int num_sockets) {
  if (!url.is_valid() || !url.has_host() || !IsPreconnectAllowed(context_)) {
    return;
  }

  url::Origin origin = url::Origin::Create(url);
  bool is_new_origin = origin != last_omnibox_origin_;
  last_omnibox_origin_ = origin;
  net::SchemefulSite site = net::SchemefulSite(origin);
  auto network_anonymization_key =
      net::NetworkAnonymizationKey::CreateSameSite(site);
  base::TimeTicks now = base::TimeTicks::Now();
  if (preconnectable) {
    if (is_new_origin || now - last_omnibox_preconnect_time_ >=
                             kMinDelayBetweenPreconnectRequests) {
      last_omnibox_preconnect_time_ = now;
      if (preconnect_manager()) {
        preconnect_manager()->StartPreconnectUrl(
            url, true, network_anonymization_key, num_sockets);
      }
    }
    return;
  }

  if (is_new_origin || now - last_omnibox_preresolve_time_ >=
                           kMinDelayBetweenPreresolveRequests) {
    last_omnibox_preresolve_time_ = now;
    if (preconnect_manager()) {
      preconnect_manager()->StartPreresolveHost(url, network_anonymization_key);
    }
  }
}

void LoadingPredictor::PreconnectFinished(
    std::unique_ptr<PreconnectStats> stats) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (shutdown_) {
    return;
  }

  DCHECK(stats);
  active_hints_.erase(stats->url);
}

void LoadingPredictor::PrefetchResource(
    const std::shared_ptr<PreRequestInfo>& request_info,
    const std::map<std::string, std::string>& request_headers,
    const std::string& cache_key,
    const uint32_t& cache_valid_time) {
  TRACE_EVENT1("net", "LoadingPredictor::PrefetchResource", "url",
               request_info->url.spec());
  // Build resource request.
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = request_info->url;
  resource_request->method = request_info->method;
  std::string output;
  for (auto& header : request_headers) {
    base::StringAppendF(&output, "%s: %s\r\n", header.first.c_str(),
                        header.second.c_str());
  }
  output.append("\r\n");
  resource_request->headers.AddHeadersFromString(output);
  // Send resource request.
  auto loader = network::SimpleURLLoader::Create(std::move(resource_request),
                                                 MISSING_TRAFFIC_ANNOTATION);
  if (!request_info->request_body.empty()) {
    loader->AttachStringForUpload(request_info->request_body,
                                  "application/x-www-form-urlencoded");
  }
  scoped_refptr<net_service::URLLoaderFactoryGetter> loader_factory_getter;
  loader_factory_getter =
      net_service::URLLoaderFactoryGetter::Create(nullptr, context_);
  loader_factory_getter_ = loader_factory_getter;
  auto loader_factory = loader_factory_getter_->GetURLLoaderFactory();
  loader->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
      loader_factory.get(),
      base::BindOnce(&LoadingPredictor::OnSimpleURLLoaderComplete,
                     weak_factory_.GetWeakPtr(), loader.get(), cache_key,
                     next_request_id_, cache_valid_time));
  loader_map_.emplace(next_request_id_++, std::move(loader));
}

void LoadingPredictor::ClearPrefetchedResource(
    const std::vector<std::string>& cache_key_list) {
  for (auto& cache_key : cache_key_list) {
    for (auto it = g_predictor_post_cache_.begin();
         it != g_predictor_post_cache_.end(); it++) {
      if ((*it)->cache_key == cache_key) {
        TRACE_EVENT1("net", "clear prefetched post cache", "cache_key",
                     cache_key);
        g_predictor_post_cache_.erase(it);
        break;
      }
    }
  }
}

void LoadingPredictor::OnSimpleURLLoaderComplete(
    network::SimpleURLLoader* url_loader,
    const std::string& cache_key,
    const size_t& request_id,
    const uint32_t& cache_valid_time,
    std::unique_ptr<std::string> response_body) {
  TRACE_EVENT1("net", "LoadingPredictor::OnSimpleURLLoaderComplete", "cacheKey",
               cache_key);
  std::shared_ptr<ResourceInfo> resource = std::make_shared<ResourceInfo>();
  resource->cache_key = cache_key;
  if (url_loader->ResponseInfo() && url_loader->ResponseInfo()->headers) {
    if (url_loader->ResponseInfo()->headers->response_code() != 200) {
      LOG(ERROR) << "prefetch resource failed.";
      return;
    }
    resource->response_time = url_loader->ResponseInfo()->response_time;
    resource->response_headers = url_loader->ResponseInfo()->headers;
  }
  if (cache_valid_time != 0) {
    resource->cache_valid_time = cache_valid_time;
  }

  if (response_body.get()) {
    resource->response_body = std::move(*response_body);
  }

  for (auto it = g_predictor_post_cache_.begin();
       it != g_predictor_post_cache_.end(); it++) {
    if ((*it)->cache_key == cache_key) {
      TRACE_EVENT0("net", "update the old same post cache");
      g_predictor_post_cache_.erase(it);
      break;
    }
  }

  if (g_predictor_post_cache_.size() == MAX_POST_NUM) {
    TRACE_EVENT0("net", "remove the oldest post cache");
    g_predictor_post_cache_.pop_front();
  }
  TRACE_EVENT0("net", "add new post cache");
  g_predictor_post_cache_.push_back(resource);
  loader_map_.erase(request_id);
}

CefRefPtr<CefResourceHandler> LoadingPredictor::GetResourceHandler(
    const GURL& url,
    const std::string& cache_key) {
  TRACE_EVENT2("net", "LoadingPredictor::GetResourceHandler", "url", url.spec(),
               "cacheKey", cache_key);
  std::shared_ptr<ResourceInfo> resource_response;
  std::list<std::shared_ptr<ResourceInfo>>::iterator it;
  for (it = g_predictor_post_cache_.begin();
       it != g_predictor_post_cache_.end(); it++) {
    if ((*it)->cache_key == cache_key) {
      resource_response = *it;
      break;
    }
  }
  if (resource_response == nullptr) {
    TRACE_EVENT0("net", "get no prefetched resource");
    return nullptr;
  }

  base::TimeDelta response_time_in_cache =
      base::Time::Now() - resource_response->response_time;
  if (response_time_in_cache >= base::TimeDelta() &&
      response_time_in_cache <
          base::Seconds(resource_response->cache_valid_time)) {
    TRACE_EVENT0("net", "get prefetched resource");
    return new PredictorResourceHandler(url, resource_response);
  }

  if (it != g_predictor_post_cache_.end()) {
    g_predictor_post_cache_.erase(it);
  }
  TRACE_EVENT0("net", "prefetched resource is invalid.");
  return nullptr;
}

}  // namespace ohos_predictors

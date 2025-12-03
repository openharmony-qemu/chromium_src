// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/preconnect_manager.h"

#include <utility>

#include "base/functional/bind.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"
#include "ohos_cef_ext/libcef/browser/predictors/resource_prefetch_predictor.h"
#include "services/network/public/mojom/network_context.mojom.h"

namespace ohos_predictors {

const net::NetworkTrafficAnnotationTag
    kLoadingPredictorPreconnectTrafficAnnotation =
        net::DefineNetworkTrafficAnnotation("predictive_preconnect",
                                            R"(
    semantics {
      sender: "Loading Predictor"
      description:
        "This request is issued near the start of a navigation to "
        "speculatively fetch resources that resulting page is predicted to "
        "request."
      trigger:
        "Navigating Chrome (by clicking on a link, bookmark, history item, "
        "using session restore, etc)."
      data:
        "Arbitrary site-controlled data can be included in the URL."
        "Requests may include cookies."
      destination: WEBSITE
    }
    policy {
      cookies_allowed: YES
      cookies_store: "user"
      setting:
        "There are a number of ways to prevent this request:"
        "A) Disable predictive operations under Settings > Performance "
        "   > Preload pages for faster browsing and searching,"
        "B) Disable 'Make searches and browsing better' under Settings > "
        "   Sync and Google services > Make searches and browsing better"
      chrome_policy {
        URLBlocklist {
          URLBlocklist: { entries: '*' }
        }
      }
      chrome_policy {
        URLAllowlist {
          URLAllowlist { }
        }
      }
    }
    comments:
      "This feature can be safely disabled, but enabling it may result in "
      "faster page loads. Using either URLBlocklist or URLAllowlist policies "
      "(or a combination of both) limits the scope of these requests."
)");

const bool kAllowCredentialsOnPreconnectByDefault = true;

PreconnectedRequestStats::PreconnectedRequestStats(const url::Origin& origin,
                                                   bool was_preconnected)
    : origin(origin), was_preconnected(was_preconnected) {}

PreconnectedRequestStats::PreconnectedRequestStats(
    const PreconnectedRequestStats& other) = default;
PreconnectedRequestStats::~PreconnectedRequestStats() = default;

PreconnectStats::PreconnectStats(const GURL& url)
    : url(url), start_time(base::TimeTicks::Now()) {}
PreconnectStats::~PreconnectStats() = default;

PreresolveInfo::PreresolveInfo(const GURL& url, size_t count)
    : url(url),
      queued_count(count),
      inflight_count(0),
      was_canceled(false),
      stats(std::make_unique<PreconnectStats>(url)) {}

PreresolveInfo::~PreresolveInfo() = default;

PreresolveJob::PreresolveJob(
    const GURL& url,
    int num_sockets,
    bool allow_credentials,
    net::NetworkAnonymizationKey network_anonymization_key,
    PreresolveInfo* info)
    : url(url),
      num_sockets(num_sockets),
      allow_credentials(allow_credentials),
      network_anonymization_key(std::move(network_anonymization_key)),
      info(info) {
  DCHECK_GE(num_sockets, 0);
}

PreresolveJob::PreresolveJob(PreconnectRequest preconnect_request,
                             PreresolveInfo* info)
    : url(preconnect_request.origin.GetURL()),
      num_sockets(preconnect_request.num_sockets),
      allow_credentials(preconnect_request.allow_credentials),
      network_anonymization_key(
          std::move(preconnect_request.network_anonymization_key)),
      info(info) {
  DCHECK_GE(num_sockets, 0);
}

PreresolveJob::PreresolveJob(PreresolveJob&& other) = default;
PreresolveJob::~PreresolveJob() = default;

PreconnectManager::PreconnectManager(base::WeakPtr<Delegate> delegate,
                                     content::BrowserContext* context)
    : delegate_(std::move(delegate)),
      context_(context),
      inflight_preresolves_count_(0) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  DCHECK(context_);
}

PreconnectManager::~PreconnectManager() = default;

void PreconnectManager::Start(const GURL& url,
                              std::vector<PreconnectRequest> requests) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  PreresolveInfo* info;
  if (preresolve_info_.find(url) == preresolve_info_.end()) {
    auto iterator_and_whether_inserted = preresolve_info_.emplace(
        url, std::make_unique<PreresolveInfo>(url, requests.size()));
    info = iterator_and_whether_inserted.first->second.get();
  } else {
    info = preresolve_info_.find(url)->second.get();
    info->queued_count += requests.size();
  }

  for (auto& request : requests) {
    PreresolveJobId job_id = preresolve_jobs_.Add(
        std::make_unique<PreresolveJob>(std::move(request), info));
    queued_jobs_.push_back(job_id);
  }

  TryToLaunchPreresolveJobs();
}

void PreconnectManager::StartPreresolveHost(
    const GURL& url,
    const net::NetworkAnonymizationKey& network_anonymization_key) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (!url.SchemeIsHTTPOrHTTPS()) {
    return;
  }
  PreresolveJobId job_id = preresolve_jobs_.Add(std::make_unique<PreresolveJob>(
      url.DeprecatedGetOriginAsURL(), 0, kAllowCredentialsOnPreconnectByDefault,
      network_anonymization_key, nullptr));
  queued_jobs_.push_front(job_id);

  TryToLaunchPreresolveJobs();
}

void PreconnectManager::StartPreresolveHosts(
    const std::vector<std::string>& hostnames,
    const net::NetworkAnonymizationKey& network_anonymization_key) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // Push jobs in front of the queue due to higher priority.
  for (auto it = hostnames.rbegin(); it != hostnames.rend(); ++it) {
    PreresolveJobId job_id =
        preresolve_jobs_.Add(std::make_unique<PreresolveJob>(
            GURL("http://" + *it), 0, kAllowCredentialsOnPreconnectByDefault,
            network_anonymization_key, nullptr));
    queued_jobs_.push_front(job_id);
  }

  TryToLaunchPreresolveJobs();
}

void PreconnectManager::StartPreconnectUrl(
    const GURL& url,
    bool allow_credentials,
    net::NetworkAnonymizationKey network_anonymization_key,
    int num_sockets) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (!url.SchemeIsHTTPOrHTTPS()) {
    return;
  }
  PreresolveJobId job_id = preresolve_jobs_.Add(std::make_unique<PreresolveJob>(
      url.DeprecatedGetOriginAsURL(), num_sockets, allow_credentials,
      std::move(network_anonymization_key), nullptr));
  queued_jobs_.push_front(job_id);

  TryToLaunchPreresolveJobs();
}

void PreconnectManager::Stop(const GURL& url) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto it = preresolve_info_.find(url);
  if (it == preresolve_info_.end()) {
    return;
  }

  it->second->was_canceled = true;
}

void PreconnectManager::PreconnectUrl(
    const GURL& url,
    int num_sockets,
    bool allow_credentials,
    const net::NetworkAnonymizationKey& network_anonymization_key) const {
  DCHECK(url.DeprecatedGetOriginAsURL() == url);
  DCHECK(url.SchemeIsHTTPOrHTTPS());
  if (observer_) {
    observer_->OnPreconnectUrl(url, num_sockets, allow_credentials);
  }

  auto* network_context = GetNetworkContext();
  if (!network_context) {
    return;
  }
  network_context->PreconnectSockets(
      num_sockets, url,
      allow_credentials ? network::mojom::CredentialsMode::kInclude
                        : network::mojom::CredentialsMode::kOmit,
      network_anonymization_key, net::MutableNetworkTrafficAnnotationTag(),
      std::nullopt, mojo::NullRemote());
}

std::unique_ptr<ResolveHostClientImpl> PreconnectManager::PreresolveUrl(
    const GURL& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    ResolveHostCallback callback) const {
  DCHECK(url.DeprecatedGetOriginAsURL() == url);
  DCHECK(url.SchemeIsHTTPOrHTTPS());

  auto* network_context = GetNetworkContext();

  return std::make_unique<ResolveHostClientImpl>(
      url, network_anonymization_key, std::move(callback), network_context);
}

std::unique_ptr<ProxyLookupClientImpl> PreconnectManager::LookupProxyForUrl(
    const GURL& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    ProxyLookupCallback callback) const {
  DCHECK(url.DeprecatedGetOriginAsURL() == url);
  DCHECK(url.SchemeIsHTTPOrHTTPS());

  auto* network_context = GetNetworkContext();
  if (!network_context) {
    std::move(callback).Run(false);
    return nullptr;
  }

  return std::make_unique<ProxyLookupClientImpl>(
      url, network_anonymization_key, std::move(callback), network_context);
}

void PreconnectManager::TryToLaunchPreresolveJobs() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  while (!queued_jobs_.empty() &&
         inflight_preresolves_count_ < kMaxInflightPreresolves) {
    auto job_id = queued_jobs_.front();
    queued_jobs_.pop_front();
    PreresolveJob* job = preresolve_jobs_.Lookup(job_id);
    DCHECK(job);
    PreresolveInfo* info = job->info;

    if (!(info && info->was_canceled)) {
      // This is used to avoid issuing DNS requests when a fixed proxy
      // configuration is in place, which improves efficiency, and is also
      // important if the unproxied DNS may contain incorrect entries.
      job->proxy_lookup_client = LookupProxyForUrl(
          job->url, job->network_anonymization_key,
          base::BindOnce(&PreconnectManager::OnProxyLookupFinished,
                         weak_factory_.GetWeakPtr(), job_id));
      if (info) {
        ++info->inflight_count;
      }
      ++inflight_preresolves_count_;
    } else {
      preresolve_jobs_.Remove(job_id);
    }

    if (info) {
      DCHECK_LE(1u, info->queued_count);
      --info->queued_count;
      if (info->is_done()) {
        AllPreresolvesForUrlFinished(info);
      }
    }
  }
}

void PreconnectManager::OnPreresolveFinished(PreresolveJobId job_id,
                                             bool success) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  PreresolveJob* job = preresolve_jobs_.Lookup(job_id);
  DCHECK(job);
  if (!job) {
    return;
  }
  if (observer_) {
    observer_->OnPreresolveFinished(job->url, job->network_anonymization_key,
                                    success);
  }

  job->resolve_host_client = nullptr;
  FinishPreresolveJob(job_id, success);
}

void PreconnectManager::OnProxyLookupFinished(PreresolveJobId job_id,
                                              bool success) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  PreresolveJob* job = preresolve_jobs_.Lookup(job_id);
  DCHECK(job);
  if (!job) {
    return;
  }
  if (observer_) {
    observer_->OnProxyLookupFinished(job->url, job->network_anonymization_key,
                                     success);
  }

  job->proxy_lookup_client = nullptr;
  if (success) {
    FinishPreresolveJob(job_id, success);
  } else {
    job->resolve_host_client =
        PreresolveUrl(job->url, job->network_anonymization_key,
                      base::BindOnce(&PreconnectManager::OnPreresolveFinished,
                                     weak_factory_.GetWeakPtr(), job_id));
  }
}

void PreconnectManager::FinishPreresolveJob(PreresolveJobId job_id,
                                            bool success) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  PreresolveJob* job = preresolve_jobs_.Lookup(job_id);
  DCHECK(job);

  if (!job) {
    return;
  }
  bool need_preconnect = success && job->need_preconnect();

  if (need_preconnect) {
    PreconnectUrl(job->url, job->num_sockets, job->allow_credentials,
                  job->network_anonymization_key);
  }

  PreresolveInfo* info = job->info;
  if (info && info->stats) {
    info->stats->requests_stats.emplace_back(url::Origin::Create(job->url),
                                             need_preconnect);
  }
  preresolve_jobs_.Remove(job_id);
  --inflight_preresolves_count_;
  if (info) {
    DCHECK_LE(1u, info->inflight_count);
    --info->inflight_count;
  }
  if (info && info->is_done()) {
    AllPreresolvesForUrlFinished(info);
  }
  TryToLaunchPreresolveJobs();
}

void PreconnectManager::AllPreresolvesForUrlFinished(PreresolveInfo* info) {
  if (!info) {
    return;
  }
  DCHECK(info);
  DCHECK(info->is_done());
  auto it = preresolve_info_.find(info->url);
  DCHECK(it != preresolve_info_.end());
  DCHECK(info == it->second.get());
  if (delegate_) {
    delegate_->PreconnectFinished(std::move(info->stats));
  }
  preresolve_info_.erase(it);
}

network::mojom::NetworkContext* PreconnectManager::GetNetworkContext() const {
  if (network_context_) {
    return network_context_;
  }

  auto* network_context =
      context_->GetDefaultStoragePartition()->GetNetworkContext();
  DCHECK(network_context);
  return network_context;
}

}  // namespace ohos_predictors

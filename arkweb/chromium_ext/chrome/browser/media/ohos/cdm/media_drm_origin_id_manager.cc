// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/media/ohos/cdm/media_drm_origin_id_manager.h"

#include <memory>
#include <utility>

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/json/values_util.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/bind_post_task.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/time/time.h"
#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "content/public/browser/network_service_instance.h"
#include "content/public/browser/provision_fetcher_factory.h"
#include "media/base/media_switches.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/base/provision_fetcher.h"
#include "services/network/public/cpp/network_connection_tracker.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "third_party/widevine/cdm/widevine_cdm_common.h"

namespace {

const char kMediaDrmOriginIds[] = "media.media_drm_origin_ids";
const char kExpirableToken[] = "expirable_token";
const char kOriginIds[] = "origin_ids";

constexpr int kMaxPreProvisionedOriginIds = 2;
constexpr int kUMAMaxPreProvisionedOriginIds = 10;
constexpr base::TimeDelta kExpirationDelta = base::Hours(24);
constexpr base::TimeDelta kStartupDelay = base::Minutes(1);
constexpr base::TimeDelta kCheckDelay = base::Minutes(5);
static_assert(kCheckDelay > kStartupDelay,
              "Must allow time for pre-provisioning to run first");

void SetExpirableToken(PrefService* const pref_service) {
  ScopedDictPrefUpdate update(pref_service, kMediaDrmOriginIds);
  update->Set(kExpirableToken,
              base::TimeToValue(base::Time::Now() + kExpirationDelta));
}

void RemoveExpirableToken(base::Value::Dict& origin_id_dict) {
  origin_id_dict.Remove(kExpirableToken);
}

bool CanPreProvision(bool is_per_application_provisioning_supported,
                     base::Value::Dict& origin_id_dict) {
  if (is_per_application_provisioning_supported) {
    return true;
  }

  const base::Value* token_value = origin_id_dict.Find(kExpirableToken);
  if (!token_value) {
    return false;
  }

  absl::optional<base::Time> expiration_time = base::ValueToTime(*token_value);
  if (!expiration_time) {
    RemoveExpirableToken(origin_id_dict);
    return false;
  }

  if (base::Time::Now() > *expiration_time) {
    RemoveExpirableToken(origin_id_dict);
    return false;
  }

  return true;
}

int CountAvailableOriginIds(const base::Value::Dict& origin_id_dict) {
  const base::Value::List* origin_ids = origin_id_dict.FindList(kOriginIds);
  if (!origin_ids) {
    return 0;
  }

  return origin_ids->size();
}

base::UnguessableToken TakeFirstOriginId(PrefService* const pref_service) {
  ScopedDictPrefUpdate update(pref_service, kMediaDrmOriginIds);

  base::Value::List* origin_ids = update->FindList(kOriginIds);
  if (!origin_ids) {
    return base::UnguessableToken::Null();
  }

  if (origin_ids->empty()) {
    return base::UnguessableToken::Null();
  }

  auto first_entry = origin_ids->begin();
  auto result = base::ValueToUnguessableToken(*first_entry);
  origin_ids->erase(first_entry);

  return result.value_or(base::UnguessableToken::Null());
}

void AddOriginId(base::Value::Dict& origin_id_dict,
                 const base::UnguessableToken& origin_id) {
  base::Value::List* origin_ids = origin_id_dict.EnsureList(kOriginIds);
  origin_ids->Append(base::UnguessableTokenToValue(origin_id));
}

class MediaDrmProvisionHelper {
 public:
  using ProvisionedOriginIdCB = base::OnceCallback<void(
      const MediaDrmOriginIdManager::MediaDrmOriginId& origin_id)>;

  explicit MediaDrmProvisionHelper(
      std::unique_ptr<network::PendingSharedURLLoaderFactory>
          pending_shared_url_loader_factory) {
    DCHECK(pending_shared_url_loader_factory);
    create_fetcher_cb_ =
        base::BindRepeating(&content::CreateProvisionFetcher,
                            network::SharedURLLoaderFactory::Create(
                                std::move(pending_shared_url_loader_factory)));
  }

  void Provision(ProvisionedOriginIdCB callback) {}

 private:
  friend class base::RefCounted<MediaDrmProvisionHelper>;
  ~MediaDrmProvisionHelper() {}

  media::CreateFetcherCB create_fetcher_cb_;
  ProvisionedOriginIdCB complete_callback_;
  base::UnguessableToken origin_id_;
  scoped_refptr<media::OHOSMediaDrmBridge> media_drm_bridge_;
};

void StartProvisioning(
    std::unique_ptr<network::PendingSharedURLLoaderFactory>
        pending_shared_url_loader_factory,
    MediaDrmOriginIdManager::ProvisioningResultCB
        provisioning_result_cb_for_testing,
    MediaDrmProvisionHelper::ProvisionedOriginIdCB callback) {
  if (provisioning_result_cb_for_testing) {
    std::move(callback).Run(provisioning_result_cb_for_testing.Run());
    return;
  }

  if (!pending_shared_url_loader_factory) {
    std::move(callback).Run(absl::nullopt);
    return;
  }

  auto* helper =
      new MediaDrmProvisionHelper(std::move(pending_shared_url_loader_factory));
  helper->Provision(std::move(callback));
}

}  // namespace

class MediaDrmOriginIdManager::NetworkObserver
    : public network::NetworkConnectionTracker::NetworkConnectionObserver {
 public:
  explicit NetworkObserver(MediaDrmOriginIdManager* parent) : parent_(parent) {
    content::GetNetworkConnectionTracker()->AddNetworkConnectionObserver(this);
  }

  ~NetworkObserver() override {
    content::GetNetworkConnectionTracker()->RemoveNetworkConnectionObserver(
        this);
  }

  bool MaxAttemptsExceeded() const {
    constexpr int kMaxAttemptsAllowed = 5;
    return number_of_attempts_ >= kMaxAttemptsAllowed;
  }

  void OnConnectionChanged(network::mojom::ConnectionType type) override {
    if (type == network::mojom::ConnectionType::CONNECTION_NONE) {
      return;
    }

    ++number_of_attempts_;
    parent_->PreProvisionIfNecessary();
  }

 private:
  const raw_ptr<MediaDrmOriginIdManager> parent_;
  int number_of_attempts_ = 0;
};

// static
void MediaDrmOriginIdManager::RegisterProfilePrefs(
    PrefRegistrySimple* registry) {
  registry->RegisterDictionaryPref(kMediaDrmOriginIds);
}

MediaDrmOriginIdManager::MediaDrmOriginIdManager(PrefService* pref_service)
    : pref_service_(pref_service) {
  DVLOG(1) << __func__;
  DCHECK(pref_service_);

  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(
          &MediaDrmOriginIdManager::RecordCountOfPreprovisionedOriginIds,
          weak_factory_.GetWeakPtr()),
      kCheckDelay);
}

MediaDrmOriginIdManager::~MediaDrmOriginIdManager() {
  while (!pending_provisioned_origin_id_cbs_.empty()) {
    std::move(pending_provisioned_origin_id_cbs_.front())
        .Run(GetOriginIdStatus::kFailure, absl::nullopt);
    pending_provisioned_origin_id_cbs_.pop();
  }
}

void MediaDrmOriginIdManager::PreProvisionIfNecessary() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  if (is_provisioning_) {
    return;
  }

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE,
      {base::MayBlock(), base::TaskPriority::BEST_EFFORT,
       base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN},
      base::BindOnce(
          &media::OHOSMediaDrmBridge::IsPerApplicationProvisioningSupported),
      base::BindOnce(&MediaDrmOriginIdManager::ResumePreProvisionIfNecessary,
                     weak_factory_.GetWeakPtr()));
}

void MediaDrmOriginIdManager::ResumePreProvisionIfNecessary(
    bool is_per_application_provisioning_supported) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  is_per_application_provisioning_supported_ =
      is_per_application_provisioning_supported;

  ScopedDictPrefUpdate update(pref_service_, kMediaDrmOriginIds);
  if (!CanPreProvision(is_per_application_provisioning_supported, *update)) {
    network_observer_.reset();
    return;
  }

  if (CountAvailableOriginIds(*update) >= kMaxPreProvisionedOriginIds) {
    network_observer_.reset();
    return;
  }

  is_provisioning_ = true;
  StartProvisioningAsync(true);
}

void MediaDrmOriginIdManager::GetOriginId(ProvisionedOriginIdCB callback) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  base::UnguessableToken origin_id = TakeFirstOriginId(pref_service_);
  if (!is_provisioning_) {
    is_provisioning_ = true;
    StartProvisioningAsync(!origin_id.is_empty());
  }

  if (!origin_id) {
    pending_provisioned_origin_id_cbs_.push(std::move(callback));
    return;
  }

  std::move(callback).Run(GetOriginIdStatus::kSuccessWithPreProvisionedOriginId,
                          origin_id);
}

void MediaDrmOriginIdManager::StartProvisioningAsync(bool run_in_background) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(is_provisioning_);
  const base::TaskPriority priority = run_in_background
                                          ? base::TaskPriority::BEST_EFFORT
                                          : base::TaskPriority::USER_VISIBLE;

  std::unique_ptr<network::PendingSharedURLLoaderFactory>
      pending_shared_url_loader_factory;
  auto* network_context_manager =
      g_browser_process->system_network_context_manager();
  if (network_context_manager) {
    pending_shared_url_loader_factory =
        network_context_manager->GetSharedURLLoaderFactory()->Clone();
  }

  scoped_refptr<base::SingleThreadTaskRunner> provisioning_task_runner =
      base::ThreadPool::CreateSingleThreadTaskRunner(
          {base::MayBlock(), priority});
  provisioning_task_runner->PostTask(
      FROM_HERE,
      base::BindOnce(&StartProvisioning,
                     std::move(pending_shared_url_loader_factory),
                     provisioning_result_cb_for_testing_,
                     base::BindPostTaskToCurrentDefault(base::BindOnce(
                         &MediaDrmOriginIdManager::OriginIdProvisioned,
                         weak_factory_.GetWeakPtr()))));
}

void MediaDrmOriginIdManager::OriginIdProvisioned(
    const MediaDrmOriginId& origin_id) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(is_provisioning_);

  if (!origin_id) {
    if (!network_observer_) {
      network_observer_ = std::make_unique<NetworkObserver>(this);
    } else if (network_observer_->MaxAttemptsExceeded()) {
      network_observer_.reset();
    }

    if (!pending_provisioned_origin_id_cbs_.empty()) {
      if (!IsPerApplicationProvisioningSupported()) {
        SetExpirableToken(pref_service_);
      }

      base::queue<ProvisionedOriginIdCB> pending_requests;
      pending_requests.swap(pending_provisioned_origin_id_cbs_);
      while (!pending_requests.empty()) {
        std::move(pending_requests.front())
            .Run(GetOriginIdStatus::kFailure, absl::nullopt);
        pending_requests.pop();
      }
    }

    is_provisioning_ = false;
    return;
  }

  if (!pending_provisioned_origin_id_cbs_.empty()) {
    std::move(pending_provisioned_origin_id_cbs_.front())
        .Run(GetOriginIdStatus::kSuccessWithNewlyProvisionedOriginId,
             origin_id);
    pending_provisioned_origin_id_cbs_.pop();
  } else {
    ScopedDictPrefUpdate update(pref_service_, kMediaDrmOriginIds);
    AddOriginId(*update, origin_id.value());
    if (CountAvailableOriginIds(*update) >= kMaxPreProvisionedOriginIds) {
      network_observer_.reset();
      RemoveExpirableToken(*update);
      is_provisioning_ = false;
      return;
    }
  }
  StartProvisioningAsync(pending_provisioned_origin_id_cbs_.empty());
}

bool MediaDrmOriginIdManager::IsPerApplicationProvisioningSupported() {
  if (!is_per_application_provisioning_supported_.has_value()) {
    is_per_application_provisioning_supported_ =
        media::OHOSMediaDrmBridge::IsPerApplicationProvisioningSupported();
  }
  return is_per_application_provisioning_supported_.value();
}

void MediaDrmOriginIdManager::RecordCountOfPreprovisionedOriginIds() {
  DVLOG(1) << __func__;
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  const auto& pref = pref_service_->GetDict(kMediaDrmOriginIds);
  int available_origin_ids = CountAvailableOriginIds(pref);

  if (IsPerApplicationProvisioningSupported()) {
    UMA_HISTOGRAM_EXACT_LINEAR(
        "Media.EME.MediaDrm.PreprovisionedOriginId.PerAppProvisioningDevice",
        available_origin_ids, kUMAMaxPreProvisionedOriginIds);
  } else {
    UMA_HISTOGRAM_EXACT_LINEAR(
        "Media.EME.MediaDrm.PreprovisionedOriginId.NonPerAppProvisioningDevice",
        available_origin_ids, kUMAMaxPreProvisionedOriginIds);
  }
}

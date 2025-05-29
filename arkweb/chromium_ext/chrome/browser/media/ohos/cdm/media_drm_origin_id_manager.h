// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_H_
#define CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_H_

#include <memory>

#include "base/containers/queue.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/unguessable_token.h"
#include "components/keyed_service/core/keyed_service.h"
#include "media/base/media_drm_storage.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

class MediaDrmOriginIdManagerFactory;
class PrefRegistrySimple;
class PrefService;

class MediaDrmOriginIdManager : public KeyedService {
 public:
  enum class GetOriginIdStatus {
    kSuccessWithPreProvisionedOriginId = 0,
    kSuccessWithNewlyProvisionedOriginId = 1,
    kFailure = 2,
  };

  using MediaDrmOriginId = media::MediaDrmStorage::MediaDrmOriginId;

  using ProvisionedOriginIdCB =
      base::OnceCallback<void(GetOriginIdStatus status,
                              const MediaDrmOriginId& origin_id)>;
  using ProvisioningResultCB = base::RepeatingCallback<MediaDrmOriginId()>;

  static void RegisterProfilePrefs(PrefRegistrySimple* registry);

  ~MediaDrmOriginIdManager() override;

  void PreProvisionIfNecessary();

  void GetOriginId(ProvisionedOriginIdCB callback);

  void SetProvisioningResultCBForTesting(ProvisioningResultCB cb) {
    provisioning_result_cb_for_testing_ = cb;
  }

 private:
  class NetworkObserver;
  friend class MediaDrmOriginIdManagerFactory;

  explicit MediaDrmOriginIdManager(PrefService* pref_service);

  void ResumePreProvisionIfNecessary(
      bool is_per_application_provisioning_supported);

  void StartProvisioningAsync(bool run_in_background);

  void OriginIdProvisioned(const MediaDrmOriginId& origin_id);

  bool IsPerApplicationProvisioningSupported();

  void RecordCountOfPreprovisionedOriginIds();

  const raw_ptr<PrefService> pref_service_;

  base::queue<ProvisionedOriginIdCB> pending_provisioned_origin_id_cbs_;

  bool is_provisioning_ = false;

  absl::optional<bool> is_per_application_provisioning_supported_;

  ProvisioningResultCB provisioning_result_cb_for_testing_;

  std::unique_ptr<NetworkObserver> network_observer_;

  THREAD_CHECKER(thread_checker_);

  base::WeakPtrFactory<MediaDrmOriginIdManager> weak_factory_{this};
};

#endif  // CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_H_

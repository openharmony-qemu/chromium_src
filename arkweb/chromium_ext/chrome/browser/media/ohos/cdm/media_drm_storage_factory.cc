// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/media/ohos/cdm/media_drm_storage_factory.h"

#include <utility>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "chrome/browser/media/ohos/cdm/media_drm_origin_id_manager.h"
#include "chrome/browser/media/ohos/cdm/media_drm_origin_id_manager_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/cdm/browser/media_drm_storage_impl.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "media/base/media_switches.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace {

using MediaDrmOriginId = media::MediaDrmStorage::MediaDrmOriginId;
using GetOriginIdStatus = MediaDrmOriginIdManager::GetOriginIdStatus;
using OriginIdReadyCB =
    base::OnceCallback<void(bool success, const MediaDrmOriginId& origin_id)>;

enum class GetOriginIdResult {
  kSuccessWithPreProvisionedOriginId = 0,
  kSuccessWithNewlyProvisionedOriginId = 1,
  kSuccessWithUnprovisionedOriginId = 2,
  kFailureOnPerAppProvisioningDevice = 3,
  kFailureOnNonPerAppProvisioningDevice = 4,
  kFailureWithNoFactory = 5,
  kMaxValue = kFailureWithNoFactory,
};

void ReportResultToUma(GetOriginIdResult result) {
  base::UmaHistogramEnumeration("Media.EME.MediaDrm.GetOriginIdResult", result);
}

void CreateOriginId(OriginIdReadyCB callback) {
  LOG(INFO) << "[DRM]" << __func__;
  auto origin_id = base::UnguessableToken::Create();
  ReportResultToUma(GetOriginIdResult::kSuccessWithUnprovisionedOriginId);
  std::move(callback).Run(true, origin_id);
}

void AllowEmptyOriginId(content::RenderFrameHost* render_frame_host,
                        base::OnceCallback<void(bool)> callback) {
  if (media::OHOSMediaDrmBridge::IsPerApplicationProvisioningSupported()) {
    std::move(callback).Run(false);
    return;
  }
  std::move(callback).Run(false);
}

}  // namespace

void CreateMediaDrmStorage(
    content::RenderFrameHost* render_frame_host,
    mojo::PendingReceiver<media::mojom::MediaDrmStorage> receiver) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  CHECK(render_frame_host);

  content::BrowserContext* browser_context =
      render_frame_host->GetBrowserContext();
  DCHECK(browser_context) << "BrowserContext not available.";

  Profile* profile = Profile::FromBrowserContext(browser_context);
  DCHECK(profile) << "Profile not available.";

  PrefService* pref_service = profile->GetPrefs();
  DCHECK(pref_service) << "PrefService not available.";

  if (render_frame_host->GetLastCommittedOrigin().opaque()) {
    return;
  }

  auto get_origin_id_cb = base::BindRepeating(&CreateOriginId);
  new cdm::MediaDrmStorageImpl(
      *render_frame_host, pref_service, get_origin_id_cb,
      base::BindRepeating(&AllowEmptyOriginId, render_frame_host),
      std::move(receiver));
}

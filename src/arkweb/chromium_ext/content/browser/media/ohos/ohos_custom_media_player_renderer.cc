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

#include "content/browser/media/ohos/ohos_custom_media_player_renderer.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/custom_media_player_listener.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#include "media/base/timestamp_constants.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "net/http/http_request_headers.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom.h"

namespace content {

namespace {

const float kDefaultVolume = 1.0;


// Returns the cookie manager for the `browser_context` at the client end of the
// mojo pipe. This will be restricted to the origin of `url`, and will apply
// policies from user and ContentBrowserClient to cookie operations.
mojo::PendingRemote<network::mojom::RestrictedCookieManager>
GetRestrictedCookieManagerForContext(
    BrowserContext* browser_context,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    RenderFrameHostImpl* render_frame_host) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  mojo::PendingRemote<network::mojom::RestrictedCookieManager> pipe;

  StoragePartition* storage_partition = nullptr;
  url::Origin request_origin = url::Origin::Create(url);
  if (browser_context != nullptr) {
    storage_partition =
      browser_context->GetDefaultStoragePartition();
  } else {
    LOG(ERROR) << "browser_context is nullptr";
    return pipe;
  }

  // `request_origin` cannot be used to create `isolation_info` since it
  // represents the media resource, not the frame origin. Here we use the
  // `top_frame_origin` as the frame origin to ensure the consistency check
  // passes when creating `isolation_info`. This is ok because
  // `isolation_info.frame_origin` is unused in RestrictedCookieManager.
  DCHECK(site_for_cookies.IsNull() ||
         site_for_cookies.IsFirstParty(top_frame_origin.GetURL()));
  net::IsolationInfo isolation_info = net::IsolationInfo::Create(
      net::IsolationInfo::RequestType::kOther, top_frame_origin,
      top_frame_origin, site_for_cookies, absl::nullopt);

  net::CookieSettingOverrides devtools_cookie_setting_overrides;
  static_cast<StoragePartitionImpl*>(storage_partition)
      ->CreateRestrictedCookieManager(
          network::mojom::RestrictedCookieManagerRole::NETWORK, request_origin,
          std::move(isolation_info),
          /* is_service_worker = */ false,
          render_frame_host ? render_frame_host->GetProcess()->GetID().GetUnsafeValue() : -1,
          render_frame_host ? render_frame_host->GetRoutingID()
                            : MSG_ROUTING_NONE,
          render_frame_host ? render_frame_host->GetCookieSettingOverrides()
                            : net::CookieSettingOverrides(),
          devtools_cookie_setting_overrides,
          pipe.InitWithNewPipeAndPassReceiver(),
          render_frame_host ? render_frame_host->CreateCookieAccessObserver(
                        CookieAccessDetails::Source::kNonNavigation)
                            : mojo::NullRemote());
  return pipe;
}

void ReturnResultOnUIThread(
    base::OnceCallback<void(const std::string&)> callback,
    const std::string& result) {
  GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(std::move(callback), result));
}

void ReturnResultOnUIThreadAndClosePipe(
    mojo::Remote<network::mojom::RestrictedCookieManager> pipe,
    base::OnceCallback<void(const std::string&)> callback,
    uint64_t version,
    base::ReadOnlySharedMemoryRegion shared_memory_region,
    const std::string& result) {
  GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(std::move(callback), result));
}

MediaInfo::Preload ConvertTo(uint32_t preload_type) {
  if (preload_type == 0) {
    return MediaInfo::Preload::NONE;
  }
  if (preload_type == 1) {
    return MediaInfo::Preload::METADATA;
  }
  if (preload_type == 2) {
    return MediaInfo::Preload::AUTO;
  }
  return MediaInfo::Preload::AUTO;
}

}  // namespace

// TODO: Reimplement for Chromium 141 compatibility
// OHOSCustomMediaPlayerRenderer class has been temporarily removed due to
// Chromium 141 API changes. Key changes needed:
// 1. Update interface methods and dependencies
// 2. Adapt to RenderFrameHost API changes
// 3. Replace removed Mojo interfaces
// 4. Update time handling methods

}  // namespace content

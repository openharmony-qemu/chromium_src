/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "components/permissions/contexts/geolocation_permission_context_ohos.h"

#include "base/logging.h"
#include "base/functional/callback.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"

namespace permissions {

static std::set<int> global_render_frame_host_ids_;
static base::Lock global_geolocation_set_lock_;

GeolocationPermissionContextOHOS::GeolocationPermissionContextOHOS(
    content::BrowserContext* browser_context,
    std::unique_ptr<Delegate> delegate)
    : GeolocationPermissionContext(browser_context, std::move(delegate)) {}

GeolocationPermissionContextOHOS::~GeolocationPermissionContextOHOS() =
    default;

void GeolocationPermissionContextOHOS::RequestPermission(
    PermissionRequestData request_data,
    BrowserPermissionCallback callback) {
  namespace ohos_permission = ohos::adapter::permission;
    base::AutoLock lock(global_geolocation_set_lock_);
    LOG(INFO) <<__func__<< " [GEOLOCATION] Current hanging premission request size: "
    << global_render_frame_host_ids_.size();
    content::GlobalRenderFrameHostId currentRequestRenderFrameHostId = request_data.id.global_render_frame_host_id();
    if (global_render_frame_host_ids_.contains(currentRequestRenderFrameHostId.child_id)) {
        LOG(WARNING) << __func__ << " [GEOLOCATION] Duplicate permission request from: "
        << currentRequestRenderFrameHostId;
        // one frame should only request Gelocation Permission once.
        return;
    } else {
        global_render_frame_host_ids_.insert(currentRequestRenderFrameHostId.child_id);
        LOG(INFO) <<__func__<< " [GEOLOCATION] New premission request from: " << currentRequestRenderFrameHostId;
        base::ThreadPool::PostTaskAndReplyWithResult(
            FROM_HERE, {base::MayBlock()},
            base::BindOnce(
                &ohos_permission::PermissionManagerAdapter::RequestPermission,
                ohos_permission::OHOSPermissionType::LOCATION),
            base::BindOnce(&GeolocationPermissionContextOHOS::RequestReply,
                weak_factory_.GetWeakPtr(), std::move(request_data), std::move(callback)));
    }
}

void GeolocationPermissionContextOHOS::RequestReply(
    PermissionRequestData request_data,
    BrowserPermissionCallback callback,
    bool reply_success) {
    base::AutoLock lock(global_geolocation_set_lock_);
    content::GlobalRenderFrameHostId currentRequestRenderFrameHostId = request_data.id.global_render_frame_host_id();
    LOG(INFO) <<__func__<< " [GEOLOCATION] Premission request finish for: " << currentRequestRenderFrameHostId
    << " result: " << reply_success;
    global_render_frame_host_ids_.erase(currentRequestRenderFrameHostId.child_id);
  if (!reply_success) {
    std::move(callback).Run(CONTENT_SETTING_ASK);
    return;
  }

  GeolocationPermissionContext::RequestPermission(std::move(request_data),
                                                  std::move(callback));
}

}  // namespace permissions

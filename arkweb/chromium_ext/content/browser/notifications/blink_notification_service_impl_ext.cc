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

#include "arkweb/chromium_ext/content/browser/notifications/blink_notification_service_impl_ext.h"

#include <utility>

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/permission_controller.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"

#if BUILDFLAG(ARKWEB_NOTIFICATION)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif  // ARKWEB_NOTIFICATION

namespace content {

#if BUILDFLAG(ARKWEB_NOTIFICATION)
void BlinkNotificationServiceImplExt::GetPermissionStatusExt(
    GetPermissionStatusCallback callback) {
  auto* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kEnableNwebEx)) {
    CheckPermissionStatusAsync(std::move(callback));
  } else {
    std::move(callback).Run(CheckPermissionStatus());
  }
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
void BlinkNotificationServiceImplExt::CheckPermissionStatusAsync(
    GetPermissionStatusCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  auto* permission_controller = browser_context_->GetPermissionController();
  if (!permission_controller) {
    std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
    return;
  }

  // TODO(crbug.com/987654): It is odd that a service instance can be created
  // for cross-origin subframes, yet the instance is completely oblivious of
  // whether it is serving a top-level browsing context or an embedded one.
  const bool is_document =
      creator_type_ ==
      RenderProcessHost::NotificationServiceCreatorType::kDocument;
  if (is_document) {
    RenderFrameHost* rfh = weak_document_ptr_.AsRenderFrameHostIfValid();
    if (!rfh) {
      std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
      return;
    }
    permission_controller->GetPermissionStatusAsync(
        blink::PermissionType::NOTIFICATIONS, true, static_cast<void*>(rfh),
        rfh->GetLastCommittedOrigin(), std::move(callback));
  } else {
    RenderProcessHost* rph = RenderProcessHost::FromID(render_process_host_id_);
    if (!rph) {
      std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
      return;
    }
    permission_controller->GetPermissionStatusAsync(
        blink::PermissionType::NOTIFICATIONS, false, static_cast<void*>(rph),
        storage_key_.origin(), std::move(callback));
  }
}
#endif  // ARKWEB_NOTIFICATION

}  // namespace content

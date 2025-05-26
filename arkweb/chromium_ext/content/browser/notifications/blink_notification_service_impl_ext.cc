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

#include <string>
#include <utility>

#include "base/check_op.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/metrics/histogram_functions.h"
#include "content/browser/notifications/blink_notification_service_impl.h"
#include "content/browser/notifications/notification_event_dispatcher_impl.h"
#include "content/browser/notifications/platform_notification_context_impl.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_database_data.h"
#include "content/public/browser/permission_controller.h"
#include "content/public/browser/platform_notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/weak_document_ptr.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_features.h"
#include "third_party/blink/public/common/notifications/notification_constants.h"
#include "third_party/blink/public/common/notifications/notification_resources.h"
#include "third_party/blink/public/common/notifications/platform_notification_data.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"
#include "third_party/blink/public/common/service_worker/service_worker_status_code.h"

#if BUILDFLAG(ARKWEB_NOTIFICATION)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif  // ARKWEB_NOTIFICATION

namespace content {

#if BUILDFLAG(ARKWEB_NOTIFICATION)
void BlinkNotificationServiceImplExt::GetPermissionStatusExt(
    GetPermissionStatusCallback callback) {
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableNwebEx)) {
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

  // TODO(crbug.com/987654): It is odd that a service instance can be created
  // for cross-origin subframes, yet the instance is completely oblivious of
  // whether it is serving a top-level browsing context or an embedded one.
  if (creator_type_ ==
      RenderProcessHost::NotificationServiceCreatorType::kDocument) {
    RenderFrameHost* rfh = weak_document_ptr_.AsRenderFrameHostIfValid();
    if (!rfh) {
      std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
      return;
    }
    browser_context_->GetPermissionController()->GetPermissionStatusAsync(
        blink::PermissionType::NOTIFICATIONS, true, (void*)rfh,
        rfh->GetLastCommittedOrigin(), std::move(callback));
  } else {
    RenderProcessHost* rph = RenderProcessHost::FromID(render_process_host_id_);
    if (!rph) {
      std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
      return;
    }
    browser_context_->GetPermissionController()->GetPermissionStatusAsync(
        blink::PermissionType::NOTIFICATIONS, false, (void*)rph,
        storage_key_.origin(), std::move(callback));
  }
}
#endif  // ARKWEB_NOTIFICATION

}  // namespace content
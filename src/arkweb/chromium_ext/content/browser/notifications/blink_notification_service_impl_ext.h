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

#ifndef CONTENT_BROWSER_NOTIFICATIONS_BLINK_NOTIFICATION_SERVICE_IMPL_EXT_H_
#define CONTENT_BROWSER_NOTIFICATIONS_BLINK_NOTIFICATION_SERVICE_IMPL_EXT_H_

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/browser/notifications/blink_notification_service_impl.h"
#include "content/browser/service_worker/service_worker_context_wrapper.h"
#include "content/common/content_export.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/weak_document_ptr.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "third_party/blink/public/mojom/notifications/notification_service.mojom.h"
#include "third_party/blink/public/mojom/permissions/permission_status.mojom.h"
#include "url/gurl.h"

namespace content {
class CONTENT_EXPORT BlinkNotificationServiceImplExt
    : public BlinkNotificationServiceImpl {
 public:
  explicit BlinkNotificationServiceImplExt(
      PlatformNotificationContextImpl* notification_context,
      BrowserContext* browser_context,
      scoped_refptr<ServiceWorkerContextWrapper> service_worker_context,
      RenderProcessHost* render_process_host,
      const blink::StorageKey& storage_key,
      const GURL& document_url,
      const WeakDocumentPtr& weak_document_ptr,
      RenderProcessHost::NotificationServiceCreatorType creator_type,
      mojo::PendingReceiver<blink::mojom::NotificationService> receiver)
      : BlinkNotificationServiceImpl(notification_context,
                                     browser_context,
                                     std::move(service_worker_context),
                                     render_process_host,
                                     storage_key,
                                     document_url,
                                     weak_document_ptr,
                                     creator_type,
                                     std::move(receiver)) {}

  BlinkNotificationServiceImplExt(const BlinkNotificationServiceImplExt&) =
      delete;
  BlinkNotificationServiceImplExt& operator=(
      const BlinkNotificationServiceImplExt&) = delete;

  BlinkNotificationServiceImplExt* AsBlinkNotificationServiceImplExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_NOTIFICATION)
  void GetPermissionStatusExt(GetPermissionStatusCallback callback);
#endif  // ARKWEB_NOTIFICATION
#if BUILDFLAG(ARKWEB_NOTIFICATION)
  void CheckPermissionStatusAsync(GetPermissionStatusCallback callback);
#endif  // ARKWEB_NOTIFICATION
};

}  // namespace content

#endif  // CONTENT_BROWSER_NOTIFICATIONS_BLINK_NOTIFICATION_SERVICE_IMPL_EXT_H_
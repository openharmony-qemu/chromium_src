/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_notification_manager_delegate.h"

#include <string.h>
#include <thread>
#include "base/logging.h"
#include "nweb_impl.h"
#include "chrome/browser/notifications/notification_platform_bridge_ohos.h"

namespace OHOS::NWeb {

static std::unique_ptr<NWebNotificationManagerDelegate> g_notification_manager_delegate_;
std::shared_ptr<NWebNotificationDelegateCallback>
  NWebNotificationManagerDelegate::notification_delegate_listener_ = nullptr;

std::unique_ptr<NWebNotificationManagerDelegate>& NWebNotificationManagerDelegate::GetOrCreateDelegate() {
    if (!g_notification_manager_delegate_) {
        g_notification_manager_delegate_ = std::make_unique<NWebNotificationManagerDelegate>();
    }

    return g_notification_manager_delegate_;
}

NWebNotificationManagerDelegate::NWebNotificationManagerDelegate() {}

void NWebNotificationManagerDelegate::RegisterNotificationManagerDelegateListener(
    std::shared_ptr<NWebNotificationDelegateCallback> notification_delegate_listener) {
  if (notification_delegate_listener_ == nullptr) {
    LOG(INFO) << "RegisterNotificationManagerDelegateListener";
    notification_delegate_listener_ = notification_delegate_listener;
  }
}

NO_SANITIZE("cfi-icall")
void NWebNotificationManagerDelegate::OnShowNotification(
    std::shared_ptr<NWebNotificationOptionsItem> options_item) {
  if (!options_item) {
    LOG(ERROR) << "NWebNotificationManagerDelegate::OnShowNotification error, "
                  "not invalid";
    return;
  }

  if (notification_delegate_listener_ != nullptr) {
    notification_delegate_listener_->showNotification(options_item);
  } else {
    LOG(ERROR) << "notification_delegate_listener is null";
  }
}

NO_SANITIZE("cfi-icall")
void NWebNotificationManagerDelegate::OnCloseNotification(std::string id) {
  if (notification_delegate_listener_ != nullptr) {
    notification_delegate_listener_->closeNotification(id);
  } else {
    LOG(ERROR) << "notification_delegate_listener is null";
  }
}

void NWebNotificationManagerDelegate::OnShowed(const std::string id) {
  LOG(INFO) << "NWebNotificationManagerDelegate::OnShowed is on call";
  NotificationPlatformBridgeOhos::OnShowed(id);
}

void NWebNotificationManagerDelegate::OnClosed(const std::string id) {
  LOG(INFO) << "NWebNotificationManagerDelegate::OnClosed is on call";
  NotificationPlatformBridgeOhos::OnClosed(id);
}

void NWebNotificationManagerDelegate::OnClicked(const std::string id, int buttonIndex) {
  LOG(INFO) << "NWebNotificationManagerDelegate::OnClicked is on call";
  NotificationPlatformBridgeOhos::OnClicked(id, buttonIndex);
}  // namespace OHOS::NWeb

}
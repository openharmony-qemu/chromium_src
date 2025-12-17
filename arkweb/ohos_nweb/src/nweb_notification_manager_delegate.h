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

#ifndef NWEB_NOTIFICATION_MANAGER_DELEGATE_H
#define NWEB_NOTIFICATION_MANAGER_DELEGATE_H
#include <cstring>
#include "capi/nweb_notification_options_item.h"

namespace OHOS::NWeb {
class NWebNotificationManagerDelegate {
 public:
  static std::unique_ptr<NWebNotificationManagerDelegate>& GetOrCreateDelegate();
  void RegisterNotificationManagerDelegateListener(
      std::shared_ptr<NWebNotificationDelegateCallback> notificationDelegateListener);
  NWebNotificationManagerDelegate();
  ~NWebNotificationManagerDelegate() = default;
  static void OnShowNotification(std::shared_ptr<NWebNotificationOptionsItem> options_item);
  static void OnCloseNotification(std::string id);
  static void OnShowed(const std::string id);
  static void OnClosed(const std::string id);
  static void OnClicked(const std::string id, int buttonIndex);

 private:
  static std::shared_ptr<NWebNotificationDelegateCallback>
      notification_delegate_listener_;
};

}  // namespace OHOS::NWeb

#endif
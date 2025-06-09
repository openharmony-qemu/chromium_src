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

#ifndef OHOS_ADAPTER_NOTIFICATION_NOTIFICATION_ADAPTER_H
#define OHOS_ADAPTER_NOTIFICATION_NOTIFICATION_ADAPTER_H

#include <cstdint>
#include <memory>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {

struct NotificationImage {
  int width = 0;
  int height = 0;
  std::unique_ptr<char[]> buff;
};

struct NotificationButton {
  std::string title = "";
  int buttonIndex = 0;
};

struct NotificationRequest {
  int notificationId = 0;
  std::string title = "";
  std::string message = "";
  bool requireInteraction;
  bool silent;
  int64_t timestamp;
  NotificationImage icon;
  NotificationImage image;
  std::vector<NotificationButton> buttons;
};

struct NotificationAdapterImage {
  int width = 0;
  int height = 0;
  aki::ArrayBuffer buff;
};

struct NotificationAdapterButton {
  std::string title = "";
  int buttonIndex = 0;
};

struct NotificationAdapterRequest {
  int notificationId = 0;
  std::string title = "";
  std::string message = "";
  bool requireInteraction;
  bool silent;
  int64_t timestamp;
  NotificationAdapterImage icon;
  NotificationAdapterImage image;
  std::vector<NotificationAdapterButton> buttons;
};

class ADAPTER_EXPORT_API NotificationAdapter {
 public:
  using OnClickCallback = std::function<void(int32_t)>;
  using OnCloseCallback = std::function<void(int32_t)>;
  using OnButtonClickCallback = std::function<void(int32_t, int32_t)>;
  static NotificationAdapter& GetInstance();
  NotificationAdapter(const NotificationAdapter&) = delete;
  NotificationAdapter(NotificationAdapter&&) = delete;
  NotificationAdapter& operator=(const NotificationAdapter&) = delete;
  virtual ~NotificationAdapter() = default;

  void SendNotification(NotificationRequest& request);
  void CloseNotification(int notificationId);
  std::vector<int> getAllDisplayedNotification();
  OnClickCallback GetOnClickCallback() { return onClickCallback_; }
  void RegisterOnClickCallback(OnClickCallback callback);
  OnCloseCallback GetOnCloseCallback() { return onCloseCallback_; }
  void RegisterOnCloseCallback(OnCloseCallback callback);
  OnButtonClickCallback GetOnButtonClickCallback() {
    return onButtonClickCallback_;
  }
  void RegisterOnButtonClickCallback(OnButtonClickCallback callback);
  void DestroyCallback();

 private:
  OnClickCallback onClickCallback_;
  OnCloseCallback onCloseCallback_;
  OnButtonClickCallback onButtonClickCallback_;
  NotificationAdapter();
  NotificationAdapterRequest ConvertNotificationRequest(
      NotificationRequest& request);
  NotificationAdapterImage ConvertNotificationImage(NotificationImage& image);
  std::vector<NotificationAdapterButton> ConvertNotificationButton(
      std::vector<NotificationButton>& buttons);
};

}  // namespace adapter
}  // namespace ohos

#endif  // OHOS_ADAPTER_NOTIFICATION_ADAPTER_NOTIFICATION_ADAPTER_H

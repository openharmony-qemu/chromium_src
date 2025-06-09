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

#include "ohos/adapter/notification/notification_adapter.h"

namespace ohos {
namespace adapter {

namespace {
const int IMAGE_PIXEL_MAP = 4;
}  // namespace

NotificationAdapter::NotificationAdapter() {}

NotificationAdapter& NotificationAdapter::GetInstance() {
  static NotificationAdapter adapter;
  return adapter;
}

void NotificationAdapter::SendNotification(NotificationRequest& request) {
  if (auto func =
          ohos::adapter::GetJSFunction("NotificationAdapter.SendNotification")) {
    NotificationAdapterRequest apapterRequest =
        ConvertNotificationRequest(request);
    func->Invoke<void>(apapterRequest);
  }
}

void NotificationAdapter::CloseNotification(int notificationId) {
  if (auto func =
          ohos::adapter::GetJSFunction("NotificationAdapter.CloseNotification")) {
    func->Invoke<void>(notificationId);
  }
}

std::vector<int> NotificationAdapter::getAllDisplayedNotification() {
  if (auto func = ohos::adapter::GetJSFunction("NotificationAdapter.GetAllNotification")) {
    std::promise<std::vector<int>> insert_promise;
    std::function<void(std::vector<int>)> callback =
        [&insert_promise](std::vector<int> ret) {
          insert_promise.set_value(ret);
        };
    func->Invoke<void>(callback);
    return insert_promise.get_future().get();
  }
  return {};
}

NotificationAdapterImage NotificationAdapter::ConvertNotificationImage(
    NotificationImage& image) {
  if (image.width == 0 || image.height == 0) {
    return {};
  }
  aki::ArrayBuffer arrayBuffer((uint8_t*)image.buff.get(),
                               image.width * image.height * IMAGE_PIXEL_MAP);
  return {image.width, image.height, arrayBuffer};
}

std::vector<NotificationAdapterButton> NotificationAdapter::ConvertNotificationButton(
    std::vector<NotificationButton>& buttons) {
  std::vector<NotificationAdapterButton> adapterButtons;
  if (buttons.size() > 0) {
    for (const auto& button : buttons) {
      adapterButtons.push_back({button.title, button.buttonIndex});
    }
  }
  return adapterButtons;
}

NotificationAdapterRequest NotificationAdapter::ConvertNotificationRequest(
    NotificationRequest& request) {
  return {request.notificationId,
          request.title,
          request.message,
          request.requireInteraction,
          request.silent,
          request.timestamp,
          ConvertNotificationImage(request.icon),
          ConvertNotificationImage(request.image),
          ConvertNotificationButton(request.buttons)};
}

void NotificationAdapter::RegisterOnClickCallback(OnClickCallback callback) {
  if (callback != nullptr) {
    onClickCallback_ = callback;
  }
}

void NotificationAdapter::RegisterOnCloseCallback(OnClickCallback callback) {
  if (callback != nullptr) {
    onCloseCallback_ = callback;
  }
}

void NotificationAdapter::RegisterOnButtonClickCallback(
    OnButtonClickCallback callback) {
  if (callback != nullptr) {
    onButtonClickCallback_ = callback;
  }
}

void NotificationAdapter::DestroyCallback() {
  if (onClickCallback_ != nullptr) {
    onClickCallback_ = nullptr;
  }
  if (onCloseCallback_ != nullptr) {
    onCloseCallback_ = nullptr;
  }
  if (onButtonClickCallback_ != nullptr) {
    onButtonClickCallback_ = nullptr;
  }
}

void OnNotificationClickCallback(int32_t id) {
  if (NotificationAdapter::GetInstance().GetOnClickCallback() != nullptr) {
    NotificationAdapter::GetInstance().GetOnClickCallback()(id);
  }
}

void OnNotificationCloseCallback(int32_t id) {
  if (NotificationAdapter::GetInstance().GetOnCloseCallback() != nullptr) {
    NotificationAdapter::GetInstance().GetOnCloseCallback()(id);
  }
}

void OnNotificationButtonClickCallback(int32_t id, int32_t buttonIndex) {
  if (NotificationAdapter::GetInstance().GetOnButtonClickCallback() !=
      nullptr) {
    NotificationAdapter::GetInstance().GetOnButtonClickCallback()(id,
                                                                  buttonIndex);
  }
}

JSBIND_CLASS(NotificationAdapterRequest) {
  JSBIND_PROPERTY(notificationId);
  JSBIND_PROPERTY(title);
  JSBIND_PROPERTY(message);
  JSBIND_PROPERTY(requireInteraction);
  JSBIND_PROPERTY(silent);
  JSBIND_PROPERTY(timestamp);
  JSBIND_PROPERTY(icon);
  JSBIND_PROPERTY(buttons);
}

JSBIND_CLASS(NotificationAdapterImage) {
  JSBIND_PROPERTY(width);
  JSBIND_PROPERTY(height);
  JSBIND_PROPERTY(buff);
}

JSBIND_CLASS(NotificationAdapterButton) {
  JSBIND_PROPERTY(title);
  JSBIND_PROPERTY(buttonIndex);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnNotificationClickCallback);
  JSBIND_FUNCTION(OnNotificationCloseCallback);
  JSBIND_FUNCTION(OnNotificationButtonClickCallback);
}

}  // namespace adapter
}  // namespace ohos

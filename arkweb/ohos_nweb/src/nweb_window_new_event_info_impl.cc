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

#include "nweb_window_new_event_info_impl.h"

namespace OHOS::NWeb {
NWebWindowNewEventInfoImpl::NWebWindowNewEventInfoImpl(
    const std::string& target_url,
    bool is_alert,
    bool is_user_trigger,
    std::shared_ptr<NWebControllerHandler> handler,
    NavigationPolicy policy,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
    : target_url_(target_url),
      is_alert_(is_alert),
      is_user_trigger_(is_user_trigger),
      handler_(handler),
      policy_(policy),
      x_(x),
      y_(y),
      width_(width),
      height_(height) {}

std::string NWebWindowNewEventInfoImpl::GetUrl() {
  return target_url_;
}

bool NWebWindowNewEventInfoImpl::IsAlert() {
  return is_alert_;
}

bool NWebWindowNewEventInfoImpl::IsUserTrigger() {
  return is_user_trigger_;
}
std::shared_ptr<NWebControllerHandler>
NWebWindowNewEventInfoImpl::GetHandler() {
  return handler_;
}

NavigationPolicy NWebWindowNewEventInfoImpl::GetNavigationPolicy() {
  return policy_;
}

int32_t NWebWindowNewEventInfoImpl::GetX() {
  return x_;
}

int32_t NWebWindowNewEventInfoImpl::GetY() {
  return y_;
}

int32_t NWebWindowNewEventInfoImpl::GetWidth() {
  return width_;
}

int32_t NWebWindowNewEventInfoImpl::GetHeight() {
  return height_;
}
}  // namespace OHOS::NWeb

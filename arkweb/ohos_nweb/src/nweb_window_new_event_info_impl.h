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

#ifndef NWEB_WINDOW_NEW_EVENT_INFO_IMPL_H
#define NWEB_WINDOW_NEW_EVENT_INFO_IMPL_H

#include "nweb.h"
#include "nweb_handler.h"

namespace OHOS::NWeb {
class NWebWindowNewEventInfoImpl : public NWebWindowNewEventInfo {
 public:
  NWebWindowNewEventInfoImpl(const std::string& target_url,
                             bool is_alert,
                             bool is_user_trigger,
                             std::shared_ptr<NWebControllerHandler> handler,
                             NavigationPolicy policy,
                             int x,
                             int y,
                             int width,
                             int height);
  ~NWebWindowNewEventInfoImpl() = default;

  std::string GetUrl() override;
  bool IsAlert() override;
  bool IsUserTrigger() override;
  std::shared_ptr<NWebControllerHandler> GetHandler() override;
  NavigationPolicy GetNavigationPolicy() override;
  int32_t GetX() override;
  int32_t GetY() override;
  int32_t GetWidth() override;
  int32_t GetHeight() override;

 private:
  std::string target_url_;
  bool is_alert_;
  bool is_user_trigger_;
  std::shared_ptr<NWebControllerHandler> handler_;
  NavigationPolicy policy_;
  int32_t x_;
  int32_t y_;
  int32_t width_;
  int32_t height_;
};
}  // namespace OHOS::NWeb
#endif  // NWEB_WINDOW_NEW_EVENT_INFO_IMPL_H

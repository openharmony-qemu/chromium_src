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

#ifndef NWEB_URL_RESOURCE_REQUEAST_IMPL_H
#define NWEB_URL_RESOURCE_REQUEAST_IMPL_H

#include "nweb_url_resource_request.h"

namespace OHOS::NWeb {
class NWebUrlResourceRequestImpl : public NWebUrlResourceRequest {
 public:
  NWebUrlResourceRequestImpl(
      const std::string& method,
      const std::map<std::string, std::string>& request_headers,
      const std::string& url,
      bool has_gesture,
      bool is_for_main_frame,
      bool is_redirect = false);
  ~NWebUrlResourceRequestImpl() = default;

  std::string Method() override;
  std::map<std::string, std::string> RequestHeaders() override;
  std::string Url() override;
  bool FromGesture() override;
  bool IsAboutMainFrame() override;
  bool IsRequestRedirect() override;

 private:
  std::string method_;
  std::map<std::string, std::string> request_headers_;
  std::string url_;
  bool has_gesture_;
  bool is_for_main_frame_;
  bool is_redirect_;
};
}  // namespace OHOS::NWeb
#endif  // NWEB_URL_RESOURCE_REQUEAST_IMPL_H

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

#include "nweb_url_resource_request_impl.h"

namespace OHOS::NWeb {
NWebUrlResourceRequestImpl::NWebUrlResourceRequestImpl(
    const std::string& method,
    const std::map<std::string, std::string>& request_headers,
    const std::string& url,
    bool has_gesture,
    bool is_for_main_frame,
    bool is_redirect)
    : method_(method),
      request_headers_(request_headers),
      url_(url),
      has_gesture_(has_gesture),
      is_for_main_frame_(is_for_main_frame),
      is_redirect_(is_redirect) {}

std::string NWebUrlResourceRequestImpl::Method() {
  return method_;
}

std::map<std::string, std::string>
NWebUrlResourceRequestImpl::RequestHeaders() {
  return request_headers_;
}

std::string NWebUrlResourceRequestImpl::Url() {
  return url_;
}

bool NWebUrlResourceRequestImpl::FromGesture() {
  return has_gesture_;
}

bool NWebUrlResourceRequestImpl::IsAboutMainFrame() {
  return is_for_main_frame_;
}

bool NWebUrlResourceRequestImpl::IsRequestRedirect() {
  return is_redirect_;
}
}  // namespace OHOS::NWeb

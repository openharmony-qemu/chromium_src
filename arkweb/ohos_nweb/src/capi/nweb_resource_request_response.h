/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_NWEB_RESOURCE_REQUEST_RESPONSE_H_
#define OHOS_NWEB_RESOURCE_REQUEST_RESPONSE_H_
 
#include <string>
#include <map>
 
struct WebUrlResourceRequest {
  bool is_request_gesture;
  bool is_main_frame;
  bool is_redirect;
  int32_t request_type;
  int32_t page_transition;
  std::string request_url;
  std::string request_method;
  std::map<std::string, std::string> request_header;
};
 
struct WebUrlResourceResponse {
  bool is_from_network;
  int32_t status_code;
  std::string mime_type;
  std::string encoding;
  std::string reason_phrase;
  std::map<std::string, std::string> response_headers;
};
 
#endif  //  OHOS_NWEB_RESOURCE_REQUEST_RESPONSE_H_
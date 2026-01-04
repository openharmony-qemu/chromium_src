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

#ifndef ARKWEB_NWEB_EX_NET_BASE_LOG_UTILS_H_
#define ARKWEB_NWEB_EX_NET_BASE_LOG_UTILS_H_

#include <string>

#include "net/base/ip_endpoint.h"
#include "net/base/net_export.h"
#include "url/third_party/mozilla/url_parse.h"

namespace net {

class NET_EXPORT LogUtils {
 public:
  // Anonymize the ip address.
  // for example: 1.0.0.*  *:*:4:8:4:4:4:d1
  static std::string AnonymizeIpAddress(const net::IPEndPoint& ip_endpoint);
};

}  // namespace net

#endif  // ARKWEB_NWEB_EX_NET_BASE_LOG_UTILS_H_

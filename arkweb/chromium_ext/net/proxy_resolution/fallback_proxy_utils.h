/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
 
#ifndef HUAWEI_NET_PROXY_RESOLUTION_FALLBACK_PROXY_UTILS_H_
#define HUAWEI_NET_PROXY_RESOLUTION_FALLBACK_PROXY_UTILS_H_
 
#include <memory>
#include <string>
 
#include "net/base/net_export.h"
#include "url/gurl.h"
 
namespace net {
 
enum SBThreatURLPolicy { PROCEED, WAIT, DENY };
 
enum class ProxyUnusedReason {
  CLOUD_CONTROL_SWITCH_DISABLED = 0,
  ERROR_CODE_NOT_IN_CLOUD_LIST,
  TOP_SITE_IN_BLOCK_LIST,
  HOST_IN_USING_PROXY_FAILED_LIST,
  MALICIOUS_TYPE_OR_HW_CODE_NOT_IN_CLOUD_LIST,
  PROXY_SERVER_UNAVAILABLE,
  HAS_USED_SYSTEM_PROXY,
  NOT_CONNECTION_ERROR,
  AUTH_FAILED,
  MAX_VALUE = 0xFF,
};

}  // namespace net
 
#endif  // HUAWEI_NET_PROXY_RESOLUTION_FALLBACK_PROXY_UTILS_H_
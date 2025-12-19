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

#ifndef HUAWEI_NET_BASE_FALLBACK_PROXY_CONSTANTS
#define HUAWEI_NET_BASE_FALLBACK_PROXY_CONSTANTS

#include <stddef.h>
#include <stdint.h>

#include "net/base/net_export.h"

namespace net {

NET_EXPORT_PRIVATE extern const char kFallbackProxyTunnelHeaderKey[];

enum class FallbackProxyStatus {
  NONE = 0,
  NORMAL = 1,
  DISABLE_BY_CLOUD_CONTROL = 2,
  UNAVAILABLE = 3,
  AUTH_FAILED = 4,
};

enum FallbackProxyResponseCode {
  TOKEN_AUTH_FAILED = 600,
  EXCEEDS_THRESHOLD = 601,
  INTERNAL_ERROR = 602,
  DEST_SERVER_TIME_OUT = 603,
  DEST_SERVER_DISCONNECT = 604,
  INSUFFICIENT_RESOURCE = 605,
  DEST_HOST_RESOLVED_FAILED = 607,
  DEST_PORT_DENY_BY_PROXY = 608,
};

}  // namespace net

#endif  // HUAWEI_NET_BASE_FALLBACK_PROXY_CONSTANTS

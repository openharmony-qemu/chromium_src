// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_
#define OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_

#include <string>
#include <vector>
namespace ohos {
namespace adapter {

/*
 * config example
 * {
 *   "trace-startup" : true,
 *   "trace-startup-file"
 *     : "/data/storage/el2/base/cache/trace.json",
 *   "trace-startup-duration": 15,
 *   "remote-debugging": true,
 *   "remote-debugging-port": 9222,
 *   "remote-allow-origins": "http://localhost:9222"
 * }
 */

class DevConfig {
 public:
  bool trace_startup = false;
  std::string trace_startup_file;
  int trace_startup_duration;
  bool remote_debug = false;
  int32_t remote_debugging_port = 9222;
  std::string remote_allow_origins;
};

DevConfig GetDevConfig(const std::string& path);
std::vector<std::string> GetDevCommandLines();

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_
